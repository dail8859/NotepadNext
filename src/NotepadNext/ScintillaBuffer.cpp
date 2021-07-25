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


#include "ScintillaBuffer.h"
#include "uchardet.h"

#include <QDir>
#include <QSaveFile>
#include <QTextCodec>


#define SC_STATUS_OK 0
const int chunkSize = 1024 * 64; // Not sure what is best


static bool writeToDisk(const QByteArray &data, const QString &path)
{
    qInfo(Q_FUNC_INFO);

    QSaveFile file(path);
    file.setDirectWriteFallback(true);

    bool writeSuccessful = false;

    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        writeSuccessful = file.commit();
    }
    else {
        qWarning("writeBufferToDisk() failure: %s", qPrintable(file.errorString()));
        writeSuccessful = false;
    }

    return writeSuccessful;
}

ScintillaBuffer::ScintillaBuffer(const QString &name) :
    ScintillaDocument(),
    name(name),
    bufferType(ScintillaBuffer::Temporary)
{
}

ScintillaBuffer::~ScintillaBuffer()
{
}

QDateTime ScintillaBuffer::fileTimestamp()
{
    Q_ASSERT(type() != ScintillaBuffer::Temporary);

    fileInfo.refresh();
    qInfo("%s last modified %s", qUtf8Printable(fileInfo.fileName()), qUtf8Printable(fileInfo.lastModified().toString()));
    return fileInfo.lastModified();
}

void ScintillaBuffer::updateTimestamp()
{
    modifiedTime = fileTimestamp();
}

bool ScintillaBuffer::save()
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(isFile());

    bool writeSuccessful = writeToDisk(this->get_character_pointer(), this->fileInfo.filePath());

    if (writeSuccessful) {
        // Make sure the read only flag is cleared
        this->set_read_only(false);

        this->set_save_point();
        this->updateTimestamp();
    }

    return writeSuccessful;
}

bool ScintillaBuffer::saveAs(const QString &newFilePath)
{
    qInfo(Q_FUNC_INFO);

    bool isRenamed = !this->isFile() || this->fileInfo.canonicalFilePath() != newFilePath;

    bool saveSuccessful = writeToDisk(this->get_character_pointer(), newFilePath);

    if (saveSuccessful) {
        this->setFileInfo(newFilePath);

        // Make sure the read only flag is cleared
        this->set_read_only(false);

        // Set it to an actual file as it might be replacing a missing file
        this->bufferType = ScintillaBuffer::File;
        this->updateTimestamp();
        this->set_save_point();
    }

    return isRenamed && saveSuccessful;
}

bool ScintillaBuffer::saveCopyAs(const QString &filePath)
{
    return writeToDisk(this->get_character_pointer(), filePath);
}

bool ScintillaBuffer::reloadFromFile()
{
    Q_ASSERT(this->isFile());

    // Ensure the file still exists.
    if (!QFile::exists(this->fileInfo.canonicalFilePath())) {
        return false;
    }

    // Remove all the text
    {
        const QSignalBlocker blocker(this);
        this->set_undo_collection(false);
        this->delete_undo_history();
        this->delete_chars(0, this->length());
        this->set_undo_collection(true);
    }

    // NOTE: if the read fails then the buffer will be completely empty...which probably
    // isn't a good thing, but this should be a rare occurrence.
    QFile f(this->fileInfo.canonicalFilePath());
    bool readSuccessful = this->readFromDisk(f);

    if (readSuccessful) {
        // TODO: emit bufferReloaded?
    }

    this->set_save_point();
    this->updateTimestamp();

    return true;
}

void ScintillaBuffer::setFileInfo(const QString &filePath)
{
    fileInfo.setFile(filePath);
    fileInfo.makeAbsolute();

    Q_ASSERT(fileInfo.exists());

    name = fileInfo.fileName();
    bufferType = ScintillaBuffer::File;
}

bool ScintillaBuffer::isFile() const
{
    return bufferType == ScintillaBuffer::File || bufferType == ScintillaBuffer::FileMissing;
}

ScintillaBuffer::BufferType ScintillaBuffer::type() const
{
    return bufferType;
}

