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


#include "ZoomEventWatcher.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>


static bool isWheelEventHorizontal(QWheelEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return event->angleDelta().y() == 0;
#else
    return event->orientation() == Qt::Horizontal;
#endif
}

static int wheelEventYDelta(QWheelEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return event->angleDelta().y();
#else
    return event->delta();
#endif
}


ZoomEventWatcher::ZoomEventWatcher(QObject *parent)
    : QObject{parent}
{
}

bool ZoomEventWatcher::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        // Scintilla pastes the primary selection on middle-click. Suppress an
        // accidental wheel-button press while Shift is held for horizontal scrolling.
        if (mouseEvent->button() == Qt::MiddleButton
                && (mouseEvent->modifiers() & Qt::ShiftModifier)) {
            return true;
        }
    }

    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        const bool horizontal = isWheelEventHorizontal(wheelEvent);
        const Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();

        if (!horizontal) {
            if (modifiers & Qt::ControlModifier) {
                if (wheelEventYDelta(wheelEvent) > 0) {
                    emit zoomIn();
                } else {
                    emit zoomOut();
                }
                return true;
            }
        }

        if ((modifiers & Qt::ShiftModifier) && !(modifiers & Qt::ControlModifier)) {
            const QPoint angleDelta = horizontal
                    ? wheelEvent->angleDelta()
                    : QPoint(wheelEvent->angleDelta().y(), wheelEvent->angleDelta().x());
            const QPoint pixelDelta = horizontal
                    ? wheelEvent->pixelDelta()
                    : QPoint(wheelEvent->pixelDelta().y(), wheelEvent->pixelDelta().x());

            QWheelEvent horizontalEvent(wheelEvent->position(), wheelEvent->globalPosition(),
                                        pixelDelta, angleDelta, wheelEvent->buttons(),
                                        modifiers & ~Qt::ShiftModifier, wheelEvent->phase(),
                                        wheelEvent->inverted(), wheelEvent->source(),
                                        wheelEvent->pointingDevice());
            QApplication::sendEvent(obj, &horizontalEvent);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
