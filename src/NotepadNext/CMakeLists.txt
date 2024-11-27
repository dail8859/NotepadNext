set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport )
if (Qt6_FOUND)
    find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_compile_definitions(APP_DISTRIBUTION="local")

add_executable(NotepadNext
    ApplicationSettings.cpp
    ColorPickerDelegate.cpp
    ColorPickerDelegate.h
    ComboBoxDelegate.cpp
    ComboBoxDelegate.h
    Converter.cpp
    Converter.h
    DockedEditor.cpp
    DockedEditor.h
    DebugManager.cpp
    DockedEditorTitleBar.h
    EditorHexViewerTableModel.cpp
    EditorManager.h
    EditorManager.cpp
    EditorPrintPreviewRenderer.h
    EditorPrintPreviewRenderer.cpp
    FadingIndicator.cpp
    Finder.cpp
    Finder.h
    FileDialogHelpers.cpp
    FileDialogHelpers.h
    FocusWatcher.h
    HtmlConverter.cpp
    HtmlConverter.h
    IFaceTable.cpp
    IFaceTable.h
    IFaceTableMixer.cpp
    IFaceTableMixer.h
    ISearchResultsHandler.h
    LanguageStylesModel.cpp
    LanguageStylesModel.h
    LuaExtension.cpp
    LuaExtension.h
    LuaState.cpp
    LuaState.h
    main.cpp
    Macro.cpp
    MacroManager.cpp
    MacroRecorder.cpp
    MacroRecorder.h
    MacroStep.cpp
    MacroStepTableModel.cpp
    MacroListModel.cpp
    NotepadNextApplication.cpp
    NotepadNextApplication.h
    NppImporter.cpp
    NppImporter.h
    QRegexSearch.cpp
    QRegexSearch.h
    QuickFindWidget.h
    QuickFindWidget.cpp
    QuickFindWidget.ui
    RangeAllocator.h
    RangeAllocator.cpp
    RecentFilesListManager.cpp
    RecentFilesListManager.h
    RecentFilesListMenuBuilder.cpp
    RecentFilesListMenuBuilder.h
    RtfConverter.cpp
    RtfConverter.h
    resources.qrc
    SciIFaceTable.cpp
    SearchResultsCollector.h
    SearchResultsCollector.cpp
    ScintillaNext.cpp
    ScintillaNext.h
    SciIFaceTable.h
    ScintillaCommenter.h
    ScintillaCommenter.cpp
    SelectionTracker.h
    SessionManager.h
    SessionManager.cpp
    SelectionTracker.cpp
    SpinBoxDelegate.h
    SpinBoxDelegate.cpp
    scripts.qrc
    TranslationManager.cpp
    UndoAction.h
    UndoAction.cpp
    ZoomEventWatcher.h
    ZoomEventWatcher.cpp

    decorators/ApplicationDecorator.cpp
    decorators/ApplicationDecorator.h
    decorators/AutoCompletion.h
    decorators/AutoIndentation.h
    decorators/AutoCompletion.cpp
    decorators/AutoIndentation.cpp
    decorators/BetterMultiSelection.h
    decorators/BetterMultiSelection.cpp
    decorators/BookMarkDecorator.h
    decorators/BookMarkDecorator.cpp
    decorators/BraceMatch.h
    decorators/BraceMatch.cpp
    decorators/EditorConfigAppDecorator.cpp
    decorators/EditorConfigAppDecorator.h
    decorators/EditorDecorator.cpp
    decorators/EditorDecorator.h
    decorators/LineNumbers.h
    decorators/LineNumbers.cpp
    decorators/HighlightedScrollBar.cpp
    decorators/HighlightedScrollBar.h
    decorators/SmartHighlighter.h
    decorators/SurroundSelection.h
    decorators/SurroundSelection.cpp
    decorators/SmartHighlighter.cpp
    decorators/URLFinder.cpp
    decorators/URLFinder.h

    dialogs/ColumnEditorDialog.h
    dialogs/ColumnEditorDialog.ui
    dialogs/ColumnEditorDialog.cpp
    dialogs/FindReplaceDialog.cpp
    dialogs/FindReplaceDialog.h
    dialogs/FindReplaceDialog.ui
    dialogs/MacroEditorDialog.cpp
    dialogs/MacroEditorDialog.h
    dialogs/MacroEditorDialog.ui
    dialogs/MacroRunDialog.cpp
    dialogs/MacroRunDialog.h
    dialogs/MacroRunDialog.ui
    dialogs/MacroSaveDialog.cpp
    dialogs/MacroSaveDialog.h
    dialogs/MacroSaveDialog.ui
    dialogs/MainWindow.cpp
    dialogs/MainWindow.h
    dialogs/MainWindow.ui
    dialogs/PreferencesDialog.cpp
    dialogs/PreferencesDialog.h
    dialogs/PreferencesDialog.ui

    docks/DebugLogDock.cpp
    docks/EditorInspectorDock.cpp
    docks/EditorInspectorDock.h
    docks/EditorInspectorDock.ui
    docks/FileListDock.cpp
    docks/FileListDock.h
    docks/FileListDock.ui
    docks/FolderAsWorkspaceDock.cpp
    docks/FolderAsWorkspaceDock.h
    docks/FolderAsWorkspaceDock.ui
    docks/LanguageInspectorDock.cpp
    docks/LanguageInspectorDock.h
    docks/LanguageInspectorDock.ui
    docks/LuaConsoleDock.cpp
    docks/LuaConsoleDock.ui
    docks/LuaConsoleDock.h
    docks/HexViewerDock.cpp
    docks/SearchResultsDock.cpp
    docks/SearchResultsDock.h
    docks/SearchResultsDock.ui

    widgets/EditorInfoStatusBar.cpp
    widgets/EditorInfoStatusBar.h
    widgets/StatusLabel.cpp
    widgets/StatusLabel.h
)

