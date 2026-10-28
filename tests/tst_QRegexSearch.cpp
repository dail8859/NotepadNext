/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
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

#include <QTest>

using namespace Scintilla;
using namespace Scintilla::Internal;

class tst_QRegexSearch : public QObject
{
    Q_OBJECT

private slots:
    void wholeMatchAndNewline();
    void captureGroupsAreReordered();
    void dollarSyntaxStaysLiteral();
    void lookbehindPattern();

private:
    // Runs the same FindText()+SubstituteByPosition() sequence Scintilla performs for a regex
    // "Replace", against the given document text, and returns the produced replacement bytes.
    QByteArray substitute(const QString &docText, const QString &pattern, const QByteArray &replaceTemplate);
};

QByteArray tst_QRegexSearch::substitute(const QString &docText, const QString &pattern, const QByteArray &replaceTemplate)
{
    Document doc(DocumentOption::Default);
    const QByteArray docBytes = docText.toUtf8();
    doc.InsertString(0, docBytes.constData(), docBytes.length());

    const QByteArray patternBytes = pattern.toUtf8();

    QRegexSearch search;
    Sci::Position matchLength = 0;
    const Sci::Position matchStart = search.FindText(&doc, 0, doc.Length(), patternBytes.constData(),
                                                       true, false, false, FindOption::RegExp, &matchLength);

    if (matchStart < 0) {
        return QByteArray();
    }

    Sci::Position templateLength = replaceTemplate.length();
    const char *result = search.SubstituteByPosition(&doc, replaceTemplate.constData(), &templateLength);

    return QByteArray(result, templateLength);
}

void tst_QRegexSearch::wholeMatchAndNewline()
{
    // \0 = whole match, \r\n = real CRLF: this is the correct Notepad++ syntax for the
    // originally reported repro (Find "Example", Replace "$0\r\n", regex enabled).
    QCOMPARE(substitute("Example", "Example", "\\0\\r\\n"), QByteArray("Example\r\n"));
}

void tst_QRegexSearch::captureGroupsAreReordered()
{
    QCOMPARE(substitute("Example", "(Exa)(mple)", "\\2\\1"), QByteArray("mpleExa"));
}

void tst_QRegexSearch::dollarSyntaxStaysLiteral()
{
    // $0 is not a supported backreference token (neither here nor in real Notepad++), so it
    // must be left as literal text; \r and \n are still recognized independently of it.
    QCOMPARE(substitute("Example", "Example", "$0\\r\\n"), QByteArray("$0\r\n"));
}

void tst_QRegexSearch::lookbehindPattern()
{
    // The previous implementation re-ran the search pattern against the isolated matched
    // substring, which broke any pattern relying on context outside the match itself.
    QCOMPARE(substitute(" Example", "(?<=\\s)Example", "[\\0]"), QByteArray("[Example]"));
}

QTEST_APPLESS_MAIN(tst_QRegexSearch)

#include "tst_QRegexSearch.moc"
