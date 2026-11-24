#!/usr/bin/env bash
# notepadnext-regexfix-v1.sh
# Fix regex Find/Replace/Count/Mark in NotepadNext (wshamroukh/NotepadNext).
#
# Root cause: Scintilla was built WITHOUT SCI_OWNREGEX, so the app's own regex
# engine (src/QRegexSearch.cpp) was never used. Every regex search went to
# Scintilla's built-in RESearch, which has no lookahead/lookbehind, no lazy
# quantifiers (*?), and treats ( ) as literal characters unless POSIX mode is on.
# A pattern like  ^.*?(?=[DEFIW]\s)  can never match there -> "No matches found."
#
# This script:
#   1. defines SCI_OWNREGEX for the scintilla target (enables the app's engine)
#   2. adds PCRE2 10.45 (static, 8-bit, JIT) via CPM and links it
#   3. replaces src/QRegexSearch.{h,cpp} with a PCRE2 engine that searches the
#      document buffer in place (Notepad++/Notepad3 regex flavour)
#   4. src/ScintillaNext.h: Find All/Count/Mark no longer stop at the first
#      zero-length match
#   5. FindReplaceDialog: wires up ". matches newline" ((?s)), fixes Extended
#      mode find text, and stops Find All overwriting the prepared options
#
# Usage:  cd /path/to/NotepadNext && bash notepadnext-regexfix-v1.sh
set -euo pipefail

[ -f src/QRegexSearch.cpp ] && [ -d thirdparty/scintilla ] || {
    echo "Run this from the NotepadNext repository root." >&2; exit 1; }

command -v python3 >/dev/null 2>&1 || { echo "python3 is required." >&2; exit 1; }

echo "[1/2] Writing src/QRegexSearch.h and src/QRegexSearch.cpp"
cat > src/QRegexSearch.h <<'__NN_QREGEXSEARCH_H__'
/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef QREGEXSEARCH_H
#define QREGEXSEARCH_H

// NOTE: despite the (historical) file name, this regex engine is PCRE2 (8-bit,
// UTF-8) running directly on Scintilla's byte buffer - the same flavour of
// regex that Notepad++ / Notepad3 users expect: lookahead/lookbehind, lazy
// quantifiers, \s \d \w, named groups, (?s), (?i), etc.

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

// Scintilla makes you include everything...in the correct order...
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <string>
#include <string_view>
#include <set>
#include <forward_list>
#include <optional>
#include <algorithm>
#include <iterator>
#include <chrono>

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaStructures.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "CharacterType.h"
#include "CharacterCategoryMap.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "PerLine.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "Scintilla.h"

#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>

using namespace Scintilla::Internal;

class QRegexSearch : public RegexSearchBase
{
public:
    QRegexSearch();
    ~QRegexSearch() override;

    QRegexSearch(const QRegexSearch &) = delete;
    QRegexSearch &operator=(const QRegexSearch &) = delete;

    Sci::Position FindText(Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *s, bool caseSensitive, bool word, bool wordStart, Scintilla::FindOption flags, Sci::Position *length) override;
    const char *SubstituteByPosition(Document *doc, const char *text, Sci::Position *length) override;

private:
    bool compile(const char *pattern, Sci::Position patternLength, bool caseSensitive, bool utf8);
    bool execute(Document *doc, Sci::Position subjectStart, Sci::Position subjectEnd, Sci::Position startPos,
                 Sci::Position &matchStart, Sci::Position &matchEnd, bool keepGroups);
    void releaseCode() noexcept;

    pcre2_compile_context *compileContext = nullptr;
    pcre2_code *code = nullptr;
    pcre2_match_data *matchData = nullptr;

    std::string compiledPattern;
    bool compiledCaseSensitive = false;
    bool compiledUtf8 = false;

    // Captured groups of the last successful match, copied out of the document
    // so they stay valid for SubstituteByPosition().
    std::vector<std::string> groups;
    std::vector<bool> groupSet;
    std::map<std::string, int> groupNames;

    std::string substituted;
};

#endif // QREGEXSEARCH_H
__NN_QREGEXSEARCH_H__

cat > src/QRegexSearch.cpp <<'__NN_QREGEXSEARCH_CPP__'
/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "QRegexSearch.h"

#include <QtGlobal>
#include <QString>

using namespace Scintilla;

