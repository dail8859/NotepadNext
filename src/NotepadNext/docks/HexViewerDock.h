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


#ifndef HEXVIEWERDOCK_H
#define HEXVIEWERDOCK_H

#include <QDockWidget>

namespace Ui {
class HexViewerDock;
}
class MainWindow;
class ScintillaNext;

class HexViewerDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit HexViewerDock(MainWindow *parent);
    ~HexViewerDock();

private slots:
    void connectToEditor(ScintillaNext *editor);

private:

    Ui::HexViewerDock *ui;
};

#endif // HEXVIEWERDOCK_H
