/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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


// TODO: Fix this mess. Scintilla makes you include everything...in the correct order...
// this was copied from Editor.cxx just to get it to compile

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <forward_list>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>
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

#include "PCRE2Search.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <QDebug>
#include <QtGlobal>


using namespace Scintilla;
using namespace Scintilla::Internal;


class PCRE2Search : public RegexSearchBase
{
public:
    PCRE2Search() { qInfo() << Q_FUNC_INFO; }

    Sci::Position FindText(Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *pattern, bool caseSensitive, bool word, bool wordStart, Scintilla::FindOption flags, Sci::Position *length) override;
    const char *SubstituteByPosition(Document *doc, const char *text, Sci::Position *length) override;

private:
    struct RegexContext {
        pcre2_code* re = nullptr;
        pcre2_match_data* match_data = nullptr;
        uint32_t compile_options = 0;
        uint32_t match_options = 0;
        Sci::Position search_start;
        Sci::Position search_end;
        std::string replacement_buffer;

        ~RegexContext() {
            if (match_data) pcre2_match_data_free(match_data);
            if (re) pcre2_code_free(re);
        }
    };

    RegexContext ctx;
};

#ifdef SCI_OWNREGEX
RegexSearchBase *Scintilla::Internal::CreateRegexSearch(CharClassify *charClassTable)
{
    Q_UNUSED(charClassTable);

    qInfo(Q_FUNC_INFO);

    return new PCRE2Search();
}
#endif

Sci::Position PCRE2Search::FindText(Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *s, bool caseSensitive, bool word, bool wordStart, Scintilla::FindOption flags, Sci::Position *length)
{
    // Clear out any context from the previous search
    if (ctx.re) {
        pcre2_match_data_free(ctx.match_data);
        ctx.match_data = nullptr;
        pcre2_code_free(ctx.re);
        ctx.re = nullptr;
    }

    const bool searchForward = (minPos <= maxPos);
    Q_ASSERT(searchForward);

    const bool dotMatchesNewLine = static_cast<int>(flags) & SCFIND_REGEXP_DOTMATCHESNL;
    std::string pattern = s;
    ctx.search_start = std::min(minPos, maxPos);
    ctx.search_end = std::max(minPos, maxPos);

    // Range endpoints should not be inside DBCS characters or between a CR and LF, but just in case, move them.
    ctx.search_start= doc->MovePositionOutsideChar(ctx.search_start, 1, true);
    ctx.search_end= doc->MovePositionOutsideChar(ctx.search_end, 1, true);

    // No need to search an empty range or empty pattern
    if (ctx.search_start == ctx.search_end || pattern.empty()) {
        return -1;
    }

    // Wrap in word boundaries. Also include additional grouping to fix potential precedence issue
    if (word) pattern = "\\b(?:" + pattern + ")\\b";
    if (wordStart) pattern = "\\b(?:" + pattern + ")";
    // For some reason PCRE2_NEWLINE_ANYCRLF | PCRE2_BSR_ANYCRLF don't seem to have an effect
    // Handles mixed line endings, could use e.g. (*LF) based on doc settings
    pattern = "(*ANYCRLF)(*BSR_ANYCRLF)" + pattern;

    //qDebug() << "--- FindText ---";
    //qDebug() << "Min Pos:" << minPos;
    //qDebug() << "Max Pos:" << maxPos;
    //qDebug() << "Min Pos (actual):" << ctx.search_start;
    //qDebug() << "Max Pos (actual):" << ctx.search_end;
    //qDebug() << "Pattern:" << s;
    //qDebug() << "Computed Pattern:" << pattern;
    //qDebug() << "Case Sensitive:" << caseSensitive;
    //qDebug() << "Word:" << word;
    //qDebug() << "Word Start:" << wordStart;
    //qDebug() << "Direction:" << (searchForward ? "forwards" : "backwards");
    //qDebug() << "BOL:" << doc->IsLineStartPosition(ctx.search_start);
    //qDebug() << "EOL:" << doc->IsLineEndPosition(ctx.search_end);

    ctx.compile_options = PCRE2_MULTILINE;
    if (!caseSensitive)
        ctx.compile_options |= PCRE2_CASELESS;
    if (dotMatchesNewLine)
        ctx.compile_options |= PCRE2_DOTALL;
    if (doc->CodePage() == SC_CP_UTF8)
        ctx.compile_options |= PCRE2_UTF | PCRE2_NO_UTF_CHECK;

    int errornumber;
    PCRE2_SIZE erroroffset;

    ctx.re = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(pattern.c_str()),
        PCRE2_ZERO_TERMINATED,
        ctx.compile_options,
        &errornumber,
        &erroroffset,
        nullptr
    );

    if (!ctx.re) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));

        qWarning().noquote() << pattern;
        qWarning().noquote() << QByteArray(erroroffset - 1, ' ') + '^';;
        qWarning().noquote() << QString::fromUtf8(reinterpret_cast<const char*>(buffer));

        doc->SetErrorStatus(SC_STATUS_WARN_REGEX);

        *length = 0;
        return -1;
    }

    ctx.match_data = pcre2_match_data_create_from_pattern(ctx.re, nullptr);
    ctx.match_options = PCRE2_NOTEMPTY | PCRE2_NOTEMPTY_ATSTART; // TODO: use 0 and handle empty matches

    // Since this string is in the middle of the document there is additional information to pass
    // to pcre2 about the context of the string.
    if (!doc->IsLineStartPosition(ctx.search_start))
        ctx.match_options |= PCRE2_NOTBOL;
    if (!doc->IsLineEndPosition(ctx.search_end))
        ctx.match_options |= PCRE2_NOTEOL;

    const int subjectLength = ctx.search_end - ctx.search_start;
    const char *subject = doc->RangePointer(ctx.search_start, subjectLength);

    int rc = pcre2_match(
        ctx.re,
        reinterpret_cast<PCRE2_SPTR>(subject),
        subjectLength,
        0, // start offset always needs to be 0 since we only give the target to pcre2
        ctx.match_options,
        ctx.match_data,
        nullptr
    );

    int ret = -1;
    if (rc >= 0) {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(ctx.match_data);
        PCRE2_SIZE matchStart = ovector[0];
        PCRE2_SIZE matchEnd = ovector[1];

        *length = matchEnd - matchStart;
        ret = ctx.search_start + matchStart; // Offset it by the start of the search

        //QByteArray a = QByteArray::fromRawData(&subject[matchStart], matchEnd - matchStart);
        //qDebug() << "First match:" << a << "at" << matchStart << "to" << matchEnd;
    } else {
        *length = 0;
        ret = -1;
        //qDebug() << "No Match";
    }

    return ret;
}

