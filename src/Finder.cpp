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

    editor->setSearchFlags(searchFlags());

    UndoAction undoAction(editor);

    int total = 0;
    Sci_Position searchStart = 0;

    // We intentionally search in the target here rather than using forEachMatch().
    //
    // Replacing text changes the document length, so the search range has
    // to be updated after every replacement. Searching in the target (instead of
    // SCI_FINDTEXT) also takes the text length, so null characters can be found.
    while (true) {
        editor->setTargetRange(searchStart, editor->length());

        if (editor->searchInTarget(searchText.length(), searchText.constData()) == INVALID_POSITION)
            break;

        // The target is now the match
        const Sci_Position start = editor->targetStart();
        const Sci_Position end = editor->targetEnd();

        Sci_Position replacementLength;
        if (isRegexSearch()) {
            replacementLength= editor->replaceTargetRE(replacementText.length(), replacementText.constData());
        }
        else {
            replacementLength = editor->replaceTarget(replacementText.length(), replacementText.constData());
        }

        // Continue searching after the replacement.
        searchStart = start + replacementLength;

        ++total;

        // An empty match must advance past the next original character, not
        // match the same position again after inserting the replacement.
        if (start == end) {
            if (searchStart == editor->length())
                break;

            searchStart = editor->positionAfter(searchStart);
        }
    }

    return total;
}
