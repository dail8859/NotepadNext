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
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        if (!isWheelEventHorizontal(wheelEvent)) {
            if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                if (wheelEventYDelta(wheelEvent) > 0) {
                    emit zoomIn();
                    qInfo("%s", "zoom in");//sqt->KeyCommand(Message::ZoomIn);
                } else {
                    emit zoomOut();
                    qInfo("%s", "zoom out");//sqt->KeyCommand(Message::ZoomOut);
                }
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
