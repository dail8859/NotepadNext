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


#ifndef MACROSTEP_H
#define MACROSTEP_H

#include "ScintillaNext.h"

class MacroStep {
public:
    MacroStep() {}
    MacroStep(Scintilla::Message message, uptr_t wParam,  sptr_t lParam);
    ~MacroStep();

    QString toString() const;
    QString name() const;

    void replay(ScintillaNext *editor) const;

    friend QDataStream &operator<<(QDataStream& stream, const MacroStep &macroStep);
    friend QDataStream &operator>>(QDataStream& stream, MacroStep &macroStep);

    static bool MessageHasString(Scintilla::Message message);

    Scintilla::Message message;
    uptr_t wParam;
    sptr_t lParam;
    QByteArray str;
};
Q_DECLARE_METATYPE(MacroStep)

#endif // MACROSTEP_H
