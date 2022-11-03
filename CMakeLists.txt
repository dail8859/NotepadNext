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


# uchardet is hosted at gitlab, but CPMAddPackage needs credentials (?)
CPMAddPackage("gh:freedesktop/uchardet#23a664560b79715c04cbf73a7e2771e26086b210")
CPMAddPackage("gh:vinniefalco/LuaBridge#9092ace9615d14e3f5926f2e8a3b612ddc6c8efa")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#22f609cfa67edbda2f40a1a87cd38ac4ab289be7")
CPMAddPackage("gh:itay-grudev/SingleApplication#f1e15081dc57a9c03f7f4f165677f18802e1437a")
CPMAddPackage("gh:editorconfig/editorconfig-core-qt#af4906ab22dc66c48e8d282708ae434de6928e55")

# the following libraries have no upstream cmake support,
# so we do own own locally (internally they clone the upstream repo)
include(lua.cmake)
include(qsimpleupdater.cmake)
include(lexilla.cmake)

# This is scintilla - which upstream uses mercurial, and as part of the build
# they need python. This fork uses github, and has auto-generated files as
# part of the repo.
CPMAddPackage("gh:elcuco/scintilla-code#acea5cf96cf139c05f03c752a19bb76810715f9b")

add_subdirectory(src/NotepadNext)
