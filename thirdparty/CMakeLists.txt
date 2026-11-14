include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME ads
    GITHUB_REPOSITORY githubuser0xFFFF/Qt-Advanced-Docking-System
    GIT_TAG v5.1.1
    OPTIONS
        "BUILD_STATIC ON"
        "BUILD_EXAMPLES OFF"
)

CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME QSimpleUpdater
    GITHUB_REPOSITORY alex-spataru/QSimpleUpdater
    GIT_TAG c5c136c0f79ca6423b0914a2676ae3d0e3811c5b
    OPTIONS
        "QSIMPLE_UPDATER_BUILD_TESTS OFF"
)

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication" FORCE)
CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME SingleApplication
    GITHUB_REPOSITORY itay-grudev/SingleApplication
    GIT_TAG v3.5.6
)

CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME editorconfig_core_qt
    GITHUB_REPOSITORY editorconfig/editorconfig-core-qt
    GIT_TAG ab62f0554abf2bbe4d45427b36a8b2f81ca7b4ab
)

add_subdirectory(lua EXCLUDE_FROM_ALL)
add_subdirectory(scintilla EXCLUDE_FROM_ALL)
add_subdirectory(lexilla EXCLUDE_FROM_ALL)
add_subdirectory(uchardet EXCLUDE_FROM_ALL)

target_link_libraries(lexilla PRIVATE scintilla)
