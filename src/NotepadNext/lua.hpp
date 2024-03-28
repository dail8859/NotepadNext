// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// This got removed in LUA v5.4.4
#ifndef LUA_QL
#define LUA_QL(x)	"'" x "'"
#endif
