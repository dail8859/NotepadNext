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


#pragma once

#include "ScintillaNext.h"

struct FindOptions
{
    QString text;
    Scintilla::FindOption flags = {};
    bool wrapAround = false;
};

struct FindResult
{
    Sci_CharacterRange range = {INVALID_POSITION, INVALID_POSITION};
    bool wrapped = false;

    explicit operator bool() const
    {
        return range.cpMin != INVALID_POSITION;
    }
};

class Finder
{
public:
    explicit Finder(ScintillaNext *editor);

    void setEditor(ScintillaNext *editor);

    FindOptions &options() { return find_options; }
    const FindOptions &options() const { return find_options; }

    FindResult findNext();
    FindResult findNextFrom(Sci_Position startPos);
    FindResult findPrev();

    int count();

    FindResult replaceSelectionIfMatch(const QString &replaceText);
    int replaceAll(const QString &replaceText);

    template<typename Func>
    void forEachMatch(Func callback)
    {
        forEachMatchInRange(callback, {0, static_cast<Sci_PositionCR>(editor->length())});
    }

    template<typename Func>
    void forEachMatchInRange(Func callback, Sci_CharacterRange range);

private:
    int searchFlags() const;
    Sci_CharacterRange searchTarget(Sci_Position start, Sci_Position end);
    bool isRegexSearch() const;

    ScintillaNext *editor = nullptr;
    FindOptions find_options;
};

template<typename Func>
void Finder::forEachMatchInRange(Func callback, Sci_CharacterRange range)
{
    editor->setSearchFlags(searchFlags());
    editor->forEachMatchInRange(find_options.text.toUtf8(), callback, range);
}
