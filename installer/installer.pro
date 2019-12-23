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


include(../src/Version.pri)

TEMPLATE = aux

INSTALLER = "NotepadNext-v$$APP_VERSION"

!win32 {
    error(Building the installer is only supported on Windows currently)
}

!exists ($$PWD/packages/app/data/data.7z) {
    error(packages/app/data/data.7z missing)
}

INPUT = config/config.xml \
    config/controller.qs \
    $$files(packages/**package.xml, true) \
    $$files(packages/**.qs, true) \
    $$files(packages/**.ui, true)

installer.input = INPUT
installer.output = $$INSTALLER
installer.CONFIG += target_predeps no_link combine
#installer.depends = predeploy
installer.commands = C:/Qt/Tools/QtInstallerFramework/3.1/bin/binarycreator --offline-only -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}

QMAKE_EXTRA_COMPILERS += installer

#win32:RC_FILE += "$$PWD/resources/res.rc"
