project(ScintillaQt CXX)

set(SCINTILLA_SOURCES
    qt/ScintillaEdit/ScintillaEdit.cpp
    qt/ScintillaEdit/ScintillaDocument.cpp
    qt/ScintillaEditBase/PlatQt.cpp
    qt/ScintillaEditBase/ScintillaQt.cpp
    qt/ScintillaEditBase/ScintillaEditBase.cpp
    src/XPM.cxx
    src/ViewStyle.cxx
    src/UniqueString.cxx
    src/UniConversion.cxx
    src/Style.cxx
    src/Selection.cxx
    src/ScintillaBase.cxx
    src/RunStyles.cxx
    src/RESearch.cxx
    src/PositionCache.cxx
    src/PerLine.cxx
    src/MarginView.cxx
    src/LineMarker.cxx
    src/KeyMap.cxx
    src/Indicator.cxx
    src/Geometry.cxx
    src/EditView.cxx
    src/Editor.cxx
    src/EditModel.cxx
    src/Document.cxx
    src/Decoration.cxx
    src/DBCS.cxx
    src/ContractionState.cxx
    src/CharClassify.cxx
    src/CharacterType.cxx
    src/CharacterCategoryMap.cxx
    src/CellBuffer.cxx
    src/CaseFolder.cxx
    src/CaseConvert.cxx
    src/CallTip.cxx
    src/AutoComplete.cxx
    src/ChangeHistory.cxx
    src/UndoHistory.cxx
)

set(SCINTILLA_HEADERS
    qt/ScintillaEdit/ScintillaEdit.h
    qt/ScintillaEdit/ScintillaDocument.h
    qt/ScintillaEditBase/ScintillaEditBase.h
    qt/ScintillaEditBase/ScintillaQt.h
)

add_library(scintilla STATIC ${SCINTILLA_SOURCES} ${SCINTILLA_HEADERS})

target_include_directories(scintilla
    PUBLIC
        qt/ScintillaEditBase
        qt/ScintillaEdit
        include
        src
)

target_compile_definitions(scintilla
    PUBLIC
        SCINTILLA_QT=1
        MAKING_LIBRARY=1
        SCI_LEXER=1
        _CRT_SECURE_NO_DEPRECATE=1
        EXPORT_IMPORT_API=
)

# Add NDEBUG for release builds
target_compile_definitions(scintilla
    PUBLIC
        $<$<CONFIG:Release>:NDEBUG=1>
)

find_package(Qt6 COMPONENTS Core Widgets Core5Compat REQUIRED)
target_link_libraries(scintilla
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Core5Compat
)

if(APPLE)
    set_target_properties(scintilla PROPERTIES
        INSTALL_NAME_DIR "@executable_path/../Frameworks"
    )
endif()
