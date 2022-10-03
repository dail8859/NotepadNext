cmake_minimum_required(VERSION 3.20)

project(NotepadNext)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup SingleApplication
set(QAPPLICATION_CLASS QApplication)
set(QT_DEFAULT_MAJOR_VERSION 6)

include(CPM.cmake)

# uchardet is hosted at gitlab, but CPMAddPackage needs crednetials (?)
CPMAddPackage("gh:freedesktop/uchardet#143b3fe513bad2cc85903ee564fe55ccbe96d86b")
CPMAddPackage("gh:vinniefalco/LuaBridge#9092ace9615d14e3f5926f2e8a3b612ddc6c8efa")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#3cd6d766f8ddb0dc306e7e7fa5b3e406f84b56c6")
CPMAddPackage("gh:itay-grudev/SingleApplication#f1e15081dc57a9c03f7f4f165677f18802e1437a")
CPMAddPackage("gh:editorconfig/editorconfig-core-qt#af4906ab22dc66c48e8d282708ae434de6928e55")

# the following libraries have no upstream cmake support,
# so we do own own locally (internally they clone the upstream repo)
include(lua.cmake)
include(qsimpleupdater.cmake)
include(lexilla.cmake)

# This is scintilla - which upstream uses mercurial, and as part of the build
# they need python. This fork uses github, and has auto-generated files as
# part of the repo. Apparently - this fork cannot be
CPMAddPackage("gh:elcuco/scintilla-code#c2ce7fd307466c4fc79355d06c445fd7fc60d339")

add_subdirectory(src/NotepadNext)
