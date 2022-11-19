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
CPMAddPackage("gh:freedesktop/uchardet#143b3fe513bad2cc85903ee564fe55ccbe96d86b")
CPMAddPackage("gh:vinniefalco/LuaBridge#9092ace9615d14e3f5926f2e8a3b612ddc6c8efa")
CPMAddPackage("gh:githubuser0xFFFF/Qt-Advanced-Docking-System#8d30fc9c3c36ac66bf3d027aaad7adff973f628f")
CPMAddPackage("gh:itay-grudev/SingleApplication#f1e15081dc57a9c03f7f4f165677f18802e1437a")
CPMAddPackage("gh:editorconfig/editorconfig-core-qt#b87e96f0e1ca38cdb49fe5a7771df746808bf156")

# the following libraries have no upstream cmake support,
# so we do own own locally (internally they clone the upstream repo)
include(lua.cmake)
include(qsimpleupdater.cmake)
include(lexilla.cmake)

# This is scintilla - which upstream uses mercurial, and as part of the build
# they need python. This fork uses github, and has auto-generated files as
# part of the repo.
#
# rel-5.3.1- this is more or less like upstream 5-3-1... but later on on master,
# b6ab9e51106f7262e77b31c3eb5ca8a4d95c2fc7 - fixes rectangular selection on unix
CPMAddPackage("gh:elcuco/scintilla-code#b6ab9e51106f7262e77b31c3eb5ca8a4d95c2fc7")

add_subdirectory(src/NotepadNext)
