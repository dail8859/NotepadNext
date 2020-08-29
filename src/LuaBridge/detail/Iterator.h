//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge
  
  Copyright 2018, Dmitry Tarakanov
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

#include <LuaBridge/detail/LuaRef.h>

#include <utility>


namespace luabridge {

/** Allows table iteration.
*/
class Iterator
{
  lua_State* m_L;
  LuaRef m_table;
  LuaRef m_key;
  LuaRef m_value;

  void next ()
  {
    m_table.push ();
    m_key.push ();
    if (lua_next (m_L, -2))
    {
      m_value.pop ();
      m_key.pop ();
    }
    else
    {
      m_key = Nil ();
      m_value = Nil ();
    }
    lua_pop (m_L, 1);
  }

public:
  explicit Iterator (const LuaRef& table, bool isEnd = false)
    : m_L (table.state ())
    , m_table (table)
    , m_key (table.state ()) // m_key is nil
    , m_value (table.state ()) // m_value is nil
  {
    if (!isEnd)
    {
      next (); // get the first (key, value) pair from table
    }
  }

  /// Return an associated Lua state.
  ///
  /// @returns A Lua state.
  ///
  lua_State* state () const
  {
    return m_L;
  }

  /// Dereference the iterator.
  ///
  /// @returns A key-value pair for a current table entry.
  ///
  std::pair <LuaRef, LuaRef> operator* () const
  {
    return std::make_pair (m_key, m_value);
  }

  /// Return the value referred by the iterator.
  ///
  /// @returns A value for the current table entry.
  ///
  LuaRef operator-> () const
  {
    return m_value;
  }

  /// Compare two iterators.
  ///
  /// @param rhs Another iterator.
  /// @returns True if iterators point to the same entry of the same table,
  ///         false otherwise.
  ///
  bool operator!= (const Iterator& rhs) const
  {
    assert (m_L == rhs.m_L);
    return !m_table.rawequal (rhs.m_table) || !m_key.rawequal (rhs.m_key);
  }

  /// Move the iterator to the next table entry.
  ///
  /// @returns This iterator.
  ///
  Iterator& operator++ ()
  {
    if (isNil ())
    {
      // if the iterator reaches the end, do nothing
      return *this;
    }
    else
    {
      next ();
      return *this;
    }
  }

  /// Check if the iterator points after the last table entry.
  ///
  /// @returns True if there are no more table entries to iterate,
  ///         false otherwise.
  ///
  bool isNil () const
  {
    return m_key.isNil ();
  }

  /// Return the key for the current table entry.
  ///
  /// @returns A reference to the entry key.
  ///
  LuaRef key () const
  {
    return m_key;
  }

  /// Return the key for the current table entry.
  ///
  /// @returns A reference to the entry value.
  ///
  LuaRef value () const
  {
    return m_value;
  }

private:
  // Don't use postfix increment, it is less efficient
  Iterator operator++ (int);
};

namespace detail {

class Range
{
  Iterator m_begin;
  Iterator m_end;

public:
  Range (const Iterator& begin, const Iterator& end)
    : m_begin (begin)
    , m_end (end)
  {
  }

  const Iterator& begin () const { return m_begin; }
  const Iterator& end () const { return m_end; }
};

} // namespace detail

/// Return a range for the Lua table reference.
///
/// @returns A range suitable for range-based for statement.
///
inline detail::Range pairs (const LuaRef& table)
{
  return detail::Range (Iterator (table, false), Iterator (table, true));
}

} // namespace luabridge
