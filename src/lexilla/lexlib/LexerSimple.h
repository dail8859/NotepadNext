// Scintilla source code edit control
/** @file LexerSimple.h
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXERSIMPLE_H
#define LEXERSIMPLE_H

namespace Lexilla {

// A simple lexer with no state
class LexerSimple : public LexerBase {
	const LexerModule *lexerModule;
	std::string wordLists;
public:
	explicit LexerSimple(const LexerModule *lexerModule_);
	const char * SCI_METHOD DescribeWordListSets() override;
	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	// ILexer5 methods
	const char * SCI_METHOD GetName() override;
	int SCI_METHOD  GetIdentifier() override;
};

}

#endif
