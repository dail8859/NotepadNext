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


#ifndef FILELISTDOCK_H
#define FILELISTDOCK_H

#include "ScintillaNext.h"

#include <QDockWidget>
#include <QTreeWidgetItem>

namespace Ui {
class FileList;
}

class MainWindow;

class FileListDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit FileListDock(MainWindow *parent);
    ~FileListDock();

private slots:
    void refreshList();
    void addEditor(ScintillaNext *editor);
    void removeEditor(ScintillaNext *editor);

    void selectCurrentEditor();

    void itemClicked(QTreeWidgetItem *item, int column);

    void editorSavePointChanged(bool dirty);
    void editorRenamed();

private:
    void setPathColumnVisible(bool visible);
    QTreeWidgetItem *lookupItemByEditor(ScintillaNext *editor);

    Ui::FileList *ui;
    MainWindow *window;
};

#endif // FILELISTDOCK_H
