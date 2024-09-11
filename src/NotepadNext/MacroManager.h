/*
 * This file is part of Notepad Next.
 * Copyright 2024 Justin Dailey
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

#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include "MacroRecorder.h"

#include <QObject>


class MacroManager : public QObject
{
    Q_OBJECT

public:
    explicit MacroManager(QObject *parent = nullptr);
    virtual ~MacroManager();

    bool isRecording() const;
    QVector<Macro *> &availableMacros() { return macros; };

    void replayCurrentMacro(ScintillaNext *editor);
    void saveCurrentMacro(const QString &macroName);
    bool hasCurrentUnsavedMacro() const;
    Macro *getCurrentMacro() const { return currentMacro; }

public slots:
    void startRecording(ScintillaNext *editor);
    void stopRecording();

private slots:
    void loadSettings();
    void saveSettings() const;

signals:
    void recordingStarted();
    void recordingStopped();

private:
    MacroRecorder recorder;
    Macro *currentMacro = Q_NULLPTR;
    QVector<Macro *> macros;
    bool _isRecording = false;
    bool isCurrentMacroSaved = false;
};

#endif // MACROMANAGER_H
