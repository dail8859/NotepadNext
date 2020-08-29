//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2019, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>

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

#include <LuaBridge/detail/ClassInfo.h>

#include <cassert>
#include <stdexcept>

namespace luabridge {

namespace detail {

//==============================================================================
/**
  Return the identity pointer for our lightuserdata tokens.

  Because of Lua's dynamic typing and our improvised system of imposing C++
  class structure, there is the possibility that executing scripts may
  knowingly or unknowingly cause invalid data to get passed to the C functions
  created by LuaBridge. In particular, our security model addresses the
  following:
    1. Scripts cannot create a userdata (ignoring the debug lib).
    2. Scripts cannot create a lightuserdata (ignoring the debug lib).
    3. Scripts cannot set the metatable on a userdata.
*/

/**
  Interface to a class pointer retrievable from a userdata.
*/
class Userdata
{
protected:
  void* m_p; // subclasses must set this

  Userdata() : m_p (0)
  {	  
  }
	
  //--------------------------------------------------------------------------
  /**
    Get an untyped pointer to the contained class.
  */
  void* getPointer ()
  {
    return m_p;
  }

private:
  //--------------------------------------------------------------------------
  /**
    Validate and retrieve a Userdata on the stack.

    The Userdata must exactly match the corresponding class table or
    const table, or else a Lua error is raised. This is used for the
    __gc metamethod.
  */
  static Userdata* getExactClass (lua_State* L, int index, void const* /*classKey*/)
  {
    return static_cast <Userdata*> (lua_touserdata (L, lua_absindex (L, index)));
  }

  //--------------------------------------------------------------------------
  /**
    Validate and retrieve a Userdata on the stack.

    The Userdata must be derived from or the same as the given base class,
    identified by the key. If canBeConst is false, generates an error if
    the resulting Userdata represents to a const object. We do the type check
    first so that the error message is informative.
  */
  static Userdata* getClass (lua_State* L,
                             int index,
                             void const* registryConstKey,
                             void const* registryClassKey,
                             bool canBeConst)
  {
    index = lua_absindex (L, index);

    lua_getmetatable (L, index); // Stack: object metatable (ot) | nil
    if (!lua_istable (L, -1))
    {
      lua_rawgetp (L, LUA_REGISTRYINDEX, registryClassKey); // Stack: registry metatable (rt) | nil
      return throwBadArg (L, index);
    }

    lua_rawgetp (L, -1, getConstKey ()); // Stack: ot | nil, const table (co) | nil
    assert (lua_istable (L, -1) || lua_isnil (L, -1));

    // If const table is NOT present, object is const. Use non-const registry table
    // if object cannot be const, so constness validation is done automatically.
    // E.g. nonConstFn (constObj)
    // -> canBeConst = false, isConst = true
    // -> 'Class' registry table, 'const Class' object table
    // -> 'expected Class, got const Class' 
    bool isConst = lua_isnil (L, -1); // Stack: ot | nil, nil, rt
    if (isConst && canBeConst)
    {
      lua_rawgetp (L, LUA_REGISTRYINDEX, registryConstKey); // Stack: ot, nil, rt
    }
    else
    {
      lua_rawgetp (L, LUA_REGISTRYINDEX, registryClassKey); // Stack: ot, co, rt
    }

    lua_insert (L, -3); // Stack: rt, ot, co | nil
    lua_pop (L, 1); // Stack: rt, ot

    for (;;)
    {
      if (lua_rawequal (L, -1, -2)) // Stack: rt, ot
      {
        lua_pop (L, 2); // Stack: -
        return static_cast <Userdata*> (lua_touserdata (L, index));
      }

      // Replace current metatable with it's base class.
      lua_rawgetp (L, -1, getParentKey ()); // Stack: rt, ot, parent ot (pot) | nil

      if (lua_isnil (L, -1)) // Stack: rt, ot, nil
      {
        // Drop the object metatable because it may be some parent metatable
        lua_pop (L, 2); // Stack: rt
        return throwBadArg (L, index);
      }

      lua_remove (L, -2); // Stack: rt, pot
    }

    // no return
  }

