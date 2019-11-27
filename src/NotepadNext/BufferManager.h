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


#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <QString>
#include <QList>

#include <ScintillaBuffer.h>
#include <QFileSystemWatcher>

class BufferManager : public QObject
{
    Q_OBJECT

public:
    enum BufferError {
        NoError = 0,
        DoesNotExist = 1,
        //WriteError = 2,
        //FatalError = 3,
        //ResourceError = 4,
        //OpenError = 5,
        //AbortError = 6,
        //TimeOutError = 7,
        //UnspecifiedError = 8,
        //RemoveError = 9,
        //RenameError = 10,
        //PositionError = 11,
        //ResizeError = 12,
        //PermissionsError = 13,
        //CopyError = 14
    };

    explicit BufferManager(QObject *parent = Q_NULLPTR);
    ~BufferManager();

    ScintillaBuffer *createEmtpyBuffer(const QString &name);
    ScintillaBuffer *createBufferFromFile(const QString &filePath);

    ScintillaBuffer *getBufferByFilePath(const QString &filePath);

    bool saveBuffer(ScintillaBuffer *buffer, bool forceSave = false, const QFileInfo *fileInfo = Q_NULLPTR);
    void closeBuffer(ScintillaBuffer *buffer);
    bool saveBufferAs(ScintillaBuffer *buffer, const QString &newFilePath);
    bool saveBufferCopyAs(ScintillaBuffer *buffer, const QString &filePath);
    bool renameBuffer(ScintillaBuffer *buffer, const QString &newFilePath);
    bool reloadBuffer(ScintillaBuffer *buffer);

signals:
    void bufferCreated(ScintillaBuffer *buffer);
    void bufferSaved(ScintillaBuffer *buffer);
    void bufferClosed(ScintillaBuffer *buffer);
    void bufferRenamed(ScintillaBuffer *buffer);

private:
    QList<ScintillaBuffer *> buffers;

    void manageBuffer(ScintillaBuffer *buffer);

    void detectEols(ScintillaBuffer *buffer);
    void detectEncoding(ScintillaBuffer *buffer);
};

#endif // BUFFERMANAGER_H
