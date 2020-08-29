// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2018, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Stack.h>

#include <list>

namespace luabridge {

template <class T>
struct Stack <std::list <T> >
{
  static void push (lua_State* L, std::list <T> const& list)
  {
    lua_createtable (L, static_cast <int> (list.size ()), 0);
    typename std::list <T>::const_iterator item = list.begin ();
    for (std::size_t i = 1; i <= list.size (); ++i)
    {
      lua_pushinteger (L, static_cast <lua_Integer> (i));
      Stack <T>::push (L, *item);
      lua_settable (L, -3);
      ++item;
    }
  }

  static std::list <T> get (lua_State* L, int index)
  {
    if (!lua_istable (L, index))
    {
      luaL_error (L, "#%d argument must be a table", index);
    }

    std::list <T> list;

    int const absindex = lua_absindex (L, index);
    lua_pushnil (L);
    while (lua_next (L, absindex) != 0)
    {
      list.push_back (Stack <T>::get (L, -1));
      lua_pop (L, 1);
    }
    return list;
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_istable (L, index);
  }
};

} // namespace luabridge
