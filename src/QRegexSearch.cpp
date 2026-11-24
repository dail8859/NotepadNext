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
