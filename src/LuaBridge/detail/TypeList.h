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

  This file incorporates work covered by the following copyright and
  permission notice:  

    The Loki Library
    Copyright (c) 2001 by Andrei Alexandrescu
    This code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
        Patterns Applied". Copyright (c) 2001. Addison-Wesley.
    Permission to use, copy, modify, distribute and sell this software for any 
        purpose is hereby granted without fee, provided that the above copyright 
        notice appear in all copies and that both that copyright notice and this 
        permission notice appear in supporting documentation.
    The author or Addison-Welsey Longman make no representations about the 
        suitability of this software for any purpose. It is provided "as is" 
        without express or implied warranty.
*/
//==============================================================================

/**
  None type means void parameters or return value.
*/
typedef void None;

template <typename Head, typename Tail = None>
struct TypeList
{
};

/**
  A TypeList with actual values.
*/
template <typename List>
struct TypeListValues
{
  static std::string const tostring (bool)
  {
    return "";
  }
};

/**
  TypeListValues recursive template definition.
*/
template <typename Head, typename Tail>
struct TypeListValues <TypeList <Head, Tail> >
{
  Head hd;
  TypeListValues <Tail> tl;

  TypeListValues (Head hd_, TypeListValues <Tail> const& tl_)
    : hd (hd_), tl (tl_)
  {
  }

  static std::string const tostring (bool comma = false)
  {
    std::string s;

    if (comma)
      s = ", ";

    s = s + typeid (Head).name ();

    return s + TypeListValues <Tail>::tostring (true);
  }
};

// Specializations of type/value list for head types that are references and
// const-references.  We need to handle these specially since we can't count
// on the referenced object hanging around for the lifetime of the list.

template <typename Head, typename Tail>
struct TypeListValues <TypeList <Head&, Tail> >
{
  Head hd;
  TypeListValues <Tail> tl;

  TypeListValues (Head& hd_, TypeListValues <Tail> const& tl_)
    : hd (hd_), tl (tl_)
  {
  }

  static std::string const tostring (bool comma = false)
  {
    std::string s;

    if (comma)
      s = ", ";

    s = s + typeid (Head).name () + "&";

    return s + TypeListValues <Tail>::tostring (true);
  }
};

template <typename Head, typename Tail>
struct TypeListValues <TypeList <Head const&, Tail> >
{
  Head hd;
  TypeListValues <Tail> tl;

  TypeListValues (Head const& hd_, const TypeListValues <Tail>& tl_)
    : hd (hd_), tl (tl_)
  {
  }

  static std::string const tostring (bool comma = false)
  {
    std::string s;

    if (comma)
      s = ", ";

    s = s + typeid (Head).name () + " const&";

    return s + TypeListValues <Tail>::tostring (true);
  }
};

//==============================================================================
/**
  Subclass of a TypeListValues constructable from the Lua stack.
*/

template <typename List, int Start = 1>
struct ArgList
{
};

template <int Start>
struct ArgList <None, Start> : public TypeListValues <None>
{
  ArgList (lua_State*)
  {
  }
};

template <typename Head, typename Tail, int Start>
struct ArgList <TypeList <Head, Tail>, Start>
  : public TypeListValues <TypeList <Head, Tail> >
{
  ArgList (lua_State* L)
    : TypeListValues <TypeList <Head, Tail> > (Stack <Head>::get (L, Start),
                                            ArgList <Tail, Start + 1> (L))
  {
  }
};
