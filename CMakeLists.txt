cmake_minimum_required(VERSION 3.20)

project(NotepadPlusPlusNext)


# Setup singleapplication - we want the GUI application, not console
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

# setup ADS - we don't need to build the examples
set(BUILD_EXAMPLES CACHE BOOL False)
set(BUILD_STATIC CACHE BOOL False)

# seetup lua bridge, we want C++ 17 support
set(LUABRIDGE_CXX17 CACHE BOOL true)

include(CPM.cmake)

CPMAddPackage("gh:vinniefalco/LuaBridge#2.6")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#3.8.2")
CPMAddPackage("gh:itay-grudev/SingleApplication#v3.3.2")

# TODO: why can't I use gitlab, which seems to tbe the real upstream?
# TODO: pin a specific tag
CPMAddPackage("gh:freedesktop/uchardet#master")

#add_subdirectory(src/lua)
# this a a repack of the 5.3.4 upstream version with a cmake file
CPMAddPackage("gh:elcuco/lua#v5.3.4-cmake")

# this a a repack of the upstream#62e0ce7 with a cmake file
CPMAddPackage("gh:elcuco/QSimpleUpdater#3b13dba")

# this a a repack of the upstream#52820d5 with a cmake file (upstream does
# not support building the library, but only unit tests)
CPMAddPackage("gh:elcuco/editorconfig-core-qt#3bf1e539f3b2d2a8cdd19ed55a2c8b4f4a704dea")

# this a a repack of the upstream#rel5-2-0 with a cmake file
CPMAddPackage("gh:elcuco/scintilla-code#5cd77e59")
CPMAddPackage("gh:elcuco/lexilla#15fd70899bdcbd788bc9131d40ff085ad9bffa8d")

add_subdirectory(src/NotepadNext)

