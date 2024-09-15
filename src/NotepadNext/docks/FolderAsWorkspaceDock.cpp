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


#include "FolderAsWorkspaceDock.h"
#include "ApplicationSettings.h"
#include "ui_FolderAsWorkspaceDock.h"

#include <QFileSystemModel>

ApplicationSetting<QString> rootPathSetting{"FolderAsWorkspace/RootPath"};

FolderAsWorkspaceDock::FolderAsWorkspaceDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FolderAsWorkspaceDock),
    model(new QFileSystemModel(this))
{
    ui->setupUi(this);

    ui->treeView->setModel(model);
    ui->treeView->header()->hideSection(1);
    ui->treeView->header()->hideSection(2);
    ui->treeView->header()->hideSection(3);

    connect(ui->treeView, &QTreeView::doubleClicked, this, [=](const QModelIndex &index) {
        if (!model->isDir(index)) {
            emit fileDoubleClicked(model->filePath(index));
        }
    });

    ApplicationSettings settings;
    setRootPath(settings.get(rootPathSetting));
}

FolderAsWorkspaceDock::~FolderAsWorkspaceDock()
{
    delete ui;
}

void FolderAsWorkspaceDock::setRootPath(const QString dir)
{
    ApplicationSettings settings;
    settings.set(rootPathSetting, dir);

    model->setRootPath(dir);
    ui->treeView->setRootIndex(model->index(dir));
}

QString FolderAsWorkspaceDock::rootPath() const
{
    return model->rootPath();
}
