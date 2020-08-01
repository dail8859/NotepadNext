/*
 * This file is part of Notepad Next.
 * Copyright 2020 Justin Dailey
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


#include "SurroundSelection.h"

#include <QEvent>
#include <QKeyEvent>


SurroundSelection::SurroundSelection(ScintillaEdit *editor) :
    EditorDecorator(editor)
{
    setObjectName("SurroundSelection");

    editor->installEventFilter(this);
}

bool SurroundSelection::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (editor->selectionEmpty() == false) {
            switch (keyEvent->key()) {
            case Qt::Key_Apostrophe:
                surroundSelections('\'', '\'');
                return true;
            case Qt::Key_QuoteDbl:
                surroundSelections('"', '"');
                return true;
            case Qt::Key_QuoteLeft:
                surroundSelections('`', '`');
                return true;
            case Qt::Key_ParenLeft:
            case Qt::Key_ParenRight:
                surroundSelections('(', ')');
                return true;
            case Qt::Key_BraceLeft:
            case Qt::Key_BraceRight:
                surroundSelections('{', '}');
                return true;
            case Qt::Key_BracketLeft:
            case Qt::Key_BracketRight:
                surroundSelections('[', ']');
                return true;
            }
        }
    }

    return false;
}


void SurroundSelection::notify(const SCNotification *pscn)
{
    Q_UNUSED(pscn)
}

void SurroundSelection::surroundSelections(const char ch1, const char ch2)
{
    std::vector<std::pair<int, int>> selections;

    int num = editor->selections();
    for (int i = 0; i < num; ++i) {
        int start = editor->selectionNStart(i);
        int end = editor->selectionNEnd(i);

        if (start != end /* && editor.LineFromPosition(start) == editor.LineFromPosition(end) */)
            selections.push_back(std::make_pair(start, end));
    }

    // Sort so they are replaced top to bottom
    std::sort(selections.begin(), selections.end());

    editor->beginUndoAction();
    editor->clearSelections();

    int offset = 0;
    for (size_t i = 0; i < selections.size(); ++i) {
        const auto &selection = selections[i];
        editor->setTargetRange(selection.first + offset, selection.second + offset);

        QByteArray target = editor->targetText();

        // Add in the characters
        target.push_front(ch1);
        target.push_back(ch2);

        editor->replaceTarget(target.length(), target.data());

        // leave cursor at end of insertion
        if (i == 0)
            editor->setSelection(selection.second + offset + 1, selection.first + offset + 1);
        else
            editor->addSelection(selection.second + offset + 1, selection.first + offset + 1);

        offset += 2; // Add 2 since the replaced string is 2 chars longer
    }

    editor->endUndoAction();
}
