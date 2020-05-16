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


#include "HighlightedScrollBar.h"
#include "ScintillaNext.h"
#include "SmartHighlighter.h"

#include <QMouseEvent>

ScintillaNext::ScintillaNext(ScintillaBuffer *buffer, QWidget *parent) :
    ScintillaEdit(parent)
{
    if (buffer) {
        setDocPointer(reinterpret_cast<sptr_t>(buffer->pointer()));

        QObject *obj = this;
        QVariant var = QVariant::fromValue(buffer);
        obj->setProperty("ScintillaBufferPointer", var);
    }
}

ScintillaBuffer *ScintillaNext::scintillaBuffer()
{
    QObject *obj = this;
    auto var = obj->property("ScintillaBufferPointer");

    return var.value<ScintillaBuffer*>();
}

void ScintillaNext::dragEnterEvent(QDragEnterEvent *event)
{
    // Ignore all drag and drop events with urls and let the main application handle it
    if (event->mimeData()->hasUrls()) {
        return;
    }

    ScintillaEdit::dragEnterEvent(event);
}

void ScintillaNext::dropEvent(QDropEvent *event)
{
    // Ignore all drag and drop events with urls and let the main application handle it
    if (event->mimeData()->hasUrls()) {
        return;
    }

    ScintillaEdit::dropEvent(event);
}