#ifdef SCI_OWNREGEX
// Scintilla calls this (once per Document) instead of creating its built-in
// RESearch engine. Requires SCI_OWNREGEX to be defined for the scintilla target.
RegexSearchBase *Scintilla::Internal::CreateRegexSearch(CharClassify *charClassTable)
{
    Q_UNUSED(charClassTable);

    return new QRegexSearch();
}
#endif

QRegexSearch::QRegexSearch()
{
}

QRegexSearch::~QRegexSearch()
{
    releaseCode();

    if (compileContext) {
        pcre2_compile_context_free(compileContext);
        compileContext = nullptr;
    }
}

void QRegexSearch::releaseCode() noexcept
{
    if (matchData) {
        pcre2_match_data_free(matchData);
        matchData = nullptr;
    }

    if (code) {
        pcre2_code_free(code);
        code = nullptr;
    }

    compiledPattern.clear();
    groupNames.clear();
}

bool QRegexSearch::compile(const char *pattern, Sci::Position patternLength, bool caseSensitive, bool utf8)
{
    const std::string p(pattern, static_cast<size_t>(patternLength));

    // Re-use the compiled pattern. Find All / Count / Mark / Replace All call
    // FindText() once per match, so recompiling every time would be wasteful.
    if (code && p == compiledPattern && caseSensitive == compiledCaseSensitive && utf8 == compiledUtf8)
        return true;

    releaseCode();

    if (!compileContext) {
        compileContext = pcre2_compile_context_create(nullptr);
        if (!compileContext)
            return false;

        // Treat CR, LF and CRLF as line endings: '.' never eats the '\r' of a
        // Windows line ending and '$' matches before "\r\n" (like Notepad++/Notepad3).
        pcre2_set_newline(compileContext, PCRE2_NEWLINE_ANYCRLF);
    }

    uint32_t options = PCRE2_MULTILINE; // ^ and $ match at every line start/end

    if (!caseSensitive)
        options |= PCRE2_CASELESS;

    if (utf8)
        options |= PCRE2_UTF | PCRE2_UCP | PCRE2_MATCH_INVALID_UTF;

    int errorCode = 0;
    PCRE2_SIZE errorOffset = 0;
    code = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(p.data()), p.size(), options, &errorCode, &errorOffset, compileContext);

    if (!code) {
        PCRE2_UCHAR message[256];
        pcre2_get_error_message(errorCode, message, sizeof(message));
        qWarning("Invalid regular expression at offset %d: %s", static_cast<int>(errorOffset), reinterpret_cast<const char *>(message));
        return false;
    }

    // JIT makes a huge difference on large files. If JIT is not available
    // (not compiled in, or blocked by the OS) PCRE2 silently uses the interpreter.
    pcre2_jit_compile(code, PCRE2_JIT_COMPLETE);

    matchData = pcre2_match_data_create_from_pattern(code, nullptr);
    if (!matchData) {
        releaseCode();
        return false;
    }

    // Remember named groups so ${name} works in the replacement text
    uint32_t nameCount = 0;
    uint32_t nameEntrySize = 0;
    PCRE2_SPTR nameTable = nullptr;
    pcre2_pattern_info(code, PCRE2_INFO_NAMECOUNT, &nameCount);
    pcre2_pattern_info(code, PCRE2_INFO_NAMEENTRYSIZE, &nameEntrySize);
    pcre2_pattern_info(code, PCRE2_INFO_NAMETABLE, &nameTable);

    for (uint32_t i = 0; i < nameCount && nameTable; ++i) {
        const unsigned char *entry = nameTable + (i * nameEntrySize);
        const int groupNumber = (entry[0] << 8) | entry[1];
        groupNames[std::string(reinterpret_cast<const char *>(entry + 2))] = groupNumber;
    }

    compiledPattern = p;
    compiledCaseSensitive = caseSensitive;
    compiledUtf8 = utf8;

    return true;
}

