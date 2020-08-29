//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2018, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2008, Nigel Atkinson <suprapilot+LuaCode@gmail.com>

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

#include <LuaBridge/detail/LuaException.h>
#include <LuaBridge/detail/Stack.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace luabridge {

//------------------------------------------------------------------------------
/**
    Type tag for representing LUA_TNIL.

    Construct one of these using `Nil ()` to represent a Lua nil. This is faster
    than creating a reference in the registry to nil. Example:

        LuaRef t (LuaRef::createTable (L));
        ...
        t ["k"] = Nil (); // assign nil
*/
struct Nil
{
};


//------------------------------------------------------------------------------
/**
    Stack specialization for Nil.
*/
template <>
struct Stack <Nil>
{
  static void push (lua_State* L, Nil)
  {
    lua_pushnil (L);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TTABLE;
  }
};

/**
 * Base class for Lua variables and table item reference classes.
 */
template <class Impl, class LuaRef>
class LuaRefBase
{
protected:
  //----------------------------------------------------------------------------
  /**
      Pop the Lua stack.

      Pops the specified number of stack items on destruction. We use this
      when returning objects, to avoid an explicit temporary variable, since
      the destructor executes after the return statement. For example:

          template <class U>
          U cast (lua_State* L)
          {
            StackPop p (L, 1);
            ...
            return U (); // dtor called after this line
          }

      @note The `StackPop` object must always be a named local variable.
  */
  class StackPop
  {
  public:
    /** Create a StackPop object.

        @param L     A Lua state.
        @param count The number of stack entries to pop on destruction.
    */
    StackPop (lua_State* L, int count)
      : m_L (L)
      , m_count (count)
    {
    }

    ~StackPop ()
    {
      lua_pop (m_L, m_count);
    }

  private:
    lua_State* m_L;
    int m_count;
  };

  friend struct Stack <LuaRef>;

  //----------------------------------------------------------------------------
  /**
      Type tag for stack construction.
  */
  struct FromStack { };

  LuaRefBase (lua_State* L)
    : m_L (L)
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to this reference.

