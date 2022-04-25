set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(NotepadNext
    ColorPickerDelegate.cpp
    ComboBoxDelegate.cpp
    DockedEditor.cpp
    EditorManager.cpp
    EditorPrintPreviewRenderer.cpp
    Finder.cpp
    IFaceTable.cpp
    IFaceTableMixer.cpp
    LanguageKeywordsModel.cpp
    LanguagePropertiesModel.cpp
    LanguageStylesModel.cpp
    LuaExtension.cpp
    LuaState.cpp
    MacroRecorder.cpp
    NotepadNextApplication.cpp
    NppImporter.cpp
    QRegexSearch.cpp
    QuickFindWidget.cpp
    RecentFilesListManager.cpp
    SciIFaceTable.cpp
    ScintillaNext.cpp
    Settings.cpp
    SpinBoxDelegate.cpp
    decorators/ApplicationDecorator.cpp
    decorators/AutoCompletion.cpp
    decorators/AutoIndentation.cpp
    decorators/BetterMultiSelection.cpp
    decorators/EditorConfigAppDecorator.cpp
    decorators/SurroundSelection.cpp
    docks/EditorInspectorDock.cpp
    dialogs/FindReplaceDialog.cpp
    docks/FolderAsWorkspaceDock.cpp
    docks/LanguageInspectorDock.cpp
    docks/LuaConsoleDock.cpp
    dialogs/MacroRunDialog.cpp
    dialogs/MacroSaveDialog.cpp
    dialogs/MainWindow.cpp
    dialogs/PreferencesDialog.cpp
    main.cpp
    decorators/BraceMatch.cpp
    decorators/EditorDecorator.cpp
    decorators/HighlightedScrollBar.cpp
    decorators/LineNumbers.cpp
    decorators/SmartHighlighter.cpp
    widgets/EditorInfoStatusBar.cpp
    widgets/StatusLabel.cpp

    ColorPickerDelegate.h
    ComboBoxDelegate.h
    DockedEditor.h
    DockedEditorTitleBar.h
    EditorManager.h
    EditorPrintPreviewRenderer.h
    Finder.h
    FocusWatcher.h
    IFaceTable.h
    IFaceTableMixer.h
    LanguageKeywordsModel.h
    LanguagePropertiesModel.h
    LanguageStylesModel.h
    LuaExtension.h
    LuaState.h
    MacroRecorder.h
    NotepadNextApplication.h
    NppImporter.h
    QRegexSearch.h
    QuickFindWidget.h
    RecentFilesListManager.h
    SciIFaceTable.h
    ScintillaNext.h
    Settings.h
    SpinBoxDelegate.h
    decorators/ApplicationDecorator.h
    decorators/AutoCompletion.h
    decorators/AutoIndentation.h
    decorators/BetterMultiSelection.h
    decorators/EditorConfigAppDecorator.h
    decorators/SurroundSelection.h
    docks/EditorInspectorDock.h
    dialogs/FindReplaceDialog.h
    docks/FolderAsWorkspaceDock.h
    docks/LanguageInspectorDock.h
    docks/LuaConsoleDock.h
    dialogs/MacroRunDialog.h
    dialogs/MacroSaveDialog.h
    dialogs/MainWindow.h
    dialogs/PreferencesDialog.h
    decorators/BraceMatch.h
    decorators/EditorDecorator.h
    decorators/HighlightedScrollBar.h
    decorators/LineNumbers.h
    decorators/SmartHighlighter.h
    widgets/EditorInfoStatusBar.h
    widgets/StatusLabel.h

    QuickFindWidget.ui
    docks/EditorInspectorDock.ui
    docks/FolderAsWorkspaceDock.ui
    docks/LanguageInspectorDock.ui
    dialogs/MainWindow.ui
    dialogs/FindReplaceDialog.ui
    docks/LuaConsoleDock.ui
    dialogs/MacroRunDialog.ui
    dialogs/MacroSaveDialog.ui
    dialogs/PreferencesDialog.ui
    resources.qrc
    scripts.qrc
)

target_link_libraries(NotepadNext Qt5::Widgets Qt5::PrintSupport
    libuchardet_static
    lua
    LuaBridge
    qtadvanceddocking
    SingleApplication
    QSimpleUpdater
    editorconfig-core-qt
    scintilla-qt-edit
    lexzilla
)
target_include_directories(NotepadNext PRIVATE decorators dialogs docks widgets src)
target_compile_definitions(NotepadNext PRIVATE APP_VERSION="0.5.0")
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

include(GNUInstallDirs)
install(TARGETS NotepadNext)
install(FILES ../../deploy/linux/NotepadNext.desktop
    DESTINATION share/applications)
install(FILES ../../icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes)

