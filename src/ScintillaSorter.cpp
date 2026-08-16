/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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

#include "ScintillaSorter.h"
#include "ByteArrayUtils.h"

ScintillaSorter::ScintillaSorter(ScintillaNext *editor)
    : editor(editor)
{
}

void ScintillaSorter::sort(const Sorter &sorter)
{
    QPair<int, int> range = determineTargetRange();
    const QByteArray eol = editor->eolString();

    const QByteArray text = readEditorText(range);
    QVector<QByteArrayView> lines = ByteArrayUtils::split(text, eol);

    if (lines.count() <= 1)
        return;

    sorter.sort(lines);

    QByteArray result = ByteArrayUtils::join(lines, eol);
    writeEditorText(result, range);

    editor->setSelection(range.first, range.second);
}

QPair<int, int> ScintillaSorter::determineTargetRange() const
{
    if (editor->selectionEmpty()) {
        return {0, editor->length()};
    } else {
        const int start = editor->positionFromLine(editor->lineFromPosition(editor->selectionStart()));
        const int end = editor->lineEndPosition(editor->lineFromPosition(editor->selectionEnd()));
        return {start, end};
    }
}

const QByteArray ScintillaSorter::readEditorText(QPair<int, int> range)
{
    int len = range.second - range.first;
    sptr_t pointer = editor->rangePointer(range.first, len);

    return QByteArray::fromRawData(reinterpret_cast<const char *>(pointer), len);
}

void ScintillaSorter::writeEditorText(const QByteArray &result, QPair<int, int> range)
{
    editor->setTargetRange(range.first, range.second);
    editor->replaceTarget(-1, result.constData());
}