bool QRegexSearch::execute(Document *doc, Sci::Position subjectStart, Sci::Position subjectEnd, Sci::Position startPos,
                           Sci::Position &matchStart, Sci::Position &matchEnd, bool keepGroups)
{
    const Sci::Position subjectLength = subjectEnd - subjectStart;
    if (subjectLength < 0 || startPos < subjectStart || startPos > subjectEnd)
        return false;

    // RangePointer() makes the range contiguous by moving the gap of the gap
    // buffer to subjectStart. Since subjectStart is always close to where the
    // previous search/replacement happened this is cheap, and the text is
    // searched in place: no copy, no UTF-16 conversion.
    static const char empty[] = "";
    const char *subject = subjectLength > 0 ? doc->RangePointer(subjectStart, subjectLength) : empty;

    const int rc = pcre2_match(code,
                               reinterpret_cast<PCRE2_SPTR>(subject),
                               static_cast<PCRE2_SIZE>(subjectLength),
                               static_cast<PCRE2_SIZE>(startPos - subjectStart),
                               0,
                               matchData,
                               nullptr);

    if (rc < 0) {
        if (rc != PCRE2_ERROR_NOMATCH)
            qWarning("Regular expression match failed (PCRE2 error %d)", rc);
        return false;
    }

    const PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    matchStart = subjectStart + static_cast<Sci::Position>(ovector[0]);
    matchEnd = subjectStart + static_cast<Sci::Position>(ovector[1]);

    // \K inside a lookahead can produce end < start
    if (matchEnd < matchStart)
        matchEnd = matchStart;

    if (keepGroups) {
        const uint32_t pairs = pcre2_get_ovector_count(matchData);
        const uint32_t used = (rc == 0) ? pairs : static_cast<uint32_t>(rc);

        groups.assign(pairs, std::string());
        groupSet.assign(pairs, false);

        for (uint32_t i = 0; i < pairs && i < used; ++i) {
            const PCRE2_SIZE start = ovector[2 * i];
            const PCRE2_SIZE end = ovector[2 * i + 1];

            if (start == PCRE2_UNSET || end == PCRE2_UNSET || end < start)
                continue;

            groups[i].assign(subject + start, end - start);
            groupSet[i] = true;
        }
    }

    return true;
}

Sci::Position QRegexSearch::FindText(Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *s, bool caseSensitive, bool word, bool wordStart, Scintilla::FindOption flags, Sci::Position *length)
{
    Q_UNUSED(word)
    Q_UNUSED(wordStart)
    Q_UNUSED(flags)

    // Scintilla asks for a backwards search by passing minPos > maxPos
    const bool forward = minPos <= maxPos;

    Sci::Position rangeStart = forward ? minPos : maxPos;
    Sci::Position rangeEnd = forward ? maxPos : minPos;

    // Make sure the positions are not inside multi-byte characters
    rangeStart = doc->MovePositionOutsideChar(rangeStart, 1, false);
    rangeEnd = doc->MovePositionOutsideChar(rangeEnd, -1, false);

    // No need to search an empty range
    if (rangeStart >= rangeEnd)
        return -1;

    // On entry *length is the length of the search pattern
    const bool utf8 = doc->CodePage() == SC_CP_UTF8;
    if (!compile(s, *length, caseSensitive, utf8))
        return -1; // Invalid regular expression

    // The subject starts at the beginning of the line *before* rangeStart, not at
    // rangeStart itself. PCRE2 then knows what precedes the start offset, so
    // ^, \b and lookbehinds behave correctly when a search continues from the
    // middle of a line (e.g. right after the previous match). Previously the
    // text was cut at rangeStart, which made ^ match in the middle of lines.
    const Sci::Line line = doc->SciLineFromPosition(rangeStart);
    const Sci::Position subjectStart = doc->LineStart(line > 0 ? line - 1 : 0);
    const Sci::Position docLength = doc->Length();

    Sci::Position matchStart = -1;
    Sci::Position matchEnd = -1;

    if (forward) {
        // Search with the rest of the document visible so lookaheads and $ see
        // the real text after rangeEnd...
        if (!execute(doc, subjectStart, docLength, rangeStart, matchStart, matchEnd, true))
            return -1;

        if (matchStart > rangeEnd)
            return -1;

        // ...but the match itself has to fit inside the requested range
        // (e.g. "Replace" checks whether the current selection is a match).
        if (matchEnd > rangeEnd) {
            if (!execute(doc, subjectStart, rangeEnd, rangeStart, matchStart, matchEnd, true))
                return -1;
        }
    }
    else {
        // Backwards: find the last match that fits in the range
        Sci::Position offset = rangeStart;
        Sci::Position lastStart = -1;
        Sci::Position s0 = -1;
        Sci::Position e0 = -1;

        while (offset <= rangeEnd && execute(doc, subjectStart, rangeEnd, offset, s0, e0, false)) {
            lastStart = s0;

            if (e0 > s0) {
                offset = e0;
            }
            else {
                if (e0 >= rangeEnd)
                    break;
                offset = doc->NextPosition(e0, 1);
            }
        }

        if (lastStart < 0)
            return -1;

        // Run it once more from the winning position to capture its groups
        if (!execute(doc, subjectStart, rangeEnd, lastStart, matchStart, matchEnd, true))
            return -1;
    }

    *length = matchEnd - matchStart;
    return matchStart;
}

