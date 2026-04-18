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
    const QByteArray eol = editor->eolString();
    const QByteArray text = readEditorText();
    QVector<QByteArrayView> lines = ByteArrayUtils::split(text, eol);

    sorter.sort(lines);

    QByteArray result = ByteArrayUtils::join(lines, eol);
    writeEditorText(result);
}

const QByteArray ScintillaSorter::readEditorText()
{
    sptr_t pointer = editor->characterPointer();
    int len = editor->length();

    return QByteArray::fromRawData(reinterpret_cast<const char *>(pointer), len);
}

void ScintillaSorter::writeEditorText(const QByteArray &result)
{
    editor->targetWholeDocument();
    editor->replaceTarget(-1, result.constData());
}
