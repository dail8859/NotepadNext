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
    qInfo(Q_FUNC_INFO);
    qInfo("\tminPos %d", minPos);
    qInfo("\tmaxPos %d", maxPos);
    qInfo("\ts %s", s);
    qInfo("\tcaseSensitive %s", caseSensitive ? "true" : "false");
    qInfo("\tword %s", word ? "true" : "false");
    qInfo("\twordStart %s", wordStart ? "true" : "false");
    qInfo("\tflags %d", flags);
    //qInfo("length %d", length);

    // No need to search an empty range
    if (minPos == maxPos)
        return -1;

    auto options = QRegularExpression::MultilineOption | QRegularExpression::UseUnicodePropertiesOption;

    if (FlagSet(flags, FindOption::MatchCase))
        options |= QRegularExpression::CaseInsensitiveOption;

    // TODO: does (*ANYCRLF) need prepended to the search string?
    QRegularExpression re(s, options);
    if (!re.isValid())
        return -1; // Invalid regular expression

    // use a local variable incase it fails
    QRegularExpressionMatch m = re.match(doc->BufferPointer(), minPos, QRegularExpression::NormalMatch, QRegularExpression::NoMatchOption);

    if (!m.hasMatch())
        return -1; // No match

    match = m;
    *length = match.capturedLength(0);

    return match.capturedStart(0);
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
