import os
import sys
import shlex
import shutil
import subprocess
import glob

import versioneer

from setuptools import setup, find_packages
from setuptools.command.build_py import build_py
from setuptools.extension import Extension
from distutils import sysconfig, dir_util, spawn, log, cmd
from distutils.dep_util import newer
import sipdistutils
import sipconfig
from PyQt5.QtCore import PYQT_CONFIGURATION
from PyQt5.pyrcc_main import processResourceFile

MODULE_NAME = "ads"
SRC_PATH = "PyQtAds"

REQUIRE_PYQT = True
if "--conda-recipe" in sys.argv:
    REQUIRE_PYQT = False
    sys.argv.remove("--conda-recipe")


class HostPythonConfiguration(object):
    def __init__(self):
        self.platform = sys.platform
        self.version = sys.hexversion>>8

        self.inc_dir = sysconfig.get_python_inc()
        self.venv_inc_dir = sysconfig.get_python_inc(prefix=sys.prefix)
        self.module_dir = sysconfig.get_python_lib(plat_specific=1)

        if sys.platform == 'win32':
            self.data_dir = sys.prefix
            self.lib_dir = sys.prefix +'\\libs'
        else:
            self.data_dir = sys.prefix + '/share'
            self.lib_dir = sys.prefix + '/lib'


class TargetQtConfiguration(object):
    def __init__(self, qmake):
        pipe = os.popen(' '.join([qmake, '-query']))

        for l in pipe:
            l = l.strip()

            tokens = l.split(':', 1)
            if isinstance(tokens, list):
                if len(tokens) != 2:
                    error("Unexpected output from qmake: '%s'\n" % l)

                name, value = tokens
            else:
                name = tokens
                value = None

            name = name.replace('/', '_')
            setattr(self, name, value)

        pipe.close()


