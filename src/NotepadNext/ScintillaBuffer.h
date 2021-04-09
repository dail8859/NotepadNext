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


#ifndef SCINTILLABUFFER_H
#define SCINTILLABUFFER_H

#include <QFileInfo>
#include <QDateTime>

#include <ScintillaDocument.h>
#include <SciLexer.h>

// TODO: Clean up this class

class ScintillaBuffer : public ScintillaDocument
{
    Q_OBJECT

    friend class EditorManager;
    friend class ScintillaNext;

public:
    enum BufferType {
        Temporary = 0, // A temporary buffer, e.g. "New 1"
        File = 1, // Buffer tied to a file on the file system
        FileMissing = 2, // Buffer with a missing file on the file system
    };

    enum BufferStateChange {
        NoChange,
        Modified,
        Deleted,
        Restored,
    };

    const QString &getName() const;
    BufferType type() const;

    BufferStateChange checkForBufferStateChange();

    QFileInfo fileInfo;

private:
    explicit ScintillaBuffer(const QString &name);
    virtual ~ScintillaBuffer();

    static ScintillaBuffer *fromFile(const QString &filePath);

    void setFileInfo(const QString &filePath);
    bool isFile() const;
    bool isSavedToDisk();

    QString name;
    BufferType bufferType;

    QDateTime modifiedTime;
    QDateTime fileTimestamp();
    void updateTimestamp();

    bool save();
    bool saveAs(const QString &newFilePath);
    bool saveCopyAs(const QString &filePath);
    bool reloadFromFile();
    bool readFromDisk(QFile &file);
};

#endif // SCINTILLABUFFER_H
