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


#include "BufferView.h"


BufferView::BufferView(ScintillaBuffer *buffer_) :
    buffer(buffer_),
    firstVisibleLine(0),
    scrollWidth(1)
{
}

void BufferView::saveView(ScintillaEdit *editor)
{
    int selections = editor->selections();

    selectedRanges.clear();
    selectedRanges.reserve(selections);

    for (int i = 0; i < selections; ++i) {
        int caret = editor->selectionNCaret(i);
        int anchor = editor->selectionNAnchor(i);
        selectedRanges.append(SelectedRange(caret, anchor));
    }

    firstVisibleLine = editor->docLineFromVisible(editor->firstVisibleLine());

    scrollWidth = editor->scrollWidth();
}

void BufferView::restoreView(ScintillaEdit *editor)
{
    for (int i = 0; i < selectedRanges.size(); ++i) {
        const SelectedRange &range = selectedRanges[i];

        // Quote from Scintilla documentation:
        // the first selection should be added with SCI_SETSELECTION and later selections added with SCI_ADDSELECTION
        if (i == 0)
            editor->setSelection(range.caret, range.anchor);
        else
            editor->addSelection(range.caret, range.anchor);
    }

    int curentTop = editor->firstVisibleLine();
    int lineTop = editor->visibleFromDocLine(firstVisibleLine);
    editor->lineScroll(0, lineTop - curentTop);
    editor->chooseCaretX();

    editor->setScrollWidth(scrollWidth);
}
