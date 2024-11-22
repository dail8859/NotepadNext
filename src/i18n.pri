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

## [i18n.pri]  Configuration related to internationalization.

CONFIG += lrelease embed_translations

LOCALES = \
    zh_CN \
    zh_TW \
    sv_SE \
    uk_UA \
    tr_TR \
    pl_PL \
    ru_RU \
    pt_PT \
    pt_BR

for(LOCALE, LOCALES) {
    # The application translation file
    TRANSLATIONS += ../../i18n/NotepadNext_$${LOCALE}.ts

    # Extra translation files that include both the language and translation
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qt_$${LOCALE}.qm)
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qtbase_$${LOCALE}.qm)

    # Check for files without the territory specified
    LANGUAGE = $$replace(LOCALE, "(..).*", "\\1")
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qt_$${LANGUAGE}.qm)
    EXTRA_TRANSLATIONS += $$files($$[QT_INSTALL_TRANSLATIONS]/qtbase_$${LANGUAGE}.qm)
}
