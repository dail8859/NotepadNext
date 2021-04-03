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


#ifndef NOTEPADNEXTAPPLICATION_H
#define NOTEPADNEXTAPPLICATION_H

#include "BufferManager.h"
#include "Settings.h"

#include "SingleApplication"

#include <QPointer>


class MainWindow;
class LuaState;
class RecentFilesListManager;

class NotepadNextApplication : public SingleApplication
{
    Q_OBJECT

public:
    NotepadNextApplication(BufferManager *bm, int &argc, char **argv);

    bool initGui();

    BufferManager *getBufferManager() const { return bufferManager; };
    RecentFilesListManager *getRecentFilesListManager() const { return recentFilesListManager; };

    LuaState *getLuaState() const;
    QString getFileDialogFilter() const;
    Settings *getSettings() const;

private:
    BufferManager *bufferManager;
    RecentFilesListManager *recentFilesListManager;

    LuaState *luaState = Q_NULLPTR;
    Settings *settings = Q_NULLPTR;

    QList<MainWindow *> windows;
    QPointer<QWidget> currentlyFocusedWidget; // Keep a weak pointer to the QWidget since we don't own it

    void applyArguments(const QStringList &args);
    MainWindow *createNewWindow();
};

#endif // NOTEPADNEXTAPPLICATION_H
