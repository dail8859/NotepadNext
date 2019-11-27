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


#include "WindowListDialog.h"
#include "ui_WindowListDialog.h"

#include "MainWindow.h"
#include "ScintillaBuffer.h"

#include <QTableView>

WindowListDialog::WindowListDialog(MainWindow *window, QList<ScintillaBuffer *> buffers, QWidget *parent) :
    window(window),
    QDialog(parent),
    ui(new Ui::WindowListDialog)
{
    ui->setupUi(this);

    ui->tableView->setModel(new BufferViewModel(buffers, this));
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    ui->tableView->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Current);

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selected, const QItemSelection &deselected) {
        const bool hasSelection = !selected.indexes().empty();
        ui->buttonActivate->setEnabled(hasSelection);
        ui->buttonSave->setEnabled(hasSelection);
        ui->buttonCloseWindows->setEnabled(hasSelection);
    });

    connect(ui->buttonSave, &QPushButton::clicked, [=]() {
        QItemSelectionModel *select = ui->tableView->selectionModel();

        foreach (const QModelIndex &index, select->selectedRows()) {
            qInfo("%d %d", index.row(), index.column());
        }
    });
}

WindowListDialog::~WindowListDialog()
{
    delete ui;
}



BufferViewModel::BufferViewModel(QList<ScintillaBuffer *> buffers, QObject *parent) :
    buffers(buffers),
    QAbstractTableModel(parent)
{

}

int BufferViewModel::rowCount(const QModelIndex &parent) const
{
    qInfo(Q_FUNC_INFO);

    return buffers.size();
}

int BufferViewModel::columnCount(const QModelIndex &parent) const
{
    qInfo(Q_FUNC_INFO);

    return 3;
}

QVariant BufferViewModel::data(const QModelIndex &index, int role) const
{
    qInfo(Q_FUNC_INFO);
    qInfo("\t%d", role);

    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        if (col == 0)
            return buffers.at(row)->getName();
        else if (col == 1)
            return buffers.at(row)->isFile() ? buffers.at(row)->fileInfo.filePath() : QString("");
        else if (col == 2)
            return buffers.at(row)->lexer;
    }

    return QVariant();
}

QVariant BufferViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    qInfo(Q_FUNC_INFO);
    qInfo("\t%d", role);
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Name");
            case 1:
                return QString("Path");
            case 2:
                return QString("Type");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags BufferViewModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}
