// Scintilla source code edit control
/** @file Lexilla.h
 ** Lexer infrastructure.
 ** Declare functions in Lexilla library.
 **/
// Copyright 2019 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

extern "C" {

Scintilla::ILexer5 *CreateLexer(const char *name);

}
