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


#include "BufferManager.h"
#include "Scintilla.h"

#include <ScintillaBuffer.h>
#include <QSaveFile>
#include <QDir>

BufferManager::BufferManager(QObject *parent) :
    QObject(parent)
{
    connect(this, &BufferManager::bufferCreated, this, &BufferManager::detectEols);
    connect(this, &BufferManager::bufferCreated, this, &BufferManager::detectEncoding);
}

BufferManager::~BufferManager()
{
    Q_ASSERT(buffers.size() == 0);
}

void BufferManager::manageBuffer(ScintillaBuffer *buffer)
{
    buffers.append(buffer);

#ifdef QT_DEBUG
    connect(buffer, &ScintillaBuffer::modify_attempt, []() { qInfo("ScintillaBuffer::modify_attempt()");});
    connect(buffer, &ScintillaBuffer::save_point, [](bool atSavePoint) { qInfo("ScintillaBuffer::save_point(%s)", atSavePoint ? "true" : "false");});
    connect(buffer, &ScintillaBuffer::modified, [](int position, int modification_type, const QByteArray& text, int length, int linesAdded, int line, int foldLevelNow, int foldLevelPrev) {
        Q_UNUSED(position);
        Q_UNUSED(text);
        Q_UNUSED(length);
        Q_UNUSED(linesAdded);
        Q_UNUSED(line);
        Q_UNUSED(foldLevelNow);
        Q_UNUSED(foldLevelPrev);
        qInfo("ScintillaBuffer::modified(%d)", modification_type);
    });
    connect(buffer, &ScintillaBuffer::style_needed, [](int pos) { qInfo("ScintillaBuffer::style_needed(%d)", pos);});
    connect(buffer, &ScintillaBuffer::lexer_changed, []() { qInfo("ScintillaBuffer::lexer_changed()");});
    connect(buffer, &ScintillaBuffer::error_occurred, [](int status) { qInfo("ScintillaBuffer::error_occurred(%d)", status);});
#endif
}

void BufferManager::detectEols(ScintillaBuffer *buffer)
{
    if (buffer->length() == 0)
        return;

    // TODO: not the most efficient way of doing this
    // Can check/warn for mixed line endings

    const auto data = buffer->get_character_pointer().left(1024);
    if (data.contains("\r\n"))
        buffer->set_eol_mode(SC_EOL_CRLF);
    else if (data.contains("\n"))
        buffer->set_eol_mode(SC_EOL_LF);
    else if (data.contains("\r"))
        buffer->set_eol_mode(SC_EOL_CR);
}

void BufferManager::detectEncoding(ScintillaBuffer *buffer)
{
    if (buffer->length() == 0)
        return;

    // TODO: just do this for now
    buffer->set_code_page(SC_CP_UTF8);
}

ScintillaBuffer *BufferManager::createEmtpyBuffer(const QString &name)
{
    auto buffer = new ScintillaBuffer(name);
    manageBuffer(buffer);
    emit bufferCreated(buffer);
    return buffer;
}

ScintillaBuffer *BufferManager::createBufferFromFile(const QString &filePath)
{
    // TODO: see if it exists first?

    auto buffer = ScintillaBuffer::fromFile(filePath);
    manageBuffer(buffer);
    emit bufferCreated(buffer);
    return buffer;
}

ScintillaBuffer *BufferManager::getBufferByFilePath(const QString &filePath)
{
    QFileInfo newInfo(filePath);
    newInfo.makeAbsolute();

    foreach (ScintillaBuffer *buffer, buffers) {
        if (buffer->fileInfo == newInfo) {
            return buffer;
        }
    }

    return Q_NULLPTR;
}

bool BufferManager::saveBuffer(ScintillaBuffer *buffer, bool forceSave, const QFileInfo *fileInfo)
{
    qInfo(Q_FUNC_INFO);

    fileInfo = fileInfo ? fileInfo : &buffer->fileInfo;

    if (buffer->type() == ScintillaBuffer::Temporary && fileInfo != Q_NULLPTR) {
        buffer->saveAs(fileInfo->filePath());
    }
    else if (buffer->isFile() && (forceSave || !buffer->is_save_point())) {
        buffer->save();
    }
    else {
        return false;
    }

    return true;
}

void BufferManager::closeBuffer(ScintillaBuffer *buffer)
{
    qInfo(Q_FUNC_INFO);

    bool b = buffers.removeOne(buffer);

    Q_ASSERT(b == true);

    // Note: the ScintillaBuffer holds a reference to the document because of the WatcherHelper
    // it uses internally. Once the ScintillaBuffer is deleted, it releases this single reference
    // which causes the document to get cleaned up.

    emit bufferClosed(buffer);
    buffer->deleteLater();
}

bool BufferManager::saveBufferAs(ScintillaBuffer *buffer, const QString &newFilePath)
{
    if (buffer->saveAs(newFilePath)) {
        emit bufferRenamed(buffer);
        return true;
    }
    return false;
}

bool BufferManager::saveBufferCopyAs(ScintillaBuffer *buffer, const QString &filePath)
{
    return buffer->saveCopyAs(filePath);
}

bool BufferManager::renameBuffer(ScintillaBuffer *buffer, const QString &newFilePath)
{
    // TODO: check if newFilePath is already opened as another buffer?

    // Write out the buffer to the new path
    if (saveBufferCopyAs(buffer, newFilePath)) {
        // Remove the old file
        const QString oldPath = buffer->fileInfo.canonicalFilePath();
        QFile::remove(oldPath);

        // Everything worked fine, so update the buffer's info
        buffer->setFileInfo(newFilePath);
        buffer->updateTimestamp();
        buffer->set_save_point();

        emit bufferRenamed(buffer);

        return true;
    }

    return false;
}

bool BufferManager::reloadBuffer(ScintillaBuffer *buffer)
{
    return buffer->reloadFromFile();
}
