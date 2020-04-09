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
    main.cpp \
    ScintillaBuffer.cpp \
    BufferManager.cpp \
    widgets/TabbedEditor.cpp \
    BufferView.cpp \
    widgets/NppTabBar.cpp \
    widgets/StatusLabel.cpp \
    NppImporter.cpp \
    RecentFilesListManager.cpp \
    ScintillaNext.cpp \
    QRegexSearch.cpp \
    LuaState.cpp \
    IFaceTable.cpp \
    LuaExtension.cpp \
    SciIFaceTable.cpp \
    IFaceTableMixer.cpp \
    NotepadNextApplication.cpp \
    MacroRecorder.cpp \
    Settings.cpp \
    dialogs/MacroRunDialog.cpp \
    dialogs/MacroSaveDialog.cpp \
    dialogs/WindowListDialog.cpp \
    dialogs/PreferencesDialog.cpp \
    dialogs/MainWindow.cpp \
    dialogs/FindReplaceDialog.cpp \
    dialogs/LuaConsoleDock.cpp \
    plugins/Plugin.cpp \
    plugins/HighlightedScrollBar.cpp \
    plugins/SmartHighlighter.cpp \
    plugins/BraceMatch.cpp \
    plugins/LineNumbers.cpp

HEADERS += \
    Finder.h \
    ScintillaBuffer.h \
    BufferManager.h \
    widgets/TabbedEditor.h \
    BufferView.h \
    widgets/NppTabBar.h \
    widgets/StatusLabel.h \
    NppImporter.h \
    RecentFilesListManager.h \
    ScintillaNext.h \
    QRegexSearch.h \
    LuaState.h \
    IFaceTable.h \
    LuaExtension.h \
    SciIFaceTable.h \
    IFaceTableMixer.h \
    NotepadNextApplication.h \
    MacroRecorder.h \
    Settings.h \
    dialogs/MainWindow.h \
    dialogs/FindReplaceDialog.h \
    dialogs/LuaConsoleDock.h \
    dialogs/MacroRunDialog.h \
    dialogs/MacroSaveDialog.h \
    dialogs/WindowListDialog.h \
    dialogs/PreferencesDialog.h \
    plugins/Plugin.h \
    plugins/HighlightedScrollBar.h \
    plugins/SmartHighlighter.h \
    plugins/BraceMatch.h \
    plugins/LineNumbers.h

FORMS += \
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

INCLUDEPATH += $$PWD/plugins
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
