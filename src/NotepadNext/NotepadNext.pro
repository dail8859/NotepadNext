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


include(../scintilla.pri)
include(../lexilla.pri)
include(../uchardet.pri)
include(../lua.pri)
include(../ads.pri)
include(../editorconfig-core-qt/EditorConfig.pri)
include(../QSimpleUpdater/QSimpleUpdater.pri)

# Set variables for output executable
VERSION = $$APP_VERSION

CONFIG += file_copies

win32 {
    QMAKE_TARGET_COMPANY = Notepad Next
    QMAKE_TARGET_DESCRIPTION = Notepad Next
    QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
    QMAKE_TARGET_PRODUCT = Notepad Next
    RC_ICONS = ../../icon/nn.ico

    COPIES += openssl
    openssl.files = $$files(../../deploy/windows/*.dll)
    openssl.path = $$OUT_PWD
}

COPIES += license
license.files = ../../LICENSE
license.path = $$OUT_PWD


SOURCES += \
    ColorPickerDelegate.cpp \
    ComboBoxDelegate.cpp \
    DockedEditor.cpp \
    EditorManager.cpp \
    Finder.cpp \
    IFaceTable.cpp \
    IFaceTableMixer.cpp \
    LanguageKeywordsModel.cpp \
    LanguagePropertiesModel.cpp \
    LanguageStylesModel.cpp \
    LuaExtension.cpp \
    LuaState.cpp \
    MacroRecorder.cpp \
    NotepadNextApplication.cpp \
    NppImporter.cpp \
    QRegexSearch.cpp \
    QuickFindWidget.cpp \
    RecentFilesListManager.cpp \
    SciIFaceTable.cpp \
    ScintillaNext.cpp \
    Settings.cpp \
    SpinBoxDelegate.cpp \
    decorators/ApplicationDecorator.cpp \
    decorators/AutoIndentation.cpp \
    decorators/BetterMultiSelection.cpp \
    decorators/EditorConfigAppDecorator.cpp \
    decorators/SurroundSelection.cpp \
    dialogs/EditorInspectorDock.cpp \
    dialogs/FindReplaceDialog.cpp \
    dialogs/LanguageInspectorDock.cpp \
    dialogs/LuaConsoleDock.cpp \
    dialogs/MacroRunDialog.cpp \
    dialogs/MacroSaveDialog.cpp \
    dialogs/MainWindow.cpp \
    dialogs/PreferencesDialog.cpp \
    main.cpp \
    decorators/BraceMatch.cpp \
    decorators/EditorDecorator.cpp \
    decorators/HighlightedScrollBar.cpp \
    decorators/LineNumbers.cpp \
    decorators/SmartHighlighter.cpp \
    widgets/EditorInfoStatusBar.cpp \
    widgets/StatusLabel.cpp

HEADERS += \
    ColorPickerDelegate.h \
    ComboBoxDelegate.h \
    DockedEditor.h \
    DockedEditorTitleBar.h \
    EditorManager.h \
    Finder.h \
    FocusWatcher.h \
    IFaceTable.h \
    IFaceTableMixer.h \
    LanguageKeywordsModel.h \
    LanguagePropertiesModel.h \
    LanguageStylesModel.h \
    LuaExtension.h \
    LuaState.h \
    MacroRecorder.h \
    NotepadNextApplication.h \
    NppImporter.h \
    QRegexSearch.h \
    QuickFindWidget.h \
    RecentFilesListManager.h \
    SciIFaceTable.h \
    ScintillaNext.h \
    Settings.h \
    SpinBoxDelegate.h \
    decorators/ApplicationDecorator.h \
    decorators/AutoIndentation.h \
    decorators/BetterMultiSelection.h \
    decorators/EditorConfigAppDecorator.h \
    decorators/SurroundSelection.h \
    dialogs/EditorInspectorDock.h \
    dialogs/FindReplaceDialog.h \
    dialogs/LanguageInspectorDock.h \
    dialogs/LuaConsoleDock.h \
    dialogs/MacroRunDialog.h \
    dialogs/MacroSaveDialog.h \
    dialogs/MainWindow.h \
    dialogs/PreferencesDialog.h \
    decorators/BraceMatch.h \
    decorators/EditorDecorator.h \
    decorators/HighlightedScrollBar.h \
    decorators/LineNumbers.h \
    decorators/SmartHighlighter.h \
    widgets/EditorInfoStatusBar.h \
    widgets/StatusLabel.h

FORMS += \
    QuickFindWidget.ui \
    dialogs/EditorInspectorDock.ui \
    dialogs/LanguageInspectorDock.ui \
    dialogs/MainWindow.ui \
    dialogs/FindReplaceDialog.ui \
    dialogs/LuaConsoleDock.ui \
    dialogs/MacroRunDialog.ui \
    dialogs/MacroSaveDialog.ui \
    dialogs/PreferencesDialog.ui

RESOURCES += \
    resources.qrc \
    scripts.qrc

INCLUDEPATH += $$PWD/decorators
INCLUDEPATH += $$PWD/dialogs
INCLUDEPATH += $$PWD/widgets


INCLUDEPATH += $$PWD/../LuaBridge
INCLUDEPATH += $$PWD/../
DEFINES += LUA_VERSION_NUM=503

INCLUDEPATH += $$PWD/../lexilla/include

win32-g++:LIBS += libUser32
win32-msvc*:LIBS += User32.lib

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/qrc
UI_DIR = build/ui
