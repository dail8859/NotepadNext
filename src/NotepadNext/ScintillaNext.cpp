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

#include "ScintillaNext.h"

#include <QMouseEvent>


ScintillaNext::ScintillaNext(ScintillaBuffer *buffer, QWidget *parent) :
    ScintillaEdit(parent)
{
    if (buffer) {
        setDocPointer(reinterpret_cast<sptr_t>(buffer->pointer()));

        this->buffer = buffer;
    }
}

bool ScintillaNext::isSavedToDisk()
{
    return this->buffer->isSavedToDisk();
}

bool ScintillaNext::isFile()
{
    return this->buffer->isFile();
}

QFileInfo ScintillaNext::fileInfo()
{
    return this->buffer->fileInfo;
}

QString ScintillaNext::getName()
{
    return this->buffer->getName();
}

QString ScintillaNext::canonicalFilePath()
{
    return this->buffer->fileInfo.canonicalFilePath();
}

QString ScintillaNext::suffix()
{
    return this->buffer->fileInfo.suffix();
}

void ScintillaNext::close()
{
    emit closed();

    deleteLater();
}

bool ScintillaNext::save()
{
    emit aboutToSave();

    return buffer->save();
}

void ScintillaNext::reload()
{
    buffer->reloadFromFile();
}

bool ScintillaNext::saveAs(const QString &newFilePath)
{
    // Store the type of the buffer before it gets saved (which can change the type)
    const ScintillaBuffer::BufferType origType = buffer->type();

    emit aboutToSave();

    const bool ret = buffer->saveAs(newFilePath);

    if (ret && origType == ScintillaBuffer::Temporary) {
        emit renamed();
    }

    return ret;
}

bool ScintillaNext::saveCopyAs(const QString &filePath)
{
    return buffer->saveCopyAs(filePath);
}

bool ScintillaNext::rename(const QString &newFilePath)
{
    // TODO: check if newFilePath is already opened as another editor?

    emit aboutToSave();

    // Write out the buffer to the new path
    if (saveCopyAs(newFilePath)) {
        // Remove the old file
        const QString oldPath = buffer->fileInfo.canonicalFilePath();
        QFile::remove(oldPath);

        // Everything worked fine, so update the buffer's info
        buffer->setFileInfo(newFilePath);
        buffer->updateTimestamp();
        buffer->set_save_point();

        emit renamed();

        return true;
    }

    return false;
}

ScintillaNext::FileStateChange ScintillaNext::checkFileForStateChange()
{
    // TODO: Is this the best way to handle this? It is a simple renaming of enums
    // but at least for now this hides the ScintillaBuffer implementation
    ScintillaBuffer::BufferStateChange bufferState = this->buffer->checkForBufferStateChange();

    switch (bufferState) {
        case ScintillaBuffer::NoChange: return ScintillaNext::NoChange;
        case ScintillaBuffer::Modified: return ScintillaNext::Modified;
        case ScintillaBuffer::Deleted: return ScintillaNext::Deleted;
        case ScintillaBuffer::Restored: return ScintillaNext::Restored;
    }

    return ScintillaNext::NoChange;
}

bool ScintillaNext::moveToTrash()
{
    return this->buffer->moveToTrash();
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
