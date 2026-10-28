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
#include <QRegularExpression>

using namespace Scintilla;

#ifdef SCI_OWNREGEX
RegexSearchBase *Scintilla::Internal::CreateRegexSearch(CharClassify *charClassTable)
{
    Q_UNUSED(charClassTable);

    qInfo(Q_FUNC_INFO);

    return new QRegexSearch();
}
#endif

QRegexSearch::QRegexSearch()
{

}

Sci::Position QRegexSearch::FindText(Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *s, bool caseSensitive, bool word, bool wordStart, Scintilla::FindOption flags, Sci::Position *length)
{
    Q_UNUSED(caseSensitive);
    Q_UNUSED(word)
    Q_UNUSED(wordStart)
    // -----------------------------------------------------------------------------------------------------------------------
    // NOTE: This section of code has to be very careful about what units of measure is being used. Scintilla wants to operate
    // in units of bytes (e.g. position 3 is 3 bytes into the text). Qt wants to operate in units of UTF16 chars. The trouble is
    // when you start using characters that are >1 byte a piece. Meaning position 3 (3 bytes into a file) could be 1 character.
    // -----------------------------------------------------------------------------------------------------------------------

    // Make sure the positiosn are outside of characters
    minPos = doc->MovePositionOutsideChar(minPos, 1, false);
    maxPos = doc->MovePositionOutsideChar(maxPos, -1, false);

    //qInfo(Q_FUNC_INFO);
    //qInfo("\tminPos %d", minPos);
    //qInfo("\tmaxPos %d", maxPos);
    //qInfo("\ts %s", s);
    //qInfo("\tcaseSensitive %s", caseSensitive ? "true" : "false");
    //qInfo("\tword %s", word ? "true" : "false");
    //qInfo("\twordStart %s", wordStart ? "true" : "false");
    //qInfo("\tflags %d", flags);

    // No need to search an empty range
    if (minPos == maxPos)
        return -1;

    auto options = QRegularExpression::MultilineOption | QRegularExpression::UseUnicodePropertiesOption;

    if (!FlagSet(flags, FindOption::MatchCase))
        options |= QRegularExpression::CaseInsensitiveOption;

    // TODO: does (*ANYCRLF) need prepended to the search string?
    QRegularExpression re(s, options);
    if (!re.isValid())
        return -1; // Invalid regular expression

    // Get the bytes from the document. No need to go past maxPos bytes
    // Not actually sure if this copies the data or not
    const Sci::Position rangeLength = maxPos - minPos;
    const QString utf8 = QString::fromUtf8(doc->RangePointer(minPos, rangeLength), rangeLength);

    // NOTE: QString uses UTF16 counts since QChars are 16 bits
    QRegularExpressionMatch m = re.match(utf8, 0, QRegularExpression::NormalMatch, QRegularExpression::NoMatchOption);

    if (!m.hasMatch())
        return -1; // No match

    match = m;

    // NOTE: Returned started is the index into the QString which uses UTF16
    const int positionStart = doc->GetRelativePositionUTF16(minPos, match.capturedStart(0));

    // Now move ahead however many characters we matched. Again, based on UTF16 count
    const int positionEnd = doc->GetRelativePositionUTF16(positionStart, match.capturedLength(0));

    // The length is the number of bytes that was matched
    *length = positionEnd - positionStart;

    return positionStart;
}

// Reads a run of decimal digits starting at i, advancing i past them. Returns -1 if there are none.
static int readNumber(const QString &s, qsizetype &i)
{
    int n = -1;
    while (i < s.length() && s[i].isDigit()) {
        n = (n < 0 ? 0 : n * 10) + s[i].digitValue();
        ++i;
    }
    return n;
}

// Reads hex digits for \xHH or \x{HHHH}, advancing i past them. Returns -1 if invalid.
static int readHex(const QString &s, qsizetype &i)
{
    const bool braced = i < s.length() && s[i] == QLatin1Char('{');
    const qsizetype start = braced ? i + 1 : i;
    qsizetype end = start;
    while (end < s.length() && (braced || end - start < 2) && QStringLiteral("0123456789abcdefABCDEF").contains(s[end]))
        ++end;

    if (end == start || (braced && (end >= s.length() || s[end] != QLatin1Char('}'))))
        return -1;

    bool ok;
    const uint value = QStringView(s).mid(start, end - start).toUInt(&ok, 16);
    if (!ok || value > 0x10FFFF)
        return -1;

    i = braced ? end + 1 : end;
    return static_cast<int>(value);
}

// Expands a Notepad++ style replacement string: $N, ${N}, ${name}, $&, $$, \N and the usual character escapes.
// Groups that did not participate in the match (or do not exist) expand to nothing.
QString QRegexSearch::ExpandReplacement(const QRegularExpressionMatch &match, const QString &replacement)
{
    QString result;
    result.reserve(replacement.length());

    for (qsizetype i = 0; i < replacement.length(); ++i) {
        const QChar c = replacement[i];

        if (c == QLatin1Char('$') && i + 1 < replacement.length()) {
            const QChar next = replacement[i + 1];
            qsizetype j = i + 1;

            if (next == QLatin1Char('$')) {
                result += QLatin1Char('$');
                i = j;
            }
            else if (next == QLatin1Char('&')) {
                result += match.captured(0);
                i = j;
            }
            else if (next.isDigit()) {
                result += match.captured(readNumber(replacement, j));
                i = j - 1;
            }
            else if (next == QLatin1Char('{')) {
                const qsizetype close = replacement.indexOf(QLatin1Char('}'), j + 1);
                if (close < 0) {
                    result += c;
                    continue;
                }

                const QString group = replacement.mid(j + 1, close - j - 1);
                bool isNumber;
                const int n = group.toInt(&isNumber);
                result += isNumber ? match.captured(n) : match.captured(group);
                i = close;
            }
            else {
                result += c;
            }
        }
        else if (c == QLatin1Char('\\') && i + 1 < replacement.length()) {
            const QChar next = replacement[i + 1];
            qsizetype j = i + 1;

            if (next.isDigit()) {
                result += match.captured(readNumber(replacement, j));
                i = j - 1;
                continue;
            }

            i = j;
            switch (next.unicode()) {
            case 'n': result += QLatin1Char('\n'); break;
            case 'r': result += QLatin1Char('\r'); break;
            case 't': result += QLatin1Char('\t'); break;
            case 'f': result += QLatin1Char('\f'); break;
            case 'v': result += QLatin1Char('\v'); break;
            case 'a': result += QLatin1Char('\a'); break;
            case 'e': result += QChar(0x1B); break;
            case 'x': {
                qsizetype k = j + 1;
                const int value = readHex(replacement, k);
                if (value < 0) {
                    result += next;
                }
                else {
                    result += QString::fromUcs4(reinterpret_cast<const char32_t *>(&value), 1);
                    i = k - 1;
                }
                break;
            }
            default:
                // Any other escaped character (e.g. \\ or \$) is taken literally
                result += next;
                break;
            }
        }
        else {
            result += c;
        }
    }

    return result;
}

const char *QRegexSearch::SubstituteByPosition(Document *doc, const char *text, Sci::Position *length)
{
    Q_UNUSED(doc);

    qInfo(Q_FUNC_INFO);

    Q_ASSERT(match.isValid());
    Q_ASSERT(match.hasMatch());

    const QString newString = ExpandReplacement(match, QString::fromUtf8(text, *length));

    // TODO: figure out why this has to be new'd and can't be an instantiated class member
    if (substituted) {
        delete substituted;
    }

    substituted = new QByteArray(newString.toUtf8());
    *length = substituted->length();
    return substituted->data();
}
