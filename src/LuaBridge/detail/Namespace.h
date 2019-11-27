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

/** Provides C++ to Lua registration capabilities.

    This class is not instantiated directly, call `getGlobalNamespace` to start
    the registration process.
*/
class Namespace
{
private:
  Namespace& operator= (Namespace const& other);

  lua_State* const L;
  int mutable m_stackSize;

private:
  //============================================================================
  /**
    Error reporting.

    VF: This function looks handy, why aren't we using it?
  */
#if 0
  static int luaError (lua_State* L, std::string message)
  {
    assert (lua_isstring (L, lua_upvalueindex (1)));
    std::string s;

    // Get information on the caller's caller to format the message,
    // so the error appears to originate from the Lua source.
    lua_Debug ar;
    int result = lua_getstack (L, 2, &ar);
    if (result != 0)
    {
      lua_getinfo (L, "Sl", &ar);
      s = ar.short_src;
      if (ar.currentline != -1)
      {
        // poor mans int to string to avoid <strstrream>.
        lua_pushnumber (L, ar.currentline);
        s = s + ":" + lua_tostring (L, -1) + ": ";
        lua_pop (L, 1);
      }
    }

    s = s + message;

    return luaL_error (L, s.c_str ());
  }
#endif

  //----------------------------------------------------------------------------
  /**
    Pop the Lua stack.
  */
  void pop (int n) const
  {
    if (m_stackSize >= n && lua_gettop (L) >= n)
    {
      lua_pop (L, n);
      m_stackSize -= n;
    }
    else
    {
      throw std::logic_error ("invalid stack");
    }
  }

private:
  /**
    Factored base to reduce template instantiations.
  */
  class ClassBase
  {
  private:
    ClassBase& operator= (ClassBase const& other);

  protected:
    friend class Namespace;

    lua_State* const L;
    int mutable m_stackSize;

