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


#include "FileListDock.h"
#include "ui_FileListDock.h"

#include "MainWindow.h"

FileListDock::FileListDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::FileList),
    window(parent)
{
    qInfo(Q_FUNC_INFO);

    ui->setupUi(this);

    connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            // Only get events when the dock is visible
            connect(window->getDockedEditor(), &DockedEditor::editorAdded, this, &FileListDock::addEditor);
            connect(window->getDockedEditor(), &DockedEditor::editorActivated, this, &FileListDock::selectCurrentEditor);
            connect(window->getDockedEditor(), &DockedEditor::editorClosed, this, &FileListDock::removeEditor);

            refreshList();
            selectCurrentEditor();
        }
        else {
            // Disconnect all the editors
            for(int i = 0; i < ui->listWidget->count(); ++i) {
                QListWidgetItem* item = ui->listWidget->item(i);
                const ScintillaNext *other = item->data(Qt::UserRole).value<ScintillaNext *>();

                disconnect(other, Q_NULLPTR, this, Q_NULLPTR);
            }

            // Don't need to clear it but why not
            ui->listWidget->clear();

            // Don't need events from the docked editor either
            disconnect(window->getDockedEditor(), Q_NULLPTR, this, Q_NULLPTR);
        }
    });

    connect(ui->listWidget, &QListWidget::itemClicked, this, &FileListDock::itemClicked);
}

FileListDock::~FileListDock()
{
    delete ui;
}

void FileListDock::refreshList()
{
    qInfo(Q_FUNC_INFO);

    ui->listWidget->clear();

    for (ScintillaNext *editor : window->getDockedEditor()->editors()) {
        addEditor(editor);
    }
}

void FileListDock::addEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    QListWidgetItem *item = new QListWidgetItem(editor->getName());
    item->setToolTip(editor->getName());
    item->setIcon(QIcon(":/icons/saved.png"));
    item->setData(Qt::UserRole, QVariant::fromValue(editor));
    item->setIcon(QIcon(editor->canSaveToDisk() ? ":/icons/unsaved.png" : ":/icons/saved.png"));

    // Need some notifications from the editor itself
    // NOTE: Cannot use a lambda here because item is not a QObject, and thus cannot be used as a context for Qt to know
    // when to cancel the connection. If item was a QObject, the lambda is disconnected when the sender or context is destroyed
    connect(editor, &ScintillaNext::savePointChanged, this, &FileListDock::editorSavePointChanged, Qt::UniqueConnection);
    connect(editor, &ScintillaNext::renamed, this, &FileListDock::editorRenamed, Qt::UniqueConnection);

    ui->listWidget->addItem(item);
}

void FileListDock::removeEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    QListWidgetItem* item = lookupItemByEditor(editor);

    if (item) {
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
}

void FileListDock::selectCurrentEditor()
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *editor = window->currentEditor();
    QListWidgetItem* item = lookupItemByEditor(editor);

    if (item) {
        ui->listWidget->setCurrentItem(item);
    }
}

void FileListDock::itemClicked(QListWidgetItem *item)
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *editor = item->data(Qt::UserRole).value<ScintillaNext *>();

    window->getDockedEditor()->switchToEditor(editor);
}

void FileListDock::editorSavePointChanged(bool dirty)
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());

    if (editor) {
        QListWidgetItem* item = lookupItemByEditor(editor);

        if (item) {
            const QString iconPath = editor->canSaveToDisk() ? ":/icons/unsaved.png" : ":/icons/saved.png";
            item->setIcon(QIcon(iconPath));
        }
    }
}

void FileListDock::editorRenamed()
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());

    if (editor) {
        QListWidgetItem* item = lookupItemByEditor(editor);

        if (item) {
            item->setText(editor->getName());
            item->setToolTip(editor->getName());
        }
    }
}

QListWidgetItem *FileListDock::lookupItemByEditor(ScintillaNext *editor)
{
    for(int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        const ScintillaNext *other = item->data(Qt::UserRole).value<ScintillaNext *>();

        if (other == editor) {
            return item;
        }
    }

    return Q_NULLPTR;
}
