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
#include "ScintillaBuffer.h"

class ScintillaNext : public ScintillaEdit
{
    Q_OBJECT

public:
    explicit ScintillaNext(ScintillaBuffer *buffer = Q_NULLPTR, QWidget *parent = Q_NULLPTR);

    template<typename Func>
    void forEachMatch(const QString &text, Func callback) { forEachMatch(text.toUtf8(), callback); }

    template<typename Func>
    void forEachMatch(const QByteArray &byteArray, Func callback);

    bool isSavedToDisk();
    bool isFile();
    QFileInfo fileInfo();
    QString getName();
    QString canonicalFilePath();
    QString suffix();

    ScintillaBuffer *getBuffer() const { return buffer; }

    enum FileStateChange {
        NoChange,
        Modified,
        Deleted,
        Restored,
    };

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
    void closed();
    void renamed();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    ScintillaBuffer *buffer = Q_NULLPTR;
};

// Stick this in the header file...because C++, that's why
template<typename Func>
void ScintillaNext::forEachMatch(const QByteArray &text, Func callback)
{
    Sci_TextToFind ttf {{0, (Sci_PositionCR)length()}, text.constData(), {-1, -1}};
    int flags = searchFlags();

    while (send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != -1) {
        ttf.chrg.cpMin = callback(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
    }
}

#endif // SCINTILLANEXT_H
