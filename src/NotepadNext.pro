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

!versionAtLeast(QT_VERSION, 5.15):error("Qt v5.15 or greater is required")

TEMPLATE = subdirs

SUBDIRS = NotepadNext


# Extra Windows targets
win32 {
    # Package up the EXE with all the other needed files
    package.target = package
    package.commands = \
        xcopy $$shell_path($${OUT_PWD}/NotepadNext/NotepadNext.exe) $$shell_path($${OUT_PWD}/package/) /Y && \
        xcopy $$shell_path($${OUT_PWD}/NotepadNext/*.dll) $$shell_path($${OUT_PWD}/package/) /Y && \
        xcopy $$shell_path($${OUT_PWD}/NotepadNext/LICENSE) $$shell_path($${OUT_PWD}/package/) /Y && \
        xcopy $$shell_path($${OUT_PWD}/NotepadNext/i18n/*.qm) $$shell_path($${OUT_PWD}/package/i18n/) /Y &&

    equals(QT_MAJOR_VERSION, 6) {
        package.commands += windeployqt --release --no-translations --no-system-d3d-compiler --no-compiler-runtime --no-opengl-sw $$shell_path($${OUT_PWD}/package/NotepadNext.exe)
    } else {
        package.commands += windeployqt --release --no-translations --no-system-d3d-compiler --no-compiler-runtime --no-angle --no-opengl-sw $$shell_path($${OUT_PWD}/package/NotepadNext.exe)
    }

    # Zip it up
    zip.target = zip
    zip.depends = package
    zip.commands = 7z a -tzip $$quote(NotepadNext-v$${APP_VERSION}.zip) $$shell_path(./package/*) -x!libssl-1_1-x64.dll -x!libcrypto-1_1-x64.dll

    # Build the Installer
    installer.target = installer
    installer.depends = package
    installer.commands = makensis /V4 $$shell_path($${OUT_PWD}/../installer/installer.nsi)

    QMAKE_EXTRA_TARGETS += package zip installer
}
