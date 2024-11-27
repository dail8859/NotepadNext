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

# SHA1 bd983ca108a1c493465270702c02bbbe792ea0ed
#    When building in debug mode, ucharder requests address santizer, it's not needed by us
#    Solution add this in the CPMAddPackage command
#    OPTIONS "CMAKE_BUILD_TYPE Release"
# SHA1 8118133e0017c4e1b3ddc9fad104c0ba19692cd7
#    Introduced something that breaks on MSVC
# SHA1 15fc8f0a0f55d6ec9373cf16ed96ebf6b35feef3
#    Last working version
CPMAddPackage(NAME uchardet
    GIT_REPOSITORY https://gitlab.freedesktop.org/uchardet/uchardet
    GIT_TAG 15fc8f0a0f55d6ec9373cf16ed96ebf6b35feef3
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
# CPMAddPackage("gh:diegoiast/lexilla#7de1d32b62dfb60aca9ad3b4a194f6333f044f60")
include(lexilla.cmake)

add_subdirectory(src/NotepadNext)
