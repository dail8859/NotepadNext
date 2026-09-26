find_package(Qt6 COMPONENTS Test QUIET)

if(NOT TARGET Qt6::Test)
    message(STATUS "Qt6::Test not found, skipping tests")
    return()
endif()

qt_add_executable(tst_QRegexSearch
    tst_QRegexSearch.cpp
    ${CMAKE_SOURCE_DIR}/src/QRegexSearch.cpp
)

target_include_directories(tst_QRegexSearch
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tst_QRegexSearch
    PRIVATE
        Qt6::Core
        Qt6::Test
        scintilla
)

add_test(NAME tst_QRegexSearch COMMAND tst_QRegexSearch)
