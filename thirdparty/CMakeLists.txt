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

# PCRE2 - regex engine used by Find/Replace (src/QRegexSearch.cpp)
if(APPLE)
    set(NN_PCRE2_JIT OFF) # avoid needing the allow-jit entitlement for hardened runtime
else()
    set(NN_PCRE2_JIT ON)
endif()
CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME pcre2
    URL https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.45/pcre2-10.45.tar.gz
    OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON"
        "PCRE2_BUILD_PCRE2_8 ON"
        "PCRE2_BUILD_PCRE2_16 OFF"
        "PCRE2_BUILD_PCRE2_32 OFF"
        "PCRE2_SUPPORT_UNICODE ON"
        "PCRE2_SUPPORT_JIT ${NN_PCRE2_JIT}"
        "PCRE2_BUILD_PCRE2GREP OFF"
        "PCRE2_BUILD_TESTS OFF"
        "PCRE2_SHOW_REPORT OFF"
        "PCRE2_SUPPORT_LIBZ OFF"
        "PCRE2_SUPPORT_LIBBZ2 OFF"
        "PCRE2_SUPPORT_LIBEDIT OFF"
        "PCRE2_SUPPORT_LIBREADLINE OFF"
)

add_subdirectory(lua EXCLUDE_FROM_ALL)
add_subdirectory(scintilla EXCLUDE_FROM_ALL)
add_subdirectory(lexilla EXCLUDE_FROM_ALL)
add_subdirectory(uchardet EXCLUDE_FROM_ALL)

target_link_libraries(lexilla PRIVATE scintilla)
