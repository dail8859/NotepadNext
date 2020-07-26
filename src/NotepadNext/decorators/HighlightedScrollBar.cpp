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


#include <QPainter>

#include "HighlightedScrollBar.h"


HighlightedScrollBarDecorator::HighlightedScrollBarDecorator(ScintillaEdit *editor)
    : EditorDecorator(editor)
{
    scrollBar = new HighlightedScrollBar(this, Qt::Vertical, editor);
    connect(scrollBar, &QScrollBar::valueChanged, editor, &ScintillaEdit::scrollVertical);

    editor->setVerticalScrollBar(scrollBar);

    cursor.line = -1;
    cursor.color = Qt::darkGray;
}

HighlightedScrollBarDecorator::~HighlightedScrollBarDecorator()
{
}

void HighlightedScrollBarDecorator::notify(const SCNotification *pscn)
{
    if (pscn->nmhdr.code == SCN_UPDATEUI && pscn->updated & (SC_UPDATE_CONTENT | SC_UPDATE_SELECTION)) {
        cursor.line = editor->visibleFromDocLine(editor->lineFromPosition(editor->currentPos()));
        scrollBar->update();
    }
    else if (pscn->nmhdr.code == SCN_MODIFIED && pscn->modificationType & SC_MOD_CHANGEMARKER) {
        scrollBar->update();
    }
}

void HighlightedScrollBar::paintEvent(QPaintEvent *event)
{
    // Paint the default scrollbar first
    QScrollBar::paintEvent(event);

    ScintillaEdit *editor = decorator->getEditor();
    QPainter p(this);

    double lineCount = static_cast<double>(editor->visibleFromDocLine(editor->lineCount()));

    drawMarker(p, 24);
    drawIndicator(p, 29);

    // Draw the current line
    if (decorator->cursor.line != -1) {
        int yy = decorator->cursor.line / lineCount * rect().height();
        yy = qMin(yy, rect().height() - 4);
        p.fillRect(rect().x() + 2, yy, rect().width() - 4, 3, decorator->cursor.color);
    }
}

void HighlightedScrollBar::drawMarker(QPainter &p, int marker)
{
    ScintillaEdit *editor = decorator->getEditor();
    const double lineCount = static_cast<double>(editor->visibleFromDocLine(editor->lineCount()));
    int curLine = 0;

    while ((curLine = editor->markerNext(curLine, 1 << marker)) != -1) {
        int yy = (curLine + 1) / lineCount * rect().height();
        p.fillRect(rect().x() + 2, yy, rect().width() - 4, 3, Qt::blue);
        curLine++;
    }
}

void HighlightedScrollBar::drawIndicator(QPainter &p, int indicator)
{
    ScintillaEdit *editor = decorator->getEditor();
    const double lineCount = static_cast<double>(editor->visibleFromDocLine(editor->lineCount()));
    int curPos = editor->indicatorEnd(indicator, 0);
    int color = editor->indicFore(indicator);

    if (curPos > 0) {
        while ((curPos = editor->indicatorEnd(29, curPos)) < editor->length()) {
            int yy = editor->lineFromPosition(curPos) / lineCount * rect().height();
            yy = qMin(yy, rect().height() - 4);
            p.fillRect(rect().x() + 2, yy, rect().width() - 4, 3, color);

            curPos = editor->indicatorEnd(29, curPos);
        }
    }
}
