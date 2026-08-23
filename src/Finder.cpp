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

#include "Finder.h"

#include "UndoAction.h"

Finder::Finder(ScintillaNext *editor) :
    editor(editor)
{
}

void Finder::setEditor(ScintillaNext *editor)
{
    this->editor = editor;
}

int Finder::searchFlags() const
{
    return static_cast<int>(find_options.flags);
}

bool Finder::isRegexSearch() const
{
    return Scintilla::FlagSet(find_options.flags, Scintilla::FindOption::RegExp);
}

Sci_CharacterRange Finder::searchTarget(Sci_Position start, Sci_Position end)
{
    const QByteArray searchText = find_options.text.toUtf8();

    editor->setTargetRange(start, end);

    if (editor->searchInTarget(searchText.length(), searchText.constData()) == INVALID_POSITION) {
        return {INVALID_POSITION, INVALID_POSITION};
    }

    return {
        static_cast<Sci_PositionCR>(editor->targetStart()),
        static_cast<Sci_PositionCR>(editor->targetEnd())
    };
}

FindResult Finder::findNext()
{
    return findNextFrom(editor->selectionEnd());
}

FindResult Finder::findNextFrom(Sci_Position startPos)
{
    FindResult result;

    if (find_options.text.isEmpty())
        return result;

    editor->setSearchFlags(searchFlags());

    result.range = searchTarget(startPos, editor->length());

    // Return the valid result, or if not wrap around, go ahead and return it anyways
    if (result || !find_options.wrapAround)
        return result;

    result.range = searchTarget(0, startPos);

    if (result)
        result.wrapped = true;

    return result;
}

FindResult Finder::findPrev()
{
    FindResult result;

    if (find_options.text.isEmpty())
        return result;

    const Sci_Position startPos = editor->selectionStart();

    editor->setSearchFlags(searchFlags());

    // A target with start > end searches backwards.
    result.range = searchTarget(startPos, 0);

    // Return the valid result, or if not wrap around, go ahead and return it anyways
    if (result || !find_options.wrapAround)
        return result;

    result.range = searchTarget(editor->length(), startPos);

    if (result)
        result.wrapped = true;

    return result;
}

int Finder::count()
{
    if (find_options.text.isEmpty())
        return 0;

    int total = 0;

    forEachMatch([&total](int start, int end) {
        Q_UNUSED(start);
        ++total;
        return end;
    });

    return total;
}

FindResult Finder::replaceSelectionIfMatch(const QString &replaceText)
{
    FindResult result;

    if (find_options.text.isEmpty())
        return result;

    const Sci_Position selectionStart = editor->selectionStart();
    const Sci_Position selectionEnd = editor->selectionEnd();

    result.range = searchTarget(selectionStart, selectionEnd);

    if (!result)
        return result;

    const QByteArray replacementText = replaceText.toUtf8();

    if (isRegexSearch()) {
        editor->replaceTargetRE(replacementText.length(), replacementText.constData());
    }
    else {
        editor->replaceTarget(replacementText.length(), replacementText.constData());
    }

    result.range = {
        static_cast<Sci_PositionCR>(editor->targetStart()),
        static_cast<Sci_PositionCR>(editor->targetEnd())
    };

    return result;
}

int Finder::replaceAll(const QString &replaceText)
{
    if (find_options.text.isEmpty())
        return 0;

    const QByteArray searchText = find_options.text.toUtf8();
    const QByteArray replacementText = replaceText.toUtf8();

    Sci_TextToFind findText{
        {0, static_cast<Sci_PositionCR>(editor->length())},
        searchText.constData(),
        {-1, -1}
    };

    editor->setSearchFlags(searchFlags());

    UndoAction undoAction(editor);

    int total = 0;

    // We intentionally use SCI_FINDTEXT here rather than forEachMatch().
    //
    // Replacing text changes the document length, so the search range has
    // to be updated after every replacement.
    while (editor->send(SCI_FINDTEXT, searchFlags(), reinterpret_cast<sptr_t>(&findText)) != INVALID_POSITION) {
        const Sci_Position start = findText.chrgText.cpMin;
        const Sci_Position end = findText.chrgText.cpMax;

        editor->setTargetRange(start, end);

        Sci_Position replacementLength;
        if (isRegexSearch()) {
            replacementLength= editor->replaceTargetRE(replacementText.length(), replacementText.constData());
        }
        else {
            replacementLength = editor->replaceTarget(replacementText.length(), replacementText.constData());
        }

        // Continue searching after the replacement.
        findText.chrg.cpMin = start + replacementLength;
        findText.chrg.cpMax = editor->length();

        ++total;
    }

    return total;
}
