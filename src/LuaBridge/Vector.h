// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2018, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Stack.h>

#include <vector>

namespace luabridge {

template <class T>
struct Stack <std::vector <T> >
{
  static void push (lua_State* L, std::vector <T> const& vector)
  {
    lua_createtable (L, static_cast <int> (vector.size ()), 0);
    for (std::size_t i = 0; i < vector.size (); ++i)
    {
      lua_pushinteger (L, static_cast <lua_Integer> (i + 1));
      Stack <T>::push (L, vector [i]);
      lua_settable (L, -3);
    }
  }

  static std::vector <T> get (lua_State* L, int index)
  {
    if (!lua_istable (L, index))
    {
      luaL_error (L, "#%d argument must be a table", index);
    }

    std::vector <T> vector;
    vector.reserve (static_cast <std::size_t> (get_length (L, index)));

    int const absindex = lua_absindex (L, index);
    lua_pushnil (L);
    while (lua_next (L, absindex) != 0)
    {
      vector.push_back (Stack <T>::get (L, -1));
      lua_pop (L, 1);
    }
    return vector;
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_istable (L, index);
  }
};

} // namespace luabridge