  static bool isInstance (lua_State* L, int index, void const* registryClassKey)
  {
    index = lua_absindex (L, index);

    int result = lua_getmetatable (L, index); // Stack: object metatable (ot) | nothing
    if (result == 0)
    {
      return false; // Nothing was pushed on the stack
    }
    if (!lua_istable (L, -1))
    {
      lua_pop (L, 1); // Stack: -
      return false;
    }

    lua_rawgetp (L, LUA_REGISTRYINDEX, registryClassKey); // Stack: ot, rt
    lua_insert (L, -2); // Stack: rt, ot

    for (;;)
    {
      if (lua_rawequal (L, -1, -2)) // Stack: rt, ot
      {
        lua_pop (L, 2); // Stack: -
        return true;
      }

      // Replace current metatable with it's base class.
      lua_rawgetp (L, -1, getParentKey ()); // Stack: rt, ot, parent ot (pot) | nil

      if (lua_isnil (L, -1)) // Stack: rt, ot, nil
      {
        lua_pop (L, 3); // Stack: -
        return false;
      }

      lua_remove (L, -2); // Stack: rt, pot
    }
  }

  static Userdata* throwBadArg (lua_State* L, int index)
  {
    assert (lua_istable (L, -1) || lua_isnil (L, -1)); // Stack: rt | nil

    const char* expected = 0;
    if (lua_isnil (L, -1)) // Stack: nil
    {
      expected = "unregistered class";
    }
    else
    {
      lua_rawgetp (L, -1, getTypeKey ()); // Stack: rt, registry type
      expected = lua_tostring (L, -1);
    }

    const char* got = 0;
    if (lua_isuserdata (L, index))
    {
      lua_getmetatable (L, index); // Stack: ..., ot | nil
      if (lua_istable (L, -1)) // Stack: ..., ot
      {
        lua_rawgetp (L, -1, getTypeKey ()); // Stack: ..., ot, object type | nil
        if (lua_isstring (L, -1))
        {
          got = lua_tostring (L, -1);
        }
      }
    }
    if (!got)
    {
      got = lua_typename (L, lua_type (L, index));
    }

    luaL_argerror (L, index, lua_pushfstring (L, "%s expected, got %s", expected, got));
    return 0;
  }

public:
  virtual ~Userdata () { }

  //--------------------------------------------------------------------------
  /**
    Returns the Userdata* if the class on the Lua stack matches.
    If the class does not match, a Lua error is raised.

    @tparam T     A registered user class.
    @param  L     A Lua state.
    @param  index The index of an item on the Lua stack.
    @returns A userdata pointer if the class matches.
  */
  template <class T>
  static Userdata* getExact (lua_State* L, int index)
  {
    return getExactClass (L, index, detail::getClassRegistryKey <T> ());
  }

  //--------------------------------------------------------------------------
  /**
    Get a pointer to the class from the Lua stack.
    If the object is not the class or a subclass, or it violates the
    const-ness, a Lua error is raised.

    @tparam T          A registered user class.
    @param  L          A Lua state.
    @param  index      The index of an item on the Lua stack.
    @param  canBeConst TBD
    @returns A pointer if the class and constness match.
  */
  template <class T>
  static T* get (lua_State* L, int index, bool canBeConst)
  {
    if (lua_isnil (L, index))
      return 0;

    return static_cast <T*> (getClass (
      L, index, detail::getConstRegistryKey <T> (),
      detail::getClassRegistryKey <T> (),
      canBeConst)->getPointer ());
  }

  template <class T>
  static bool isInstance (lua_State* L, int index)
  {
    return isInstance (L, index, detail::getClassRegistryKey <T> ());
  }
};

//----------------------------------------------------------------------------
/**
  Wraps a class object stored in a Lua userdata.

  The lifetime of the object is managed by Lua. The object is constructed
  inside the userdata using placement new.
*/
template <class T>
class UserdataValue : public Userdata
{
private:
  UserdataValue <T> (UserdataValue <T> const&);
  UserdataValue <T> operator= (UserdataValue <T> const&);

  char m_storage [sizeof (T)];

private:
  /**
    Used for placement construction.
  */
  UserdataValue ()
  {
    m_p = 0;
  }

