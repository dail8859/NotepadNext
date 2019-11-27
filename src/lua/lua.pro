# This file is part of Notepad Next.
# Copyright 2019 Justin Dailey
#
# Notepad Next is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Notepad Next is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.


TARGET = Lua
TEMPLATE = lib
CONFIG += lib_bundle
CONFIG += staticlib
CONFIG -= debug_and_release debug_and_release_target

# Set the paths to an empty string
DEFINES += LUA_PATH_DEFAULT=\\\"\\\"
DEFINES += LUA_CPATH_DEFAULT=\\\"\\\"

HEADERS += \
    src/lapi.h \
    src/lauxlib.h \
    src/lcode.h \
    src/lctype.h \
    src/ldebug.h \
    src/ldo.h \
    src/lfunc.h \
    src/lgc.h \
    src/llex.h \
    src/llimits.h \
    src/lmem.h \
    src/lobject.h \
    src/lopcodes.h \
    src/lparser.h \
    src/lprefix.h \
    src/lstate.h \
    src/lstring.h \
    src/ltable.h \
    src/ltm.h \
    src/lua.h \
    src/luaconf.h \
    src/lualib.h \
    src/lundump.h \
    src/lvm.h \
    src/lzio.h

SOURCES += \
    src/lapi.c \
    src/lauxlib.c \
    src/lbaselib.c \
    src/lbitlib.c \
    src/lcode.c \
    src/lcorolib.c \
    src/lctype.c \
    src/ldblib.c \
    src/ldebug.c \
    src/ldo.c \
    src/ldump.c \
    src/lfunc.c \
    src/lgc.c \
    src/linit.c \
    src/liolib.c \
    src/llex.c \
    src/lmathlib.c \
    src/lmem.c \
    src/loadlib.c \
    src/lobject.c \
    src/lopcodes.c \
    src/loslib.c \
    src/lparser.c \
    src/lstate.c \
    src/lstring.c \
    src/lstrlib.c \
    src/ltable.c \
    src/ltablib.c \
    src/ltm.c \
    src/lundump.c \
    src/lutf8lib.c \
    src/lvm.c \
    src/lzio.c
