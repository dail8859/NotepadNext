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

# Set the paths to an empty string
DEFINES += LUA_PATH_DEFAULT=\\\"\\\"
DEFINES += LUA_CPATH_DEFAULT=\\\"\\\"

HEADERS += \
    $$PWD/lua/src/lapi.h \
    $$PWD/lua/src/lauxlib.h \
    $$PWD/lua/src/lcode.h \
    $$PWD/lua/src/lctype.h \
    $$PWD/lua/src/ldebug.h \
    $$PWD/lua/src/ldo.h \
    $$PWD/lua/src/lfunc.h \
    $$PWD/lua/src/lgc.h \
    $$PWD/lua/src/llex.h \
    $$PWD/lua/src/llimits.h \
    $$PWD/lua/src/lmem.h \
    $$PWD/lua/src/lobject.h \
    $$PWD/lua/src/lopcodes.h \
    $$PWD/lua/src/lparser.h \
    $$PWD/lua/src/lprefix.h \
    $$PWD/lua/src/lstate.h \
    $$PWD/lua/src/lstring.h \
    $$PWD/lua/src/ltable.h \
    $$PWD/lua/src/ltm.h \
    $$PWD/lua/src/lua.h \
    $$PWD/lua/src/luaconf.h \
    $$PWD/lua/src/lualib.h \
    $$PWD/lua/src/lundump.h \
    $$PWD/lua/src/lvm.h \
    $$PWD/lua/src/lzio.h

SOURCES += \
    $$PWD/lua/src/lapi.c \
    $$PWD/lua/src/lauxlib.c \
    $$PWD/lua/src/lbaselib.c \
    $$PWD/lua/src/lbitlib.c \
    $$PWD/lua/src/lcode.c \
    $$PWD/lua/src/lcorolib.c \
    $$PWD/lua/src/lctype.c \
    $$PWD/lua/src/ldblib.c \
    $$PWD/lua/src/ldebug.c \
    $$PWD/lua/src/ldo.c \
    $$PWD/lua/src/ldump.c \
    $$PWD/lua/src/lfunc.c \
    $$PWD/lua/src/lgc.c \
    $$PWD/lua/src/linit.c \
    $$PWD/lua/src/liolib.c \
    $$PWD/lua/src/llex.c \
    $$PWD/lua/src/lmathlib.c \
    $$PWD/lua/src/lmem.c \
    $$PWD/lua/src/loadlib.c \
    $$PWD/lua/src/lobject.c \
    $$PWD/lua/src/lopcodes.c \
    $$PWD/lua/src/loslib.c \
    $$PWD/lua/src/lparser.c \
    $$PWD/lua/src/lstate.c \
    $$PWD/lua/src/lstring.c \
    $$PWD/lua/src/lstrlib.c \
    $$PWD/lua/src/ltable.c \
    $$PWD/lua/src/ltablib.c \
    $$PWD/lua/src/ltm.c \
    $$PWD/lua/src/lundump.c \
    $$PWD/lua/src/lutf8lib.c \
    $$PWD/lua/src/lvm.c \
    $$PWD/lua/src/lzio.c

INCLUDEPATH += $$PWD/lua/src/