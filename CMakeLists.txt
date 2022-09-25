cmake_minimum_required(VERSION 3.20)

project(NotepadPlusPlusNext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# uchardet setup
set(BUILD_BINARY CACHE BOOL OFF)
set(BUILD_STATICCACHE CACHE BOOL OFF)
set(BUILD_SHARED_LIBS CACHE BOOL OFF)

# setup singleapplication - we want the GUI application, not console
set(QAPPLICATION_CLASS QApplication)

# setup ADS - we don't need to build the examples
set(BUILD_EXAMPLES CACHE BOOL OFF)
set(BUILD_STATIC CACHE BOOL ON)
set(BUILD_CACHE CACHE BOOL ON)
set(USE_BUILD_SHARED_LIBS CACHE BOOL OFF)

# setup lua bridge, we want C++ 17 support
set(LUABRIDGE_CXX17 CACHE BOOL true)

# SingleApplication needs this - otherwise it gets confused with Qt5
set(QT_DEFAULT_MAJOR_VERSION 6)


include(CPM.cmake)

# note: when using "gl" - you canot link to a branch, only tag/sha1
# note: latest tag is 0.0.7 - way too old to be usable
# note: gitlab requires a user, so I am using the githab mirror
CPMAddPackage("gh:freedesktop/uchardet#23a664560b79715c04cbf73a7e2771e26086b210")

# Using a modern sha1, 2.6 is 2 years old
CPMAddPackage("gh:vinniefalco/LuaBridge#efb62e5be5c12d75e085357072039db9199f7ebf")

CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#3.8.3")

CPMAddPackage("gh:itay-grudev/SingleApplication#v3.3.4")

# this a a repack of the 8426d9b4d4df1da3c5b2d759e509ae1c50a86667 upstream version with a cmake file
#CPMAddPackage("gh:elcuco/lua#9f06e8ad5b562d71c10fb38e6213c2a07465191a")
include(lua.cmake)

# this a a repack of the upstream#52820d5 with a cmake file (upstream does
# not support building the library, but only unit tests)
CPMAddPackage("gh:elcuco/editorconfig-core-qt#6ef2b2db29dad565e43002b64dfbaaf8a6687bc4")

# this a a repack of the upstream#62e0ce7 with a cmake file
CPMAddPackage("gh:elcuco/QSimpleUpdater#dfbbd8db78cf3c40df5fb3ee9b5a75cd50cae185")

# this a a repack of the upstream#f85a8baec6f401d481341ead87f39d9fe9d284ec with a cmake file
CPMAddPackage("gh:elcuco/scintilla-code#923fac200dd228256ca263361c974f78402d2461")

# TODO - should we backport this to a stable release?
# this a a repack of the upstream#0bd13d84b3fef7a72553ab95c43d0d4c9677f70f with a cmake file
CPMAddPackage("gh:elcuco/lexilla#270fb410dd7518736141c9c9c84880b938d957a7")

# Instead of all the above, you can use a moving branch. All track
# upstream `master`.
#CPMAddPackage("gh:elcuco/editorconfig-core-qt#cmake-support")
#CPMAddPackage("gh:elcuco/lua#9f06e8ad5b562d71c10fb38e6213c2a07465191a")
#CPMAddPackage("gh:elcuco/QSimpleUpdater#cmake-support")
#CPMAddPackage("gh:elcuco/scintilla-code#cmake-support")
#CPMAddPackage("gh:elcuco/lexilla#cmake-support")
#CPMAddPackage("gh:elcuco/scintilla-code#cmake-support")

add_subdirectory(src/NotepadNext)
