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


#include "LineNumbers.h"


static inline int countDigits(quint32 x)
{
    // Ugly but efficient
    return (x < 10 ? 1 :
           (x < 100 ? 2 :
           (x < 1000 ? 3 :
           (x < 10000 ? 4 :
           (x < 100000 ? 5 :
           (x < 1000000 ? 6 :
           (x < 10000000 ? 7 :
           (x < 100000000 ? 8 :
           (x < 1000000000 ? 9 :
           10)))))))));
}

LineNumbers::LineNumbers(ScintillaEdit *editor) :
    EditorDecorator(editor)
{
    connect(this, &EditorDecorator::stateChanged, [=](bool b) {
        if (b) {
            adjustMarginWidth();
        }
        else {
            editor->setMarginWidthN(0, 0);
        }
    });
}

void LineNumbers::adjustMarginWidth()
{
    int lineCount = editor->lineCount();
    int pixelWidth = 8 + (qMax(countDigits(lineCount), 3)) * editor->textWidth(STYLE_LINENUMBER, "8");
    editor->setMarginWidthN(0, pixelWidth);
}

void LineNumbers::notify(const SCNotification *pscn)
{
    if ((pscn->nmhdr.code == SCN_UPDATEUI && pscn->updated & SC_UPDATE_V_SCROLL) || pscn->nmhdr.code == SCN_ZOOM) {
        adjustMarginWidth();
    }
}
