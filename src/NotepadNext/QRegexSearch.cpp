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
    // -----------------------------------------------------------------------------------------------------------------------
    // NOTE: This section of code has to be very careful about what units of measure is being used. Scintilla wants to operate
    // in units of bytes (e.g. position 3 is 3 bytes into the text). Qt wants to operate in units of characters. The trouble is
    // when you start using characters that are >1 byte a piece. Meaning position 3 (3 bytes into a file) could be 1 character.
    // -----------------------------------------------------------------------------------------------------------------------

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
    QByteArray view = QByteArray::fromRawData(doc->BufferPointer(), maxPos);

    // NOTE: translate minPos into a character count for Qt
    QRegularExpressionMatch m = re.match(view, doc->CountCharacters(0, minPos), QRegularExpression::NormalMatch, QRegularExpression::NoMatchOption);

    if (!m.hasMatch())
        return -1; // No match

    match = m;

    // NOTE: Qt is using character count, so translate this back into a Scintilla position
    const int characterStart = match.capturedStart(0);
    const int positionStart = doc->GetRelativePosition(0, characterStart);

    // Now move ahead however many characters we matched
    const int positionEnd = doc->GetRelativePosition(positionStart, match.capturedLength(0));

    // The length is the number of bytes that was matched
    *length = positionEnd - positionStart;

    return positionStart;
}

const char *QRegexSearch::SubstituteByPosition(Document *doc, const char *text, Sci::Position *length)
{
    Q_UNUSED(doc);

    qInfo(Q_FUNC_INFO);

    Q_ASSERT(match.isValid());
    Q_ASSERT(match.hasMatch());

    // Get the captured text and replace the match
    QString newString = match.captured();
    newString.replace(match.regularExpression(), QByteArray(text, *length));

    // TODO: figure out why this has to be new'd and can't be an instantiated class member
    if (substituted) {
        delete substituted;
    }

    substituted = new QByteArray(newString.toUtf8());
    *length = substituted->length();
    return substituted->data();
}
