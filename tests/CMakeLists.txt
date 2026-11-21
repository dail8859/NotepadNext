set(SCINTILLA_UNIT_DIR "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/test/unit")
set(LEXILLA_UNIT_DIR "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/test/unit")

file(GLOB SCINTILLA_TEST_SOURCES CONFIGURE_DEPENDS
    "${SCINTILLA_UNIT_DIR}/test*.cxx"
)

add_executable(scintilla-unit-tests
    "${SCINTILLA_UNIT_DIR}/unitTest.cxx"
    ${SCINTILLA_TEST_SOURCES}
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/CaseConvert.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/CaseFolder.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/CellBuffer.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/ChangeHistory.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/CharacterCategoryMap.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/CharClassify.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/ContractionState.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/Decoration.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/Document.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/Geometry.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/PerLine.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/RESearch.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/RunStyles.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/Selection.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/UndoHistory.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/UniConversion.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src/UniqueString.cxx"
)
target_compile_definitions(scintilla-unit-tests PRIVATE DO_NOT_USE_WMAIN)
if(MSVC)
    target_compile_options(scintilla-unit-tests PRIVATE /USCI_OWNREGEX)
else()
    target_compile_options(scintilla-unit-tests PRIVATE -USCI_OWNREGEX)
endif()
target_include_directories(scintilla-unit-tests PRIVATE
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/include"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/src"
)
add_test(NAME scintilla-unit COMMAND scintilla-unit-tests)

file(GLOB LEXILLA_TEST_SOURCES CONFIGURE_DEPENDS
    "${LEXILLA_UNIT_DIR}/test*.cxx"
)

add_executable(lexilla-unit-tests
    "${LEXILLA_UNIT_DIR}/unitTest.cxx"
    ${LEXILLA_TEST_SOURCES}
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/Accessor.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/CharacterSet.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/InList.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/LexerBase.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/LexerModule.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/LexerSimple.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/PropSetSimple.cxx"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib/WordList.cxx"
)
target_compile_definitions(lexilla-unit-tests PRIVATE DO_NOT_USE_WMAIN)
target_include_directories(lexilla-unit-tests PRIVATE
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/include"
    "${PROJECT_SOURCE_DIR}/thirdparty/lexilla/lexlib"
    "${PROJECT_SOURCE_DIR}/thirdparty/scintilla/include"
)
add_test(NAME lexilla-unit COMMAND lexilla-unit-tests)

add_subdirectory("${PROJECT_SOURCE_DIR}/thirdparty/uchardet/test" uchardet-test)

if(NOTEPADNEXT_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(scintilla-unit-tests PRIVATE /W4 /WX)
        target_compile_options(lexilla-unit-tests PRIVATE /W4 /WX)
        target_compile_options(uchardet-tests PRIVATE /W4 /WX)
    else()
        target_compile_options(scintilla-unit-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
        target_compile_options(lexilla-unit-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
        target_compile_options(uchardet-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()