  ~UserdataValue ()
  {
    if (getPointer () != 0)
    {
      getObject ()->~T ();
    }
  }

public:
  /**
    Push a T via placement new.

    The caller is responsible for calling placement new using the
    returned uninitialized storage.

    @param L A Lua state.
    @returns An object referring to the newly created userdata value.
  */
  static UserdataValue <T>* place (lua_State* const L)
  {
    UserdataValue <T>* const ud = new (
      lua_newuserdata (L, sizeof (UserdataValue <T>))) UserdataValue <T> ();
    lua_rawgetp (L, LUA_REGISTRYINDEX, detail::getClassRegistryKey <T> ());
    if (!lua_istable (L, -1))
    {
      throw std::logic_error ("The class is not registered in LuaBridge");
    }
    lua_setmetatable (L, -2);
    return ud;
  }

  /**
    Push T via copy construction from U.

    @tparam U A container type.
    @param  L A Lua state.
    @param  u A container object reference.
  */
  template <class U>
  static inline void push (lua_State* const L, U const& u)
  {
    UserdataValue <T>* ud = place (L);
    new (ud->getObject ()) U (u);
    ud->commit ();
  }

  /**
    Confirm object construction.
  */
  void commit ()
  {
    m_p = getObject ();
  }

  T* getObject ()
  {
    // If this fails to compile it means you forgot to provide
    // a Container specialization for your container!
    //
    return reinterpret_cast <T*> (&m_storage [0]);
  }
};

//----------------------------------------------------------------------------
/**
  Wraps a pointer to a class object inside a Lua userdata.

  The lifetime of the object is managed by C++.
*/
class UserdataPtr : public Userdata
{
private:
  UserdataPtr (UserdataPtr const&);
  UserdataPtr operator= (UserdataPtr const&);

private:
  /** Push a pointer to object using metatable key.
  */
  static void push (lua_State* L, const void* p, void const* const key)
  {
    new (lua_newuserdata (L, sizeof (UserdataPtr))) UserdataPtr (const_cast <void*> (p));
    lua_rawgetp (L, LUA_REGISTRYINDEX, key);
    if (!lua_istable (L, -1))
    {
      lua_pop (L, 1); // possibly: a nil
      throw std::logic_error ("The class is not registered in LuaBridge");
    }
    lua_setmetatable (L, -2);
  }

  explicit UserdataPtr (void* const p)
  {
    m_p = p;

    // Can't construct with a null pointer!
    //
    assert (m_p != 0);
  }

public:
  /** Push non-const pointer to object.

    @tparam T A user registered class.
    @param  L A Lua state.
    @param  p A pointer to the user class instance.
  */
  template <class T>
  static void push (lua_State* const L, T* const p)
  {
    if (p)
      push (L, p, getClassRegistryKey <T> ());
    else
      lua_pushnil (L);
  }

  /** Push const pointer to object.

    @tparam T A user registered class.
    @param  L A Lua state.
    @param  p A pointer to the user class instance.
  */
  template <class T>
  static void push (lua_State* const L, T const* const p)
  {
    if (p)
      push (L, p, getConstRegistryKey <T> ());
    else
      lua_pushnil (L);
  }
};

//============================================================================
/**
  Wraps a container that references a class object.

  The template argument C is the container type, ContainerTraits must be
  specialized on C or else a compile error will result.
*/
template <class C>
class UserdataShared : public Userdata
{
private:
  UserdataShared (UserdataShared <C> const&);
  UserdataShared <C>& operator= (UserdataShared <C> const&);

  typedef typename TypeTraits::removeConst <
    typename ContainerTraits <C>::Type>::Type T;

  C m_c;

private:
  ~UserdataShared ()
  {
  }

public:
  /**
    Construct from a container to the class or a derived class.

    @tparam U A container type.
    @param  u A container object reference.
  */
  template <class U>
  explicit UserdataShared (U const& u) : m_c (u)
  {
    m_p = const_cast <void*> (reinterpret_cast <void const*> (
        (ContainerTraits <C>::get (m_c))));
  }

  /**
    Construct from a pointer to the class or a derived class.

    @tparam U A container type.
    @param  u A container object pointer.
  */
  template <class U>
  explicit UserdataShared (U* u) : m_c (u)
  {
    m_p = const_cast <void*> (reinterpret_cast <void const*> (
        (ContainerTraits <C>::get (m_c))));
  }
};

//----------------------------------------------------------------------------
//
// SFINAE helpers.
//

// non-const objects
template <class C, bool makeObjectConst>
struct UserdataSharedHelper
{
  typedef typename TypeTraits::removeConst <
    typename ContainerTraits <C>::Type>::Type T;

