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


#ifndef RECENTFILESLISTMANAGER_H
#define RECENTFILESLISTMANAGER_H

#include <QObject>
#include <QList>
#include <QAction>

class QMenu;

class RecentFilesListManager : public QObject
{
    Q_OBJECT

public:
    RecentFilesListManager(QObject *parent = Q_NULLPTR);
    ~RecentFilesListManager();

    void addFile(const QString &filePath);
    void removeFile(const QString &filePath);
    void clear();

    QString mostRecentFile() const;
    QStringList fileList() const;
    void setFileList(const QStringList &list);

    int count() const { return recentFiles.size(); }

    void populateMenu(QMenu *menu);

signals:
    void fileOpenRequest(const QString &filePath);

private slots:
    void recentFileActionTriggered();

private:
    QStringList recentFiles;
};

#endif // RECENTFILESLISTMANAGER_H
