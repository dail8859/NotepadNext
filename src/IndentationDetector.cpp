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


#include "IndentationDetector.h"

#include <array>

DetectedIndentation IndentationDetector::detect(QByteArrayView text)
{
    const int MIN_INDENTED_LINES = 3;
    const int MAX_INDENT_SIZE = 8;

    int tabLines = 0;
    int spaceLines = 0;

    // How often each change of indentation between two consecutive lines indented with spaces occurs
    std::array<int, MAX_INDENT_SIZE + 1> deltas{};
    int previousSpaces = 0; // -1 when the previous line was indented with tabs

    qsizetype pos = 0;
    while (pos < text.size()) {
        qsizetype end = pos;
        while (end < text.size() && text[end] != '\n' && text[end] != '\r') {
            ++end;
        }

        const QByteArrayView line = text.sliced(pos, end - pos);
        pos = end + 1;

        qsizetype indentLength = 0;
        while (indentLength < line.size() && (line[indentLength] == ' ' || line[indentLength] == '\t')) {
            ++indentLength;
        }

        // Blank lines do not tell anything
        if (indentLength == line.size()) {
            continue;
        }

        // Skip block comment continuations such as " * text" which would look like an indentation of one space
        if (indentLength > 0 && line[indentLength] == '*' && line[indentLength - 1] == ' ') {
            continue;
        }

        const QByteArrayView indent = line.first(indentLength);

        if (indent.contains('\t')) {
            if (indent.startsWith('\t')) {
                tabLines++;
            }

            previousSpaces = -1;
            continue;
        }

        const int spaces = static_cast<int>(indentLength);

        if (spaces > 0) {
            spaceLines++;
        }

        if (previousSpaces != -1) {
            const int delta = qAbs(spaces - previousSpaces);

            if (delta >= 2 && delta <= MAX_INDENT_SIZE) {
                deltas[delta]++;
            }
        }

        previousSpaces = spaces;
    }

    DetectedIndentation result;

    if (tabLines + spaceLines < MIN_INDENTED_LINES || tabLines == spaceLines) {
        return result;
    }

    result.detected = true;
    result.useTabs = tabLines > spaceLines;

    if (!result.useTabs) {
        // Use the most common change of indentation, preferring the smaller size on a tie
        for (int size = 2; size <= MAX_INDENT_SIZE; ++size) {
            if (deltas[size] > 0 && (result.indentSize == 0 || deltas[size] > deltas[result.indentSize])) {
                result.indentSize = size;
            }
        }
    }

    return result;
}
