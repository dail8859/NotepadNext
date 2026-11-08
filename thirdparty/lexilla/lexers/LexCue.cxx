// Scintilla source code edit control
/** @file LexCue.cxx
 ** Lexer for CUE sheets (https://en.wikipedia.org/wiki/Cue_sheet).
 **
 ** CUE sheets are line oriented: the first word of each line is a command,
 ** and the remaining words are its arguments. Unlike generic "conf" style
 ** lexers, the word class of an argument depends on which command introduced
 ** the line (most notably the fields that follow REM), so the lexer keeps two
 ** bits of per-line state and always starts colouring at the beginning of the
 ** line that contains startPos.
 **
 ** Word lists (all lower case, comparison is case insensitive):
 **   0 keywords   - FILE TRACK INDEX PREGAP POSTGAP
 **   1 commands   - CATALOG CDTEXTFILE FLAGS ISRC PERFORMER REM ... TITLE
 **   2 attributes - fields that immediately follow REM
 **   3 types      - WAVE MP3 AIFF AUDIO ...
 **   4 values     - DCP 4CH PRE SCMS DATA FPS
 **
 ** This file is part of Notepad Next's bundled Lexilla and carries the same
 ** license as the rest of Lexilla (see License.txt).
 **/
// Copyright 2026 Notepad Next contributors
// SPDX-License-Identifier: HPND

#include <cassert>	// LexAccessor.h uses assert() without including it itself
#include <cctype>
#include <string>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

// CUE tokens are commands, bare values, ISRC ids, or MM:SS:FF timecodes.
// Everything that is not part of such a token is a separator.
bool IsWordChar(char ch) noexcept {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
	       (ch >= '0' && ch <= '9') ||
	       ch == '_' || ch == '-' || ch == ':' || ch == '.';
}

bool IsAllDigits(std::string_view s) noexcept {
	if (s.empty()) return false;
	for (const char c : s) {
		if (c < '0' || c > '9') return false;
	}
	return true;
}

// Matches the whole \d{1,3}:\d{2}:\d{2} timecode as one styled run. A generic
// conf lexer cannot do this: it stops a number at ':' and styles the colons as
// operators, which splits every timecode into five differently coloured pieces.
bool IsTimecode(std::string_view s) noexcept {
	const size_t firstColon = s.find(':');
	if (firstColon == std::string_view::npos || firstColon < 1 || firstColon > 3) return false;
	const size_t secondColon = s.find(':', firstColon + 1);
	if (secondColon == std::string_view::npos || secondColon != firstColon + 3) return false;
	if (s.size() != secondColon + 3) return false;
	for (size_t i = 0; i < s.size(); ++i) {
		if (i == firstColon || i == secondColon) continue;
		if (s[i] < '0' || s[i] > '9') return false;
	}
	return true;
}

void ColouriseCueDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int, WordList *keywordLists[], Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &commands = *keywordLists[1];
	const WordList &attributes = *keywordLists[2];
	const WordList &types = *keywordLists[3];
	const WordList &values = *keywordLists[4];

	const Sci_PositionU endPos = startPos + lengthDoc;

	// Per-line state (firstToken / afterRem) is significant, so rewind to the
	// start of the line containing startPos when Scintilla asks for a partial
	// re-colour. ColourTo() simply overwrites the styles in that range.
	Sci_PositionU pos = startPos;
	while (pos > 0) {
		const char preceding = styler.SafeGetCharAt(pos - 1);
		if (preceding == '\n' || preceding == '\r') break;
		--pos;
	}

	styler.StartAt(pos);
	styler.StartSegment(pos);

	bool firstToken = true;
	bool afterRem = false;
	std::string token;

	while (pos < endPos) {
		const char ch = styler.SafeGetCharAt(pos);

		if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') {
			styler.ColourTo(pos, SCE_CUE_DEFAULT);
			if (ch == '\n' || ch == '\r') {
				firstToken = true;
				afterRem = false;
			}
			++pos;
			continue;
		}

		if (ch == '"') {
			Sci_PositionU j = pos + 1;
			while (j < endPos) {
				const char c = styler.SafeGetCharAt(j);
				if (c == '\n' || c == '\r') break;
				if (c == '"') {
					++j;
					break;
				}
				++j;
			}
			styler.ColourTo(j - 1, SCE_CUE_STRING);
			pos = j;
			firstToken = false;
			afterRem = false;
			continue;
		}

		if (IsWordChar(ch)) {
			token.clear();
			Sci_PositionU j = pos;
			while (j < endPos && IsWordChar(styler.SafeGetCharAt(j))) {
				token.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(styler.SafeGetCharAt(j)))));
				++j;
			}

			int style = SCE_CUE_IDENTIFIER;
			if (IsTimecode(token) || IsAllDigits(token)) {
				style = SCE_CUE_NUMBER;
				firstToken = false;
				afterRem = false;
			} else if (firstToken) {
				// The command that opens a line. CotEditor's bundle splits these
				// into two classes, which is what gives the file its structure.
				if (keywords.InList(token)) {
					style = SCE_CUE_KEYWORD;
				} else if (commands.InList(token)) {
					style = SCE_CUE_COMMAND;
					afterRem = (token == "rem");
				}
				firstToken = false;
			} else {
				// Only the word immediately following REM is an attribute;
				// this mirrors the (?<=REM ) lookbehind in the CotEditor bundle.
				if (afterRem && attributes.InList(token)) {
					style = SCE_CUE_ATTRIBUTE;
				} else if (types.InList(token)) {
					style = SCE_CUE_TYPE;
				} else if (values.InList(token)) {
					style = SCE_CUE_VALUE;
				}
				afterRem = false;
			}

			styler.ColourTo(j - 1, style);
			pos = j;
			continue;
		}

		// CUE has no operators worth styling (colons only occur inside a
		// timecode, which is consumed whole above).
		styler.ColourTo(pos, SCE_CUE_DEFAULT);
		++pos;
	}
}

const char * const cueWordListDesc[] = {
	"Keywords",
	"Commands",
	"Attributes",
	"Types",
	"Values",
	nullptr,
};

}

extern const LexerModule lmCue(SCLEX_CUE, ColouriseCueDoc, "cue", nullptr, cueWordListDesc);
