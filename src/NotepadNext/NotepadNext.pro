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


QT += core widgets printsupport network

TARGET = NotepadNext

TEMPLATE = app

include(../Config.pri)

include(../singleapplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

include(../uchardet.pri)
include(../lua.pri)
include(../editorconfig-core-qt/EditorConfig.pri)

# Set variables for output executable
VERSION = $$APP_VERSION

win32 {
    QMAKE_TARGET_COMPANY = Notepad Next
    QMAKE_TARGET_DESCRIPTION = Notepad Next
    QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
    QMAKE_TARGET_PRODUCT = Notepad Next
    RC_ICONS = icons/newfile.ico
}


SOURCES += \
    BufferManager.cpp \
    DockedEditor.cpp \
    Finder.cpp \
    IFaceTable.cpp \
    IFaceTableMixer.cpp \
    LuaExtension.cpp \
    LuaState.cpp \
    MacroRecorder.cpp \
    NotepadNextApplication.cpp \
    NppImporter.cpp \
    QRegexSearch.cpp \
    QuickFindWidget.cpp \
    RecentFilesListManager.cpp \
    SciIFaceTable.cpp \
    ScintillaBuffer.cpp \
    ScintillaNext.cpp \
    Settings.cpp \
    decorators/SurroundSelection.cpp \
    dialogs/FindReplaceDialog.cpp \
    dialogs/LuaConsoleDock.cpp \
    dialogs/MacroRunDialog.cpp \
    dialogs/MacroSaveDialog.cpp \
    dialogs/MainWindow.cpp \
    dialogs/PreferencesDialog.cpp \
    dialogs/WindowListDialog.cpp \
    main.cpp \
    decorators/BraceMatch.cpp \
    decorators/EditorDecorator.cpp \
    decorators/HighlightedScrollBar.cpp \
    decorators/LineNumbers.cpp \
    decorators/SmartHighlighter.cpp \
    widgets/StatusLabel.cpp

HEADERS += \
    BufferManager.h \
    DockedEditor.h \
    Finder.h \
    FocusWatcher.h \
    IFaceTable.h \
    IFaceTableMixer.h \
    LuaExtension.h \
    LuaState.h \
    MacroRecorder.h \
    NotepadNextApplication.h \
    NppImporter.h \
    QRegexSearch.h \
    QuickFindWidget.h \
    RecentFilesListManager.h \
    SciIFaceTable.h \
    ScintillaBuffer.h \
    ScintillaNext.h \
    Settings.h \
    decorators/SurroundSelection.h \
    dialogs/FindReplaceDialog.h \
    dialogs/LuaConsoleDock.h \
    dialogs/MacroRunDialog.h \
    dialogs/MacroSaveDialog.h \
    dialogs/MainWindow.h \
    dialogs/PreferencesDialog.h \
    dialogs/WindowListDialog.h \
    decorators/BraceMatch.h \
    decorators/EditorDecorator.h \
    decorators/HighlightedScrollBar.h \
    decorators/LineNumbers.h \
    decorators/SmartHighlighter.h \
    widgets/StatusLabel.h

FORMS += \
    QuickFindWidget.ui \
    dialogs/MainWindow.ui \
    dialogs/FindReplaceDialog.ui \
    dialogs/LuaConsoleDock.ui \
    dialogs/MacroRunDialog.ui \
    dialogs/MacroSaveDialog.ui \
    dialogs/WindowListDialog.ui \
    dialogs/PreferencesDialog.ui

RESOURCES += \
    resources.qrc \
    scripts.qrc

INCLUDEPATH += $$PWD/decorators
INCLUDEPATH += $$PWD/dialogs
INCLUDEPATH += $$PWD/widgets

# Statically link in Scintilla
DEFINES += EXPORT_IMPORT_API=
LIBS += -L$$OUT_PWD/../scintilla/qt/ScintillaEdit/ -lScintillaEdit
INCLUDEPATH += \
    $$PWD/../scintilla/qt/ScintillaEditBase \
    $$PWD/../scintilla/qt/ScintillaEdit \
    $$PWD/../scintilla/include \
    $$PWD/../scintilla/src \
    $$PWD/../scintilla/lexlib
DEPENDPATH += $$PWD/../scintilla/qt/ScintillaEdit

INCLUDEPATH += $$PWD/../LuaBridge
INCLUDEPATH += $$PWD/../
DEFINES += LUA_VERSION_NUM=503

win32-g++:LIBS += libUser32
win32-msvc*:LIBS += User32.lib

LIBS += -L$$OUT_PWD/../ads/src -lads
INCLUDEPATH += $$PWD/../ads/src
DEPENDPATH += $$PWD/../ads/src
