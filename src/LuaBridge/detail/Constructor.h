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

#ifndef LUABRIDGE_CONSTRUCTOR_HEADER
#define LUABRIDGE_CONSTRUCTOR_HEADER

/*
* Constructor generators.  These templates allow you to call operator new and
* pass the contents of a type/value list to the Constructor.  Like the
* function pointer containers, these are only defined up to 8 parameters.
*/

/** Constructor generators.

    These templates call operator new with the contents of a type/value
    list passed to the Constructor with up to 8 parameters. Two versions
    of call() are provided. One performs a regular new, the other performs
    a placement new.
*/
template <class T, typename List>
struct Constructor {};

template <class T>
struct Constructor <T, None>
{
  static T* call (TypeListValues <None> const&)
  {
    return new T;
  }
  static T* call (void* mem, TypeListValues <None> const&)
  {
    return new (mem) T;
  }
};

template <class T, class P1>
struct Constructor <T, TypeList <P1> >
{
  static T* call (const TypeListValues<TypeList <P1> > &tvl)
  {
    return new T(tvl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1> > &tvl)
  {
    return new (mem) T(tvl.hd);
  }
};

template <class T, class P1, class P2>
struct Constructor <T, TypeList <P1, TypeList <P2> > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2> > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2> > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3> > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3> > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3> > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3, class P4>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3,
  TypeList <P4> > > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4> > > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4> > > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3, class P4,
  class P5>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3,
  TypeList <P4, TypeList <P5> > > > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5> > > > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5> > > > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3, class P4,
  class P5, class P6>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3,
  TypeList <P4, TypeList <P5, TypeList <P6> > > > > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6> > > > > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6> > > > > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3, class P4,
  class P5, class P6, class P7>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3,
  TypeList <P4, TypeList <P5, TypeList <P6, TypeList <P7> > > > > > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6,
    TypeList <P7> > > > > > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6,
    TypeList <P7> > > > > > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.tl.tl.hd);
  }
};

template <class T, class P1, class P2, class P3, class P4,
  class P5, class P6, class P7, class P8>
struct Constructor <T, TypeList <P1, TypeList <P2, TypeList <P3,
  TypeList <P4, TypeList <P5, TypeList <P6, TypeList <P7, 
  TypeList <P8> > > > > > > > >
{
  static T* call (const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6,
    TypeList <P7, TypeList <P8> > > > > > > > > &tvl)
  {
    return new T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd);
  }
  static T* call (void* mem, const TypeListValues<TypeList <P1, TypeList <P2,
    TypeList <P3, TypeList <P4, TypeList <P5, TypeList <P6,
    TypeList <P7, TypeList <P8> > > > > > > > > &tvl)
  {
    return new (mem) T(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.hd,
      tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd);
  }
};

#endif
