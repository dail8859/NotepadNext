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


#ifndef MACRORECORDER_H
#define MACRORECORDER_H

#include <QObject>

#include "ScintillaNext.h"


class MacroAction;

class Macro
{
public:
    ~Macro();

    void addMacroStep(int message, uptr_t wParam, sptr_t lParam);
    int size() const { return actions.size(); }
    void replay(ScintillaNext *editor, int n = 1) const;
    void replayTillEndOfFile(ScintillaNext *editor) const;

    QString getName() const;
    void setName(const QString &value);

private:
    QVector<MacroAction *> actions;
    QString name;
};


class MacroRecorder : public QObject
{
    Q_OBJECT

public:
    MacroRecorder(QObject *parent = nullptr);

    void startRecording(ScintillaNext *editor);
    Macro *stopRecording();

public slots:
    void recordMacroStep(int message, uptr_t wParam, sptr_t lParam);

private:
    ScintillaNext *editor = Q_NULLPTR;
    Macro *macro = Q_NULLPTR;
};

#endif // MACRORECORDER_H
