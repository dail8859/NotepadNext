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


#ifndef SCINTILLANEXT_H
#define SCINTILLANEXT_H

#include "ScintillaEdit.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>


class ScintillaNext : public ScintillaEdit
{
    Q_OBJECT

public:
    explicit ScintillaNext(QString name, QWidget *parent = Q_NULLPTR);
    static ScintillaNext *fromFile(const QString &filePath);

    template<typename Func>
    void forEachMatch(const QString &text, Func callback) { forEachMatch(text.toUtf8(), callback); }

    template<typename Func>
    void forEachMatch(const QByteArray &byteArray, Func callback) { forEachMatchInRange(byteArray, callback, {0, (Sci_PositionCR)length()}); }

    template<typename Func>
    void forEachMatchInRange(const QByteArray &byteArray, Func callback, Sci_CharacterRange range);

    bool isFile() const;
    bool isSavedToDisk() const;
    QFileInfo getFileInfo() const;

    QString getName() const { return name; }
    QString getPath() const;
    QString getFilePath() const;

    enum FileStateChange {
        NoChange,
        Modified,
        Deleted,
        Restored,
    };

    enum BufferType {
        Temporary = 0, // A temporary buffer, e.g. "New 1"
        File = 1, // Buffer tied to a file on the file system
        FileMissing = 2, // Buffer with a missing file on the file system
    };

    void setFoldMarkers(const QString &type);

    QString languageName;

public slots:
    void close();
    bool save();
    void reload();
    bool saveAs(const QString &newFilePath);
    bool saveCopyAs(const QString &filePath);
    bool rename(const QString &newFilePath);
    ScintillaNext::FileStateChange checkFileForStateChange();
    bool moveToTrash();

signals:
    void aboutToSave();
    void saved();
    void closed();
    void renamed();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString name;
    BufferType bufferType = BufferType::Temporary;
    QFileInfo fileInfo;
    QDateTime modifiedTime;

    bool readFromDisk(QFile &file);
    QDateTime fileTimestamp();
    void updateTimestamp();
    void setFileInfo(const QString &filePath);
};



// Stick this in the header file...because C++, that's why
template<typename Func>
void ScintillaNext::forEachMatchInRange(const QByteArray &text, Func callback, Sci_CharacterRange range)
{
    Sci_TextToFind ttf {range, text.constData(), {-1, -1}};
    int flags = searchFlags();

    while (send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != -1) {
        ttf.chrg.cpMin = callback(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
    }
}

#endif // SCINTILLANEXT_H