  protected:
    //--------------------------------------------------------------------------
    /**
      __index metamethod for a class.

      This implements member functions, data members, and property members.
      Functions are stored in the metatable and const metatable. Data members
      and property members are in the __propget table.

      If the key is not found, the search proceeds up the hierarchy of base
      classes.
    */
    static int indexMetaMethod (lua_State* L)
    {
      int result = 0;

      assert (lua_isuserdata (L, 1));               // warn on security bypass
      lua_getmetatable (L, 1);                      // get metatable for object
      for (;;)
      {
        lua_pushvalue (L, 2);                       // push key arg2
        lua_rawget (L, -2);                         // lookup key in metatable
        if (lua_iscfunction (L, -1))                // ensure its a cfunction
        {
          lua_remove (L, -2);                       // remove metatable
          result = 1;
          break;
        }
        else if (lua_isnil (L, -1))
        {
          lua_pop (L, 1);
        }
        else
        {
          lua_pop (L, 2);
          throw std::logic_error ("not a cfunction");
        }

        rawgetfield (L, -1, "__propget");           // get __propget table
        if (lua_istable (L, -1))                    // ensure it is a table
        {
          lua_pushvalue (L, 2);                     // push key arg2
          lua_rawget (L, -2);                       // lookup key in __propget
          lua_remove (L, -2);                       // remove __propget
          if (lua_iscfunction (L, -1))              // ensure its a cfunction
          {
            lua_remove (L, -2);                     // remove metatable
            lua_pushvalue (L, 1);                   // push class arg1
            lua_call (L, 1, 1);
            result = 1;
            break;
          }
          else if (lua_isnil (L, -1))
          {
            lua_pop (L, 1);
          }
          else
          {
            lua_pop (L, 2);

            // We only put cfunctions into __propget.
            throw std::logic_error ("not a cfunction");
          }
        }
        else
        {
          lua_pop (L, 2);

          // __propget is missing, or not a table.
          throw std::logic_error ("missing __propget table");
        }

        // Repeat the lookup in the __parent metafield,
        // or return nil if the field doesn't exist.
        rawgetfield (L, -1, "__parent");
        if (lua_istable (L, -1))
        {
          // Remove metatable and repeat the search in __parent.
          lua_remove (L, -2);
        }
        else if (lua_isnil (L, -1))
        {
          result = 1;
          break;
        }
        else
        {
          lua_pop (L, 2);

          throw std::logic_error ("__parent is not a table");
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    /**
      __newindex metamethod for classes.

      This supports writable variables and properties on class objects. The
      corresponding object is passed in the first parameter to the set function.
    */
    static int newindexMetaMethod (lua_State* L)
    {
      int result = 0;

      lua_getmetatable (L, 1);

      for (;;)
      {
        // Check __propset
        rawgetfield (L, -1, "__propset");
        if (!lua_isnil (L, -1))
        {
          lua_pushvalue (L, 2);
          lua_rawget (L, -2);
          if (!lua_isnil (L, -1))
          {
            // found it, call the setFunction.
            assert (lua_isfunction (L, -1));
            lua_pushvalue (L, 1);
            lua_pushvalue (L, 3);
            lua_call (L, 2, 0);
            result = 0;
            break;
          }
          lua_pop (L, 1);
        }
        lua_pop (L, 1);

        // Repeat the lookup in the __parent metafield.
        rawgetfield (L, -1, "__parent");
        if (lua_isnil (L, -1))
        {
          // Either the property or __parent must exist.
          result = luaL_error (L,
            "no member named '%s'", lua_tostring (L, 2));
        }
        lua_remove (L, -2);
      }

      return result;
    }

    //--------------------------------------------------------------------------
    /**
      Create the const table.
    */
    void createConstTable (char const* name)
    {
      lua_newtable (L);
      lua_pushvalue (L, -1);
      lua_setmetatable (L, -2);
      lua_pushboolean (L, 1);
      lua_rawsetp (L, -2, getIdentityKey ());
      lua_pushstring (L, (std::string ("const ") + name).c_str ());
      rawsetfield (L, -2, "__type");
      lua_pushcfunction (L, &indexMetaMethod);
      rawsetfield (L, -2, "__index");
      lua_pushcfunction (L, &newindexMetaMethod);
      rawsetfield (L, -2, "__newindex");
      lua_newtable (L);
      rawsetfield (L, -2, "__propget");
      
      if (Security::hideMetatables ())
      {
        lua_pushnil (L);
        rawsetfield (L, -2, "__metatable");
      }
    }

    //--------------------------------------------------------------------------
    /**
      Create the class table.

      The Lua stack should have the const table on top.
    */
    void createClassTable (char const* name)
    {
      lua_newtable (L);
      lua_pushvalue (L, -1);
      lua_setmetatable (L, -2);
      lua_pushboolean (L, 1);
      lua_rawsetp (L, -2, getIdentityKey ());
      lua_pushstring (L, name);
      rawsetfield (L, -2, "__type");
      lua_pushcfunction (L, &indexMetaMethod);
      rawsetfield (L, -2, "__index");
      lua_pushcfunction (L, &newindexMetaMethod);
      rawsetfield (L, -2, "__newindex");
      lua_newtable (L);
      rawsetfield (L, -2, "__propget");
      lua_newtable (L);
      rawsetfield (L, -2, "__propset");

      lua_pushvalue (L, -2);
      rawsetfield (L, -2, "__const"); // point to const table

      lua_pushvalue (L, -1);
      rawsetfield (L, -3, "__class"); // point const table to class table

      if (Security::hideMetatables ())
      {
        lua_pushnil (L);
        rawsetfield (L, -2, "__metatable");
      }
    }

    //--------------------------------------------------------------------------
    /**
      Create the static table.

      The Lua stack should have:
        -1 class table
        -2 const table
        -3 enclosing namespace
    */
    void createStaticTable (char const* name)
    {
      lua_newtable (L);
      lua_newtable (L);
      lua_pushvalue (L, -1);
      lua_setmetatable (L, -3);
      lua_insert (L, -2);
      rawsetfield (L, -5, name);

#if 0
      lua_pushlightuserdata (L, this);
      lua_pushcclosure (L, &tostringMetaMethod, 1);
      rawsetfield (L, -2, "__tostring");
#endif
      lua_pushcfunction (L, &CFunc::indexMetaMethod);
      rawsetfield (L, -2, "__index");
      lua_pushcfunction (L, &CFunc::newindexMetaMethod);
      rawsetfield (L, -2, "__newindex");
      lua_newtable (L);
      rawsetfield (L, -2, "__propget");
      lua_newtable (L);
      rawsetfield (L, -2, "__propset");

      lua_pushvalue (L, -2);
      rawsetfield (L, -2, "__class"); // point to class table

      if (Security::hideMetatables ())
      {
        lua_pushnil (L);
        rawsetfield (L, -2, "__metatable");
      }
    }

    //==========================================================================
    /**
      lua_CFunction to construct a class object wrapped in a container.
    */
    template <class Params, class C>
    static int ctorContainerProxy (lua_State* L)
    {
      typedef typename ContainerTraits <C>::Type T;
      ArgList <Params, 2> args (L);
      T* const p = Constructor <T, Params>::call (args);
      UserdataSharedHelper <C, false>::push (L, p);
      return 1;
    }

    //--------------------------------------------------------------------------
    /**
      lua_CFunction to construct a class object in-place in the userdata.
    */
    template <class Params, class T>
    static int ctorPlacementProxy (lua_State* L)
    {
      ArgList <Params, 2> args (L);
      Constructor <T, Params>::call (UserdataValue <T>::place (L), args);
      return 1;
    }

    //--------------------------------------------------------------------------
    /**
      Pop the Lua stack.
    */
    void pop (int n) const
    {
      if (m_stackSize >= n && lua_gettop (L) >= n)
      {
        lua_pop (L, n);
        m_stackSize -= n;
      }
      else
      {
        throw std::logic_error ("invalid stack");
      }
    }

  public:
    //--------------------------------------------------------------------------
    explicit ClassBase (lua_State* L_)
      : L (L_)
      , m_stackSize (0)
    {
    }

    //--------------------------------------------------------------------------
    /**
      Copy Constructor.
    */
    ClassBase (ClassBase const& other)
      : L (other.L)
      , m_stackSize (0)
    {
      m_stackSize = other.m_stackSize;
      other.m_stackSize = 0;
    }

    ~ClassBase ()
    {
      pop (m_stackSize);
    }
  };

  //============================================================================
  //
  // Class
  //
  //============================================================================
  /**
    Provides a class registration in a lua_State.

    After contstruction the Lua stack holds these objects:
      -1 static table
      -2 class table
      -3 const table
      -4 (enclosing namespace)
  */
  template <class T>
  class Class : public ClassBase
  {
  public:
    //==========================================================================
    /**
      Register a new class or add to an existing class registration.
    */
    Class (char const* name, Namespace const* parent) : ClassBase (parent->L)
    {
      m_stackSize = parent->m_stackSize + 3;
      parent->m_stackSize = 0;

      assert (lua_istable (L, -1));
      rawgetfield (L, -1, name);
      
      if (lua_isnil (L, -1))
      {
        lua_pop (L, 1);

        createConstTable (name);
        lua_pushcfunction (L, &CFunc::gcMetaMethod <T>);
        rawsetfield (L, -2, "__gc");

        createClassTable (name);
        lua_pushcfunction (L, &CFunc::gcMetaMethod <T>);
        rawsetfield (L, -2, "__gc");

        createStaticTable (name);

        // Map T back to its tables.
        lua_pushvalue (L, -1);
        lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getStaticKey ());
        lua_pushvalue (L, -2);
        lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getClassKey ());
        lua_pushvalue (L, -3);
        lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getConstKey ());
      }
      else
      {
        rawgetfield (L, -1, "__class");
        rawgetfield (L, -1, "__const");

        // Reverse the top 3 stack elements
        lua_insert (L, -3);
        lua_insert (L, -2);
      }
    }

