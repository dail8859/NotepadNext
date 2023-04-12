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

TRANSLATIONS = \
    ../../i18n/NotepadNext_zh_CN.ts \
    ../../i18n/NotepadNext_sv_SE.ts \
    ../../i18n/NotepadNext_uk_UA.ts \
    ../../i18n/NotepadNext_tr_TR.ts

EXTRA_TRANSLATIONS = \
    $$[QT_INSTALL_TRANSLATIONS]/qt_zh_CN.qm \
    $$[QT_INSTALL_TRANSLATIONS]/qt_sv.qm \
    $$[QT_INSTALL_TRANSLATIONS]/qt_uk.qm \
    $$[QT_INSTALL_TRANSLATIONS]/qt_tr.qm
