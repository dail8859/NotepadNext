project(lua C)

set(LUA_HEADERS
    src/lapi.h
    src/lauxlib.h
    src/lcode.h
    src/lctype.h
    src/ldebug.h
    src/ldo.h
    src/lfunc.h
    src/lgc.h
    src/llex.h
    src/llimits.h
    src/lmem.h
    src/lobject.h
    src/lopcodes.h
    src/lparser.h
    src/lprefix.h
    src/lstate.h
    src/lstring.h
    src/ltable.h
    src/ltm.h
    src/lua.h
    src/luaconf.h
    src/lualib.h
    src/lundump.h
    src/lvm.h
    src/lzio.h
)

set(LUA_SOURCES
    src/lapi.c
    src/lauxlib.c
    src/lbaselib.c
    src/lbitlib.c
    src/lcode.c
    src/lcorolib.c
    src/lctype.c
    src/ldblib.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/linit.c
    src/liolib.c
    src/llex.c
    src/lmathlib.c
    src/lmem.c
    src/loadlib.c
    src/lobject.c
    src/lopcodes.c
    src/loslib.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/lstrlib.c
    src/ltable.c
    src/ltablib.c
    src/ltm.c
    src/lundump.c
    src/lutf8lib.c
    src/lvm.c
    src/lzio.c
)

add_library(lua STATIC ${LUA_SOURCES} ${LUA_HEADERS})

target_include_directories(lua
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(lua
    PUBLIC
        LUA_PATH_DEFAULT=""
        LUA_CPATH_DEFAULT=""
)

# Optional: Lua 5.3 compatibility (if you want)
# target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3)