    //==========================================================================
    /**
      Derive a new class.
    */
    Class (char const* name, Namespace const* parent, void const* const staticKey)
      : ClassBase (parent->L)
    {
      m_stackSize = parent->m_stackSize + 3;
      parent->m_stackSize = 0;

      assert (lua_istable (L, -1));

      createConstTable (name);
      lua_pushcfunction (L, &CFunc::gcMetaMethod <T>);
      rawsetfield (L, -2, "__gc");

      createClassTable (name);
      lua_pushcfunction (L, &CFunc::gcMetaMethod <T>);
      rawsetfield (L, -2, "__gc");

      createStaticTable (name);

      lua_rawgetp (L, LUA_REGISTRYINDEX, staticKey);
      assert (lua_istable (L, -1));
      rawgetfield (L, -1, "__class");
      assert (lua_istable (L, -1));
      rawgetfield (L, -1, "__const");
      assert (lua_istable (L, -1));

      rawsetfield (L, -6, "__parent");
      rawsetfield (L, -4, "__parent");
      rawsetfield (L, -2, "__parent");

      lua_pushvalue (L, -1);
      lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getStaticKey ());
      lua_pushvalue (L, -2);
      lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getClassKey ());
      lua_pushvalue (L, -3);
      lua_rawsetp (L, LUA_REGISTRYINDEX, ClassInfo <T>::getConstKey ());
    }

    //--------------------------------------------------------------------------
    /**
      Continue registration in the enclosing namespace.
    */
    Namespace endClass ()
    {
      return Namespace (this);
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static data member.
    */
    template <class U>
    Class <T>& addStaticData (char const* name, U* pu, bool isWritable = true)
    {
      assert (lua_istable (L, -1));

      rawgetfield (L, -1, "__propget");
      assert (lua_istable (L, -1));
      lua_pushlightuserdata (L, pu);
      lua_pushcclosure (L, &CFunc::getVariable <U>, 1);
      rawsetfield (L, -2, name);
      lua_pop (L, 1);

      rawgetfield (L, -1, "__propset");
      assert (lua_istable (L, -1));
      if (isWritable)
      {
        lua_pushlightuserdata (L, pu);
        lua_pushcclosure (L, &CFunc::setVariable <U>, 1);
      }
      else
      {
        lua_pushstring (L, name);
        lua_pushcclosure (L, &CFunc::readOnlyError, 1);
      }
      rawsetfield (L, -2, name);
      lua_pop (L, 1);

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static property member.

      If the set function is null, the property is read-only.
    */
    template <class U>
    Class <T>& addStaticProperty (char const* name, U (*get)(), void (*set)(U) = 0)
    {
      typedef U (*get_t)();
      typedef void (*set_t)(U);
      
      assert (lua_istable (L, -1));

      rawgetfield (L, -1, "__propget");
      assert (lua_istable (L, -1));
      new (lua_newuserdata (L, sizeof (get))) get_t (get);
      lua_pushcclosure (L, &CFunc::Call <U (*) (void)>::f, 1);
      rawsetfield (L, -2, name);
      lua_pop (L, 1);

      rawgetfield (L, -1, "__propset");
      assert (lua_istable (L, -1));
      if (set != 0)
      {
        new (lua_newuserdata (L, sizeof (set))) set_t (set);
        lua_pushcclosure (L, &CFunc::Call <void (*) (U)>::f, 1);
      }
      else
      {
        lua_pushstring (L, name);
        lua_pushcclosure (L, &CFunc::readOnlyError, 1);
      }
      rawsetfield (L, -2, name);
      lua_pop (L, 1);

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static member function.
    */
    template <class FP>
    Class <T>& addStaticFunction (char const* name, FP const fp)
    {
      new (lua_newuserdata (L, sizeof (fp))) FP (fp);
      lua_pushcclosure (L, &CFunc::Call <FP>::f, 1);
      rawsetfield (L, -2, name);

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a lua_CFunction.
    */
    Class <T>& addStaticCFunction (char const* name, int (*const fp)(lua_State*))
    {
      lua_pushcfunction (L, fp);
      rawsetfield (L, -2, name);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a data member.
    */
    template <class U>
    Class <T>& addData (char const* name, const U T::* mp, bool isWritable = true)
    {
      typedef const U T::*mp_t;

      // Add to __propget in class and const tables.
      {
        rawgetfield (L, -2, "__propget");
        rawgetfield (L, -4, "__propget");
        new (lua_newuserdata (L, sizeof (mp_t))) mp_t (mp);
        lua_pushcclosure (L, &CFunc::getProperty <T,U>, 1);
        lua_pushvalue (L, -1);
        rawsetfield (L, -4, name);
        rawsetfield (L, -2, name);
        lua_pop (L, 2);
      }

      if (isWritable)
      {
        // Add to __propset in class table.
        rawgetfield (L, -2, "__propset");
        assert (lua_istable (L, -1));
        new (lua_newuserdata (L, sizeof (mp_t))) mp_t (mp);
        lua_pushcclosure (L, &CFunc::setProperty <T,U>, 1);
        rawsetfield (L, -2, name);
        lua_pop (L, 1);
      }

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member.
    */
    template <class TG, class TS>
    Class <T>& addProperty (char const* name, TG (T::* get) () const, void (T::* set) (TS))
    {
      // Add to __propget in class and const tables.
      {
        rawgetfield (L, -2, "__propget");
        rawgetfield (L, -4, "__propget");
        typedef TG (T::*get_t) () const;
        new (lua_newuserdata (L, sizeof (get_t))) get_t (get);
        lua_pushcclosure (L, &CFunc::CallConstMember <get_t>::f, 1);
        lua_pushvalue (L, -1);
        rawsetfield (L, -4, name);
        rawsetfield (L, -2, name);
        lua_pop (L, 2);
      }

      {
        // Add to __propset in class table.
        rawgetfield (L, -2, "__propset");
        assert (lua_istable (L, -1));
        typedef void (T::* set_t) (TS);
        new (lua_newuserdata (L, sizeof (set_t))) set_t (set);
        lua_pushcclosure (L, &CFunc::CallMember <set_t>::f, 1);
        rawsetfield (L, -2, name);
        lua_pop (L, 1);
      }

      return *this;
    }

    // read-only
    template <class TG>
    Class <T>& addProperty (char const* name, TG (T::* get) () const)
    {
      // Add to __propget in class and const tables.
      rawgetfield (L, -2, "__propget");
      rawgetfield (L, -4, "__propget");
      typedef TG (T::*get_t) () const;
      new (lua_newuserdata (L, sizeof (get_t))) get_t (get);
      lua_pushcclosure (L, &CFunc::CallConstMember <get_t>::f, 1);
      lua_pushvalue (L, -1);
      rawsetfield (L, -4, name);
      rawsetfield (L, -2, name);
      lua_pop (L, 2);

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member, by proxy.

      When a class is closed for modification and does not provide (or cannot
      provide) the function signatures necessary to implement get or set for
      a property, this will allow non-member functions act as proxies.

      Both the get and the set functions require a T const* and T* in the first
      argument respectively.
    */
    template <class TG, class TS>
    Class <T>& addProperty (char const* name, TG (*get) (T const*), void (*set) (T*, TS))
    {
      // Add to __propget in class and const tables.
      {
        rawgetfield (L, -2, "__propget");
        rawgetfield (L, -4, "__propget");
        typedef TG (*get_t) (T const*);
        new (lua_newuserdata (L, sizeof (get_t))) get_t (get);
        lua_pushcclosure (L, &CFunc::Call <get_t>::f, 1);
        lua_pushvalue (L, -1);
        rawsetfield (L, -4, name);
        rawsetfield (L, -2, name);
        lua_pop (L, 2);
      }

      if (set != 0)
      {
        // Add to __propset in class table.
        rawgetfield (L, -2, "__propset");
        assert (lua_istable (L, -1));
        typedef void (*set_t) (T*, TS);
        new (lua_newuserdata (L, sizeof (set_t))) set_t (set);
        lua_pushcclosure (L, &CFunc::Call <set_t>::f, 1);
        rawsetfield (L, -2, name);
        lua_pop (L, 1);
      }

      return *this;
    }

    // read-only
    template <class TG, class TS>
    Class <T>& addProperty (char const* name, TG (*get) (T const*))
    {
      // Add to __propget in class and const tables.
      rawgetfield (L, -2, "__propget");
      rawgetfield (L, -4, "__propget");
      typedef TG (*get_t) (T const*);
      new (lua_newuserdata (L, sizeof (get_t))) get_t (get);
      lua_pushcclosure (L, &CFunc::Call <get_t>::f, 1);
      lua_pushvalue (L, -1);
      rawsetfield (L, -4, name);
      rawsetfield (L, -2, name);
      lua_pop (L, 2);

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member function.
    */
    template <class MemFn>
    Class <T>& addFunction (char const* name, MemFn mf)
    {
      CFunc::CallMemberFunctionHelper <MemFn, FuncTraits <MemFn>::isConstMemberFunction>::add (L, name, mf);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member lua_CFunction.
    */
    Class <T>& addCFunction (char const* name, int (T::*mfp)(lua_State*))
    {
      typedef int (T::*MFP)(lua_State*);
      assert (lua_istable (L, -1));
      new (lua_newuserdata (L, sizeof (mfp))) MFP (mfp);
      lua_pushcclosure (L, &CFunc::CallMemberCFunction <T>::f, 1);
      rawsetfield (L, -3, name); // class table

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a const member lua_CFunction.
    */
    Class <T>& addCFunction (char const* name, int (T::*mfp)(lua_State*) const)
    {
      typedef int (T::*MFP)(lua_State*) const;
      assert (lua_istable (L, -1));
      new (lua_newuserdata (L, sizeof (mfp))) MFP (mfp);
      lua_pushcclosure (L, &CFunc::CallConstMemberCFunction <T>::f, 1);
      lua_pushvalue (L, -1);
      rawsetfield (L, -5, name); // const table
      rawsetfield (L, -3, name); // class table

      return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a primary Constructor.

      The primary Constructor is invoked when calling the class type table
      like a function.

      The template parameter should be a function pointer type that matches
      the desired Constructor (since you can't take the address of a Constructor
      and pass it as an argument).
    */
    template <class MemFn, class C>
    Class <T>& addConstructor ()
    {
      lua_pushcclosure (L,
        &ctorContainerProxy <typename FuncTraits <MemFn>::Params, C>, 0);
      rawsetfield(L, -2, "__call");

      return *this;
    }

    template <class MemFn>
    Class <T>& addConstructor ()
    {
      lua_pushcclosure (L,
        &ctorPlacementProxy <typename FuncTraits <MemFn>::Params, T>, 0);
      rawsetfield(L, -2, "__call");

      return *this;
    }
  };

private:
  //----------------------------------------------------------------------------
  /**
      Open the global namespace for registrations.
  */
  explicit Namespace (lua_State* L_)
    : L (L_)
    , m_stackSize (0)
  {
    lua_getglobal (L, "_G");
    ++m_stackSize;
  }

  //----------------------------------------------------------------------------
  /**
      Open a namespace for registrations.

      The namespace is created if it doesn't already exist.
      The parent namespace is at the top of the Lua stack.
  */
  Namespace (char const* name, Namespace const* parent)
    : L (parent->L)
    , m_stackSize (0)
  {
    m_stackSize = parent->m_stackSize + 1;
    parent->m_stackSize = 0;

    assert (lua_istable (L, -1));
    rawgetfield (L, -1, name);
    if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);

      lua_newtable (L);
      lua_pushvalue (L, -1);
      lua_setmetatable (L, -2);
      lua_pushcfunction (L, &CFunc::indexMetaMethod);
      rawsetfield (L, -2, "__index");
      lua_pushcfunction (L, &CFunc::newindexMetaMethod);
      rawsetfield (L, -2, "__newindex");
      lua_newtable (L);
      rawsetfield (L, -2, "__propget");
      lua_newtable (L);
      rawsetfield (L, -2, "__propset");
      lua_pushvalue (L, -1);
      rawsetfield (L, -3, name);
#if 0
      lua_pushcfunction (L, &tostringMetaMethod);
      rawsetfield (L, -2, "__tostring");
#endif
    }
  }

  //----------------------------------------------------------------------------
  /**
      Creates a continued registration from a child namespace.
  */
  explicit Namespace (Namespace const* child)
    : L (child->L)
    , m_stackSize (0)
  {
    m_stackSize = child->m_stackSize - 1;
    child->m_stackSize = 1;
    child->pop (1);

    // It is not necessary or valid to call
    // endNamespace() for the global namespace!
    //
    assert (m_stackSize != 0);
  }

  //----------------------------------------------------------------------------
  /**
      Creates a continued registration from a child class.
  */
  explicit Namespace (ClassBase const* child)
    : L (child->L)
    , m_stackSize (0)
  {
    m_stackSize = child->m_stackSize - 3;
    child->m_stackSize = 3;
    child->pop (3);
  }

public:
  //----------------------------------------------------------------------------
  /**
      Copy Constructor.

      Ownership of the stack is transferred to the new object. This happens
      when the compiler emits temporaries to hold these objects while chaining
      registrations across namespaces.
  */
  Namespace (Namespace const& other) : L (other.L)
  {
    m_stackSize = other.m_stackSize;
    other.m_stackSize = 0;
  }

  //----------------------------------------------------------------------------
  /**
      Closes this namespace registration.
  */
  ~Namespace ()
  {
    pop (m_stackSize);
  }

  //----------------------------------------------------------------------------
  /**
      Open the global namespace.
  */
  static Namespace getGlobalNamespace (lua_State* L)
  {
    return Namespace (L);
  }

  //----------------------------------------------------------------------------
  /**
      Open a new or existing namespace for registrations.
  */
  Namespace beginNamespace (char const* name)
  {
    return Namespace (name, this);
  }

  //----------------------------------------------------------------------------
  /**
      Continue namespace registration in the parent.

      Do not use this on the global namespace.
  */
  Namespace endNamespace ()
  {
    return Namespace (this);
  }

  //----------------------------------------------------------------------------
  /**
      Add or replace a variable.
  */
  template <class T>
  Namespace& addVariable (char const* name, T* pt, bool isWritable = true)
  {
    assert (lua_istable (L, -1));

    rawgetfield (L, -1, "__propget");
    assert (lua_istable (L, -1));
    lua_pushlightuserdata (L, pt);
    lua_pushcclosure (L, &CFunc::getVariable <T>, 1);
    rawsetfield (L, -2, name);
    lua_pop (L, 1);

    rawgetfield (L, -1, "__propset");
    assert (lua_istable (L, -1));
    if (isWritable)
    {
      lua_pushlightuserdata (L, pt);
      lua_pushcclosure (L, &CFunc::setVariable <T>, 1);
    }
    else
    {
      lua_pushstring (L, name);
      lua_pushcclosure (L, &CFunc::readOnlyError, 1);
    }
    rawsetfield (L, -2, name);
    lua_pop (L, 1);

    return *this;
  }
  
  //----------------------------------------------------------------------------
  /**
      Add or replace a property.

      If the set function is omitted or null, the property is read-only.
  */
  template <class TG, class TS>
  Namespace& addProperty (char const* name, TG (*get) (), void (*set)(TS) = 0)
  {
    assert (lua_istable (L, -1));

    rawgetfield (L, -1, "__propget");
    assert (lua_istable (L, -1));
    typedef TG (*get_t) ();
    new (lua_newuserdata (L, sizeof (get_t))) get_t (get);
    lua_pushcclosure (L, &CFunc::Call <TG (*) (void)>::f, 1);
    rawsetfield (L, -2, name);
    lua_pop (L, 1);

    rawgetfield (L, -1, "__propset");
    assert (lua_istable (L, -1));
    if (set != 0)
    {
      typedef void (*set_t) (TS);
      new (lua_newuserdata (L, sizeof (set_t))) set_t (set);
      lua_pushcclosure (L, &CFunc::Call <void (*) (TS)>::f, 1);
    }
    else
    {
      lua_pushstring (L, name);
      lua_pushcclosure (L, &CFunc::readOnlyError, 1);
    }
    rawsetfield (L, -2, name);
    lua_pop (L, 1);

    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Add or replace a free function.
  */
  template <class FP>
  Namespace& addFunction (char const* name, FP const fp)
  {
    assert (lua_istable (L, -1));

    new (lua_newuserdata (L, sizeof (fp))) FP (fp);
    lua_pushcclosure (L, &CFunc::Call <FP>::f, 1);
    rawsetfield (L, -2, name);

    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Add or replace a lua_CFunction.
  */
  Namespace& addCFunction (char const* name, int (*const fp)(lua_State*))
  {
    lua_pushcfunction (L, fp);
    rawsetfield (L, -2, name);

    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Open a new or existing class for registrations.
  */
  template <class T>
  Class <T> beginClass (char const* name)
  {
    return Class <T> (name, this);
  }

  //----------------------------------------------------------------------------
  /**
      Derive a new class for registrations.

      To continue registrations for the class later, use beginClass().
      Do not call deriveClass() again.
  */
  template <class T, class U>
  Class <T> deriveClass (char const* name)
  {
    return Class <T> (name, this, ClassInfo <U>::getStaticKey ());
  }
};

//------------------------------------------------------------------------------
/**
    Retrieve the global namespace.

    It is recommended to put your namespace inside the global namespace, and
    then add your classes and functions to it, rather than adding many classes
    and functions directly to the global namespace.
*/
inline Namespace getGlobalNamespace (lua_State* L)
{
  return Namespace::getGlobalNamespace (L);
}
