/*
 * This file is part of Notepad Next.
 * Copyright 2021 Justin Dailey
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


#include "AutoIndentation.h"


using namespace Scintilla;

AutoIndentation::AutoIndentation(ScintillaEdit *editor) :
    EditorDecorator(editor)
{
}

void AutoIndentation::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::CharAdded) {
        const int ch = pscn->ch;
        const int eolMode = editor->eOLMode();

        if (((eolMode == SC_EOL_CRLF || eolMode == SC_EOL_LF) && ch == '\n') || (eolMode == SC_EOL_CR && ch == '\r')) {
            autoIndentLine(editor->lineFromPosition(editor->currentPos()));
        }
    }
}

void AutoIndentation::autoIndentLine(int line) const
{
    const int previousLine = line - 1;
    const int previousIndentation = editor->lineIndentation(previousLine);

    if (previousIndentation > 0) {
        editor->setLineIndentation(line, previousIndentation);
        editor->gotoPos(editor->findColumn(line, previousIndentation));
    }
}
