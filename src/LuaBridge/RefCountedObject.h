//==============================================================================
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2004-11 by Raw Material Software Ltd.

  This is a derivative work used by permission from part of
  JUCE, available at http://www.rawaterialsoftware.com

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

  This file incorporates work covered by the following copyright and
  permission notice:  

    This file is part of the JUCE library - "Jules' Utility Class Extensions"
    Copyright 2004-11 by Raw Material Software Ltd.
*/
//==============================================================================

#ifndef LUABRIDGE_REFCOUNTEDOBJECT_HEADER
#define LUABRIDGE_REFCOUNTEDOBJECT_HEADER

//#define LUABRIDGE_COMPILER_SUPPORTS_MOVE_SEMANTICS 1

#include <cassert>

//==============================================================================
/**
  Adds reference-counting to an object.

  To add reference-counting to a class, derive it from this class, and
  use the RefCountedObjectPtr class to point to it.

  e.g. @code
  class MyClass : public RefCountedObjectType
  {
      void foo();

      // This is a neat way of declaring a typedef for a pointer class,
      // rather than typing out the full templated name each time..
      typedef RefCountedObjectPtr<MyClass> Ptr;
  };

  MyClass::Ptr p = new MyClass();
  MyClass::Ptr p2 = p;
  p = 0;
  p2->foo();
  @endcode

  Once a new RefCountedObjectType has been assigned to a pointer, be
  careful not to delete the object manually.
*/
template <class CounterType>
class RefCountedObjectType
{
public:
  //==============================================================================
  /** Increments the object's reference count.

      This is done automatically by the smart pointer, but is public just
      in case it's needed for nefarious purposes.
  */
  inline void incReferenceCount() const
  {
    ++refCount;
  }

  /** Decreases the object's reference count.

      If the count gets to zero, the object will be deleted.
  */
  inline void decReferenceCount() const
  {
    assert (getReferenceCount() > 0);

    if (--refCount == 0)
      delete this;
  }

  /** Returns the object's current reference count. */
  inline int getReferenceCount() const
  {
    return static_cast <int> (refCount);
  }

protected:
  //==============================================================================
  /** Creates the reference-counted object (with an initial ref count of zero). */
  RefCountedObjectType() : refCount ()
  {
  }

  /** Destructor. */
  virtual ~RefCountedObjectType()
  {
    // it's dangerous to delete an object that's still referenced by something else!
    assert (getReferenceCount() == 0);
  }

private:
  //==============================================================================
  CounterType mutable refCount;
};

//==============================================================================

/** Non thread-safe reference counted object.

    This creates a RefCountedObjectType that uses a non-atomic integer
    as the counter.
*/
typedef RefCountedObjectType <int> RefCountedObject;

//==============================================================================
/**
    A smart-pointer class which points to a reference-counted object.

    The template parameter specifies the class of the object you want to point
    to - the easiest way to make a class reference-countable is to simply make
    it inherit from RefCountedObjectType, but if you need to, you could roll
    your own reference-countable class by implementing a pair of methods called
    incReferenceCount() and decReferenceCount().

    When using this class, you'll probably want to create a typedef to
    abbreviate the full templated name - e.g.

    @code
    
    typedef RefCountedObjectPtr <MyClass> MyClassPtr;
    
    @endcode
*/
template <class ReferenceCountedObjectClass>
class RefCountedObjectPtr
{
public:
  /** The class being referenced by this pointer. */
  typedef ReferenceCountedObjectClass ReferencedType;

  //==============================================================================
  /** Creates a pointer to a null object. */
  inline RefCountedObjectPtr() : referencedObject (0)
  {
  }

  /** Creates a pointer to an object.

      This will increment the object's reference-count if it is non-null.
  */
  inline RefCountedObjectPtr (ReferenceCountedObjectClass* const refCountedObject)
      : referencedObject (refCountedObject)
  {
    if (refCountedObject != 0)
        refCountedObject->incReferenceCount();
  }

