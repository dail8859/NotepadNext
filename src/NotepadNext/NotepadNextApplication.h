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

#include "QtSingleApplication"
#include "Settings.h"

class MainWindow;
class LuaState;

class NotepadNextApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    NotepadNextApplication(const QString &id, int &argc, char **argv);

    bool init();
    LuaState *getLuaState() const;
    QString getFileDialogFilter() const;

private:
    QList<MainWindow *> windows;
    QWidget *currentlyFocusedWidget = Q_NULLPTR;
    LuaState *luaState = Q_NULLPTR;
    Settings *settings = Q_NULLPTR;

    void applyArguments(const QStringList &args);
    MainWindow *createNewWindow();
};

#endif // NOTEPADNEXTAPPLICATION_H
