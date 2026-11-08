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


#ifndef CODEOUTPUTDOCK_H
#define CODEOUTPUTDOCK_H

#include <QDockWidget>
#include <string>

class ScintillaNext;

namespace Ui {
class CodeOutputDock;
}

class CodeOutputDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit CodeOutputDock(QWidget *parent = 0);
    ~CodeOutputDock();
	
	void executeCommand(QString cmd);
	void writeOutputToDock(QString cmdOutput);
	void setupStyle(ScintillaNext *editor);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::CodeOutputDock *ui;

    ScintillaNext *codeOutput;
};

#endif