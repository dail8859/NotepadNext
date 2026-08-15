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
#include <qmenu.h>

ApplicationSetting<QString> rootPathSetting{"FolderAsWorkspace/RootPath"};

namespace {
    // QFileSystemModel's standard column order.
    enum FileSystemColumn
    {
        NameColumn = 0,
        SizeColumn,
        TypeColumn,
        DateColumn
    };
}

FolderAsWorkspaceDock::FolderAsWorkspaceDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FolderAsWorkspaceDock),
    model(new QFileSystemModel(this))
{
    ui->setupUi(this);

    ui->treeView->setModel(model);

    QMenu *menu = new QMenu(ui->toolButton);
    menu->addAction(ui->actionShowSize);
    menu->addAction(ui->actionShowType);
    menu->addAction(ui->actionShowDateModified);
    menu->addSeparator();
    menu->addAction(ui->actionShowHidden);

    ui->toolButton->setMenu(menu);
    ui->toolButton->setPopupMode(QToolButton::MenuButtonPopup);

    auto bindColumnVisibility = [this](QAction* action, int column) {
        ui->treeView->setColumnHidden(column, !action->isChecked());
        connect(action, &QAction::toggled, this, [this, column](bool checked) {
            ui->treeView->setColumnHidden(column, !checked);
        });
    };

    auto bindActionToSetting = [](QAction* action, const QString& key) {
        ApplicationSettings settings;

        action->setChecked(settings.value(key, false).toBool());
        connect(action, &QAction::toggled, [key](bool checked) {
            ApplicationSettings settings;
            settings.setValue(key, checked);
        });
    };

    auto bindColumn = [&](QAction* action, int column, const QString& key) {
        bindColumnVisibility(action, column);
        bindActionToSetting(action, key);
    };

    bindColumn(ui->actionShowSize, SizeColumn, "FolderAsWorkspace/ShowSize");
    bindColumn(ui->actionShowType, TypeColumn, "FolderAsWorkspace/ShowType");
    bindColumn(ui->actionShowDateModified, DateColumn, "FolderAsWorkspace/ShowDateModified");

    connect(ui->actionShowHidden, &QAction::toggled, this, [this](bool showHidden) {
        auto filters = QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot;
        if (showHidden) filters |= QDir::Hidden;
        model->setFilter(filters);
    });
    bindActionToSetting(ui->actionShowHidden, "FolderAsWorkspace/ShowHidden");

    // Default sorting
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);

    connect(ui->treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &sourceIndex) {
        const QFileInfo info = model->fileInfo(sourceIndex);

        if (info.isFile()) {
            emit fileDoubleClicked(info.absoluteFilePath());
        }
    });

    // Configure header settings
    QHeaderView *header = ui->treeView->header();
    header->setSectionResizeMode(NameColumn, QHeaderView::Stretch);
    header->setSectionResizeMode(SizeColumn, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(TypeColumn, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(DateColumn, QHeaderView::ResizeToContents);

    ApplicationSettings settings;
    setRootPath(settings.get(rootPathSetting));
}

FolderAsWorkspaceDock::~FolderAsWorkspaceDock()
{
    delete ui;
}

void FolderAsWorkspaceDock::setRootPath(const QString &dir)
{
    ApplicationSettings settings;
    settings.set(rootPathSetting, dir);

    const QModelIndex sourceIndex = model->setRootPath(dir);
    ui->treeView->setRootIndex(sourceIndex);
}

QString FolderAsWorkspaceDock::rootPath() const
{
    return model->rootPath();
}
