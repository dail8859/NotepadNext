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

#include <LuaBridge/RefCountedObject.h>

#include <unordered_map>

namespace luabridge {

namespace detail {

//==============================================================================
/**
  Support for our RefCountedPtr.
*/
struct RefCountedPtrBase
{
  // Declaration of container for the refcounts
  typedef std::unordered_map <const void *, int> RefCountsType;

protected:
  RefCountsType& getRefCounts () const
  {
    static RefCountsType refcounts;
    return refcounts ;
  }
};

} // namespace detail

//==============================================================================
/**
  A reference counted smart pointer.

  The api is compatible with boost::RefCountedPtr and std::RefCountedPtr, in the
  sense that it implements a strict subset of the functionality.

  This implementation uses a hash table to look up the reference count
  associated with a particular pointer.

  @tparam T The class type.

  @todo Decompose RefCountedPtr using a policy. At a minimum, the underlying
        reference count should be policy based (to support atomic operations)
        and the delete behavior should be policy based (to support custom
        disposal methods).

  @todo Provide an intrusive version of RefCountedPtr.
*/
template <class T>
class RefCountedPtr : private detail::RefCountedPtrBase
{
public:
  template <typename Other>
  struct rebind
  {
    typedef RefCountedPtr <Other> other;
  };

  /** Construct as nullptr or from existing pointer to T.

      @param p The optional, existing pointer to assign from.
  */
  RefCountedPtr (T* p = 0) : m_p (p)
  {
    ++getRefCounts () [m_p];
  }

  /** Construct from another RefCountedPtr.

      @param rhs The RefCountedPtr to assign from.
  */
  RefCountedPtr (RefCountedPtr <T> const& rhs) : m_p (rhs.get())
  {
    ++getRefCounts () [m_p];
  }

  /** Construct from a RefCountedPtr of a different type.

      @invariant A pointer to U must be convertible to a pointer to T.

      @tparam U   The other object type.
      @param  rhs The RefCountedPtr to assign from.
  */
  template <typename U>
  RefCountedPtr (RefCountedPtr <U> const& rhs) : m_p (static_cast <T*> (rhs.get()))
  {
    ++getRefCounts () [m_p];
  }

  /** Release the object.

      If there are no more references then the object is deleted.
  */
  ~RefCountedPtr ()
  {
    reset();
  }

  /** Assign from another RefCountedPtr.

      @param  rhs The RefCountedPtr to assign from.
      @returns     A reference to the RefCountedPtr.
  */
  RefCountedPtr <T>& operator= (RefCountedPtr <T> const& rhs)
  {
    if (m_p != rhs.m_p)
    {
      reset ();
      m_p = rhs.m_p;
      ++getRefCounts () [m_p];
    }
    return *this;
  }

  /** Assign from another RefCountedPtr of a different type.

      @note A pointer to U must be convertible to a pointer to T.

      @tparam U   The other object type.
      @param  rhs The other RefCountedPtr to assign from.
      @returns     A reference to the RefCountedPtr.
  */
  template <typename U>
  RefCountedPtr <T>& operator= (RefCountedPtr <U> const& rhs)
  {
    reset ();
    m_p = static_cast <T*> (rhs.get());
    ++getRefCounts () [m_p];
    return *this;
  }

  /** Retrieve the raw pointer.

      @returns A pointer to the object.
  */
  T* get () const
  {
    return m_p;
  }

  /** Retrieve the raw pointer.

      @returns A pointer to the object.
  */
  T* operator* () const
  {
    return m_p;
  }

  /** Retrieve the raw pointer.

      @returns A pointer to the object.
  */
  T* operator-> () const
  {
    return m_p;
  }

  /** Determine the number of references.

      @note This is not thread-safe.

      @returns The number of active references.
  */
  long use_count () const
  {
    return getRefCounts () [m_p];
  }

  /** Release the pointer.

      The reference count is decremented. If the reference count reaches
      zero, the object is deleted.
  */
  void reset ()
  {
    if (m_p != 0)
    {
      if (--getRefCounts () [m_p] <= 0)
        delete m_p;

      m_p = 0;
    }
  }

private:
  T* m_p;
};

template <class T>
bool operator== (const RefCountedPtr <T>& lhs, const RefCountedPtr <T>& rhs)
{
  return lhs.get () == rhs.get ();
}

template <class T>
bool operator!= (const RefCountedPtr <T>& lhs, const RefCountedPtr <T>& rhs)
{
  return lhs.get() != rhs.get();
}

//==============================================================================

// forward declaration
template <class T>
struct ContainerTraits;

template <class T>
struct ContainerTraits <RefCountedPtr <T> >
{
  typedef T Type;

  static T* get (RefCountedPtr <T> const& c)
  {
    return c.get ();
  }
};

} // namespace luabridge
