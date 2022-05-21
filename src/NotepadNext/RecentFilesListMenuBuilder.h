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


#ifndef RECENTFILESLISTMENUBUILDER_H
#define RECENTFILESLISTMENUBUILDER_H

#include "RecentFilesListManager.h"
#include <QObject>

class QMenu;

class RecentFilesListMenuBuilder : public QObject
{
    Q_OBJECT

public:
    explicit RecentFilesListMenuBuilder(RecentFilesListManager *manager);
    void populateMenu(QMenu *menu);

signals:
    void fileOpenRequest(const QString &filePath);

private slots:
    void recentFileActionTriggered();

private:
    RecentFilesListManager *manager;
};

#endif // RECENTFILESLISTMENUBUILDER_H
