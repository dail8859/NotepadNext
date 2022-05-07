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


#include "ScintillaCommenter.h"
#include "SelectionTracker.h"

ScintillaCommenter::ScintillaCommenter(ScintillaNext *editor) :
    editor(editor), st(editor), ua(editor)
{
}

void ScintillaCommenter::toggleSelection()
{
    editor->forEachLineInSelection(editor->mainSelection(), [&](int line) {
        toggleLine(line);
    });
}

void ScintillaCommenter::commentSelection()
{
    editor->forEachLineInSelection(editor->mainSelection(), [&](int line) {
        commentLine(line);
    });
}

void ScintillaCommenter::uncommentSelection()
{
    editor->forEachLineInSelection(editor->mainSelection(), [&](int line) {
        uncommentLine(line);
    });
}

void ScintillaCommenter::toggleLine(int line)
{
    auto indentPos = editor->lineIndentPosition(line);
    auto lineEnd = qMin(indentPos + editor->languageSingleLineComment.length(), editor->lineEndPosition(line));
    const QByteArray commentText = editor->get_text_range(indentPos, lineEnd);

    if (commentText == editor->languageSingleLineComment) {
        editor->deleteRange(indentPos, editor->languageSingleLineComment.length());

        st.trackDeletion(indentPos, editor->languageSingleLineComment.length());
    }
    else {
        if (indentPos == editor->lineEndPosition(line)) {
            return;
        }

        editor->insertText(indentPos, editor->languageSingleLineComment.constData());

        st.trackInsertion(indentPos, editor->languageSingleLineComment.length());
    }
}

void ScintillaCommenter::commentLine(int line)
{
    auto indentPos = editor->lineIndentPosition(line);

    // Don't comment lines with only indentation
    if (indentPos == editor->lineEndPosition(line)) {
        return;
    }

    editor->insertText(indentPos, editor->languageSingleLineComment.constData());

    st.trackInsertion(indentPos, editor->languageSingleLineComment.length());
}

void ScintillaCommenter::uncommentLine(int line)
{
    auto indentPos = editor->lineIndentPosition(line);
    auto lineEnd = qMin(indentPos + editor->languageSingleLineComment.length(), editor->lineEndPosition(line));
    const QByteArray commentText = editor->get_text_range(indentPos, lineEnd);

    if (commentText == editor->languageSingleLineComment) {
        editor->deleteRange(indentPos, editor->languageSingleLineComment.length());

        st.trackDeletion(indentPos, editor->languageSingleLineComment.length());
    }
}