      @returns An index in the Lua registry.
  */
  int createRef () const
  {
    impl ().push ();
    return luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

public:
  //----------------------------------------------------------------------------
  /**
      Convert to a string using lua_tostring function.

      @returns A string representation of the referred Lua value.
  */
  std::string tostring () const
  {
    lua_getglobal (m_L, "tostring");
    impl ().push ();
    lua_call (m_L, 1, 1);
    const char* str = lua_tostring (m_L, -1);
    lua_pop (m_L, 1);
    return str;
  }

  //----------------------------------------------------------------------------
  /**
      Print a text description of the value to a stream.
      This is used for diagnostics.

      @param os An output stream.
  */
  void print (std::ostream& os) const
  {
    switch (type ())
    {
    case LUA_TNIL:
      os << "nil";
      break;

    case LUA_TNUMBER:
      os << cast <lua_Number> ();
      break;

    case LUA_TBOOLEAN:
      os << (cast <bool> () ? "true" : "false");
      break;

    case LUA_TSTRING:
      os << '"' << cast <std::string> () << '"';
      break;

    case LUA_TTABLE:
      os << "table: " << tostring ();
      break;

    case LUA_TFUNCTION:
      os << "function: " << tostring ();
      break;

    case LUA_TUSERDATA:
      os << "userdata: " << tostring ();
      break;

    case LUA_TTHREAD:
      os << "thread: " << tostring ();
      break;

    case LUA_TLIGHTUSERDATA:
      os << "lightuserdata: " << tostring ();
      break;

    default:
      os << "unknown";
      break;
    }
  }

  //------------------------------------------------------------------------------
  /**
    Insert a Lua value or table item reference to a stream.

    @param os  An output stream.
    @param ref A Lua reference.
    @returns The output stream.
  */
  friend std::ostream& operator<< (std::ostream& os, LuaRefBase const& ref)
  {
    ref.print (os);
    return os;
  }

  //============================================================================
  //
  // This group of member functions is mirrored in TableItem
  //

  /** Retrieve the lua_State associated with the reference.

    @returns A Lua state.
  */
  lua_State* state () const
  {
    return m_L;
  }

  //----------------------------------------------------------------------------
  /**
      Place the object onto the Lua stack.

      @param L A Lua state.
  */
  void push (lua_State* L) const
  {
    assert (equalstates (L, m_L));
    (void) L;
    impl ().push ();
  }

  //----------------------------------------------------------------------------
  /**
      Pop the top of Lua stack and assign it to the reference.

      @param L A Lua state.
  */
  void pop (lua_State* L)
  {
    assert (equalstates (L, m_L));
    (void) L;
    impl ().pop ();
  }

  //----------------------------------------------------------------------------
  /**
      Return the Lua type of the referred value. This invokes lua_type().

      @returns The type of the referred value.
      @see lua_type()
  */
  /** @{ */
  int type () const
  {
    impl ().push ();
    StackPop p (m_L, 1);
    return lua_type (m_L, -1);
  }

  // should never happen
  // bool isNone () const { return m_ref == LUA_NOREF; }

  /// Indicate whether it is a nil reference.
  ///
  /// @returns True if this is a nil reference, false otherwice.
  /// 
  bool isNil () const { return type () == LUA_TNIL; }

  /// Indicate whether it is a reference to a boolean.
  ///
  /// @returns True if it is a reference to a boolean, false otherwice.
  /// 
  bool isBool () const { return type () == LUA_TBOOLEAN; }

  /// Indicate whether it is a reference to a number.
  ///
  /// @returns True if it is a reference to a number, false otherwise.
  /// 
  bool isNumber () const { return type () == LUA_TNUMBER; }

  /// Indicate whether it is a reference to a string.
  ///
  /// @returns True if it is a reference to a string, false otherwise.
  /// 
  bool isString () const { return type () == LUA_TSTRING; }

  /// Indicate whether it is a reference to a table.
  ///
  /// @returns True if it is a reference to a table, false otherwise.
  /// 
  bool isTable () const { return type () == LUA_TTABLE; }

  /// Indicate whether it is a reference to a function.
  ///
  /// @returns True if it is a reference to a function, false otherwise.
  /// 
  bool isFunction () const { return type () == LUA_TFUNCTION; }

  /// Indicate whether it is a reference to a full userdata.
  ///
  /// @returns True if it is a reference to a full userdata, false otherwise.
  /// 
  bool isUserdata () const { return type () == LUA_TUSERDATA; }

  /// Indicate whether it is a reference to a Lua thread.
  ///
  /// @returns True if it is a reference to a Lua thread, false otherwise.
  /// 
  bool isThread () const { return type () == LUA_TTHREAD; }

  /// Indicate whether it is a reference to a light userdata.
  ///
  /// @returns True if it is a reference to a light userdata, false otherwise.
  /// 
  bool isLightUserdata () const { return type () == LUA_TLIGHTUSERDATA; }

  /** @} */

  //----------------------------------------------------------------------------
  /**
      Perform an explicit conversion to the type T.

      @returns A value of the type T converted from this reference.
  */
  template <class T>
  T cast () const
  {
    StackPop p (m_L, 1);
    impl ().push ();
    return Stack <T>::get (m_L, -1);
  }

  //----------------------------------------------------------------------------
  /**
      Indicate if this reference is convertible to the type T.

      @returns True if the referred value is convertible to the type T,
              false otherwise.
  */
  template <class T>
  bool isInstance () const
  {
    StackPop p (m_L, 1);
    impl ().push ();
    return Stack <T>::isInstance (m_L, -1);
  }

  //----------------------------------------------------------------------------
  /**
      Type cast operator.

      @returns A value of the type T converted from this reference.
  */
  template <class T>
  operator T () const
  {
    return cast <T> ();
  }

  //----------------------------------------------------------------------------
  /** @{ */
  /**
      Compare this reference with a specified value using lua_compare().
      This invokes metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is equal to the specified one.
  */
  template <class T>
  bool operator== (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();
    Stack <T>::push (m_L, rhs);
    return lua_compare (m_L, -2, -1, LUA_OPEQ) == 1;
  }

  /**
      Compare this reference with a specified value using lua_compare().
      This invokes metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is less than the specified one.
  */
  template <class T>
  bool operator< (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType < rhsType;
    }
    return lua_compare (m_L, -2, -1, LUA_OPLT) == 1;
  }

