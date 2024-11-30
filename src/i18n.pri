# This file is part of Notepad Next.
# Copyright 2020 Justin Dailey
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

CONFIG += lrelease embed_translations

# All the application ts files
TS_FILES = $$files($$PWD/../i18n/*.ts)

for(TS_FILE, TS_FILES) {
    # Add the application translation file
    TRANSLATIONS += $$TS_FILE

    # Extract the locale from the file name (e.g. NotepadNext_pl_PL.ts to pl_PL)
    LOCALE =  $$str_member($$TS_FILE, -8, -4)

    # Extra translation files that include both the language and translation
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qt_$${LOCALE}.qm)
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qtbase_$${LOCALE}.qm)

    # Check for files without the territory specified (e.g. pl)
    LANGUAGE = $$replace(LOCALE, "(..).*", "\\1")
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qt_$${LANGUAGE}.qm)
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qtbase_$${LANGUAGE}.qm)
}
