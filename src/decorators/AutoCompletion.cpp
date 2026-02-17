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


#include "AutoCompletion.h"

#include <QSet>


using namespace Scintilla;

AutoCompletion::AutoCompletion(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    editor->autoCSetOrder(SC_ORDER_PERFORMSORT);
    editor->autoCSetMaxHeight(10);
}

void AutoCompletion::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::CharAdded) {
        if (editor->autoCActive())
            return;

        showAutoCompletion();
    }
}

void AutoCompletion::showAutoCompletion()
{
    int curPos = editor->currentPos();
    int startPos = editor->wordStartPosition(curPos, true);
    int endPos = editor->wordEndPosition(curPos, true);

    // Need a minimum number of characters to trigger auto completion
    if ((curPos - startPos) < 3)
        return;

    const QByteArray current_word = editor->get_text_range(startPos, curPos);
    const QByteArray regex = "\\b" + current_word + "[\\w]*";
    QSet<QByteArray> words;

    editor->setSearchFlags(SCFIND_REGEXP | SCFIND_MATCHCASE);

    // Don't want to find the word that's currently being typed

    // Find everything before this word
    editor->forEachMatchInRange(regex, [&](int start, int end) {
        words.insert(editor->get_text_range(start, end));
        return end;
    }, { 0, startPos});

    // Find everything after this word
    editor->forEachMatchInRange(regex, [&](int start, int end) {
        words.insert(editor->get_text_range(start, end));
        return end;
    }, { endPos, (Sci_PositionCR)editor->length()});

    if (!words.isEmpty()) {
        editor->autoCShow(current_word.length(), words.values().join(' '));
    }
}
