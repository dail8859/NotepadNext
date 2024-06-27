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

equals(QT_MAJOR_VERSION, 6): QT += core5compat

SOURCES += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    $$PWD/scintilla/src/XPM.cxx \
    $$PWD/scintilla/src/ViewStyle.cxx \
    $$PWD/scintilla/src/UniqueString.cxx \
    $$PWD/scintilla/src/UniConversion.cxx \
    $$PWD/scintilla/src/Style.cxx \
    $$PWD/scintilla/src/Selection.cxx \
    $$PWD/scintilla/src/ScintillaBase.cxx \
    $$PWD/scintilla/src/RunStyles.cxx \
    $$PWD/scintilla/src/RESearch.cxx \
    $$PWD/scintilla/src/PositionCache.cxx \
    $$PWD/scintilla/src/PerLine.cxx \
    $$PWD/scintilla/src/MarginView.cxx \
    $$PWD/scintilla/src/LineMarker.cxx \
    $$PWD/scintilla/src/KeyMap.cxx \
    $$PWD/scintilla/src/Indicator.cxx \
    $$PWD/scintilla/src/Geometry.cxx \
    $$PWD/scintilla/src/EditView.cxx \
    $$PWD/scintilla/src/Editor.cxx \
    $$PWD/scintilla/src/EditModel.cxx \
    $$PWD/scintilla/src/Document.cxx \
    $$PWD/scintilla/src/Decoration.cxx \
    $$PWD/scintilla/src/DBCS.cxx \
    $$PWD/scintilla/src/ContractionState.cxx \
    $$PWD/scintilla/src/CharClassify.cxx \
    $$PWD/scintilla/src/CharacterType.cxx \
    $$PWD/scintilla/src/CharacterCategoryMap.cxx \
    $$PWD/scintilla/src/CellBuffer.cxx \
    $$PWD/scintilla/src/CaseFolder.cxx \
    $$PWD/scintilla/src/CaseConvert.cxx \
    $$PWD/scintilla/src/CallTip.cxx \
    $$PWD/scintilla/src/AutoComplete.cxx \
    $$PWD/scintilla/src/ChangeHistory.cxx \
    $$PWD/scintilla/src/UndoHistory.cxx

HEADERS  += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaQt.h

INCLUDEPATH += \
    $$PWD/scintilla/qt/ScintillaEditBase/ \
    $$PWD/scintilla/qt/ScintillaEdit/ \
    $$PWD/scintilla/include \
    $$PWD/scintilla/src

DEFINES += SCINTILLA_QT=1 MAKING_LIBRARY=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 EXPORT_IMPORT_API=
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}
