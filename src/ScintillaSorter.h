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

#pragma once

#include "ScintillaNext.h"
#include "Sorter.h"

class ScintillaSorter
{
public:
    explicit ScintillaSorter(ScintillaNext *editor);

    void sort(const Sorter &sorter);

private:
    ScintillaNext *editor;

    const QByteArray readEditorText();
    void writeEditorText(const QByteArray &result);
};