const char *QRegexSearch::SubstituteByPosition(Document *doc, const char *text, Sci::Position *length)
{
    Q_UNUSED(doc);

    // Supported replacement syntax (Notepad++ / Notepad3 / boost style):
    //   $0..$99  ${N}  ${name}  $&  $$
    //   \0..\9   \n \r \t \\ \$
    //   \U \L \E (upper/lower case until \E)   \u \l (next character only)
    enum class Case { None, Upper, Lower };

    Case caseMode = Case::None;
    Case nextCharCase = Case::None;

    substituted.clear();

    auto append = [&](const std::string &str) {
        if (str.empty())
            return;

        if (caseMode == Case::None && nextCharCase == Case::None) {
            substituted += str;
            return;
        }

        QString qs = QString::fromUtf8(str.data(), static_cast<qsizetype>(str.size()));

        if (caseMode == Case::Upper)
            qs = qs.toUpper();
        else if (caseMode == Case::Lower)
            qs = qs.toLower();

        if (nextCharCase != Case::None && !qs.isEmpty()) {
            const qsizetype firstLength = (qs.size() > 1 && qs.at(0).isHighSurrogate()) ? 2 : 1;
            const QString first = qs.left(firstLength);
            qs.replace(0, firstLength, nextCharCase == Case::Upper ? first.toUpper() : first.toLower());
            nextCharCase = Case::None;
        }

        substituted += qs.toStdString();
    };

    auto appendGroup = [&](size_t number) {
        if (number < groups.size() && groupSet[number])
            append(groups[number]);
    };

    const std::string_view replacement(text, static_cast<size_t>(*length));
    std::string literal;

    auto flushLiteral = [&]() {
        append(literal);
        literal.clear();
    };

    auto isDigit = [](char c) { return c >= '0' && c <= '9'; };

    for (size_t i = 0; i < replacement.size(); ++i) {
        const char c = replacement[i];

        if (c == '\\' && i + 1 < replacement.size()) {
            const char n = replacement[++i];

            if (isDigit(n)) {
                flushLiteral();
                appendGroup(static_cast<size_t>(n - '0'));
                continue;
            }

            switch (n) {
            case 'n': literal += '\n'; break;
            case 'r': literal += '\r'; break;
            case 't': literal += '\t'; break;
            case 'U': flushLiteral(); caseMode = Case::Upper; break;
            case 'L': flushLiteral(); caseMode = Case::Lower; break;
            case 'E': flushLiteral(); caseMode = Case::None; nextCharCase = Case::None; break;
            case 'u': flushLiteral(); nextCharCase = Case::Upper; break;
            case 'l': flushLiteral(); nextCharCase = Case::Lower; break;
            default:  literal += n; break; // \\ -> \   \$ -> $   \x -> x
            }
            continue;
        }

        if (c == '$' && i + 1 < replacement.size()) {
            const char n = replacement[i + 1];

            if (n == '$') {
                literal += '$';
                ++i;
                continue;
            }

            if (n == '&') {
                flushLiteral();
                appendGroup(0);
                ++i;
                continue;
            }

            if (isDigit(n)) {
                size_t number = static_cast<size_t>(n - '0');
                ++i;

                // Allow a 2nd digit only if that group actually exists ($10 vs $1 followed by 0)
                if (i + 1 < replacement.size() && isDigit(replacement[i + 1])) {
                    const size_t twoDigits = number * 10 + static_cast<size_t>(replacement[i + 1] - '0');
                    if (twoDigits < groups.size()) {
                        number = twoDigits;
                        ++i;
                    }
                }

                flushLiteral();
                appendGroup(number);
                continue;
            }

            if (n == '{') {
                const size_t close = replacement.find('}', i + 2);
                if (close != std::string_view::npos) {
                    const std::string name(replacement.substr(i + 2, close - (i + 2)));
                    const bool numeric = !name.empty() && std::all_of(name.begin(), name.end(), isDigit);

                    flushLiteral();
                    if (numeric) {
                        appendGroup(static_cast<size_t>(std::stoul(name)));
                    }
                    else {
                        const auto it = groupNames.find(name);
                        if (it != groupNames.end())
                            appendGroup(static_cast<size_t>(it->second));
                    }

                    i = close;
                    continue;
                }
            }
        }

        literal += c;
    }

    flushLiteral();

    *length = static_cast<Sci::Position>(substituted.size());
    return substituted.c_str();
}
__NN_QREGEXSEARCH_CPP__