target_link_libraries(NotepadNext Qt::Widgets Qt::PrintSupport
    libuchardet
    lua
    LuaBridge
    qt6advanceddocking
    SingleApplication
    QSimpleUpdater
    editorconfig-core-qt
    scintilla-qt-edit
    lexilla
)

if (Qt6_FOUND)
    target_link_libraries(NotepadNext Qt6::Core5Compat)
endif()

target_include_directories(NotepadNext PRIVATE decorators dialogs docks widgets src)
target_compile_definitions(NotepadNext PRIVATE APP_VERSION="0.5.6")
target_compile_definitions(NotepadNext PRIVATE APP_COPYRIGHT="Copyright 2019-2022 Justin Dailey")
target_compile_definitions(NotepadNext PRIVATE CMAKE_EXPERIMENTAL)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(NotepadNext PRIVATE -Wall -Wextra -pedantic --warn-no-unused-variable -Wformat -Wformat-security)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(NotepadNext PRIVATE -Wall -Wextra -pedantic --warn-no-unused-variable -Wformat -Wformat-security)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    MESSAGE("Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(NotepadNext PRIVATE /utf-8 /guard:cf)
    target_link_options(NotepadNext PRIVATE /guard:cf)
endif()

# For some reason - this library slips the linking part.
if(UNIX AND NOT APPLE)
	target_link_libraries(NotepadNext xcb)
endif()

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
    set(MACOSX_BUNDLE_BUNDLE_NAME "NotepadNext")
#    set(MACOSX_BUNDLE_BUNDLE_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
#    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
#    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION})
    set(MACOSX_BUNDLE_ICON_FILE ../icon/NotepadNext.icns)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.dail8859.NotepadNext")
endif()

include(GNUInstallDirs)
install(TARGETS NotepadNext)
install(FILES ${CMAKE_SOURCE_DIR}/deploy/linux/NotepadNext.desktop
    DESTINATION share/applications)
install(FILES ${CMAKE_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes)
install(FILES ${CMAKE_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/apps/)