  /**
      Compare this reference with a specified value using lua_compare().
      This invokes metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is less than or equal to the specified one.
  */
  template <class T>
  bool operator<= (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType <= rhsType;
    }
    return lua_compare (m_L, -2, -1, LUA_OPLE) == 1;
  }

  /**
      Compare this reference with a specified value using lua_compare().
      This invokes metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is greater than the specified one.
  */
  template <class T>
  bool operator> (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType > rhsType;
    }
    return lua_compare (m_L, -1, -2, LUA_OPLT) == 1;
  }

  /**
      Compare this reference with a specified value using lua_compare().
      This invokes metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is greater than or equal to the specified one.
  */
  template <class T>
  bool operator>= (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType >= rhsType;
    }
    return lua_compare (m_L, -1, -2, LUA_OPLE) == 1;
  }

  /**
      Compare this reference with a specified value using lua_compare().
      This does not invoke metamethods.

      @param rhs A value to compare with.
      @returns True if the referred value is equal to the specified one.
  */
  template <class T>
  bool rawequal (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    return lua_rawequal (m_L, -1, -2) == 1;
  }
  /** @} */

  //----------------------------------------------------------------------------
  /**
      Append a value to a referred table.
      If the table is a sequence this will add another element to it.

      @param v A value to append to the table.
  */
  template <class T>
  void append (T v) const
  {
    impl ().push ();;
    Stack <T>::push (m_L, v);
    luaL_ref (m_L, -2);
    lua_pop (m_L, 1);
  }

  //----------------------------------------------------------------------------
  /**
      Return the length of a referred array.
      This is identical to applying the Lua # operator.

      @returns The length of the referred array.
  */
  int length () const
  {
    StackPop p (m_L, 1);
    impl ().push ();;
    return get_length (m_L, -1);
  }

  //----------------------------------------------------------------------------
  /**
      Call Lua code.
      These overloads allow Lua code to be called with up to 8 parameters.
      The return value is provided as a LuaRef (which may be LUA_REFNIL).
      If an error occurs, a LuaException is thrown.

      @returns A result of the call.
  */
  /** @{ */
  LuaRef operator() () const
  {
    impl ().push ();;
    LuaException::pcall (m_L, 0, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1>
  LuaRef operator() (P1 p1) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    LuaException::pcall (m_L, 1, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2>
  LuaRef operator() (P1 p1, P2 p2) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    LuaException::pcall (m_L, 2, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3>
  LuaRef operator() (P1 p1, P2 p2, P3 p3) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    LuaException::pcall (m_L, 3, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    LuaException::pcall (m_L, 4, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    LuaException::pcall (m_L, 5, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    LuaException::pcall (m_L, 6, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    Stack <P7>::push (m_L, p7);
    LuaException::pcall (m_L, 7, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) const
  {
    impl ().push ();
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    Stack <P7>::push (m_L, p7);
    Stack <P8>::push (m_L, p8);
    LuaException::pcall (m_L, 8, 1);
    return LuaRef::fromStack (m_L);
  }
  /** @} */

  //============================================================================

protected:
  lua_State* m_L;

private:
  const Impl& impl () const
  {
    return static_cast <const Impl&> (*this);
  }

  Impl& impl ()
  {
    return static_cast <Impl&> (*this);
  }
};

//------------------------------------------------------------------------------
/**
    Lightweight reference to a Lua object.

    The reference is maintained for the lifetime of the C++ object.
*/
class LuaRef : public LuaRefBase <LuaRef, LuaRef>
{
  //----------------------------------------------------------------------------
  /**
      A proxy for representing table values.
  */
  class TableItem : public LuaRefBase <TableItem, LuaRef>
  {
    friend class LuaRef;

  public:
    //--------------------------------------------------------------------------
    /**
        Construct a TableItem from a table value.
        The table is in the registry, and the key is at the top of the stack.
        The key is popped off the stack.

        @param L        A lua state.
        @param tableRef The index of a table in the Lua registry.
    */
    TableItem (lua_State* L, int tableRef)
      : LuaRefBase (L)
      , m_tableRef (LUA_NOREF)
      , m_keyRef (luaL_ref (L, LUA_REGISTRYINDEX))
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, tableRef);
      m_tableRef = luaL_ref (L, LUA_REGISTRYINDEX);
    }

    //--------------------------------------------------------------------------
    /**
        Create a TableItem via copy constructor.
        It is best to avoid code paths that invoke this, because it creates
        an extra temporary Lua reference. Typically this is done by passing
        the TableItem parameter as a `const` reference.

        @param other Another Lua table item reference.
    */
    TableItem (TableItem const& other)
      : LuaRefBase (other.m_L)
      , m_tableRef (LUA_NOREF)
      , m_keyRef (LUA_NOREF)
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, other.m_tableRef);
      m_tableRef = luaL_ref (m_L, LUA_REGISTRYINDEX);

      lua_rawgeti (m_L, LUA_REGISTRYINDEX, other.m_keyRef);
      m_keyRef = luaL_ref (m_L, LUA_REGISTRYINDEX);
    }

    //--------------------------------------------------------------------------
    /**
        Destroy the proxy.
        This does not destroy the table value.
    */
    ~TableItem ()
    {
      luaL_unref (m_L, LUA_REGISTRYINDEX, m_keyRef);
      luaL_unref (m_L, LUA_REGISTRYINDEX, m_tableRef);
    }

    //--------------------------------------------------------------------------
    /**
        Assign a new value to this table key.
        This may invoke metamethods.

        @tparam T The type of a value to assing.
        @param  v A value to assign.
        @returns This reference.
    */
    template <class T>
    TableItem& operator= (T v)
    {
      StackPop p (m_L, 1);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      Stack <T>::push (m_L, v);
      lua_settable (m_L, -3);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Assign a new value to this table key.
        The assignment is raw, no metamethods are invoked.

        @tparam T The type of a value to assing.
        @param  v A value to assign.
        @returns This reference.
    */
    template <class T>
    TableItem& rawset (T v)
    {
      StackPop p (m_L, 1);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      Stack <T>::push (m_L, v);
      lua_rawset (m_L, -3);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Push the value onto the Lua stack.
    */
    using LuaRefBase::push;

    void push () const
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      lua_gettable (m_L, -2);
      lua_remove (m_L, -2); // remove the table
    }

    //--------------------------------------------------------------------------
    /**
        Access a table value using a key.
        This invokes metamethods.

        @tparam T   The type of a key.
        @param  key A key value.
        @returns A Lua table item reference.
    */
    template <class T>
    TableItem operator[] (T key) const
    {
      return LuaRef (*this) [key];
    }

    //--------------------------------------------------------------------------
    /**
        Access a table value using a key.
        The operation is raw, metamethods are not invoked. The result is
        passed by value and may not be modified.

        @tparam T   The type of a key.
        @param  key A key value.
        @returns A Lua value reference.
    */
    template <class T>
    LuaRef rawget (T key) const
    {
      return LuaRef (*this).rawget (key);
    }

  private:
    int m_tableRef;
    int m_keyRef;
  };

  friend struct Stack <TableItem>;
  friend struct Stack <TableItem&>;

  //----------------------------------------------------------------------------
  /**
      Create a reference to an object at the top of the Lua stack and pop it.
      This constructor is private and not invoked directly.
      Instead, use the `fromStack` function.

      @param L A Lua state.
      @note The object is popped.
  */
  LuaRef (lua_State* L, FromStack)
    : LuaRefBase (L)
    , m_ref (luaL_ref (m_L, LUA_REGISTRYINDEX))
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to an object on the Lua stack.
      This constructor is private and not invoked directly.
      Instead, use the `fromStack` function.

      @param L     A Lua state.
      @param index The index of the value on the Lua stack.
      @note The object is not popped.
  */
  LuaRef (lua_State* L, int index, FromStack)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
    lua_pushvalue (m_L, index);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }


public:
  //----------------------------------------------------------------------------
  /**
      Create a nil reference.
      The Lua reference may be assigned later.

      @param L A Lua state.
  */
  LuaRef (lua_State* L)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
  }

  //----------------------------------------------------------------------------
  /**
      Push a value onto a Lua stack and return a reference to it.

      @param L A Lua state.
      @param v A value to push. 
  */
  template <class T>
  LuaRef (lua_State* L, T v)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
    Stack <T>::push (m_L, v);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to a table item.

      @param v A table item reference.
  */
  LuaRef (TableItem const& v)
    : LuaRefBase (v.state ())
    , m_ref (v.createRef ())
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a new reference to an existing Lua value.

      @param other An existing reference.
  */
  LuaRef (LuaRef const& other)
    : LuaRefBase (other.m_L)
    , m_ref (other.createRef ())
  {
  }

  //----------------------------------------------------------------------------
  /**
      Destroy a reference.

      The corresponding Lua registry reference will be released.

      @note If the state refers to a thread, it is the responsibility of the
            caller to ensure that the thread still exists when the LuaRef
            is destroyed.
  */
  ~LuaRef ()
  {
    luaL_unref (m_L, LUA_REGISTRYINDEX, m_ref);
  }

  //----------------------------------------------------------------------------
  /**
      Return a reference to a top Lua stack item.
      The stack item is not popped.

      @param L A Lua state.
      @returns A reference to a value on the top of a Lua stack.
  */
  static LuaRef fromStack (lua_State* L)
  {
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Return a reference to a Lua stack item with a specified index.
      The stack item is not removed.

      @param L     A Lua state.
      @param index An index in the Lua stack.
      @returns A reference to a value in a Lua stack.
  */
  static LuaRef fromStack (lua_State* L, int index)
  {
    lua_pushvalue (L, index);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Create a new empty table on the top of a Lua stack
      and return a reference to it.

      @param L A Lua state.
      @returns A reference to the newly created table.
      @see luabridge::newTable()
  */
  static LuaRef newTable (lua_State* L)
  {
    lua_newtable (L);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Return a reference to a named global Lua variable.

      @param L    A Lua state.
      @param name The name of a global variable.
      @returns A reference to the Lua variable.
      @see luabridge::getGlobal()
  */
  static LuaRef getGlobal (lua_State *L, char const* name)
  {
    lua_getglobal (L, name);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Assign another LuaRef to this LuaRef.

      @param rhs A reference to assign from.
      @returns This reference.
  */
  LuaRef& operator= (LuaRef const& rhs)
  {
    LuaRef ref (rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Assign a table item reference.

      @param rhs A table item reference.
      @returns This reference.
  */
  LuaRef& operator= (LuaRef::TableItem const& rhs)
  {
    LuaRef ref (rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
    Assign nil to this reference.

    @returns This reference.
  */
  LuaRef& operator= (Nil const&)
  {
    LuaRef ref (m_L);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Assign a different value to this reference.

      @param rhs A value to assign.
      @returns This reference.
  */
  template <class T>
  LuaRef& operator= (T rhs)
  {
    LuaRef ref (m_L, rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Place the object onto the Lua stack.
  */
  using LuaRefBase::push;

  void push () const
  {
    lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_ref);
  }

  //----------------------------------------------------------------------------
  /**
      Pop the top of Lua stack and assign the ref to m_ref
  */
  void pop ()
  {
    luaL_unref (m_L, LUA_REGISTRYINDEX, m_ref);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

  //----------------------------------------------------------------------------
  /**
      Access a table value using a key.
      This invokes metamethods.

      @param key A key in the table.
      @returns A reference to the table item.
  */
  template <class T>
  TableItem operator[] (T key) const
  {
    Stack <T>::push (m_L, key);
    return TableItem (m_L, m_ref);
  }

  //--------------------------------------------------------------------------
  /**
      Access a table value using a key.
      The operation is raw, metamethods are not invoked. The result is
      passed by value and may not be modified.

      @param key A key in the table.
      @returns A reference to the table item.
  */
  template <class T>
  LuaRef rawget (T key) const
  {
    StackPop (m_L, 1);
    push (m_L);
    Stack <T>::push (m_L, key);
    lua_rawget (m_L, -2);
    return LuaRef (m_L, FromStack ());
  }

private:
  void swap (LuaRef& other)
  {
    std::swap (m_L, other.m_L);
    std::swap (m_ref, other.m_ref);
  }

  int m_ref;
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `LuaRef`.
 */
template <>
struct Stack <LuaRef>
{
  // The value is const& to prevent a copy construction.
  //
  static void push (lua_State* L, LuaRef const& v)
  {
    v.push (L);
  }

  static LuaRef get (lua_State* L, int index)
  {
    return LuaRef::fromStack (L, index);
  }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `TableItem`.
 */
template <>
struct Stack <LuaRef::TableItem>
{
  // The value is const& to prevent a copy construction.
  //
  static void push (lua_State* L, LuaRef::TableItem const& v)
  {
    v.push (L);
  }
};

//------------------------------------------------------------------------------
/**
    Create a reference to a new, empty table.

    This is a syntactic abbreviation for LuaRef::newTable ().
*/
inline LuaRef newTable (lua_State* L)
{
  return LuaRef::newTable (L);
}

//------------------------------------------------------------------------------
/**
    Create a reference to a value in the global table.

    This is a syntactic abbreviation for LuaRef::getGlobal ().
*/
inline LuaRef getGlobal (lua_State *L, char const* name)
{
  return LuaRef::getGlobal (L, name);
}

//------------------------------------------------------------------------------

// more C++-like cast syntax
//
template <class T>
T LuaRef_cast (LuaRef const& lr)
{
  return lr.cast <T> ();
}

} // namespace luabridge