  static void push (lua_State* L, C const& c)
  {
    if (ContainerTraits <C>::get (c) != 0)
    {
      new (lua_newuserdata (L, sizeof (UserdataShared <C>))) UserdataShared <C> (c);
      lua_rawgetp (L, LUA_REGISTRYINDEX, getClassRegistryKey <T> ());
      // If this goes off it means the class T is unregistered!
      assert (lua_istable (L, -1));
      lua_setmetatable (L, -2);
    }
    else
    {
      lua_pushnil (L);
    }
  }

  static void push (lua_State* L, T* const t)
  {
    if (t)
    {
      new (lua_newuserdata (L, sizeof (UserdataShared <C>))) UserdataShared <C> (t);
      lua_rawgetp (L, LUA_REGISTRYINDEX, getClassRegistryKey <T> ());
      // If this goes off it means the class T is unregistered!
      assert (lua_istable (L, -1));
      lua_setmetatable (L, -2);
    }
    else
    {
      lua_pushnil (L);
    }
  }
};

// const objects
template <class C>
struct UserdataSharedHelper <C, true>
{
  typedef typename TypeTraits::removeConst <
    typename ContainerTraits <C>::Type>::Type T;

  static void push (lua_State* L, C const& c)
  {
    if (ContainerTraits <C>::get (c) != 0)
    {
      new (lua_newuserdata (L, sizeof (UserdataShared <C>))) UserdataShared <C> (c);
      lua_rawgetp (L, LUA_REGISTRYINDEX, getConstRegistryKey <T> ());
      // If this goes off it means the class T is unregistered!
      assert (lua_istable (L, -1));
      lua_setmetatable (L, -2);
    }
    else
    {
      lua_pushnil (L);
    }
  }

  static void push (lua_State* L, T* const t)
  {
    if (t)
    {
      new (lua_newuserdata (L, sizeof (UserdataShared <C>))) UserdataShared <C> (t);
      lua_rawgetp (L, LUA_REGISTRYINDEX, getConstRegistryKey <T> ());
      // If this goes off it means the class T is unregistered!
      assert (lua_istable (L, -1));
      lua_setmetatable (L, -2);
    }
    else
    {
      lua_pushnil (L);
    }
  }
};

/**
  Pass by container.

  The container controls the object lifetime. Typically this will be a
  lifetime shared by C++ and Lua using a reference count. Because of type
  erasure, containers like std::shared_ptr will not work. Containers must
  either be of the intrusive variety, or in the style of the RefCountedPtr
  type provided by LuaBridge (that uses a global hash table).
*/
template <class C, bool byContainer>
struct StackHelper
{
  static void push (lua_State* L, C const& c)
  {
    UserdataSharedHelper <C,
      TypeTraits::isConst <typename ContainerTraits <C>::Type>::value>::push (L, c);
  }

  typedef typename TypeTraits::removeConst <
    typename ContainerTraits <C>::Type>::Type T;

  static C get (lua_State* L, int index)
  {
    return Userdata::get <T> (L, index, true);
  }
};

/**
  Pass by value.

  Lifetime is managed by Lua. A C++ function which accesses a pointer or
  reference to an object outside the activation record in which it was
  retrieved may result in undefined behavior if Lua garbage collected it.
*/
template <class T>
struct StackHelper <T, false>
{
  static inline void push (lua_State* L, T const& t)
  {
    UserdataValue <T>::push (L, t);
  }

  static inline T const& get (lua_State* L, int index)
  {
    return *Userdata::get <T> (L, index, true);
  }
};


//------------------------------------------------------------------------------
/**
  Lua stack conversions for pointers and references to class objects.

  Lifetime is managed by C++. Lua code which remembers a reference to the
  value may result in undefined behavior if C++ destroys the object. The
  handling of the const and volatile qualifiers happens in UserdataPtr.
*/

template <class C, bool byContainer>
struct RefStackHelper
{
  typedef C return_type;

