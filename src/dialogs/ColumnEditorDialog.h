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


#ifndef COLUMNEDITORDIALOG_H
#define COLUMNEDITORDIALOG_H

#include "MainWindow.h"
#include <QDialog>

namespace Ui {
class ColumnEditorDialog;
}

class ColumnEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnEditorDialog(MainWindow *parent);
    ~ColumnEditorDialog();

    void insertTextStartingAtCurrentColumn(const std::function <QString (void)>& f);
    void insertTextAtColumn(ScintillaNext *editor, int line, int column, const QString &str);

private:
    Ui::ColumnEditorDialog *ui;
    MainWindow *parent;
};

#endif // COLUMNEDITORDIALOG_H