const char *PCRE2Search::SubstituteByPosition(Document *doc, const char *text, Sci::Position *length)
{
    Q_ASSERT(ctx.re);
    Q_ASSERT(ctx.match_data);
    Q_ASSERT(ctx.search_end > ctx.search_start);

    //qDebug() << "--- SubstituteByPosition ----";
    //qDebug() << "Replacement Text:" << text;

    // A common pattern is to just remove the match, so short circuit this case
    if (!text[0]) {
        *length = 0;
        return "";
    }

    const int subjectLength = ctx.search_end - ctx.search_start;
    const char *subject = doc->RangePointer(ctx.search_start, subjectLength);
    const uint32_t options = ctx.match_options|
                             PCRE2_SUBSTITUTE_EXTENDED |
                             PCRE2_SUBSTITUTE_REPLACEMENT_ONLY |
                             PCRE2_SUBSTITUTE_OVERFLOW_LENGTH |
                             PCRE2_SUBSTITUTE_MATCHED;

    PCRE2_SIZE outlen = 1024; // Arbitrary size which should handle most cases
    ctx.replacement_buffer.resize(outlen);

    int rc = pcre2_substitute(
        ctx.re,
        reinterpret_cast<PCRE2_SPTR>(subject),
        subjectLength,
        0,
        options,
        ctx.match_data,
        nullptr,
        reinterpret_cast<PCRE2_SPTR>(text),
        PCRE2_ZERO_TERMINATED,
        reinterpret_cast<PCRE2_UCHAR8*>(ctx.replacement_buffer.data()),
        &outlen
    );

    // Check to see if a second pass is necessary if there was not enough memory
    if (rc == PCRE2_ERROR_NOMEMORY) {
        ctx.replacement_buffer.resize(outlen);

        rc = pcre2_substitute(
            ctx.re,
            reinterpret_cast<PCRE2_SPTR>(subject),
            subjectLength,
            0,
            options,
            ctx.match_data,
            nullptr,
            reinterpret_cast<PCRE2_SPTR>(text),
            PCRE2_ZERO_TERMINATED,
            reinterpret_cast<PCRE2_UCHAR8*>(ctx.replacement_buffer.data()),
            &outlen
        );
    }

    //if (rc == PCRE2_ERROR_NOMATCH) {
    //    // Is this bad? There should have been a match
    //    *length = 0;
    //    return nullptr;
    //}

    if (rc < 0) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(rc, buffer, sizeof(buffer));
        qWarning().noquote() << QString::fromUtf8(reinterpret_cast<const char*>(buffer));

        *length = 0;
        return nullptr;
    }

    *length = static_cast<Sci::Position>(outlen);
    return ctx.replacement_buffer.c_str();
}
