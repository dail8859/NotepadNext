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

#include <QFileSystemModel>
#include <QMessageBox>

#include "ApplicationSettings.h"
#include "MainWindow.h"
#include "ui_FolderAsWorkspaceDock.h"

QString NEW_DIR_TEMPLATE("dir_%1");

ApplicationSetting<QString> rootPathSetting{"FolderAsWorkspace/RootPath"};

FolderAsWorkspaceDock::FolderAsWorkspaceDock(MainWindow *parent) :
    QDockWidget(parent),
    window(parent)
{
    ui = new Ui::FolderAsWorkspaceDock;
    model = new QFileSystemModel(this);
    ui->setupUi(this);

    model->setReadOnly(false);
    ui->treeView->setModel(model);
    ui->treeView->header()->hideSection(1);
    ui->treeView->header()->hideSection(2);
    ui->treeView->header()->hideSection(3);

    connect(ui->treeView, &QTreeView::doubleClicked, this, [=](const QModelIndex &index) {
        if (!model->isDir(index)) {
            emit fileDoubleClicked(model->filePath(index));
        }
    });
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &FolderAsWorkspaceDock::onCustomContextMenu);
    connect(model, &QFileSystemModel::fileRenamed, this, &FolderAsWorkspaceDock::onFileRenamed);

    ApplicationSettings settings;
    setRootPath(settings.get(rootPathSetting));
}

FolderAsWorkspaceDock::~FolderAsWorkspaceDock()
{
    ApplicationSettings settings;
    settings.set(rootPathSetting, rootPath());

    delete ui;
}

void FolderAsWorkspaceDock::setRootPath(const QString dir)
{
    model->setRootPath(dir);
    ui->treeView->setRootIndex(model->index(dir));
}

QString FolderAsWorkspaceDock::rootPath() const
{
    return model->rootPath();
}

void FolderAsWorkspaceDock::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    if (!index.isValid()) {
        lastSelectedItem = model->index(rootPath());
        ui->menuEmpty->exec(ui->treeView->viewport()->mapToGlobal(point));
        return;
    }
    lastSelectedItem = index;
    if (model->isDir(index)) {
        ui->menuDirectory->exec(ui->treeView->viewport()->mapToGlobal(point));
    }
    else {
        ui->menuFile->exec(ui->treeView->viewport()->mapToGlobal(point));
    }
}

void FolderAsWorkspaceDock::on_actionSaveHere_triggered()
{
    QDir parentDir(model->filePath(lastSelectedItem));
    auto editor = window->currentEditor();
    QString dstName(parentDir.absoluteFilePath(editor->getName()));

    if (editor->saveAs(dstName) == QFileDevice::NoError) {
        auto newItem = model->index(dstName);
        if (!editor->isFile()) {
            editor->setFileInfo(dstName);
        }
        ui->treeView->setCurrentIndex(newItem);
        ui->treeView->edit(newItem);
    }
    else {
        qWarning("Unable to save %s", dstName.toUtf8().constData());
    }
}

void FolderAsWorkspaceDock::on_actionNewFolder_triggered()
{
    QDir parentDir(model->filePath(lastSelectedItem));
    int i = 1;

    for (;parentDir.exists(NEW_DIR_TEMPLATE.arg(i)); i++) {
    // Intentional
    }
    QString dstName = NEW_DIR_TEMPLATE.arg(i);

    auto newItem = model->mkdir(lastSelectedItem, dstName);
    if (newItem.isValid()) {
        ui->treeView->setCurrentIndex(newItem);
        ui->treeView->edit(newItem);
    }
    else {
        qWarning("Unable to create %s", dstName.toUtf8().constData());
    }
}

void FolderAsWorkspaceDock::on_actionRename_triggered()
{
    ui->treeView->setCurrentIndex(lastSelectedItem);
    ui->treeView->edit(lastSelectedItem);
}

void FolderAsWorkspaceDock::onFileRenamed(const QString &parentPath, const QString &oldName, const QString &newName)
{
    QDir parentDir(parentPath);
    QString oldPath = parentDir.absoluteFilePath(oldName);
    QString newPath = parentDir.absoluteFilePath(newName);

    window->forEachEditorByPath(oldPath, [=](ScintillaNext* editor) {
            editor->renameEditorPath(newPath);
        });
}

void FolderAsWorkspaceDock::on_actionDelete_triggered()
{
    QString path(model->filePath(lastSelectedItem));

    if (window->askDeletePermanent(path))
    {
        if (model->remove(lastSelectedItem)) {
            window->closeByPath(path);
        }
        else {
            qWarning("Unable to delete %s", path.toUtf8().constData());
        }
    }
}

void FolderAsWorkspaceDock::on_actionMoveToTrash_triggered()
{
    QString path(model->filePath(lastSelectedItem));

    if (window->askMoveToTrash(path)) {
        if (QFile::moveToTrash(path)) {
            window->closeByPath(path);
        }
        else {
            qWarning("Unable to remove %s", path.toUtf8().constData());
        }
    }
}
