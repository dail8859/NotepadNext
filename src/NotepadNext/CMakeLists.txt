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
    ColorPickerDelegate.cpp
    ColorPickerDelegate.h
    ComboBoxDelegate.cpp
    ComboBoxDelegate.h
    DockedEditor.h
    DockedEditor.cpp
    DebugManager.cpp
    DockedEditorTitleBar.h
    EditorHexViewerTableModel.cpp
    EditorManager.h
    EditorManager.cpp
    EditorPrintPreviewRenderer.h
    EditorPrintPreviewRenderer.cpp
    Finder.cpp
    Finder.h
    FocusWatcher.h
    IFaceTable.cpp
    IFaceTable.h
    IFaceTableMixer.cpp
    IFaceTableMixer.h
    LanguageKeywordsModel.h
    LanguageKeywordsModel.cpp
    LanguagePropertiesModel.cpp
    LanguagePropertiesModel.h
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
    RecentFilesListManager.cpp
    RecentFilesListManager.h
    RecentFilesListMenuBuilder.cpp
    RecentFilesListMenuBuilder.h
    resources.qrc
    SciIFaceTable.cpp
    ScintillaNext.cpp
    ScintillaNext.h
    SciIFaceTable.h
    ScintillaCommenter.h
    ScintillaCommenter.cpp
    SelectionTracker.h
    Settings.cpp
    Settings.h
    SelectionTracker.cpp
    SpinBoxDelegate.h
    SpinBoxDelegate.cpp
    scripts.qrc
    UndoAction.h
    UndoAction.cpp

    decorators/ApplicationDecorator.cpp
    decorators/AutoCompletion.cpp
    decorators/AutoIndentation.cpp
    decorators/BetterMultiSelection.cpp
    decorators/EditorConfigAppDecorator.cpp
    decorators/SurroundSelection.cpp
    decorators/BraceMatch.cpp
    decorators/EditorDecorator.cpp
    decorators/HighlightedScrollBar.cpp
    decorators/LineNumbers.cpp
    decorators/SmartHighlighter.cpp
    decorators/ApplicationDecorator.h
    decorators/AutoCompletion.h
    decorators/AutoIndentation.h
    decorators/BetterMultiSelection.h
    decorators/EditorConfigAppDecorator.h
    decorators/SurroundSelection.h
    decorators/BraceMatch.h
    decorators/EditorDecorator.h
    decorators/HighlightedScrollBar.h
    decorators/LineNumbers.h
    decorators/SmartHighlighter.h

    dialogs/FindReplaceDialog.cpp
    dialogs/MacroEditorDialog.cpp
    dialogs/MacroRunDialog.cpp
    dialogs/MacroSaveDialog.cpp
    dialogs/MainWindow.cpp
    dialogs/PreferencesDialog.cpp
    dialogs/FindReplaceDialog.h
    dialogs/MacroRunDialog.h
    dialogs/MacroSaveDialog.h
    dialogs/MainWindow.h
    dialogs/PreferencesDialog.h
    dialogs/MainWindow.ui
    dialogs/FindReplaceDialog.ui
    dialogs/MacroEditorDialog.ui
    dialogs/MacroRunDialog.ui
    dialogs/MacroSaveDialog.ui
    dialogs/PreferencesDialog.ui

    docks/DebugLogDock.cpp
    docks/EditorInspectorDock.h
    docks/EditorInspectorDock.cpp
    docks/EditorInspectorDock.ui
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
    qtadvanceddocking
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
target_compile_definitions(NotepadNext PRIVATE APP_VERSION="0.5.5")
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

