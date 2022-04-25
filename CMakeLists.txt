cmake_minimum_required(VERSION 3.20)

project(NotepadPlusPlusNext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# uchardet setup
set(BUILD_BINARY OFF)
set(USE_BUILD_SHARED_LIBS CACHE BOOL OFF)
set(BUILD_STATICCACHE OFF)

# setup singleapplication - we want the GUI application, not console
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

# setup ADS - we don't need to build the examples
set(BUILD_EXAMPLES OFF)
set(BUILD_STATIC ON)
set(BUILD_CACHE ON)
set(USE_BUILD_SHARED_LIBS OFF)

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

# this a a repack of the 8426d9b4d4df1da3c5b2d759e509ae1c50a86667 upstream version with a cmake file
CPMAddPackage("gh:elcuco/lua#9f06e8ad5b562d71c10fb38e6213c2a07465191a")
#include(lua.cmake)

# this a a repack of the upstream#52820d5 with a cmake file (upstream does
# not support building the library, but only unit tests)
CPMAddPackage("gh:elcuco/editorconfig-core-qt#ffd7d12657fb543a480b5c924cd450c31d107c72")

# this a a repack of the upstream#62e0ce7 with a cmake file
CPMAddPackage("gh:elcuco/QSimpleUpdater#3407c2d257cc2c1758f864df7aa8657addc5e73f")

# this a a repack of the upstream#f85a8baec6f401d481341ead87f39d9fe9d284ec with a cmake file
CPMAddPackage("gh:elcuco/scintilla-code#27dca0231d437db3c1c1aedf8afd243d1854ee6c")

# TODO - should we backport this to a stable release?
# this a a repack of the upstream#0bd13d84b3fef7a72553ab95c43d0d4c9677f70f with a cmake file
CPMAddPackage("gh:elcuco/lexilla#270fb410dd7518736141c9c9c84880b938d957a7")

# Instead of the two above, you can use a moving branch. Both track
# upstream `master`. Seem to compile and work.
#CPMAddPackage("gh:elcuco/scintilla-code#cmake-support")
#CPMAddPackage("gh:elcuco/lexilla#cmake-support")

add_subdirectory(src/NotepadNext)
