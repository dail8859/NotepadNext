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
#include "ScintillaNext.h"
#include "ui_SearchResultsDock.h"

#include <QKeyEvent>
#include <QPointer>
#include <QMenu>
#include <QShortcut>
#include <QClipboard>

enum SearchResultData
{
    LineNumber = Qt::UserRole,
    LinePosStart,
    LinePosEnd
};

SearchResultsDock::SearchResultsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchResultsDock)
{
    ui->setupUi(this);

#ifdef Q_OS_MACOS
    // Set a slightly larger font on MacOS
    QFont font("Courier New", 14);
    ui->treeWidget->setFont(font);
#endif

    // Close the results when escape is pressed
    new QShortcut(QKeySequence::Cancel, this, this, &SearchResultsDock::close, Qt::WidgetWithChildrenShortcut);

    connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &SearchResultsDock::itemActivated);
    connect(ui->treeWidget, &QTreeWidget::itemExpanded, this, &SearchResultsDock::itemExpanded);
    connect(ui->btnCopyResults, &QPushButton::released,this, &SearchResultsDock::copySearchResultsToClipboard);

    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);

        if (item == Q_NULLPTR) {
            return;
        }

        // Create the menu and show it
        QMenu menu(this);
        menu.addAction(tr("Collapse All"), this, &SearchResultsDock::collapseAll);
        menu.addAction(tr("Expand All"), this, &SearchResultsDock::expandAll);
        menu.addSeparator();
        menu.addAction(tr("Delete Entry"), this, [=]() { deleteEntry(item); });
        menu.addSeparator();
        menu.addAction(tr("Delete All"), this, &SearchResultsDock::deleteAll);

        menu.exec(QCursor::pos());
    });
}

SearchResultsDock::~SearchResultsDock()
{
    delete ui;
}

void SearchResultsDock::newSearch(const QString searchTerm)
{
    show();

    this->searchTerm = searchTerm;

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        const QTreeWidgetItem* topLevelItem = ui->treeWidget->topLevelItem(i);
        ui->treeWidget->collapseItem(topLevelItem);
    }

    currentSearch = new QTreeWidgetItem(ui->treeWidget);
    currentSearch->setBackground(0, QColor(232, 232, 255));
    currentSearch->setForeground(0, QColor(0, 0, 170));
    currentSearch->setExpanded(true);
    currentSearch->setFirstColumnSpanned(true);

    updateSearchStatus();
}

void SearchResultsDock::newFileEntry(ScintillaNext *editor)
{
    // Store a QPointer since there is no guarantee this editor will be around later
    QPointer<ScintillaNext> editor_pointer = editor;

    totalFileHitCount = 0;
    currentFilePath = editor->isFile() ? editor->getFilePath() : editor->getName();

    currentFile = new QTreeWidgetItem(currentSearch);
    currentFile->setData(0, Qt::UserRole, QVariant::fromValue(editor_pointer));

    currentFile->setBackground(0, QColor(213, 255, 213));
    currentFile->setForeground(0, QColor(0, 128, 0));
    currentFile->setExpanded(true);
    currentFile->setFirstColumnSpanned(true);

    currentFileCount++;
    updateSearchStatus();
}

void SearchResultsDock::newResultsEntry(const QString line, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning, int hitCount)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(currentFile);

    // Scintilla internally references line numbers starting at 0, however it needs displayed starting at 1
    item->setText(0, QString::number(lineNumber + 1));
    item->setData(0, SearchResultData::LineNumber, lineNumber);
    item->setData(0, SearchResultData::LinePosStart, startPositionFromBeginning);
    item->setData(0, SearchResultData::LinePosEnd, endPositionFromBeginning);
    item->setBackground(0, QBrush(QColor(220, 220, 220)));
    item->setTextAlignment(0, Qt::AlignRight);

    item->setText(1, line);

    totalFileHitCount += hitCount;
    totalHitCount += hitCount;

    updateSearchStatus();
}

void SearchResultsDock::completeSearch()
{
    currentSearch = Q_NULLPTR;
    currentFile = Q_NULLPTR;
    currentFileCount = 0;
    totalFileHitCount = 0;
    totalHitCount = 0;

    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
}

void SearchResultsDock::collapseAll() const
{
    ui->treeWidget->collapseAll();
}

void SearchResultsDock::expandAll() const
{
    ui->treeWidget->expandAll();
}

void SearchResultsDock::deleteEntry(QTreeWidgetItem *item)
{
    QTreeWidgetItem *parent = item->parent();

    if (parent != Q_NULLPTR) {
        parent->removeChild(item);
        delete item;
    }
    else {
        const int index = ui->treeWidget->indexOfTopLevelItem(item);
        delete ui->treeWidget->takeTopLevelItem(index);
    }
}

void SearchResultsDock::deleteAll()
{
    ui->treeWidget->clear();
}

void SearchResultsDock::itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    // Result entries have no children
    // Make sure the entry has a parent since search entries can have no children
    if (item->childCount() == 0 && item->parent() != Q_NULLPTR) {
        QPointer<ScintillaNext> editor = item->parent()->data(0, Qt::UserRole).value<QPointer<ScintillaNext>>();

        // The editor may no longer exist
        if (editor) {
            int lineNumber = item->data(0, SearchResultData::LineNumber).toInt();
            int startPositionFromBeginning = item->data(0, SearchResultData::LinePosStart).toInt();
            int endPositionFromBeginning = item->data(0, SearchResultData::LinePosEnd).toInt();

            emit searchResultActivated(editor, lineNumber, startPositionFromBeginning, endPositionFromBeginning);
        }
    }
}

void SearchResultsDock::itemExpanded(QTreeWidgetItem *)
{
    ui->treeWidget->resizeColumnToContents(1);
}

void SearchResultsDock::updateSearchStatus()
{
    currentSearch->setText(0, QStringLiteral("Search \"%1\" (%L2 hits in %L3 files)").arg(searchTerm).arg(totalHitCount).arg(currentFileCount));

    if (currentFile)
        currentFile->setText(0, QStringLiteral("%1 (%L2 hits)").arg(currentFilePath).arg(totalFileHitCount));
}

void SearchResultsDock::copySearchResultsToClipboard()
{
    QStringList results;
    QTreeWidgetItemIterator it(ui->treeWidget);

    while (*it) {
        const QTreeWidgetItem *item = *it;
        results.append(QStringLiteral("%1 %2").arg(item->text(0)).arg(item->text(1)));
        ++it;
    }

    QGuiApplication::clipboard()->setText(results.join('\n'));
}

