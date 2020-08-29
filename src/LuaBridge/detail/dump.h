//==============================================================================
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2019, Dmitry Tarakanov
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

#include "LuaBridge/detail/ClassInfo.h"

#include <iostream>
#include <string>


namespace luabridge {
namespace debug {

inline void putIndent (std::ostream& stream, unsigned level)
{
  for (unsigned i = 0; i < level; ++i)
  {
    stream << "  ";
  }
}

inline void dumpTable (lua_State* L, int index, std::ostream& stream, unsigned level);

inline void dumpValue (lua_State* L, int index, std::ostream& stream, unsigned level = 0)
{
  const int type = lua_type (L, index);
  switch (type)
  {
  case LUA_TNIL:
    stream << "nil";
    break;

  case LUA_TBOOLEAN:
    stream << (lua_toboolean (L, index) ? "true" : "false");
    break;

  case LUA_TNUMBER:
    stream << lua_tonumber (L, index);
    break;

  case LUA_TSTRING:
    stream << '"' << lua_tostring (L, index) << '"';
    break;

  case LUA_TFUNCTION:
    if (lua_iscfunction (L, index))
    {
      stream << "cfunction@" << lua_topointer (L, index);
    }
    else
    {
      stream << "function@" << lua_topointer (L, index);
    }
    break;

  case LUA_TTHREAD:
    stream << "thread@" << lua_tothread (L, index);
    break;

  case LUA_TLIGHTUSERDATA:
    stream << "lightuserdata@" << lua_touserdata (L, index);
    break;

  case LUA_TTABLE:
    dumpTable (L, index, stream, level);
    break;

  case LUA_TUSERDATA:
    stream << "userdata@" << lua_touserdata (L, index);
    break;

  default:
    stream << lua_typename (L, type);;
    break;
  }
}

inline void dumpTable (lua_State* L, int index, std::ostream& stream, unsigned level)
{
  stream << "table@" << lua_topointer (L, index);

  if (level > 0)
  {
    return;
  }

  index = lua_absindex (L, index);
  stream << " {";
  lua_pushnil (L); // Initial key
  while (lua_next (L, index))
  {
    stream << "\n";
    putIndent (stream, level + 1);
    dumpValue (L, -2, stream, level + 1); // Key
    stream << ": ";
    dumpValue (L, -1, stream, level + 1); // Value
    lua_pop (L, 1); // Value
  }
  putIndent (stream, level);
  stream << "\n}";
}

inline void dumpState (lua_State *L, std::ostream& stream = std::cerr)
{
  int top = lua_gettop (L);
  for (int i = 1; i <= top; ++i)
  {
    stream << "stack #" << i << ": ";
    dumpValue (L, i, stream, 0);
    stream << "\n";
  }
}

} // namespace debug
} // namespace luabridge
