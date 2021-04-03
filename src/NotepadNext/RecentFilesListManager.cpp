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


#include <QMenu>

#include "RecentFilesListManager.h"

RecentFilesListManager::RecentFilesListManager(QWidget *parent) :
    QObject(parent)
{
}

RecentFilesListManager::~RecentFilesListManager()
{
    clear();
}

void RecentFilesListManager::addFile(const QString &filePath)
{
    qInfo(Q_FUNC_INFO);

    // Attempt to remove it first to make sure it is not added twice
    removeFile(filePath);

    // Set a limit on how many can be in the list
    if (recentFiles.size() >= 10) {
        recentFiles.removeLast();
    }

    recentFiles.prepend(filePath);
}

void RecentFilesListManager::removeFile(const QString &filePath)
{
    recentFiles.removeOne(filePath);
}

void RecentFilesListManager::clear()
{
    // Clear the file list
    recentFiles.clear();
}

QString RecentFilesListManager::mostRecentFile() const
{
    Q_ASSERT(!recentFiles.empty());

    return recentFiles.first();
}

QStringList RecentFilesListManager::fileList() const
{
    return recentFiles;
}

void RecentFilesListManager::setFileList(const QStringList &list)
{
    clear();
    recentFiles.append(list);
}

void RecentFilesListManager::populateMenu(QMenu *menu)
{
    int i = 0;

    QList<QAction *> recentFileListActions;
    foreach (const QString &file, recentFiles) {
        ++i;
        QAction *action = new QAction(QString("%1%2: %3").arg(i < 10 ? "&" : "").arg(i).arg(file), menu);

        action->setData(file);
        connect(action, &QAction::triggered, this, &RecentFilesListManager::recentFileActionTriggered);

        recentFileListActions.append(action);
    }

    menu->addActions(recentFileListActions);
}

void RecentFilesListManager::recentFileActionTriggered()
{
    const QAction *action = qobject_cast<const QAction *>(sender());
    emit fileOpenRequest(action->data().toString());
}
