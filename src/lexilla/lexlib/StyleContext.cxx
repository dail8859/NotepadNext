// Scintilla source code edit control
/** @file StyleContext.cxx
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#include <cstdlib>
#include <cassert>

#include <string>
#include <string_view>

#include "ILexer.h"

#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"

using namespace Lexilla;

bool StyleContext::MatchIgnoreCase(const char *s) {
	if (MakeLowerCase(ch) != static_cast<unsigned char>(*s))
		return false;
	s++;
	if (MakeLowerCase(chNext) != static_cast<unsigned char>(*s))
		return false;
	s++;
	for (int n = 2; *s; n++) {
		if (*s !=
			MakeLowerCase(styler.SafeGetCharAt(currentPos + n, 0)))
			return false;
		s++;
	}
	return true;
}

void StyleContext::GetCurrent(char *s, Sci_PositionU len) {
	styler.GetRange(styler.GetStartSegment(), currentPos, s, len);
}

void StyleContext::GetCurrentLowered(char *s, Sci_PositionU len) {
	styler.GetRangeLowered(styler.GetStartSegment(), currentPos, s, len);
}
