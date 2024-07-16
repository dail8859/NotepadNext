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


#ifndef SEARCHRESULTSDOCK_H
#define SEARCHRESULTSDOCK_H

#include <QDockWidget>

#include "ISearchResultsHandler.h"

namespace Ui {
class SearchResultsDock;
}

class QTreeWidgetItem;
class ScintillaNext;

class SearchResultsDock : public QDockWidget, public ISearchResultsHandler
{
    Q_OBJECT

public:
    explicit SearchResultsDock(QWidget *parent = nullptr);
    ~SearchResultsDock();

    void newSearch(const QString searchTerm) override;
    void newFileEntry(ScintillaNext *editor) override;
    void newResultsEntry(const QString line, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning, int hitCount=1) override;
    void completeSearch() override;

public slots:
    void collapseAll() const;
    void expandAll() const;
    void deleteEntry(QTreeWidgetItem *item);
    void deleteAll();

private slots:
    void itemActivated(QTreeWidgetItem *item, int column);
    void itemExpanded(QTreeWidgetItem *item);
    void copySearchResultsToClipboard() ;


signals:
    void searchResultActivated(ScintillaNext *editor, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning);

private:
    void updateSearchStatus();
    Ui::SearchResultsDock *ui;

    QString searchTerm;
    QString currentFilePath;

    int currentFileCount = 0;
    int totalFileHitCount = 0;
    int totalHitCount = 0;

    QTreeWidgetItem *currentSearch = Q_NULLPTR;
    QTreeWidgetItem *currentFile = Q_NULLPTR;
};

#endif // SEARCHRESULTSDOCK_H
