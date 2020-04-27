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


#ifndef WINDOWLISTDIALOG_H
#define WINDOWLISTDIALOG_H

#include <QAbstractTableModel>
#include <QDialog>
#include <QList>

namespace Ui {
class WindowListDialog;
}

class MainWindow;
class ScintillaBuffer;

class BufferViewModel : public QAbstractTableModel {
    Q_OBJECT

public:
    BufferViewModel(QList<ScintillaBuffer *> buffers, QObject *parent = Q_NULLPTR);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<ScintillaBuffer *> buffers;

};

class WindowListDialog : public QDialog
{
    Q_OBJECT

public:
    WindowListDialog(MainWindow *window, QList<ScintillaBuffer *> buffers, QWidget *parent = Q_NULLPTR);
    ~WindowListDialog();

private:
    Ui::WindowListDialog *ui;
    MainWindow *window;
};

#endif // WINDOWLISTDIALOG_H