echo "[2/2] Patching CMake, ScintillaNext.h and FindReplaceDialog.cpp"
python3 - <<'__NN_APPLY_PY__'
import re, sys, pathlib

root = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
changed = []

def edit(rel, fn):
    p = root / rel
    if not p.exists():
        sys.exit(f"ERROR: {rel} not found - run this from the NotepadNext repo root")
    old = p.read_text(encoding="utf-8")
    new = fn(old)
    if new != old:
        p.write_text(new, encoding="utf-8", newline="")
        changed.append(rel)
    else:
        print(f"  = {rel} (already patched)")

def must(cond, msg):
    if not cond:
        sys.exit("ERROR: " + msg)

# 1) Tell Scintilla to use our regex engine instead of its built-in RESearch
def scintilla_cmake(t):
    if "SCI_OWNREGEX" in t:
        return t
    m = re.search(r"^([ \t]*)SCINTILLA_QT=1[ \t]*\r?\n", t, re.M)
    must(m, "SCINTILLA_QT=1 not found in thirdparty/scintilla/CMakeLists.txt")
    eol = "\r\n" if m.group(0).endswith("\r\n") else "\n"
    return t[:m.end()] + f"{m.group(1)}SCI_OWNREGEX=1{eol}" + t[m.end():]
edit("thirdparty/scintilla/CMakeLists.txt", scintilla_cmake)

# 2) Fetch + build PCRE2 (8-bit, static, JIT) with CPM like the other deps
PCRE2_BLOCK = '''# PCRE2 - regex engine used by Find/Replace (src/QRegexSearch.cpp)
if(APPLE)
    set(NN_PCRE2_JIT OFF) # avoid needing the allow-jit entitlement for hardened runtime
else()
    set(NN_PCRE2_JIT ON)
endif()
CPMAddPackage(
    EXCLUDE_FROM_ALL YES
    NAME pcre2
    URL https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.45/pcre2-10.45.tar.gz
    OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON"
        "PCRE2_BUILD_PCRE2_8 ON"
        "PCRE2_BUILD_PCRE2_16 OFF"
        "PCRE2_BUILD_PCRE2_32 OFF"
        "PCRE2_SUPPORT_UNICODE ON"
        "PCRE2_SUPPORT_JIT ${NN_PCRE2_JIT}"
        "PCRE2_BUILD_PCRE2GREP OFF"
        "PCRE2_BUILD_TESTS OFF"
        "PCRE2_SHOW_REPORT OFF"
        "PCRE2_SUPPORT_LIBZ OFF"
        "PCRE2_SUPPORT_LIBBZ2 OFF"
        "PCRE2_SUPPORT_LIBEDIT OFF"
        "PCRE2_SUPPORT_LIBREADLINE OFF"
)

'''
def thirdparty_cmake(t):
    if "PCRE2Project/pcre2" in t:
        return t
    m = re.search(r"^add_subdirectory\(lua", t, re.M)
    must(m, "add_subdirectory(lua ...) not found in thirdparty/CMakeLists.txt")
    block = PCRE2_BLOCK.replace("\n", "\r\n") if "\r\n" in t else PCRE2_BLOCK
    return t[:m.start()] + block + t[m.start():]
edit("thirdparty/CMakeLists.txt", thirdparty_cmake)

# 3) Link the app against PCRE2
def src_cmake(t):
    if "pcre2-8-static" in t:
        return t
    m = re.search(r"target_link_libraries\(NotepadNext\b.*?\)", t, re.S)
    must(m, "target_link_libraries(NotepadNext ...) not found in src/CMakeLists.txt")
    block = m.group(0)
    m2 = re.search(r"^([ \t]*)uchardet[ \t]*\r?\n", block, re.M)
    must(m2, "uchardet not found in target_link_libraries(NotepadNext ...)")
    eol = "\r\n" if m2.group(0).endswith("\r\n") else "\n"
    nb = block[:m2.end()] + f"{m2.group(1)}pcre2-8-static{eol}" + block[m2.end():]
    return t[:m.start()] + nb + t[m.end():]
edit("src/CMakeLists.txt", src_cmake)

