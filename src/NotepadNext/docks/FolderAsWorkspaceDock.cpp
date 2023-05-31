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
#include "ui_FolderAsWorkspaceDock.h"
#include "TitleBar.h"

#include <QFileSystemModel>
#include <QMenu>

FolderAsWorkspaceDock::FolderAsWorkspaceDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FolderAsWorkspaceDock),
    model(new QFileSystemModel(this)),
    m_pHideFile(nullptr)
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
    
    CTitleBar* pDockTitleBar = new CTitleBar(this);
    if(pDockTitleBar) {
        setTitleBarWidget(pDockTitleBar);
        QMenu* pMenu = new QMenu(tr("Filter"), pDockTitleBar);
        QString szTitle;
        QDir::Filters f = model->filter();
        if(QDir::Hidden & f)
        {
            szTitle = tr("Don't show hidden files");
        } else {
            szTitle = tr("show hidden files");
        }
        m_pHideFile = pMenu->addAction(szTitle, this, [&](){
            if(!(m_pHideFile && model)) return;
            QDir::Filters f = model->filter();
            if(QDir::Hidden & f)
            {
                m_pHideFile->setText(tr("show hidden files"));
                model->setFilter(f & (~QDir::Hidden));
            } else {
                m_pHideFile->setText(tr("Don't show hidden files"));
                model->setFilter(f | QDir::Hidden);
            }
        });
        QPushButton* pFilter = pDockTitleBar->CreateSmallPushButton(QIcon(":/icons/filter.svg"), pDockTitleBar);
        pFilter->setToolTip(tr("Filter"));
        pFilter->setMenu(pMenu);
        QList<QWidget*> lstWidget;
        lstWidget << pFilter;
        pDockTitleBar->SetWidgets(lstWidget);
    }
    
}

FolderAsWorkspaceDock::~FolderAsWorkspaceDock()
{
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

void FolderAsWorkspaceDock::setFilter(const QDir::Filters filter)
{
    if(!model) return;

    model->setFilter(filter);

    if(m_pHideFile) {
        if(QDir::Hidden & filter)
            m_pHideFile->setText(tr("Don't show hidden files"));
        else
            m_pHideFile->setText(tr("show hidden files"));
    }
}

QDir::Filters FolderAsWorkspaceDock::filter() const
{
    if(model)
        return model->filter();
    return QDir::AllEntries;
}
