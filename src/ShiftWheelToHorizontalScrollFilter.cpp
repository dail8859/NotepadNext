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

#include "ShiftWheelToHorizontalScrollFilter.h"

#include <QApplication>
#include <QEvent>
#include <QWheelEvent>


bool isHorizontalWheelEvent(QWheelEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return event->angleDelta().y() == 0;
#else
    return event->orientation() == Qt::Horizontal;
#endif
}

ShiftWheelToHorizontalScrollFilter::ShiftWheelToHorizontalScrollFilter(QObject *parent)
    : QObject(parent)
{
}

bool ShiftWheelToHorizontalScrollFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::Wheel) {
        return QObject::eventFilter(obj, event);
    }

    auto *wheelEvent = static_cast<QWheelEvent *>(event);

    if (!(wheelEvent->modifiers() & Qt::ShiftModifier)
        || (wheelEvent->modifiers() & Qt::ControlModifier)
        || isHorizontalWheelEvent(wheelEvent)) {
        return QObject::eventFilter(obj, event);
    }

    const QPoint angleDelta(wheelEvent->angleDelta().y(), wheelEvent->angleDelta().x());

    const QPoint pixelDelta(wheelEvent->pixelDelta().y(), wheelEvent->pixelDelta().x());

    QWheelEvent horizontalEvent(
        wheelEvent->position(),
        wheelEvent->globalPosition(),
        pixelDelta,
        angleDelta,
        wheelEvent->buttons(),
        wheelEvent->modifiers() & ~Qt::ShiftModifier,
        wheelEvent->phase(),
        wheelEvent->inverted(),
        wheelEvent->source(),
        wheelEvent->pointingDevice());

    QApplication::sendEvent(obj, &horizontalEvent);
    return true;
}