# 4) Find All / Count / Mark: don't stop at the first zero-length match
NEW_LOOP = '''    while (ttf.chrg.cpMin <= ttf.chrg.cpMax && send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != INVALID_POSITION) {
        const Sci_PositionCR matchStart = ttf.chrgText.cpMin;
        const Sci_PositionCR matchEnd = ttf.chrgText.cpMax;

        Sci_PositionCR next = callback(matchStart, matchEnd);

        // Zero-length regex matches (^, $, lookarounds...) are valid results.
        // Step over them one character so the loop keeps going instead of
        // stopping at the first one (or finding the same spot forever).
        if (matchStart == matchEnd || next <= ttf.chrg.cpMin) {
            if (matchEnd >= ttf.chrg.cpMax)
                break;
            next = static_cast<Sci_PositionCR>(positionAfter(matchEnd));
        }

        ttf.chrg.cpMin = next;
    }'''
def scintillanext_h(t):
    if "Zero-length regex matches" in t:
        return t
    pat = re.compile(r"[ \t]*while \(send\(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>\(&ttf\)\) != INVALID_POSITION\) \{\s*"
                     r"if\s*\(ttf\.chrgText\.cpMin == ttf\.chrgText\.cpMax\)\s*break;\s*"
                     r"ttf\.chrg\.cpMin = callback\(ttf\.chrgText\.cpMin, ttf\.chrgText\.cpMax\);\s*\}")
    m = pat.search(t)
    must(m, "forEachMatchInRange loop not found in src/ScintillaNext.h")
    loop = NEW_LOOP.replace("\n", "\r\n") if "\r\n" in t else NEW_LOOP
    return t[:m.start()] + loop + t[m.end():]
edit("src/ScintillaNext.h", scintillanext_h)

# 5) Find dialog: honour ". matches newline", fix Extended mode text, and keep
#    the prepared options in Find All
def dialog_cpp(t):
    if "matches newline" in t and "(?s)" in t:
        return t
    eol = "\r\n" if "\r\n" in t else "\n"

    m = re.search(r"(FindOptions FindReplaceDialog::findOptions\(\) const\s*\{.*?\n\})", t, re.S)
    must(m, "FindReplaceDialog::findOptions() not found")
    body = m.group(1)

    m1 = re.search(r"^([ \t]*)options\.text = findString\(\);[ \t]*\r?\n", body, re.M)
    must(m1, "options.text = findString(); not found in findOptions()")
    ind = m1.group(1)
    body = (body[:m1.end()] + eol +
            f"{ind}// Extended mode: turn \\n, \\t... into the real characters{eol}" +
            f"{ind}if (ui->radioExtendedSearch->isChecked()){eol}" +
            f"{ind}    convertToExtended(options.text);{eol}" + body[m1.end():])

    m2 = re.search(r"^([ \t]*)if \(ui->radioRegexSearch->isChecked\(\)\)\s*\r?\n\s*options\.flags \|= Scintilla::FindOption::RegExp;[ \t]*\r?\n", body, re.M)
    must(m2, "RegExp flag block not found in findOptions()")
    ind = m2.group(1)
    body = (body[:m2.start()] +
            f"{ind}if (ui->radioRegexSearch->isChecked()) {{{eol}" +
            f"{ind}    options.flags |= Scintilla::FindOption::RegExp;{eol}{eol}" +
            f"{ind}    // \". matches newline\" checkbox{eol}" +
            f"{ind}    if (ui->checkBoxRegexMatchesNewline->isChecked()){eol}" +
            f"{ind}        options.text.prepend(QStringLiteral(\"(?s)\"));{eol}" +
            f"{ind}}}{eol}" + body[m2.end():])
    t = t[:m.start()] + body + t[m.end():]

    # findAllInCurrentDocument() overwrote the prepared text with the raw combo text
    m3 = re.search(r"(void FindReplaceDialog::findAllInCurrentDocument\(\)\s*\{.*?\n\})", t, re.S)
    must(m3, "findAllInCurrentDocument() not found")
    fb = m3.group(1)
    fb2 = re.sub(r"^[ \t]*QString text = findString\(\);[ \t]*\r?\n", "", fb, flags=re.M)
    fb2 = re.sub(r"^[ \t]*finder->options\(\)\.text = text;[ \t]*\r?\n", "", fb2, flags=re.M)
    t = t[:m3.start()] + fb2 + t[m3.end():]
    return t
edit("src/dialogs/FindReplaceDialog.cpp", dialog_cpp)

for c in changed:
    print(f"  + {c}")
__NN_APPLY_PY__

echo
echo "Done. Rebuild from a clean CMake configure (PCRE2 is downloaded by CPM), e.g.:"
echo "  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build"
echo "Then test with: Find > Regular expression >  ^.*?(?=[DEFIW]\\s)  > Count"
