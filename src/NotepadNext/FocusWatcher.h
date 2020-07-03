/*
 * This file is part of Notepad Next.
 * Copyright 2020 Justin Dailey
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


#ifndef FOCUSWATCHER_H
#define FOCUSWATCHER_H

#include <QEvent>
#include <QObject>


class FocusWatcher : public QObject {
    Q_OBJECT

public:
    explicit FocusWatcher(QObject* parent = nullptr) : QObject(parent) {
        if (parent)
            parent->installEventFilter(this);
    }

 protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        Q_UNUSED(obj)

        if (event->type() == QEvent::FocusIn) {
            emit focusChanged(true);
            emit focusIn();
        }
        else if (event->type() == QEvent::FocusOut) {
            emit focusChanged(false);
            emit focusOut();
        }

        return false;
    }

signals:
    void focusChanged(bool in);
    void focusIn();
    void focusOut();
};

#endif // FOCUSWATCHER_H
