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


#include "SearchResultsDock.h"
#include "ui_SearchResultsDock.h"

SearchResultsDock::SearchResultsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchResultsDock)
{
    ui->setupUi(this);
}

SearchResultsDock::~SearchResultsDock()
{
    delete ui;
}

void SearchResultsDock::newSearch(const QString searchTerm)
{
    this->searchTerm = searchTerm;

    ui->treeWidget->collapseAll();

    currentSearch = new QTreeWidgetItem(ui->treeWidget);
    currentSearch->setBackground(0, QColor(232, 232, 255));
    currentSearch->setForeground(0, QColor(0, 0, 170));
    currentSearch->setExpanded(true);

    updateSearchStatus();
}

void SearchResultsDock::newFileEntry(const QString filePath)
{
    totalFileHitCount = 0;
    currentFilePath = filePath;

    currentFile = new QTreeWidgetItem(currentSearch);

    currentFile->setBackground(0, QColor(213, 255, 213));
    currentFile->setForeground(0, QColor(0, 128, 0));
    currentFile->setExpanded(true);

    currentFileCount++;
    updateSearchStatus();
}

void SearchResultsDock::newResultsEntry(const QString line)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(currentFile);
    item->setText(0, line);

    totalFileHitCount++;
    totalHitCount++;
    updateSearchStatus();
}

void SearchResultsDock::completeSearch()
{
    currentSearch = Q_NULLPTR;
    currentFile = Q_NULLPTR;
    currentFileCount = 0;
    totalFileHitCount = 0;
    totalHitCount = 0;
}

void SearchResultsDock::updateSearchStatus()
{
    currentSearch->setText(0, QStringLiteral("Search \"%1\" (%2 hits in %3 file)").arg(searchTerm).arg(totalHitCount).arg(currentFileCount));

    if (currentFile)
        currentFile->setText(0, QStringLiteral("%1 (%2 hits)").arg(currentFilePath).arg(totalFileHitCount));
}
