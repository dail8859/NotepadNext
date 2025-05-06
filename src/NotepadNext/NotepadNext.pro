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

macos {
    QMAKE_INFO_PLIST = ../../deploy/macos/info.plist
}

COPIES += license
license.files = ../../LICENSE
license.path = $$OUT_PWD

SOURCES += \
    ApplicationSettings.cpp \
    ColorPickerDelegate.cpp \
    ComboBoxDelegate.cpp \
    Converter.cpp \
    DebugManager.cpp \
    DockedEditor.cpp \
    EditorHexViewerTableModel.cpp \
    EditorManager.cpp \
    EditorPrintPreviewRenderer.cpp \
    widgets/FadingIndicator.cpp \
    FileDialogHelpers.cpp \
    Finder.cpp \
    HtmlConverter.cpp \
    IFaceTable.cpp \
    IFaceTableMixer.cpp \
    LanguageStylesModel.cpp \
    LuaExtension.cpp \
    LuaState.cpp \
    Macro.cpp \
    MacroListModel.cpp \
    MacroManager.cpp \
    MacroRecorder.cpp \
    MacroStep.cpp \
    MacroStepTableModel.cpp \
    NotepadNextApplication.cpp \
    NppImporter.cpp \
    QRegexSearch.cpp \
    widgets/QuickFindWidget.cpp \
    RangeAllocator.cpp \
    RecentFilesListManager.cpp \
    RecentFilesListMenuBuilder.cpp \
    RtfConverter.cpp \
    SciIFaceTable.cpp \
    ScintillaCommenter.cpp \
    ScintillaNext.cpp \
    SearchResultsCollector.cpp \
    SelectionTracker.cpp \
    SessionManager.cpp \
    SpinBoxDelegate.cpp \
    TranslationManager.cpp \
    UndoAction.cpp \
    ZoomEventWatcher.cpp \
    decorators/ApplicationDecorator.cpp \
    decorators/AutoCompletion.cpp \
    decorators/AutoIndentation.cpp \
    decorators/BetterMultiSelection.cpp \
    decorators/BookMarkDecorator.cpp \
    decorators/EditorConfigAppDecorator.cpp \
    decorators/HTMLAutoCompleteDecorator.cpp \
    decorators/SurroundSelection.cpp \
    decorators/URLFinder.cpp \
    dialogs/ColumnEditorDialog.cpp \
    dialogs/MacroEditorDialog.cpp \
    docks/DebugLogDock.cpp \
    docks/EditorInspectorDock.cpp \
    dialogs/FindReplaceDialog.cpp \
    docks/FileListDock.cpp \
    docks/FolderAsWorkspaceDock.cpp \
    docks/HexViewerDock.cpp \
    docks/LanguageInspectorDock.cpp \
    docks/LuaConsoleDock.cpp \
    dialogs/MacroRunDialog.cpp \
    dialogs/MacroSaveDialog.cpp \
    dialogs/MainWindow.cpp \
    dialogs/PreferencesDialog.cpp \
    docks/SearchResultsDock.cpp \
    main.cpp \
    decorators/BraceMatch.cpp \
    decorators/EditorDecorator.cpp \
    decorators/HighlightedScrollBar.cpp \
    decorators/LineNumbers.cpp \
    decorators/SmartHighlighter.cpp \
    widgets/EditorInfoStatusBar.cpp \
    widgets/StatusLabel.cpp

HEADERS += \
    ApplicationSettings.h \
    ColorPickerDelegate.h \
    ComboBoxDelegate.h \
    Converter.h \
    DebugManager.h \
    DockedEditor.h \
    DockedEditorTitleBar.h \
    EditorHexViewerTableModel.h \
    EditorManager.h \
    EditorPrintPreviewRenderer.h \
    widgets/FadingIndicator.h \
    FileDialogHelpers.h \
    Finder.h \
    FocusWatcher.h \
    HtmlConverter.h \
    IFaceTable.h \
    IFaceTableMixer.h \
    ISearchResultsHandler.h \
    LanguageStylesModel.h \
    LuaExtension.h \
    LuaState.h \
    Macro.h \
    MacroListModel.h \
    MacroManager.h \
    MacroRecorder.h \
    MacroStep.h \
    MacroStepTableModel.h \
    NotepadNextApplication.h \
    NppImporter.h \
    QRegexSearch.h \
    widgets/QuickFindWidget.h \
    RangeAllocator.h \
    RecentFilesListManager.h \
    RecentFilesListMenuBuilder.h \
    RtfConverter.h \
    SciIFaceTable.h \
    ScintillaCommenter.h \
    ScintillaEnums.h \
    ScintillaNext.h \
    SearchResultsCollector.h \
    SelectionTracker.h \
    SessionManager.h \
    SpinBoxDelegate.h \
    TranslationManager.h \
    UndoAction.h \
    ZoomEventWatcher.h \
    decorators/ApplicationDecorator.h \
    decorators/AutoCompletion.h \
    decorators/AutoIndentation.h \
    decorators/BetterMultiSelection.h \
    decorators/BookMarkDecorator.h \
    decorators/EditorConfigAppDecorator.h \
    decorators/HTMLAutoCompleteDecorator.h \
    decorators/SurroundSelection.h \
    decorators/URLFinder.h \
    dialogs/ColumnEditorDialog.h \
    dialogs/MacroEditorDialog.h \
    docks/DebugLogDock.h \
    docks/EditorInspectorDock.h \
    dialogs/FindReplaceDialog.h \
    docks/FileListDock.h \
    docks/FolderAsWorkspaceDock.h \
    docks/HexViewerDock.h \
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
    docks/SearchResultsDock.h \
    widgets/EditorInfoStatusBar.h \
    widgets/StatusLabel.h

FORMS += \
    widgets/QuickFindWidget.ui \
    dialogs/ColumnEditorDialog.ui \
    dialogs/MacroEditorDialog.ui \
    docks/DebugLogDock.ui \
    docks/EditorInspectorDock.ui \
    docks/FileListDock.ui \
    docks/FolderAsWorkspaceDock.ui \
    docks/HexViewerDock.ui \
    docks/LanguageInspectorDock.ui \
    dialogs/MainWindow.ui \
    dialogs/FindReplaceDialog.ui \
    docks/LuaConsoleDock.ui \
    dialogs/MacroRunDialog.ui \
    dialogs/MacroSaveDialog.ui \
    dialogs/PreferencesDialog.ui \
    docks/SearchResultsDock.ui

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

    mimeicon.path = $$PREFIX/share/icons/hicolor/scalable/mimetypes/
    mimeicon.files = ../../icon/NotepadNext.svg
    INSTALLS += mimeicon

    appicon.path = $$PREFIX/share/icons/hicolor/scalable/apps/
    appicon.files = ../../icon/NotepadNext.svg
    INSTALLS += appicon
}

macx: {
    ICON = ../../icon/NotepadNext.icns
    
    versionAtLeast(QT_VERSION, 6.2) { 
        QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    }
}
