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


#ifndef ZOOMEVENTWATCHER_H
#define ZOOMEVENTWATCHER_H

#include <QObject>

class ZoomEventWatcher : public QObject
{
    Q_OBJECT

public:
    explicit ZoomEventWatcher(QObject *parent = nullptr);

signals:
    void zoomIn();
    void zoomOut();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // ZOOMEVENTWATCHER_H
