// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2018, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#pragma once

#include <LuaBridge/detail/Stack.h>
#include <LuaBridge/detail/dump.h>

#include <map>

namespace luabridge {

template <class K, class V>
struct Stack <std::map <K, V> >
{
  typedef std::map <K, V> Map;

  static void push (lua_State* L, const Map& map)
  {
    lua_createtable (L, 0, static_cast <int> (map.size ()));
    typedef typename Map::const_iterator ConstIter;
    for (ConstIter i = map.begin (); i != map.end (); ++i)
    {
      Stack <K>::push (L, i->first);
      Stack <V>::push (L, i->second);
      lua_settable (L, -3);
    }
  }

  static Map get (lua_State* L, int index)
  {
    if (!lua_istable (L, index))
    {
      luaL_error (L, "#%d argument must be a table", index);
    }

    Map map;
    int const absindex = lua_absindex (L, index);
    lua_pushnil (L);
    while (lua_next (L, absindex) != 0)
    {
      map.emplace (Stack <K>::get (L, -2), Stack <V>::get (L, -1));
      lua_pop (L, 1);
    }
    return map;
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_istable (L, index);
  }
};

} // namespace luabridge