ScintillaBuffer::BufferStateChange ScintillaBuffer::checkForBufferStateChange()
{
    if (type() == ScintillaBuffer::Temporary) {
        return BufferStateChange::NoChange;
    }
    else if (type() == ScintillaBuffer::File) {
        // Make sure it exists
        fileInfo.refresh(); // refresh else exists() fails to notice missing file
        if (!fileInfo.exists()) {
            bufferType = BufferType::FileMissing;
            emit save_point(false); // Notify that this file is now dirty
            return BufferStateChange::Deleted;
        }

        // See if the timestamp changed
        if (modifiedTime != fileTimestamp()) {
            return BufferStateChange::Modified;
        }
        else {
            return BufferStateChange::NoChange;
        }
    }
    else if (type() == ScintillaBuffer::FileMissing) {
        // See if it reappeared
        fileInfo.refresh();
        if (fileInfo.exists()) {
            bufferType = BufferType::File;
            return BufferStateChange::Restored;
        }
        else {
            return BufferStateChange::NoChange;
        }
    }

    qInfo("type() = %d", type());
    Q_ASSERT(false);
    return BufferStateChange::NoChange;
}

bool ScintillaBuffer::moveToTrash()
{
    if (QFile::exists(this->fileInfo.canonicalFilePath())) {
        QFile f(this->fileInfo.canonicalFilePath());

        return f.moveToTrash();
    }

    return false;
}

bool ScintillaBuffer::isSavedToDisk()
{
    return bufferType != ScintillaBuffer::FileMissing && is_save_point();
}

ScintillaBuffer *ScintillaBuffer::fromFile(const QString &filePath)
{
    QFile file(filePath);
    QFileInfo info(filePath);

    // TODO: check permissions
    // On windows the permissions look like this:
    // 0110 0110 0110 0110 - normal file
    // 0100 0100 0100 0100 - readonly

    if(!info.exists()) {
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    auto buffer = new ScintillaBuffer(info.fileName());
    bool readSuccessful = buffer->readFromDisk(file);

    if (!readSuccessful) {
        delete buffer;
        return Q_NULLPTR;
    }

    // Set readonly flag if it is not writable
    if (!info.isWritable()) {
        buffer->set_read_only(true);
    }

    buffer->setFileInfo(filePath);
    buffer->updateTimestamp();

    return buffer;
}

bool ScintillaBuffer::readFromDisk(QFile &file)
{
    if (!file.exists()) {
        qWarning("Cannot read \"%s\": doesn't exist", qUtf8Printable(file.fileName()));
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Something bad happend when opening \"%s\": (%d) %s", qUtf8Printable(file.fileName()), file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    // TODO: figure out what to do if "size" is too big
    //this->allocate(file.size());

    // Turn off undo collection and block signals during loading
    this->set_undo_collection(false);
    this->blockSignals(true);

    QByteArray chunk;
    qint64 bytesRead;
    int sciError = SC_STATUS_OK;

    QTextDecoder *decoder = nullptr;
    bool first_read = true;
    do {
        // Try to read as much as possible
        chunk.resize(chunkSize);
        bytesRead = file.read(chunk.data(), chunkSize);

        chunk.resize(bytesRead);

        qDebug("Read %d bytes", bytesRead);

        if (first_read) {
            first_read = false;
            bool hasByteOrderMark = QTextCodec::codecForUtfText(chunk, nullptr) != nullptr;

            // Try uchardet library first
            uchardet_t ud = uchardet_new();
            if (uchardet_handle_data(ud, chunk.constData(), chunk.size()) != 0) {
                qWarning("uchardet failed to detect encoding");
            }
            uchardet_data_end(ud);

            QByteArray encoding(uchardet_get_charset(ud));
            uchardet_delete(ud);

            qInfo("Encoding detected as: %s", qUtf8Printable(encoding));

            QTextCodec *codec = QTextCodec::codecForName(encoding);
            if (codec) {
                decoder = codec->makeDecoder();
            } else {
                qWarning("No avialable Codecs for: \"%s\"", qUtf8Printable(encoding));
                qWarning("Falling back to QTextCodec::codecForUtfText()");

                if (chunk.size() >= 2)
                    qWarning("%d %d", chunk.at(0), chunk.at(1));

                codec = QTextCodec::codecForUtfText(chunk);
                decoder = codec->makeDecoder();

                qWarning("Using: %s", qUtf8Printable(codec->name()));
            }
        }

        QByteArray utf8_data = decoder->toUnicode(chunk).toUtf8();
        sciError = this->add_data(utf8_data.constData(), utf8_data.size());
    } while (!file.atEnd() && sciError == SC_STATUS_OK);

    delete decoder;

    file.close();

    // Restore it back
    this->blockSignals(false);
    this->set_undo_collection(true);

    if (sciError != SC_STATUS_OK) {
        qWarning("something bad happend in document->add_data() %d", sciError);
        return false;
    }

    if (bytesRead == -1) {
        qWarning("Something bad happend when reading disk %d %s", file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    return true;
}

QString ScintillaBuffer::getName() const
{
    return name;
}
