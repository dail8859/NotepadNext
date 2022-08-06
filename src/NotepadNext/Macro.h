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


#ifndef MACRO_H
#define MACRO_H

#include "MacroStep.h"

class Macro
{
public:
    Macro();
    ~Macro();

    void addMacroStep(Scintilla::Message message, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam);

    int size() const { return steps.size(); }

    void replay(ScintillaNext *editor, int n = 1) const;
    void replayTillEndOfFile(ScintillaNext *editor) const;

    QString getName() const;
    void setName(const QString &value);

    const QVector<MacroStep> &getSteps() const { return steps; }

    friend QDataStream &operator<<(QDataStream& stream, const Macro &Macro);
    friend QDataStream &operator>>(QDataStream& stream, Macro &Macro);

private:
    QVector<MacroStep> steps;
    QString name;
};
Q_DECLARE_METATYPE(Macro)
Q_DECLARE_METATYPE(Macro*)

#endif // MACRO_H
