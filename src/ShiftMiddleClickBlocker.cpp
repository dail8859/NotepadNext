/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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

#include "ShiftMiddleClickBlocker.h"

#include <QEvent>
#include <QMouseEvent>

ShiftMiddleClickBlocker::ShiftMiddleClickBlocker(QObject *parent)
    : QObject(parent)
{
}

bool ShiftMiddleClickBlocker::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonDblClick) {
        return QObject::eventFilter(obj, event);
    }

    auto *mouseEvent = static_cast<QMouseEvent *>(event);

    if (mouseEvent->button() == Qt::MiddleButton && (mouseEvent->modifiers() & Qt::ShiftModifier)) {
        return true;
    }

    return QObject::eventFilter(obj, event);
}