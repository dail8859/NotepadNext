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


#ifndef STATUSLABEL_H
#define STATUSLABEL_H

#include <QLabel>
#include <QMouseEvent>


class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(parent, f) {}
    explicit ClickableLabel(const QString &text, QWidget *parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(text, parent, f) {}

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override { Q_UNUSED(event); emit clicked(); }
    void mouseDoubleClickEvent(QMouseEvent *event) override { emit customContextMenuRequested(event->pos()); }
};


class StatusLabel : public ClickableLabel
{
    Q_OBJECT

public:
    explicit StatusLabel(int size = 200, QWidget *parent = 0) : ClickableLabel(parent), preferredSize(size) {
        // Since these get set alot and plain text is always used go ahead and set the format
        setTextFormat(Qt::PlainText);
    }

protected:
    QSize sizeHint() const override {
        QSize size = ClickableLabel::sizeHint();
        size.setWidth(preferredSize);
        return size;
    }

    QSize minimumSizeHint() const override { return QSize(); }

private:
    int preferredSize;
};

#endif // STATUSLABEL_H
