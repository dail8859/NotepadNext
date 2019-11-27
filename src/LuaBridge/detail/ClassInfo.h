//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge
  
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

/** Unique Lua registry keys for a class.

    Each registered class inserts three keys into the registry, whose
    values are the corresponding static, class, and const metatables. This
    allows a quick and reliable lookup for a metatable from a template type.
*/
template <class T>
class ClassInfo
{
public:
  /** Get the key for the static table.

      The static table holds the static data members, static properties, and
      static member functions for a class.
  */
  static void const* getStaticKey ()
  {
    static char value;
    return &value;
  }

  /** Get the key for the class table.

      The class table holds the data members, properties, and member functions
      of a class. Read-only data and properties, and const member functions are
      also placed here (to save a lookup in the const table).
  */
  static void const* getClassKey ()
  {
    static char value;
    return &value;
  }

  /** Get the key for the const table.

      The const table holds read-only data members and properties, and const
      member functions of a class.
  */
  static void const* getConstKey ()
  {
    static char value;
    return &value;
  }
};

