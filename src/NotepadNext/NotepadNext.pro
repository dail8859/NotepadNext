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
    Finder.cpp \
    LineNumbers.cpp \
    main.cpp \
    ScintillaBuffer.cpp \
    MainWindow.cpp \
    BufferManager.cpp \
    TabbedEditor.cpp \
    BufferView.cpp \
    NppTabBar.cpp \
    StatusLabel.cpp \
    HighlightedScrollBar.cpp \
    NppImporter.cpp \
    RecentFilesListManager.cpp \
    FindReplaceDialog.cpp \
    SmartHighlighter.cpp \
    ScintillaNext.cpp \
    QRegexSearch.cpp \
    LuaConsoleDock.cpp \
    LuaState.cpp \
    IFaceTable.cpp \
    LuaExtension.cpp \
    SciIFaceTable.cpp \
    IFaceTableMixer.cpp \
    NotepadNextApplication.cpp \
    MacroRecorder.cpp \
    MacroRunDialog.cpp \
    MacroSaveDialog.cpp \
    WindowListDialog.cpp \
    PreferencesDialog.cpp \
    Settings.cpp \
    Plugin.cpp \
    BraceMatch.cpp

HEADERS += \
    Finder.h \
    LineNumbers.h \
    ScintillaBuffer.h \
    MainWindow.h \
    BufferManager.h \
    TabbedEditor.h \
    BufferView.h \
    NppTabBar.h \
    StatusLabel.h \
    HighlightedScrollBar.h \
    NppImporter.h \
    RecentFilesListManager.h \
    FindReplaceDialog.h \
    SmartHighlighter.h \
    ScintillaNext.h \
    QRegexSearch.h \
    LuaConsoleDock.h \
    LuaState.h \
    IFaceTable.h \
    LuaExtension.h \
    SciIFaceTable.h \
    IFaceTableMixer.h \
    NotepadNextApplication.h \
    MacroRecorder.h \
    MacroRunDialog.h \
    MacroSaveDialog.h \
    WindowListDialog.h \
    PreferencesDialog.h \
    Settings.h \
    Plugin.h \
    BraceMatch.h

FORMS += \
    MainWindow.ui \
    FindReplaceDialog.ui \
    LuaConsoleDock.ui \
    MacroRunDialog.ui \
    MacroSaveDialog.ui \
    WindowListDialog.ui \
    PreferencesDialog.ui

RESOURCES += \
    resources.qrc \
    scripts.qrc

# Statically link in Scintilla
DEFINES += EXPORT_IMPORT_API= SCI_OWNREGEX
LIBS += -L$$OUT_PWD/../scintilla/qt/ScintillaEdit/ -lScintillaEdit
INCLUDEPATH += \
    $$PWD/../scintilla/qt/ScintillaEditBase \
    $$PWD/../scintilla/qt/ScintillaEdit \
    $$PWD/../scintilla/include \
    $$PWD/../scintilla/src \
    $$PWD/../scintilla/lexlib
DEPENDPATH += $$PWD/../scintilla/qt/ScintillaEdit

# Statically link in qtsingleapplication
LIBS += -L$$OUT_PWD/../qtsingleapplication/ -lqtsingleapplication
INCLUDEPATH += $$PWD/../qtsingleapplication/src
DEPENDPATH += $$PWD/../qtsingleapplication/src

# Statically link in Lua
LIBS += -L$$OUT_PWD/../lua/ -lLua
INCLUDEPATH += $$PWD/../lua/src
DEPENDPATH += $$PWD/../lua/src

# Statically link in uchardet
LIBS += -L$$OUT_PWD/../uchardet/ -luchardet
INCLUDEPATH += $$PWD/../uchardet/src
DEPENDPATH += $$PWD/../uchardet/src

INCLUDEPATH += $$PWD/../LuaBridge
DEFINES += LUA_VERSION_NUM=503

win32-g++:LIBS += libUser32
win32-msvc*:LIBS += User32.lib

DISTFILES +=
