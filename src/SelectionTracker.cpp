/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "SelectionTracker.h"
#include "ScintillaNext.h"

SelectionTracker::SelectionTracker(ScintillaNext *editor) :
    editor(editor)
{
    saveSelection();
}

SelectionTracker::~SelectionTracker()
{
    restoreSelection();
}

void SelectionTracker::saveSelection()
{
    caret = editor->selectionNCaret(editor->mainSelection());
    anchor = editor->selectionNAnchor(editor->mainSelection());
}

void SelectionTracker::restoreSelection()
{
    editor->setSelection(caret, anchor);
}

void SelectionTracker::trackInsertion(int pos, int length)
{
    if (caret >= pos) {
        caret += length;
    }
    if (anchor >=pos) {
        anchor += length;
    }
}

void SelectionTracker::trackDeletion(int pos, int length)
{
    // Adjust the caret and anchor. Use the min in case they are within the range being deleted
    if (caret > pos) {
        caret -= qMin(static_cast<int>(caret - pos), length);
    }
    if (anchor > pos) {
        anchor -= qMin(static_cast<int>(anchor - pos), length);
    }
}
