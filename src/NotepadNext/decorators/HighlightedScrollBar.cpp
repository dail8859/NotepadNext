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


using namespace Scintilla;

HighlightedScrollBarDecorator::HighlightedScrollBarDecorator(ScintillaEdit *editor)
    : EditorDecorator(editor), scrollBar(new HighlightedScrollBar(this, Qt::Vertical, editor))
{
    connect(scrollBar, &QScrollBar::valueChanged, editor, &ScintillaEdit::scrollVertical);

    editor->setVerticalScrollBar(scrollBar);
}

HighlightedScrollBarDecorator::~HighlightedScrollBarDecorator()
{
}

void HighlightedScrollBarDecorator::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::UpdateUI && (FlagSet(pscn->updated, Update::Content) || FlagSet(pscn->updated, Update::Selection))) {
        scrollBar->update();
    }
    else if (pscn->nmhdr.code == Notification::Modified && FlagSet(pscn->modificationType, ModificationFlags::ChangeMarker)) {
        scrollBar->update();
    }
}

void HighlightedScrollBar::paintEvent(QPaintEvent *event)
{
    // Paint the default scrollbar first
    QScrollBar::paintEvent(event);
    QPainter p(this);

    drawMarker(p, 24);
    drawIndicator(p, 29);
    drawCursor(p);
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

void HighlightedScrollBar::drawCursor(QPainter &p)
{
    ScintillaEdit *editor = decorator->getEditor();
    int caretLine = editor->visibleFromDocLine(editor->lineFromPosition(editor->currentPos()));
    int anchorLine = editor->visibleFromDocLine(editor->lineFromPosition(editor->anchor()));
    int lineCount = editor->visibleFromDocLine(editor->lineCount());

    if (!editor->endAtLastLine()) {
        lineCount += editor->linesOnScreen();
    }

    // There is no offical way to get the height of the scrollbar arrow buttons, however for now we can
    // assume that the buttons are square, meaning the height of them will be the same as we width of
    // the scroll bar.
    int scrollbarArrowHeight = rect().width();

    // What percentage we are at in the document
    double documentPercentageCaret = static_cast<double>(caretLine) / lineCount;
    double documentPercentageAnchor = static_cast<double>(anchorLine) / lineCount;

    int startCaretY = documentPercentageCaret * (rect().height() - scrollbarArrowHeight * 2);
    int startAnchorY = documentPercentageAnchor * (rect().height() - scrollbarArrowHeight * 2);

    if (startCaretY != startAnchorY) {
        p.fillRect(rect().x() + 4, startAnchorY + scrollbarArrowHeight, rect().width() - 8, startCaretY - startAnchorY, QColor(1, 1, 1, 25));
    }

    p.fillRect(rect().x() + 4, startCaretY + scrollbarArrowHeight, rect().width() - 8, 3, Qt::darkGray);
}
