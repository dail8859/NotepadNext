# This file is part of Notepad Next.
# Copyright 2019 Justin Dailey
#
# Notepad Next is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Notepad Next is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.


include(Version.pri)

lessThan(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 13) {
        error(Qt v5.13.x is required)
    }
}

TEMPLATE = subdirs

SUBDIRS = NotepadNext

win32 {
    package.target = make_package
    package.commands = \
        xcopy $$shell_path($${OUT_PWD}/NotepadNext/NotepadNext.exe) $$shell_path($${OUT_PWD}/package/) /Y && \
        xcopy $$shell_path($${PWD}/../LICENSE) $$shell_path($${OUT_PWD}/package/) /Y && \
        windeployqt --release --no-translations --no-system-d3d-compiler --no-compiler-runtime --no-angle --no-opengl-sw $$shell_path($${OUT_PWD}/package/NotepadNext.exe)

    zip.target = zip
    zip.depends = package
    zip.commands = 7z a -tzip $$quote(NotepadNext-v$${APP_VERSION}.zip) $$shell_path(./package/*)

    prepare_installer.target = prepare_installer
    prepare_installer.depends = package
    prepare_installer.commands = \
        xcopy $$shell_path(./package/*) $$shell_path(../installer/packages/app/data/) /Y/E && \
        del $$shell_path(../installer/packages/app/data/LICENSE)

    QMAKE_EXTRA_TARGETS += package prepare_installer zip
}
