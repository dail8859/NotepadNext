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
#include "uchardet.h"

#include <QDir>
#include <QMouseEvent>
#include <QSaveFile>
#include <QTextCodec>


const int CHUNK_SIZE = 1024 * 64; // Not sure what is best


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
        qWarning("writeToDisk() failure: %s", qPrintable(file.errorString()));
        writeSuccessful = false;
    }

    return writeSuccessful;
}


ScintillaNext::ScintillaNext(QString name, QWidget *parent) :
    ScintillaEdit(parent),
    name(name)
{
}

ScintillaNext *ScintillaNext::fromFile(const QString &filePath)
{
    QFileInfo info(filePath);

    // TODO: check file permissions

    if(!info.exists()) {
        return Q_NULLPTR;
    }

    ScintillaNext *editor = new ScintillaNext(info.fileName());

    QFile file(filePath);
    bool readSuccessful = editor->readFromDisk(file);

    if (!readSuccessful) {
        delete editor;
        return Q_NULLPTR;
    }

    editor->setFileInfo(filePath);
    editor->updateTimestamp();

    return editor;
}

bool ScintillaNext::isSavedToDisk() const
{
    return bufferType != ScintillaNext::FileMissing && !modify();
}

bool ScintillaNext::isFile() const
{
    return bufferType == ScintillaNext::File || bufferType == ScintillaNext::FileMissing;;
}

QFileInfo ScintillaNext::getFileInfo() const
{
    Q_ASSERT(isFile());

    return fileInfo;
}

QString ScintillaNext::getFilePath() const
{
    Q_ASSERT(isFile());

    return QDir::toNativeSeparators(fileInfo.canonicalFilePath());
}

void ScintillaNext::close()
{
    emit closed();

    deleteLater();
}

bool ScintillaNext::save()
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(isFile());

    emit aboutToSave();

    bool writeSuccessful = writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), fileInfo.filePath());

    if (writeSuccessful) {
        setSavePoint();
        updateTimestamp();
    }

    return writeSuccessful;
}

void ScintillaNext::reload()
{
    Q_ASSERT(isFile());

    // Ensure the file still exists.
    if (!QFile::exists(fileInfo.canonicalFilePath())) {
        return; // TODO: fasle
    }

    // Remove all the text
    {
        const QSignalBlocker blocker(this);
        setUndoCollection(false);
        emptyUndoBuffer();
        setText("");
        setUndoCollection(true);
    }

    // NOTE: if the read fails then the buffer will be completely empty...which probably
    // isn't a good thing, but this should be a rare occurrence.
    QFile f(fileInfo.canonicalFilePath());
    bool readSuccessful = readFromDisk(f);

    if (readSuccessful) {
        // TODO: emit bufferReloaded?
    }

    setSavePoint();
    updateTimestamp();

    return; // TODO: true
}

bool ScintillaNext::saveAs(const QString &newFilePath)
{
    // Store the type of the buffer before it gets saved (which can change the type)
    const ScintillaNext::BufferType origType = bufferType;

    emit aboutToSave();

    bool isRenamed = !isFile() || fileInfo.canonicalFilePath() != newFilePath;

    bool saveSuccessful = writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), newFilePath);

    if (saveSuccessful) {
        this->setFileInfo(newFilePath);
        setSavePoint();
        updateTimestamp();
    }

    if (isRenamed && saveSuccessful && origType == ScintillaNext::Temporary) {
        emit renamed();

        return true;
    }

    return false;
}

bool ScintillaNext::saveCopyAs(const QString &filePath)
{
    return writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), filePath);
}

bool ScintillaNext::rename(const QString &newFilePath)
{
    // TODO: check if newFilePath is already opened as another editor?

    emit aboutToSave();

    // Write out the buffer to the new path
    if (saveCopyAs(newFilePath)) {
        // Remove the old file
        const QString oldPath = fileInfo.canonicalFilePath();
        QFile::remove(oldPath);

        // Everything worked fine, so update the buffer's info
        setFileInfo(newFilePath);
        setSavePoint();
        updateTimestamp();

        emit renamed();

        return true;
    }

    return false;
}

ScintillaNext::FileStateChange ScintillaNext::checkFileForStateChange()
{
    return checkForBufferStateChange();
}

bool ScintillaNext::moveToTrash()
{
    if (QFile::exists(fileInfo.canonicalFilePath())) {
        QFile f(fileInfo.canonicalFilePath());

        return f.moveToTrash();
    }

    return false;
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

bool ScintillaNext::readFromDisk(QFile &file)
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
    allocate(file.size());

    // Turn off undo collection and block signals during loading
    setUndoCollection(false);
    blockSignals(true);
    // TODO disable notifications
    // modEventMask(SC_MOD_NONE)?

    QByteArray chunk;
    qint64 bytesRead;

    QTextDecoder *decoder = nullptr;
    bool first_read = true;
    do {
        // Try to read as much as possible
        chunk.resize(CHUNK_SIZE);
        bytesRead = file.read(chunk.data(), CHUNK_SIZE);

        chunk.resize(bytesRead);

        qDebug("Read %d bytes", bytesRead);

        // TODO: this needs moved out of here. Would make much more sense to have a class (or classes)
        // responsible for handling low level situations like this to do things like:
        // - determine encoding
        // - determine space vs tabs
        // - determine indentation size
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
        addText(utf8_data.size(), utf8_data.constData());


    } while (!file.atEnd() && status() == SC_STATUS_OK);

    delete decoder;

    file.close();

    // Restore it back
    this->blockSignals(false);
    setUndoCollection(true);
    // modEventMask(SC_MODEVENTMASKALL)?

    if (status() != SC_STATUS_OK) {
        qWarning("something bad happend in document->add_data() %d", status());
        return false;
    }

    if (bytesRead == -1) {
        qWarning("Something bad happend when reading disk %d %s", file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    return true;
}

ScintillaNext::FileStateChange ScintillaNext::checkForBufferStateChange()
{
    if (bufferType == BufferType::Temporary) {
        return FileStateChange::NoChange;
    }
    else if (bufferType == BufferType::File) {
        // Make sure it exists
        fileInfo.refresh(); // refresh else exists() fails to notice missing file
        if (!fileInfo.exists()) {
            bufferType = BufferType::FileMissing;
            emit savePointChanged(false);
            return FileStateChange::Deleted;
        }

        // See if the timestamp changed
        if (modifiedTime != fileTimestamp()) {
            return FileStateChange::Modified;
        }
        else {
            return FileStateChange::NoChange;
        }
    }
    else if (bufferType == BufferType::FileMissing) {
        // See if it reappeared
        fileInfo.refresh();
        if (fileInfo.exists()) {
            bufferType = BufferType::File;
            return FileStateChange::Restored;
        }
        else {
            return FileStateChange::NoChange;
        }
    }

    qInfo("type() = %d", bufferType);
    Q_ASSERT(false);
    return FileStateChange::NoChange;
}

QDateTime ScintillaNext::fileTimestamp()
{
    Q_ASSERT(bufferType != ScintillaNext::Temporary);

    fileInfo.refresh();
    qInfo("%s last modified %s", qUtf8Printable(fileInfo.fileName()), qUtf8Printable(fileInfo.lastModified().toString()));
    return fileInfo.lastModified();
}

void ScintillaNext::updateTimestamp()
{
    modifiedTime = fileTimestamp();
}

void ScintillaNext::setFileInfo(const QString &filePath)
{
    fileInfo.setFile(filePath);
    fileInfo.makeAbsolute();

    Q_ASSERT(fileInfo.exists());

    name = fileInfo.fileName();
    bufferType = ScintillaNext::File;
}
