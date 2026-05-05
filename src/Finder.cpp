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

void Finder::setSelectionRange(Sci_CharacterRange range)
{
    use_selection_range = true;
    selection_range = range;
}

void Finder::clearSelectionRange()
{
    use_selection_range = false;
}

Sci_CharacterRange Finder::findNext(int startPos)
{
    did_latest_search_wrap = false;

    if (text.isEmpty())
        return {INVALID_POSITION, INVALID_POSITION};

    const QByteArray textData = text.toUtf8();

    if (use_selection_range) {
        // Clamp start position to within the selection
        const int rangeEnd = selection_range.cpMax;
        int pos = startPos == INVALID_POSITION ? editor->selectionEnd() : startPos;
        if (pos < selection_range.cpMin) pos = selection_range.cpMin;
        if (pos > rangeEnd) pos = rangeEnd;

        editor->setTargetRange(pos, rangeEnd);
        editor->setSearchFlags(search_flags);

        if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
            return {static_cast<Sci_PositionCR>(editor->targetStart()), static_cast<Sci_PositionCR>(editor->targetEnd())};
        }
        // Wrap within selection
        if (wrap && pos > selection_range.cpMin) {
            editor->setTargetRange(selection_range.cpMin, pos);
            if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
                did_latest_search_wrap = true;
                return {static_cast<Sci_PositionCR>(editor->targetStart()), static_cast<Sci_PositionCR>(editor->targetEnd())};
            }
        }
        return {INVALID_POSITION, INVALID_POSITION};
    }

    const int pos = startPos == INVALID_POSITION ? editor->selectionEnd() : startPos;

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

    const QByteArray textData = text.toUtf8();

    if (use_selection_range) {
        int pos = editor->selectionStart();
        if (pos > selection_range.cpMax) pos = selection_range.cpMax;
        if (pos < selection_range.cpMin) pos = selection_range.cpMin;

        editor->setSearchFlags(search_flags);
        auto range = editor->findText(search_flags, textData.constData(), pos, selection_range.cpMin);

        if (range.first != INVALID_POSITION) {
            return {static_cast<Sci_PositionCR>(range.first), static_cast<Sci_PositionCR>(range.second)};
        }
        // Wrap within selection
        if (wrap && pos < selection_range.cpMax) {
            range = editor->findText(search_flags, textData.constData(), selection_range.cpMax, pos);
            if (range.first != INVALID_POSITION) {
                did_latest_search_wrap = true;
                return {static_cast<Sci_PositionCR>(range.first), static_cast<Sci_PositionCR>(range.second)};
            }
        }
        return {INVALID_POSITION, INVALID_POSITION};
    }

    const int pos = editor->selectionStart();

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

// Count all occurrences in the document (or selection if set)
int Finder::count()
{
    int total = 0;

    if (text.length() > 0) {
        auto counter = [&](int start, int end) {
            Q_UNUSED(start);
            total++;
            return end;
        };
        if (use_selection_range)
            forEachMatchInRange(counter, selection_range);
        else
            forEachMatch(counter);
    }

    return total;
}

Sci_CharacterRange Finder::replaceSelectionIfMatch(const QString &replaceText)
{
    const QByteArray textData = text.toUtf8();
    const bool isRegex = search_flags & SCFIND_REGEXP;
    const Sci_PositionCR selectionStart = editor->selectionStart();
    const Sci_PositionCR selectionEnd = editor->selectionEnd();

    if (use_selection_range && (selectionStart < selection_range.cpMin || selectionEnd > selection_range.cpMax))
        return {INVALID_POSITION, INVALID_POSITION};

    // Search just in the selection to see if the current selection is a match
    editor->setTargetStart(selectionStart);
    editor->setTargetEnd(selectionEnd);
    editor->setSearchFlags(search_flags);

    if (editor->searchInTarget(textData.length(), textData.constData()) != INVALID_POSITION) {
        const QByteArray replaceData = replaceText.toUtf8();
        const Sci_PositionCR matchStart = editor->targetStart();
        const Sci_PositionCR matchEnd = editor->targetEnd();
        int replaceLength;

        if (isRegex)
            replaceLength = editor->replaceTargetRE(replaceData.length(), replaceData.constData());
        else
            replaceLength = editor->replaceTarget(replaceData.length(), replaceData.constData());

        if (use_selection_range)
            selection_range.cpMax += replaceLength - (matchEnd - matchStart);

        return {matchStart, static_cast<Sci_PositionCR>(matchStart + replaceLength)};
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
    const Sci_PositionCR rangeStart = use_selection_range ? selection_range.cpMin : 0;
    Sci_PositionCR rangeEnd         = use_selection_range ? selection_range.cpMax : (Sci_PositionCR)editor->length();
    Sci_TextToFind ttf {{rangeStart, rangeEnd}, c, {-1, -1}};
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

        // Update the end of the search range based on the length delta of this replacement.
        // When confined to a selection, track the selection boundary precisely; otherwise
        // expand to the full (possibly grown) document length.
        if (use_selection_range) {
            rangeEnd += ttf.chrg.cpMin - end;  // delta = newLength - oldMatchLength
            ttf.chrg.cpMax = rangeEnd;
        } else {
            ttf.chrg.cpMax = editor->length();
        }

        total++;
    }

    // Keep the stored selection range in sync with the post-replacement boundary
    if (use_selection_range)
        selection_range.cpMax = rangeEnd;

    return total;
}
