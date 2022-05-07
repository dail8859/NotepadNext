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
win32:include(../QSimpleUpdater/QSimpleUpdater.pri)
include(../i18n.pri)

# Set variables for output executable
VERSION = $$APP_VERSION

CONFIG += file_copies

win32 {
    QMAKE_TARGET_COMPANY = Notepad Next
    QMAKE_TARGET_DESCRIPTION = Notepad Next
    QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
    QMAKE_TARGET_PRODUCT = Notepad Next
    RC_ICONS = ../../icon/NotepadNext.ico

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
    EditorPrintPreviewRenderer.cpp \
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
    ScintillaCommenter.cpp \
    ScintillaNext.cpp \
    SelectionTracker.cpp \
    Settings.cpp \
    SpinBoxDelegate.cpp \
    UndoAction.cpp \
    decorators/ApplicationDecorator.cpp \
    decorators/AutoCompletion.cpp \
    decorators/AutoIndentation.cpp \
    decorators/BetterMultiSelection.cpp \
    decorators/EditorConfigAppDecorator.cpp \
    decorators/SurroundSelection.cpp \
    docks/EditorInspectorDock.cpp \
    dialogs/FindReplaceDialog.cpp \
    docks/FolderAsWorkspaceDock.cpp \
    docks/LanguageInspectorDock.cpp \
    docks/LuaConsoleDock.cpp \
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
    EditorPrintPreviewRenderer.h \
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
    ScintillaCommenter.h \
    ScintillaNext.h \
    SelectionTracker.h \
    Settings.h \
    SpinBoxDelegate.h \
    UndoAction.h \
    decorators/ApplicationDecorator.h \
    decorators/AutoCompletion.h \
    decorators/AutoIndentation.h \
    decorators/BetterMultiSelection.h \
    decorators/EditorConfigAppDecorator.h \
    decorators/SurroundSelection.h \
    docks/EditorInspectorDock.h \
    dialogs/FindReplaceDialog.h \
    docks/FolderAsWorkspaceDock.h \
    docks/LanguageInspectorDock.h \
    docks/LuaConsoleDock.h \
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
    docks/EditorInspectorDock.ui \
    docks/FolderAsWorkspaceDock.ui \
    docks/LanguageInspectorDock.ui \
    dialogs/MainWindow.ui \
    dialogs/FindReplaceDialog.ui \
    docks/LuaConsoleDock.ui \
    dialogs/MacroRunDialog.ui \
    dialogs/MacroSaveDialog.ui \
    dialogs/PreferencesDialog.ui

RESOURCES += \
    resources.qrc \
    scripts.qrc

INCLUDEPATH += $$PWD/decorators
INCLUDEPATH += $$PWD/dialogs
INCLUDEPATH += $$PWD/docks
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

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    target.path = $$PREFIX/bin
    INSTALLS += target

    desktopfile.path = $$PREFIX/share/applications/
    desktopfile.files += ../../deploy/linux/NotepadNext.desktop
    INSTALLS += desktopfile

    iconscalable.path = $$PREFIX/share/icons/hicolor/scalable/mimetypes/
    iconscalable.files = ../../icon/NotepadNext.svg
    INSTALLS += iconscalable
}

macx: {
    ICON = ../../icon/NotepadNext.icns
}
