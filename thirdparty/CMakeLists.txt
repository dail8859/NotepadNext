include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME ads
    GITHUB_REPOSITORY githubuser0xFFFF/Qt-Advanced-Docking-System
    GIT_TAG 34b68d6eab1556cf851d24e033909332771f3dfe
    OPTIONS
        "BUILD_STATIC ON"
        "BUILD_EXAMPLES OFF"
)

CPMAddPackage(
    NAME QSimpleUpdater
    GITHUB_REPOSITORY alex-spataru/QSimpleUpdater
    GIT_TAG 8e7017f7fbdc2b4b1a26ed1eef9ebcba6a50639c
    OPTIONS
        "QSIMPLE_UPDATER_BUILD_TESTS OFF"
)

set(QT_DEFAULT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication" FORCE)
CPMAddPackage(
    NAME SingleApplication
    GITHUB_REPOSITORY chen3feng/SingleApplication
    GIT_TAG 1bb301a46c5727fac959ef35a6bc6d05ef3115be
)

CPMAddPackage(
    NAME editorconfig_core_qt
    GITHUB_REPOSITORY editorconfig/editorconfig-core-qt
    GIT_TAG ab62f0554abf2bbe4d45427b36a8b2f81ca7b4ab
)

add_subdirectory(lua)
add_subdirectory(scintilla)
add_subdirectory(lexilla)
add_subdirectory(uchardet)

target_link_libraries(lexilla PRIVATE scintilla)
