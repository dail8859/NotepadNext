cmake_minimum_required(VERSION 3.20)

project(NotepadNext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(CPM.cmake)

# setup SingleApplication
set(QAPPLICATION_CLASS QApplication)
set(QT_DEFAULT_MAJOR_VERSION 6)

# setup QDS
set(BUILD_EXAMPLES OFF)

# setup uchardet
set(BUILD_BINARY OFF)
set(BUILD_STATICCACHE OFF)
set(BUILD_SHARED_LIBS OFF)


# an commit after v0.0.8
CPMAddPackage(NAME uchardet
    GIT_REPOSITORY https://gitlab.freedesktop.org/uchardet/uchardet
    GIT_TAG 2f5c24006ebc7f005040358f58f22a61a3c92522)
CPMAddPackage("gh:vinniefalco/LuaBridge#5d21e35633a1f87ed08af115b07d3386096f792b")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#4.1.1")
CPMAddPackage("gh:itay-grudev/SingleApplication#v3.5.1")
CPMAddPackage("gh:editorconfig/editorconfig-core-qt#bf86460fe29b53731bba650d2a59d4c47eb25de9")

# the following libraries have no upstream cmake support,
# so we do own own locally (internally they clone the upstream repo)
include(lua.cmake)
include(qsimpleupdater.cmake)

# This is scintilla - which upstream uses mercurial, and as part of the build
# they need python. This fork uses github, and has auto-generated files as
# part of the repo.
#
CPMAddPackage("gh:elcuco/scintilla-code#rel-5-3-3-cmake")
#CPMAddPackage("gh:elcuco/lexilla#rel-5-2-1-cmake")
include(lexilla.cmake)

add_subdirectory(src/NotepadNext)
