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


#include "NppTabBar.h"

#include <QMouseEvent>

NppTabBar::NppTabBar(QWidget *parent) :
    QTabBar(parent)
{
    // Only set absolutely required properties. Let the CSS configure everything else
    setObjectName(QStringLiteral("nppTabBar"));
    setAutoHide(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void NppTabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        int index = tabAt(event->pos());
        if (index != -1) {
            emit tabCloseRequested(index);
            event->accept();
            return;
        }
    }

    QTabBar::mousePressEvent(event);
}
