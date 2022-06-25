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

    Sci_CharacterRange replaceSelectionIfMatch(const QString &replaceText);
    int replaceAll(const QString &replaceText);

    template<typename Func>
    void forEachMatch(const QByteArray &byteArray, Func callback) { forEachMatchInRange(byteArray, callback, {0, (Sci_PositionCR)editor->length()}); }

    template<typename Func>
    void forEachMatchInRange(const QByteArray &byteArray, Func callback, Sci_CharacterRange range);

private:
    ScintillaNext *editor;

    bool wrap = false;
    int search_flags = 0;
    QString text;
};


template<typename Func>
void Finder::forEachMatchInRange(const QByteArray &text, Func callback, Sci_CharacterRange range)
{
    Sci_TextToFind ttf {range, text.constData(), {-1, -1}};

    while (editor->send(SCI_FINDTEXT, editor->searchFlags(), reinterpret_cast<sptr_t>(&ttf)) != -1) {
        ttf.chrg.cpMin = callback(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
    }
}

#endif // FINDER_H
