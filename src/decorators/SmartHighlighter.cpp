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


#include "SmartHighlighter.h"

using namespace Scintilla;


SmartHighlighter::SmartHighlighter(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    setObjectName("SmartHighlighter");

    indicator = editor->allocateIndicator("smart_highlighter");

    editor->indicSetFore(indicator, 0x00FF00);
    editor->indicSetStyle(indicator, INDIC_ROUNDBOX);
    editor->indicSetOutlineAlpha(indicator, 150);
    editor->indicSetAlpha(indicator, 100);
    editor->indicSetUnder(indicator, true);
}

void SmartHighlighter::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::UpdateUI && (FlagSet(pscn->updated, Update::Content) || FlagSet(pscn->updated, Update::Selection))) {
        highlightCurrentView();
    }
}

void SmartHighlighter::highlightCurrentView()
{
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, editor->length());

    if (editor->selectionEmpty()) {
        return;
    }

    const int mainSelection = editor->mainSelection();
    const int selectionStart = editor->selectionNStart(mainSelection);
    const int selectionEnd = editor->selectionNEnd(mainSelection);

    // Make sure the current selection is valid
    if (selectionStart == selectionEnd) {
        return;
    }

    const int curPos = editor->currentPos();
    const int wordStart = editor->wordStartPosition(curPos, true);
    const int wordEnd = editor->wordEndPosition(wordStart, true);

    // Make sure the selection is on word boundaries
    if (wordStart == wordEnd || wordStart != selectionStart || wordEnd != selectionEnd) {
        return;
    }

    const QByteArray selText = editor->get_text_range(selectionStart, selectionEnd);

    // TODO: Handle large files. By default Notepad++ only monitors the text on screen. However,
    // that will not work when using a highlighted scroll bar. Testing with small files seems to
    // have minimal impact. For large files, Qt can have a timer set to 0 to do heavier processing.
    // Using threads seems to be a bit overkill and too burdensome to do it properly.

    //const int startLine = editor->firstVisibleLine();
    //const int linesOnScreen = editor->linesOnScreen();
    //const int startPos = editor->positionFromLine(startLine);
    //const int endPos = editor->lineEndPosition(startLine + linesOnScreen);

    // TODO: skip hidden or folded lines?

    Sci_TextToFind ttf {{0, (Sci_PositionCR)editor->length()}, selText.constData(), {-1, -1}};
    const int flags = SCFIND_MATCHCASE | SCFIND_WHOLEWORD;

    while (editor->send(SCI_FINDTEXT, flags, (sptr_t)&ttf) != -1) {
        editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
        ttf.chrg.cpMin = ttf.chrgText.cpMax;
    }
}
