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


#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <ScintillaEdit.h>
#include <ScintillaBuffer.h>

struct SelectedRange {
    int caret;
    int anchor;
    SelectedRange(int caret_= INVALID_POSITION, int anchor_= INVALID_POSITION) :
        caret(caret_), anchor(anchor_) {
    }
};

class BufferView
{
public:
    explicit BufferView(ScintillaBuffer *buffer_);

    ScintillaBuffer *getBuffer() { return buffer; }

    void saveView(ScintillaEdit *editor);
    void restoreView(ScintillaEdit *editor);

private:
    ScintillaBuffer *buffer;

    QVector<SelectedRange> selectedRanges;
    int firstVisibleLine;
    int scrollWidth;
};

#endif // BUFFERVIEW_H
