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

Finder::Finder(ScintillaNext *edit) :
    editor(edit)
{
    search_flags = editor->searchFlags();
}

void Finder::setEditor(ScintillaNext *editor)
{
    this->editor = editor;
}

void Finder::setSearchFlags(int flags)
{
    this->search_flags = flags;
}

void Finder::setWrap(bool wrap)
{
    this->wrap = wrap;
}

void Finder::setSearchText(const QString &text)
{
    this->text = text;
}

Sci_CharacterRange Finder::findNext(int startPos)
{
    did_latest_search_wrap = false;

    if (text.isEmpty())
        return {INVALID_POSITION, INVALID_POSITION};

    const int pos = startPos == INVALID_POSITION ? editor->selectionEnd() : startPos;
    const QByteArray textData = text.toUtf8();

    editor->setTargetRange(pos, editor->length());
    editor->setSearchFlags(search_flags);

    if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
        return {static_cast<Sci_PositionCR>(editor->targetStart()), static_cast<Sci_PositionCR>(editor->targetEnd())};
    }
    else if (wrap) {
        editor->setTargetRange(0, pos);
        if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
            did_latest_search_wrap = true;

            return {static_cast<Sci_PositionCR>(editor->targetStart()), static_cast<Sci_PositionCR>(editor->targetEnd())};
        }
    }

    return {INVALID_POSITION, INVALID_POSITION};
}

Sci_CharacterRange Finder::findPrev()
{
    did_latest_search_wrap = false;

    if (text.isEmpty())
        return {INVALID_POSITION, INVALID_POSITION};

    const int pos = editor->selectionStart();
    const QByteArray textData = text.toUtf8();

    editor->setTargetRange(pos, editor->length());
    editor->setSearchFlags(search_flags);

    auto range = editor->findText(editor->searchFlags(), textData.constData(), pos, 0);

    if (range.first != INVALID_POSITION) {
        return {static_cast<Sci_PositionCR>(range.first), static_cast<Sci_PositionCR>(range.second)};
    }
    else if (wrap) {
        range = editor->findText(editor->searchFlags(), textData.constData(), editor->length(), pos);
        if (range.first != INVALID_POSITION) {
            did_latest_search_wrap = true;

            return {static_cast<Sci_PositionCR>(range.first), static_cast<Sci_PositionCR>(range.second)};
        }
    }

    return {INVALID_POSITION, INVALID_POSITION};
}

// Count all occurrences in the document
int Finder::count()
{
    int total = 0;

    if (text.length() > 0) {
        forEachMatch([&](int start, int end) {
            Q_UNUSED(start);
            total++;
            return end;
        });
    }

    return total;
}

Sci_CharacterRange Finder::replaceSelectionIfMatch(const QString &replaceText)
{
    const QByteArray textData = text.toUtf8();
    bool isRegex = editor->searchFlags() & SCFIND_REGEXP;

    // Search just in the selection to see if the current selection is a match
    editor->setTargetStart(editor->selectionStart());
    editor->setTargetEnd(editor->selectionEnd());
    editor->setSearchFlags(search_flags);

    if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
        const QByteArray replaceData = replaceText.toUtf8();

        if (isRegex)
            editor->replaceTargetRE(replaceData.length(), replaceData.constData());
        else
            editor->replaceTarget(replaceData.length(), replaceData.constData());

        return {static_cast<Sci_PositionCR>(editor->targetStart()), static_cast<Sci_PositionCR>(editor->targetEnd())};
    }

    return {INVALID_POSITION, INVALID_POSITION};
}

int Finder::replaceAll(const QString &replaceText)
{
    if (text.isEmpty())
        return 0;

    const QByteArray &replaceData = replaceText.toUtf8();
    const QByteArray &b = text.toUtf8();
    const char *c = b.constData();
    Sci_TextToFind ttf {{0, (Sci_PositionCR)editor->length()}, c, {-1, -1}};
    const bool isRegex = search_flags & SCFIND_REGEXP;
    int total = 0;

    // Don't technically need to set the search flags here but do it just in case something looks at the search flags later
    editor->setSearchFlags(search_flags);

    // NOTE: can't use editor->forEachMatch() here since the search range can grow since the document is changing

    const UndoAction ua(editor);
    while (editor->send(SCI_FINDTEXT, search_flags, reinterpret_cast<sptr_t>(&ttf)) != -1) {
        const int start = ttf.chrgText.cpMin;
        const int end = ttf.chrgText.cpMax;

        editor->setTargetRange(start, end);

        if (isRegex)
            ttf.chrg.cpMin = start + editor->replaceTargetRE(replaceData.length(), replaceData.constData());
        else
            ttf.chrg.cpMin = start + editor->replaceTarget(replaceData.length(), replaceData.constData());

        // The replace could have changed the document size, so update the end of the search range
        ttf.chrg.cpMax = editor->length();

        total++;
    }

    return total;
}
