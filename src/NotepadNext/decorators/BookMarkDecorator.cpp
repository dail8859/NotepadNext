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


#include "BookMarkDecorator.h"

const int MARK_BOOKMARK = 24;
const int MARGIN = 1;

BookMarkDecorator::BookMarkDecorator(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    editor->markerSetAlpha(MARK_BOOKMARK, 70);
    editor->markerDefine(MARK_BOOKMARK, SC_MARK_BOOKMARK);
    editor->markerSetFore(MARK_BOOKMARK, 0xFF2020);
    editor->markerSetBack(MARK_BOOKMARK, 0xFF2020);

    int mask = editor->marginMaskN(MARGIN);
    editor->setMarginMaskN(MARGIN, (1 << MARK_BOOKMARK) | mask);

    editor->setMarginSensitiveN(MARGIN, true);
}

void BookMarkDecorator::toggleBookmark(int line)
{
    if (editor->markerGet(line) & (1 << MARK_BOOKMARK)) {
        // The marker can be set multiple times, so keep deleting it till it is no longer set
        while (editor->markerGet(line) & (1 << MARK_BOOKMARK)) {
            editor->markerDelete(line, MARK_BOOKMARK);
        }
    }
    else {
        editor->markerAdd(line, MARK_BOOKMARK);
    }
}

int BookMarkDecorator::nextBookmarkAfter(int line)
{
    int nextMarkedLine = editor->markerNext(line, 1 << MARK_BOOKMARK);

    if (nextMarkedLine == -1) {
        return editor->markerNext(0, 1 << MARK_BOOKMARK);
    }
    else {
        return nextMarkedLine;
    }
}

int BookMarkDecorator::previousBookMarkBefore(int line)
{
    int prevMarkedLine = editor->markerPrevious(line, 1 << MARK_BOOKMARK);

    if (prevMarkedLine == -1) {
        return editor->markerPrevious(editor->lineCount(), 1 << MARK_BOOKMARK);
    }
    else {
        return prevMarkedLine;
    }
}

void BookMarkDecorator::clearBookmarks()
{
    editor->markerDeleteAll(MARK_BOOKMARK);
}

void BookMarkDecorator::notify(const Scintilla::NotificationData *pscn)
{
    if (pscn->nmhdr.code == Scintilla::Notification::MarginClick) {
        if (pscn->margin == MARGIN) {
            int line = editor->lineFromPosition(pscn->position);
            toggleBookmark(line);
        }
    }
}
