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


#ifndef FINDER_H
#define FINDER_H

#include "ScintillaNext.h"

class Finder
{
public:
    explicit Finder(ScintillaNext *edit);

    void setEditor(ScintillaNext *editor);
    void setSearchFlags(int flags);
    void setWrap(bool wrap);
    void setSearchText(const QString &text);

    Sci_CharacterRange findNext(int startPos = INVALID_POSITION);
    Sci_CharacterRange findPrev();
    int count();

    bool didLatestSearchWrapAround() const { return did_latest_search_wrap; }

    Sci_CharacterRange replaceSelectionIfMatch(const QString &replaceText);
    int replaceAll(const QString &replaceText);

    template<typename Func>
    void forEachMatch(Func callback) { forEachMatchInRange(callback, {0, (Sci_PositionCR)editor->length()}); }

    template<typename Func>
    void forEachMatchInRange(Func callback, Sci_CharacterRange range);

private:
    ScintillaNext *editor;
    bool did_latest_search_wrap = false;

    bool wrap = false;
    int search_flags = 0;
    QString text;
};


template<typename Func>
void Finder::forEachMatchInRange(Func callback, Sci_CharacterRange range)
{
    editor->setSearchFlags(search_flags);
    editor->forEachMatchInRange(text.toUtf8(), callback, range);
}

#endif // FINDER_H