class build_ext(sipdistutils.build_ext):

    description = "Builds the " + MODULE_NAME + " module."

    user_options = sipdistutils.build_ext.user_options + [
        ('qmake-bin=', None, "Path to qmake binary"),
        ('sip-bin=', None, "Path to sip binary"),
        ('qt-include-dir=', None, "Path to Qt headers"),
        ('pyqt-sip-dir=', None, "Path to PyQt's SIP files"),
        ('pyqt-sip-flags=', None, "SIP flags used to generate PyQt bindings"),
        ('sip-dir=', None, "Path to module's SIP files"),
        ('inc-dir=', None, "Path to module's include files")
    ]

    def initialize_options (self):
        super().initialize_options()
        self.qmake_bin = 'qmake'
        self.sip_bin = None
        self.qt_include_dir = None
        self.qt_libinfix = ''
        self.pyqt_sip_dir = None
        self.pyqt_sip_flags = None
        self.sip_files_dir = None
        self.sip_inc_dir = None
        self.inc_dir = None
        self.pyconfig = HostPythonConfiguration()
        self.qtconfig = TargetQtConfiguration(self.qmake_bin)
        self.config = sipconfig.Configuration()
        self.config.default_mod_dir = ("/usr/local/lib/python%i.%i/dist-packages" %
                                      (sys.version_info.major, sys.version_info.minor))

    def finalize_options (self):
        super().finalize_options()

        if not self.qt_include_dir:
            self.qt_include_dir = self.qtconfig.QT_INSTALL_HEADERS

        if not self.qt_libinfix:
            try:
                with open(os.path.join(self.qtconfig.QT_INSTALL_PREFIX, 'mkspecs', 'qconfig.pri'), 'r') as f:
                    for line in f.readlines():
                        if line.startswith('QT_LIBINFIX'):
                            self.qt_libinfix = line.split('=')[1].strip('\n').strip()
            except (FileNotFoundError, IndexError):
                pass

        if not self.pyqt_sip_dir:
            self.pyqt_sip_dir = os.path.join(self.pyconfig.data_dir, 'sip', 'PyQt5')

        if not self.pyqt_sip_flags:
            self.pyqt_sip_flags = PYQT_CONFIGURATION.get('sip_flags', '')

        if not self.sip_files_dir:
            self.sip_files_dir = os.path.abspath(os.path.join(".", "sip"))

        if not self.sip_inc_dir:
            self.sip_inc_dir = self.pyconfig.venv_inc_dir

        if not self.inc_dir:
            self.inc_dir = os.path.abspath(os.path.join(".", "src"))

        if not self.qt_include_dir:
            raise SystemExit('Could not find Qt5 headers. '
                             'Please specify via --qt-include-dir=')

        if not self.pyqt_sip_dir:
            raise SystemExit('Could not find PyQt SIP files. '
                             'Please specify containing directory via '
                             '--pyqt-sip-dir=')

        if not self.pyqt_sip_flags:
            raise SystemExit('Could not find PyQt SIP flags. '
                             'Please specify via --pyqt-sip-flags=')

    def _find_sip(self):
        """override _find_sip to allow for manually speficied sip path."""
        
        # 1. Manually specified sip_bin
        if self.sip_bin:
            return self.sip_bin

        # 2. Path determined from sipconfig.Configuration()
        #    This may not exist, depending on conda build configuration.
        sip_bin = super()._find_sip()
        if os.path.exists(sip_bin):
            return sip_bin

        # 3. Finally, fall back to sip binary found in path
        sip_bin = shutil.which('sip')
        if sip_bin:
            return sip_bin

        raise SystemExit('Could not find PyQt SIP binary.')
        
    def _sip_sipfiles_dir(self):
        sip_dir = super()._sip_sipfiles_dir()
        if os.path.exists(sip_dir):
            return sip_dir
        
        return os.path.join(sys.prefix, 'sip', 'PyQt5')

    def _sip_compile(self, sip_bin, source, sbf):
        target_dir = os.path.dirname(__file__) if self.inplace else self.build_lib
        pyi = os.path.join(target_dir, "PyQtAds", "QtAds", "ads.pyi")
        if not os.path.exists(os.path.dirname(pyi)):
            os.makedirs(os.path.dirname(pyi))
        
        cmd = [sip_bin]
        if hasattr(self, 'sip_opts'):
            cmd += self.sip_opts
        if hasattr(self, '_sip_sipfiles_dir'):
            cmd += ['-I', self._sip_sipfiles_dir()]
        cmd += [
            "-I", self.sip_files_dir,
            "-I", self.pyqt_sip_dir,
            "-I", self.sip_inc_dir,
            "-I", self.inc_dir,
            "-c", self._sip_output_dir(),
            "-b", sbf,
            "-y", pyi,
            "-w", "-o"]

        cmd += shlex.split(self.pyqt_sip_flags)  # use same SIP flags as for PyQt5
        cmd.append(source)
        self.spawn(cmd)
        
        if os.path.exists(pyi):
            with open(pyi) as f:
                content = f.readlines()
            with open(pyi, "w") as f:
                for line in content:
                    if not line.startswith("class ads"):
                        f.write(line)

    def swig_sources (self, sources, extension=None):
        if not self.extensions:
            return

        # Add the local include directory to the include path
        if extension is not None:
            extension.extra_compile_args += ['-D', 'QT_CORE_LIB',
                                             '-D', 'QT_GUI_LIB',
                                             '-D', 'QT_WIDGETS_LIB',
                                             '-D', 'ADS_SHARED_EXPORT']
            extension.include_dirs += [self.qt_include_dir, self.inc_dir,
                            os.path.join(self.qt_include_dir, 'QtCore'),
                            os.path.join(self.qt_include_dir, 'QtGui'),
                            os.path.join(self.qt_include_dir, 'QtWidgets')]
            extension.libraries += ['Qt5Core' + self.qt_libinfix,
                                    'Qt5Gui' + self.qt_libinfix,
                                    'Qt5Widgets' + self.qt_libinfix]

            if sys.platform == 'win32':
                extension.library_dirs += [self.qtconfig.QT_INSTALL_LIBS,
                                       self.inc_dir, self._sip_output_dir()]
            elif sys.platform == 'darwin':
                extension.extra_compile_args += ['-F' + self.qtconfig.QT_INSTALL_LIBS,
                    '-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.9']
                extension.extra_link_args += ['-F' + self.qtconfig.QT_INSTALL_LIBS,
                    '-mmacosx-version-min=10.9']
            elif sys.platform == 'linux':
                extension.extra_compile_args += ['-D', 'QT_X11EXTRAS_LIB', '-std=c++11']
                extension.include_dirs += [os.path.join(self.qt_include_dir, 'QtX11Extras')]
                extension.libraries += ['Qt5X11Extras' + self.qt_libinfix]

        return super().swig_sources(sources, extension)

    def build_extension(self, ext):        
        cppsources = [source for source in ext.sources if source.endswith(".cpp")]
        headersources = ['src/DockAreaTitleBar_p.h']

        dir_util.mkpath(self.build_temp, dry_run=self.dry_run)

        def get_moc_args(out_file, source):
            if sys.platform.startswith('linux'):
                return ["moc", "-D", "Q_OS_LINUX=1", "-o", out_file, source]
            if sys.platform.startswith('darwin'):
                return ["moc", "-D", "Q_OS_MACOS=1", "-o", out_file, source]
            if sys.platform.startswith('win'):
                return ["moc", "-D", "Q_OS_WIN=1", "-o", out_file, source]
            return ["moc", "-o", out_file, source]

        # Run moc on all header files.
        for source in cppsources:
            # *.cpp -> *.moc
            moc_file = os.path.basename(source).replace(".cpp", ".moc")
            out_file = os.path.join(self.build_temp, moc_file)

            if newer(source, out_file) or self.force:
                spawn.spawn(get_moc_args(out_file, source), dry_run=self.dry_run)

            header = source.replace(".cpp", ".h")
            if os.path.exists(header):
                # *.h -> moc_*.cpp
                moc_file = "moc_" + os.path.basename(header).replace(".h", ".cpp")
                out_file = os.path.join(self.build_temp, moc_file)

                if newer(header, out_file) or self.force:
                    spawn.spawn(get_moc_args(out_file, header), dry_run=self.dry_run)

                if os.path.getsize(out_file) > 0:
                    ext.sources.append(out_file)

        # Run moc on all orphan header files.
        for source in headersources:
            # *.cpp -> *.moc
            moc_file = os.path.basename(source).replace(".h", ".moc")
            out_file = os.path.join(self.build_temp, moc_file)

            if newer(source, out_file) or self.force:
                spawn.spawn(get_moc_args(out_file, source),
                            dry_run=self.dry_run)

            header = source
            if os.path.exists(header):
                # *.h -> moc_*.cpp
                moc_file = "moc_" + os.path.basename(header).replace(
                    ".h", ".cpp")
                out_file = os.path.join(self.build_temp, moc_file)

                if newer(header, out_file) or self.force:
                    spawn.spawn(get_moc_args(out_file, header),
                                dry_run=self.dry_run)

                if os.path.getsize(out_file) > 0:
                    ext.sources.append(out_file)

        # Add the temp build directory to include path, for compiler to find
        # the created .moc files
        ext.include_dirs += [self._sip_output_dir()]
        
        # Run rcc on all resource files
        resources = [source for source in ext.sources if source.endswith(".qrc")]
        for source in resources:
            ext.sources.remove(source)
            out_file = os.path.join(self.build_temp, "qrc_" + os.path.basename(source).replace(".qrc", ".cpp"))
            if newer(header, out_file) or self.force:
                spawn.spawn(["rcc", "-name", os.path.splitext(os.path.basename(source))[0], source, "-o", out_file], dry_run=self.dry_run)

            if os.path.getsize(out_file) > 0:
                ext.sources.append(out_file)

        sipdistutils.build_ext.build_extension(self, ext)
        
        import inspect
        sys.path.append(os.path.join(self.build_lib, 'PyQtAds', 'QtAds'))
        import ads

        with open(os.path.join(self.build_lib, 'PyQtAds', 'QtAds', '__init__.py'), 'w') as f:
            f.write('from .._version import *\n')
            f.write('from .ads import ads\n')
            for name, member in sorted(inspect.getmembers(ads.ads)):
                if not name.startswith('_'):
                    f.write('{0} = ads.{0}\n'.format(name))