  /** Copies another pointer.
      This will increment the object's reference-count (if it is non-null).
  */
  inline RefCountedObjectPtr (const RefCountedObjectPtr& other)
      : referencedObject (other.referencedObject)
  {
    if (referencedObject != 0)
        referencedObject->incReferenceCount();
  }

#if LUABRIDGE_COMPILER_SUPPORTS_MOVE_SEMANTICS
  /** Takes-over the object from another pointer. */
  inline RefCountedObjectPtr (RefCountedObjectPtr&& other)
      : referencedObject (other.referencedObject)
  {
    other.referencedObject = 0;
  }
#endif

  /** Copies another pointer.
      This will increment the object's reference-count (if it is non-null).
  */
  template <class DerivedClass>
  inline RefCountedObjectPtr (const RefCountedObjectPtr<DerivedClass>& other)
      : referencedObject (static_cast <ReferenceCountedObjectClass*> (other.getObject()))
  {
    if (referencedObject != 0)
      referencedObject->incReferenceCount();
  }

  /** Changes this pointer to point at a different object.

      The reference count of the old object is decremented, and it might be
      deleted if it hits zero. The new object's count is incremented.
  */
  RefCountedObjectPtr& operator= (const RefCountedObjectPtr& other)
  {
    return operator= (other.referencedObject);
  }

  /** Changes this pointer to point at a different object.

      The reference count of the old object is decremented, and it might be
      deleted if it hits zero. The new object's count is incremented.
  */
  template <class DerivedClass>
  RefCountedObjectPtr& operator= (const RefCountedObjectPtr<DerivedClass>& other)
  {
    return operator= (static_cast <ReferenceCountedObjectClass*> (other.getObject()));
  }

#if LUABRIDGE_COMPILER_SUPPORTS_MOVE_SEMANTICS
  /** Takes-over the object from another pointer. */
  RefCountedObjectPtr& operator= (RefCountedObjectPtr&& other)
  {
    std::swap (referencedObject, other.referencedObject);
    return *this;
  }
#endif

  /** Changes this pointer to point at a different object.

      The reference count of the old object is decremented, and it might be
      deleted if it hits zero. The new object's count is incremented.
  */
  RefCountedObjectPtr& operator= (ReferenceCountedObjectClass* const newObject)
  {
    if (referencedObject != newObject)
    {
      if (newObject != 0)
        newObject->incReferenceCount();

      ReferenceCountedObjectClass* const oldObject = referencedObject;
      referencedObject = newObject;

      if (oldObject != 0)
        oldObject->decReferenceCount();
    }

    return *this;
  }

  /** Destructor.

      This will decrement the object's reference-count, and may delete it if it
      gets to zero.
  */
  inline ~RefCountedObjectPtr()
  {
    if (referencedObject != 0)
      referencedObject->decReferenceCount();
  }

  /** Returns the object that this pointer references.
      The pointer returned may be zero, of course.
  */
  inline operator ReferenceCountedObjectClass*() const
  {
    return referencedObject;
  }

  // the -> operator is called on the referenced object
  inline ReferenceCountedObjectClass* operator->() const
  {
    return referencedObject;
  }

  /** Returns the object that this pointer references.
      The pointer returned may be zero, of course.
  */
  inline ReferenceCountedObjectClass* getObject() const
  {
    return referencedObject;
  }

private:
  //==============================================================================
  ReferenceCountedObjectClass* referencedObject;
};

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator== (const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1, ReferenceCountedObjectClass* const object2)
{
    return object1.getObject() == object2;
}

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator== (const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1, const RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1.getObject() == object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator== (ReferenceCountedObjectClass* object1, RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1 == object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator!= (const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1, const ReferenceCountedObjectClass* object2)
{
    return object1.getObject() != object2;
}

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator!= (const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1, RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1.getObject() != object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template <class ReferenceCountedObjectClass>
bool operator!= (ReferenceCountedObjectClass* object1, RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1 != object2.getObject();
}

//==============================================================================

namespace luabridge
{

// forward declaration
template <class T>
struct ContainerTraits;

template <class T>
struct ContainerTraits <RefCountedObjectPtr <T> >
{
  typedef T Type;

  static T* get (RefCountedObjectPtr <T> const& c)
  {
    return c.getObject ();
  }
};

}

//==============================================================================

#endif

