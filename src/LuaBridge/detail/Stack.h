//------------------------------------------------------------------------------
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

#include <LuaBridge/detail/LuaHelpers.h>
#include <LuaBridge/detail/Userdata.h>

#include <string>

namespace luabridge {

/// Lua stack traits for C++ types.
///
/// @tparam T A C++ type.
///
template <class T>
struct Stack;

template <>
struct Stack <void>
{
  static void push (lua_State* L)
  {
  }
};

//------------------------------------------------------------------------------
/**
    Receive the lua_State* as an argument.
*/
template <>
struct Stack <lua_State*>
{
  static lua_State* get (lua_State* L, int)
  {
    return L;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for a lua_CFunction.
*/
template <>
struct Stack <lua_CFunction>
{
  static void push (lua_State* L, lua_CFunction f)
  {
    lua_pushcfunction (L, f);
  }

  static lua_CFunction get (lua_State* L, int index)
  {
    return lua_tocfunction (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_iscfunction (L, index);
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `int`.
*/
template <>
struct Stack <int>
{
  static void push (lua_State* L, int value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static int get (lua_State* L, int index)
  {
    return static_cast <int> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned int`.
*/
template <>
struct Stack <unsigned int>
{
  static void push (lua_State* L, unsigned int value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static unsigned int get (lua_State* L, int index)
  {
    return static_cast <unsigned int> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned char`.
*/
template <>
struct Stack <unsigned char>
{
  static void push (lua_State* L, unsigned char value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static unsigned char get (lua_State* L, int index)
  {
    return static_cast <unsigned char> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `short`.
*/
template <>
struct Stack <short>
{
  static void push (lua_State* L, short value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static short get (lua_State* L, int index)
  {
    return static_cast <short> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned short`.
*/
template <>
struct Stack <unsigned short>
{
  static void push (lua_State* L, unsigned short value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static unsigned short get (lua_State* L, int index)
  {
    return static_cast <unsigned short> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `long`.
*/
template <>
struct Stack <long>
{
  static void push (lua_State* L, long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static long get (lua_State* L, int index)
  {
    return static_cast <long> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned long`.
*/
template <>
struct Stack <unsigned long>
{
  static void push (lua_State* L, unsigned long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static unsigned long get (lua_State* L, int index)
  {
    return static_cast <unsigned long> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `long long`.
 */
template <>
struct Stack <long long>
{
  static void push (lua_State* L, long long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }

  static long long get (lua_State* L, int index)
  {
    return static_cast <long long> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `unsigned long long`.
 */
template <>
struct Stack <unsigned long long>
{
  static void push (lua_State* L, unsigned long long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  static unsigned long long get (lua_State* L, int index)
  {
    return static_cast <unsigned long long> (luaL_checkinteger (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `float`.
*/
template <>
struct Stack <float>
{
  static void push (lua_State* L, float value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static float get (lua_State* L, int index)
  {
    return static_cast <float> (luaL_checknumber (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `double`.
*/
template <>
struct Stack <double>
{
  static void push (lua_State* L, double value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static double get (lua_State* L, int index)
  {
    return static_cast <double> (luaL_checknumber (L, index));
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TNUMBER;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `bool`.
*/
template <>
struct Stack <bool>
{
  static void push (lua_State* L, bool value)
  {
    lua_pushboolean (L, value ? 1 : 0);
  }
  
  static bool get (lua_State* L, int index)
  {
    return lua_toboolean (L, index) ? true : false;
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_isboolean (L, index);
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `char`.
*/
template <>
struct Stack <char>
{
  static void push (lua_State* L, char value)
  {
    lua_pushlstring (L, &value, 1);
  }
  
  static char get (lua_State* L, int index)
  {
    return luaL_checkstring (L, index) [0];
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TSTRING;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `const char*`.
*/
template <>
struct Stack <char const*>
{
  static void push (lua_State* L, char const* str)
  {
    if (str != 0)
      lua_pushstring (L, str);
    else
      lua_pushnil (L);
  }

  static char const* get (lua_State* L, int index)
  {
    return lua_isnil (L, index) ? 0 : luaL_checkstring (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_isnil (L, index) || lua_type (L, index) == LUA_TSTRING;
  }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `std::string`.
*/
template <>
struct Stack <std::string>
{
  static void push (lua_State* L, std::string const& str)
  {
    lua_pushlstring (L, str.data (), str.size ());
  }

  static std::string get (lua_State* L, int index)
  {
    size_t len;
    if (lua_type (L, index) == LUA_TSTRING)
    {
      const char* str = lua_tolstring (L, index, &len);
      return std::string (str, len);
    }

    // Lua reference manual:
    // If the value is a number, then lua_tolstring also changes the actual value in the stack to a string.
    // (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
    lua_pushvalue (L, index);
    const char* str = lua_tolstring(L, -1, &len);
    std::string string (str, len);
    lua_pop (L, 1); // Pop the temporary string
    return string;
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TSTRING;
  }
};

namespace detail {

template <class T>
struct StackOpSelector <T&, false>
{
  typedef T ReturnType;

  static void push (lua_State* L, T& value)
  {
    Stack <T>::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Stack <T>::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Stack <T>::isInstance (L, index);
  }
};

template <class T>
struct StackOpSelector <const T&, false>
{
  typedef T ReturnType;

  static void push (lua_State* L, const T& value)
  {
    Stack <T>::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Stack <T>::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Stack <T>::isInstance (L, index);
  }
};

template <class T>
struct StackOpSelector <T*, false>
{
  typedef T ReturnType;

  static void push (lua_State* L, T* value)
  {
    Stack <T>::push (L, *value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Stack <T>::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Stack <T>::isInstance (L, index);
  }
};

template <class T>
struct StackOpSelector <const T*, false>
{
  typedef T ReturnType;

  static void push (lua_State* L, const T* value)
  {
    Stack <T>::push (L, *value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Stack <T>::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Stack <T>::isInstance (L, index);
  }
};

} // namespace detail

template <class T>
struct Stack <T&>
{
  typedef detail::StackOpSelector <T&, detail::IsUserdata <T>::value> Helper;
  typedef typename Helper::ReturnType ReturnType;

  static void push (lua_State* L, T& value)
  {
    Helper::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }
};

template <class T>
struct Stack <const T&>
{
  typedef detail::StackOpSelector <const T&, detail::IsUserdata <T>::value> Helper;
  typedef typename Helper::ReturnType ReturnType;

  static void push (lua_State* L, const T& value)
  {
    Helper::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }
};

template <class T>
struct Stack <T*>
{
  typedef detail::StackOpSelector <T*, detail::IsUserdata <T>::value> Helper;
  typedef typename Helper::ReturnType ReturnType;

  static void push (lua_State* L, T* value)
  {
    Helper::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }
};

template <class T>
struct Stack <const T*>
{
  typedef detail::StackOpSelector <const T*, detail::IsUserdata <T>::value> Helper;
  typedef typename Helper::ReturnType ReturnType;

  static void push (lua_State* L, const T* value)
  {
    Helper::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }
};

//------------------------------------------------------------------------------
/**
 * Push an object onto the Lua stack.
 */
template <class T>
inline void push (lua_State* L, T t)
{
  Stack <T>::push (L, t);
}

//------------------------------------------------------------------------------
/**
 * Check whether an object on the Lua stack is of type T.
 */
template <class T>
inline bool isInstance (lua_State* L, int index)
{
  return Stack <T>::isInstance (L, index);
}

} // namespace luabridge