class ProcessResourceCommand(cmd.Command):
    """A custom command to compile the resource file into a Python file"""

    description = "Compile the qrc file into a python file"

    def initialize_options(self):
        return

    def finalize_options(self):
        return

    def run(self):
        processResourceFile([os.path.join('src', 'ads.qrc')],
                            os.path.join(SRC_PATH, 'rc.py'), False)


class BuildPyCommand(build_py):
    """Custom build command to include ProcessResource command"""

    def run(self):
        self.run_command("process_resource")
        build_py.run(self)


setup_requires = ["PyQt5"] if REQUIRE_PYQT else []
cpp_sources = glob.glob(os.path.join('src', '*.cpp'))
sip_sources = [os.path.join('sip', MODULE_NAME + '.sip')]
resources = [os.path.join('src', MODULE_NAME + '.qrc')]
if sys.platform == 'linux':
    cpp_sources += glob.glob(os.path.join('src', 'linux', '*.cpp'))
ext_modules = [Extension('PyQtAds.QtAds.ads', cpp_sources + sip_sources + resources)]

install_requires = ["PyQt5"]
if sys.platform == 'win32':
    install_requires.append("pywin32")


with open('README.md', 'r') as f:
    LONG_DESCRIPTION = f.read()

setup(
    name = SRC_PATH,
    author = "Nicolas Elie",
    author_email = "nicolas.elie@cnrs.fr",
    url = "https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System",
    version = versioneer.get_version(),
    description = "Advanced Docking System for Qt",
    long_description = LONG_DESCRIPTION,
    keywords = ["qt"],
    license = "LGPLv2+",
    classifiers = ["Development Status :: 4 - Beta",
                   "Intended Audience :: Developers",
                   "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
                   "Operating System :: OS Independent",
                   "Topic :: Software Development :: Libraries :: Python Modules",
                   "Environment :: Win32 (MS Windows)",
                   "Environment :: MacOS X",
                   "Environment :: X11 Applications :: Qt",
                   "Programming Language :: Python :: 3",
                   "Programming Language :: Python :: 3.2",
                   "Programming Language :: Python :: 3.3",
                   "Programming Language :: Python :: 3.4",
                   "Programming Language :: Python :: 3.5",
                   "Programming Language :: Python :: 3.6",
                   "Programming Language :: Python :: 3.7"],
    ext_modules = ext_modules,
    cmdclass = versioneer.get_cmdclass({'process_resource': ProcessResourceCommand,
                                        'build_py': BuildPyCommand,
                                        'build_ext': build_ext}),
    packages = find_packages(),
    setup_requires = setup_requires,
    install_requires = install_requires,
    zip_safe=False
)