  static inline void push (lua_State* L, C const& t)
  {
    UserdataSharedHelper <C,
      TypeTraits::isConst <typename ContainerTraits <C>::Type>::value>::push (L, t);
  }

  typedef typename TypeTraits::removeConst <
    typename ContainerTraits <C>::Type>::Type T;

  static return_type get (lua_State* L, int index)
  {
    return Userdata::get <T> (L, index, true);
  }
};

template <class T>
struct RefStackHelper <T, false>
{
  typedef T& return_type;

  static void push (lua_State* L, T const& t)
  {
    UserdataPtr::push (L, &t);
  }

  static return_type get (lua_State* L, int index)
  {
    T* t = Userdata::get <T> (L, index, true);

    if (!t)
      luaL_error (L, "nil passed to reference");
    return *t;
  }
};


/**
 * Voider class template. Used to force a comiler to instantiate
 * an otherwise probably unused template parameter type T.
 * See the C++20 std::void_t <> for details.
 */
template <class T>
struct Void
{
  typedef void Type;
};


/**
 * Trait class that selects whether to return a user registered
 * class object by value or by reference.
 */

template <class T, class Enabler = void>
struct UserdataGetter
{
  typedef T* ReturnType;

  static ReturnType get (lua_State* L, int index)
  {
    return Userdata::get <T> (L, index, false);
  }
};

template <class T>
struct UserdataGetter <T, typename Void <T (*) ()>::Type>
{
  typedef T ReturnType;

  static ReturnType get (lua_State* L, int index)
  {
    return StackHelper <T, TypeTraits::isContainer <T>::value>::get (L, index);
  }
};

} // namespace detail

//==============================================================================

/**
  Lua stack conversions for class objects passed by value.
*/
template <class T>
struct Stack
{
  typedef void IsUserdata;

  typedef detail::UserdataGetter <T> Getter;
  typedef typename Getter::ReturnType ReturnType;

  static void push (lua_State* L, T const& value)
  {
    using namespace detail;
    StackHelper <T, TypeTraits::isContainer <T>::value>::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Getter::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return detail::Userdata::isInstance <T> (L, index);
  }
};

namespace detail {

/**
 * Trait class indicating whether the parameter type must be
 * a user registered class. The trait checks the existence of
 * member type Stack::IsUserdata specialization for detection.
 */
template <class T, class Enable = void>
struct IsUserdata
{
  static const bool value = false;
};

template <class T>
struct IsUserdata <T, typename Void <typename Stack <T>::IsUserdata>::Type>
{
  static const bool value = true;
};


/**
 * Trait class that selects a specific push/get implemenation.
 */
template <class T, bool isUserdata>
struct StackOpSelector;

// pointer
template <class T>
struct StackOpSelector <T*, true>
{
  typedef T* ReturnType;

  static void push (lua_State* L, T* value)
  {
    UserdataPtr::push (L, value);
  }

  static T* get (lua_State* L, int index)
  {
    return Userdata::get <T> (L, index, false);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Userdata::isInstance <T> (L, index);
  }
};

// pointer to const
template <class T>
struct StackOpSelector <const T*, true>
{
  typedef const T* ReturnType;

  static void push (lua_State* L, const T* value)
  {
    UserdataPtr::push (L, value);
  }

  static const T* get (lua_State* L, int index)
  {
    return Userdata::get <T> (L, index, true);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Userdata::isInstance <T> (L, index);
  }
};

// reference
template <class T>
struct StackOpSelector <T&, true>
{
  typedef RefStackHelper <T, TypeTraits::isContainer <T>::value> Helper;
  typedef typename Helper::return_type ReturnType;

  static void push (lua_State* L, T& value)
  {
    UserdataPtr::push (L, &value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Userdata::isInstance <T> (L, index);
  }
};

// reference to const
template <class T>
struct StackOpSelector <const T&, true>
{
  typedef RefStackHelper <T, TypeTraits::isContainer <T>::value> Helper;
  typedef typename Helper::return_type ReturnType;

  static void push (lua_State* L, const T& value)
  {
    Helper::push (L, value);
  }

  static ReturnType get (lua_State* L, int index)
  {
    return Helper::get (L, index);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return Userdata::isInstance <T> (L, index);
  }
};

} // namespace detail

} // namespace luabridge
