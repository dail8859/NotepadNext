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

#ifndef LUABRIDGE_LUABRIDGE_HEADER
#define LUABRIDGE_LUABRIDGE_HEADER

// All #include dependencies are listed here
// instead of in the individual header files.
//
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#define LUABRIDGE_MAJOR_VERSION 2
#define LUABRIDGE_MINOR_VERSION 0
#define LUABRIDGE_VERSION 200

namespace luabridge
{

// Forward declaration
//
template <class T>
struct Stack;

#include "detail/LuaHelpers.h"

#include "detail/TypeTraits.h"
#include "detail/TypeList.h"
#include "detail/FuncTraits.h"
#include "detail/Constructor.h"
#include "detail/Stack.h"
#include "detail/ClassInfo.h"

class LuaRef;

#include "detail/LuaException.h"
#include "detail/LuaRef.h"
#include "detail/Iterator.h"

//------------------------------------------------------------------------------
/**
    security options.
*/
class Security
{
public:
  static bool hideMetatables ()
  {
    return getSettings().hideMetatables;
  }

  static void setHideMetatables (bool shouldHide)
  {
    getSettings().hideMetatables = shouldHide;
  }

private:
  struct Settings
  {
    Settings () : hideMetatables (true)
    {
    }

    bool hideMetatables;
  };

  static Settings& getSettings ()
  {
    static Settings settings;
    return settings;
  }
};

#include "detail/Userdata.h"
#include "detail/CFunctions.h"
#include "detail/Namespace.h"

//------------------------------------------------------------------------------
/**
    Push an object onto the Lua stack.
*/
template <class T>
inline void push (lua_State* L, T t)
{
  Stack <T>::push (L, t);
}

//------------------------------------------------------------------------------
/**
  Set a global value in the lua_State.

  @note This works on any type specialized by `Stack`, including `LuaRef` and
        its table proxies.
*/
template <class T>
inline void setGlobal (lua_State* L, T t, char const* name)
{
  push (L, t);
  lua_setglobal (L, name);
}

//------------------------------------------------------------------------------
/**
  Change whether or not metatables are hidden (on by default).
*/
inline void setHideMetatables (bool shouldHide)
{
  Security::setHideMetatables (shouldHide);
}

}

#endif
