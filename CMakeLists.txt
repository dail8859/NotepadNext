cmake_minimum_required(VERSION 3.20)

project(NotepadPlusPlusNext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup singleapplication - we want the GUI application, not console
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

# setup ADS - we don't need to build the examples
set(BUILD_EXAMPLES CACHE BOOL False)
set(BUILD_STATIC CACHE BOOL False)

# setup lua bridge, we want C++ 17 support
set(LUABRIDGE_CXX17 CACHE BOOL true)

include(CPM.cmake)

# note: when using "gl" - you canot link to a branch, only tag/sha1
# note: latest tag is 0.0.7 - way too old to be usable
# note: gitlab requires a user, so I am using the githab mirror
CPMAddPackage("gh:freedesktop/uchardet#23a664560b79715c04cbf73a7e2771e26086b210")

# Using a modern sha1, 2.6 is 2 years old
CPMAddPackage("gh:vinniefalco/LuaBridge#efb62e5be5c12d75e085357072039db9199f7ebf")

CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#3.8.2")
CPMAddPackage("gh:itay-grudev/SingleApplication#v3.3.4")

# this a a repack of the 5.3.4 upstream version with a cmake file
CPMAddPackage("gh:elcuco/lua#v5.4.4-cmake2")

# this a a repack of the upstream#62e0ce7 with a cmake file
CPMAddPackage("gh:elcuco/QSimpleUpdater#3b13dba")

# this a a repack of the upstream#52820d5 with a cmake file (upstream does
# not support building the library, but only unit tests)
CPMAddPackage("gh:elcuco/editorconfig-core-qt#3bf1e539f3b2d2a8cdd19ed55a2c8b4f4a704dea")

# this a a repack of the upstream#rel5-2-2 with a cmake file
#CPMAddPackage("gh:elcuco/scintilla-code#rel-5-2-2-cmake")

# this a a repack of the upstream#rel5-1-6 with a cmake file
CPMAddPackage("gh:elcuco/lexilla#rel-5-1-6-cmake")

# Instead of the two above, you can use a moving branch. Both track
# upstream `master`. Seem to compile and work.
CPMAddPackage("gh:elcuco/scintilla-code#cmake-support")
#CPMAddPackage("gh:elcuco/lexilla#cmake-support")

add_subdirectory(src/NotepadNext)
