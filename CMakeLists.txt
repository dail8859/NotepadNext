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

# When building in debug mode, ucharder requests address santizer, it's not needed by us
CPMAddPackage(NAME uchardet
    GIT_REPOSITORY https://gitlab.freedesktop.org/uchardet/uchardet
    GIT_TAG edae8e81cfb8092496f94da1a306c4c9f0ce32bb
    OPTIONS "CMAKE_BUILD_TYPE Release"
)
CPMAddPackage("gh:vinniefalco/LuaBridge#5d21e35633a1f87ed08af115b07d3386096f792b")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#4.3.1")
CPMAddPackage("gh:itay-grudev/SingleApplication#v3.5.1")
CPMAddPackage("gh:editorconfig/editorconfig-core-qt#ee967262db4fdbd735f9971cc0c90cf4f3100d3a")
CPMAddPackage("gh:alex-spataru/QSimpleUpdater#911727370d90001077f93ea94545099e197400a0")

# the following libraries have no upstream cmake support,
# so we do own own locally (internally they clone the upstream repo)
include(lua.cmake)

# This is scintilla - which upstream uses mercurial, and as part of the build
# they need python. This fork uses github, and has auto-generated files as
# part of the repo.
#
CPMAddPackage(
  NAME scintilla-code
  GITHUB_REPOSITORY diegoiast/scintilla-code
  GIT_TAG rel-5-5-3-cmake2
  GIT_SHALLOW ON
)
CPMAddPackage("gh:diegoiast/lexilla#7de1d32b62dfb60aca9ad3b4a194f6333f044f60")
# include(lexilla.cmake)

add_subdirectory(src/NotepadNext)
