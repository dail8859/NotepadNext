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


#ifndef LUACONSOLEDOCK_H
#define LUACONSOLEDOCK_H

#include <QDockWidget>

class ScintillaNext;
class LuaState;

namespace Ui {
class LuaConsoleDock;
}

class LuaConsoleDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit LuaConsoleDock(LuaState *l, QWidget *parent = 0);
    ~LuaConsoleDock();

    void writeToOutput(const char *s);
    void writeErrorToOutput(const char *s);
    LuaState *L = Q_NULLPTR;

    void historyNext();
    void historyPrevious();
    void historyAdd(QString line);
    void historyEnd();

public slots:
    void runCurrentCommand(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::LuaConsoleDock *ui;

    ScintillaNext *output;
    ScintillaNext *input;

    QStringList history;
    int currentHistoryIndex = 0;

    void setupStyle(ScintillaNext *editor);
};

#endif // LUACONSOLEDOCK_H
