//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge
  
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2007, Nathan Reed

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#pragma once

#include <cassert>

namespace luabridge {

// These are for Lua versions prior to 5.2.0.
//
#if LUA_VERSION_NUM < 502
inline int lua_absindex (lua_State* L, int idx)
{
  if (idx > LUA_REGISTRYINDEX && idx < 0)
    return lua_gettop (L) + idx + 1;
  else
    return idx;
}

inline void lua_rawgetp (lua_State* L, int idx, void const* p)
{
  idx = lua_absindex (L, idx);
  lua_pushlightuserdata (L, const_cast <void*> (p));
  lua_rawget (L,idx);
}

inline void lua_rawsetp (lua_State* L, int idx, void const* p)
{
  idx = lua_absindex (L, idx);
  lua_pushlightuserdata (L, const_cast <void*> (p));
  // put key behind value
  lua_insert (L, -2);
  lua_rawset (L, idx);
}

#define LUA_OPEQ 1
#define LUA_OPLT 2
#define LUA_OPLE 3

inline int lua_compare (lua_State* L, int idx1, int idx2, int op)
{
  switch (op)
  {
  case LUA_OPEQ:
    return lua_equal (L, idx1, idx2);
    break;

  case LUA_OPLT:
    return lua_lessthan (L, idx1, idx2);
    break;

  case LUA_OPLE:
    return lua_equal (L, idx1, idx2) || lua_lessthan (L, idx1, idx2);
    break;

  default:
    return 0;
  };
}

inline int get_length (lua_State* L, int idx)
{
  return int (lua_objlen (L, idx));
}

#else
inline int get_length (lua_State* L, int idx)
{
  lua_len (L, idx);
  int len = int (luaL_checknumber (L, -1));
  lua_pop (L, 1);
  return len;
}

#endif

#ifndef LUA_OK
# define LUABRIDGE_LUA_OK 0
#else
# define LUABRIDGE_LUA_OK LUA_OK
#endif

/** Get a table value, bypassing metamethods.
*/  
inline void rawgetfield (lua_State* L, int index, char const* key)
{
  assert (lua_istable (L, index));
  index = lua_absindex (L, index);
  lua_pushstring (L, key);
  lua_rawget (L, index);
}

/** Set a table value, bypassing metamethods.
*/  
inline void rawsetfield (lua_State* L, int index, char const* key)
{
  assert (lua_istable (L, index));
  index = lua_absindex (L, index);
  lua_pushstring (L, key);
  lua_insert (L, -2);
  lua_rawset (L, index);
}

/** Returns true if the value is a full userdata (not light).
*/
inline bool isfulluserdata (lua_State* L, int index)
{
  return lua_isuserdata (L, index) && !lua_islightuserdata (L, index);
}

/** Test lua_State objects for global equality.

    This can determine if two different lua_State objects really point
    to the same global state, such as when using coroutines.

    @note This is used for assertions.
*/
inline bool equalstates (lua_State* L1, lua_State* L2)
{
  return lua_topointer (L1, LUA_REGISTRYINDEX) ==
         lua_topointer (L2, LUA_REGISTRYINDEX);
}

} // namespace luabridge
