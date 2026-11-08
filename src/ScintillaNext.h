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

#include "RangeAllocator.h"
#include "ScintillaEdit.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>




class ScintillaNext : public ScintillaEdit
{
    Q_OBJECT

public:
    static bool isRangeValid(const Sci_CharacterRange &range)
    {
        return range.cpMin != INVALID_POSITION && range.cpMax != INVALID_POSITION;
    }

    explicit ScintillaNext(QString name, QWidget *parent = Q_NULLPTR);
    virtual ~ScintillaNext();

    static ScintillaNext *fromFile(const QString &filePath, bool tryToCreate=false);
    static QString eolModeToString(int eolMode);
    static int stringToEolMode(QString eolMode);

    int allocateIndicator(const QString &name);

    template<typename Func>
    void forEachMatch(const QString &text, Func callback) { forEachMatch(text.toUtf8(), callback); }

    template<typename Func>
    void forEachMatch(const QByteArray &byteArray, Func callback) { forEachMatchInRange(byteArray, callback, {0, (Sci_PositionCR)length()}); }

    template<typename Func>
    void forEachMatchInRange(const QByteArray &byteArray, Func callback, Sci_CharacterRange range);

    template<typename Func>
    void forEachLineInSelection(int selection, Func callback);

    void goToRange(const Sci_CharacterRange &range);

    QByteArray eolString() const;

    bool lineIsEmpty(int line);

    void deleteLine(int line);

    void cutAllowLine();

    Sci_CharacterRange getContextText();
    Sci_CharacterRange wordAtPosition(int pos);

    void modifyFoldLevels(int level, int action);
    void foldAllLevels(int level);
    void unFoldAllLevels(int level);

    void deleteLeadingEmptyLines();
    void deleteTrailingEmptyLines();

    bool isFile() const;
    QFileInfo getFileInfo() const;

    bool isSavedToDisk() const;
    bool canSaveToDisk() const;

    QString getName() const { return name; }
    void setName(const QString &name);
    QString getPath() const;
    QString getFilePath() const;
    QString getToolTip() const;

    // NOTE: this is dangerous and should only be used in very rare situations
    void setFileInfo(const QString &filePath);

    void detachFileInfo(const QString &newName);

    enum FileStateChange {
        NoChange,
        Modified,
        Deleted,
        Restored,
    };

    enum BufferType {
        New, // A temporary buffer, e.g. "New 1"
        File, // Buffer tied to a file on the file system
        FileMissing, // Buffer with a missing file on the file system
    };

    enum class BomType {
        None,
        Utf8,
        Utf16LE,
        Utf16BE
    };

    BomType bom() const { return bomType; }

    // Text encoding of the file on disk. The returned string is one of the
    // entries offered by the Encoding menu, e.g. "UTF-8" or "GB18030".
    QString encoding() const;

    // "Convert": keep the text and write it in the new encoding on next save.
    void setEncoding(const QString &spec);

    // "Reinterpret": re-read the file from disk using the given encoding.
    bool reinterpretWithEncoding(const QString &spec);

    // False when the document contains characters the target encoding cannot
    // represent (CotEditor calls these "incompatible characters").
    bool canBeConvertedTo(const QString &spec);

    // Number of bytes that had no mapping to text when the file was read. They
    // are shown as U+FFFD and cannot be written back, so a save drops them.
    int undecodableByteCount() const { return decodeInvalidChars; }

    bool isTemporary() const { return temporary; }
    void setTemporary(bool temp);

    void setFoldMarkers(const QString &type);

    QString languageName;
    QByteArray languageSingleLineComment;

    #include "ScintillaEnums.h"


public slots:
    void close();
    QFileDevice::FileError save();
    // Returns false when the file could not be read (the buffer may be empty
    // in that case, see reload()'s original NOTE).
    bool reload();
    void omitModifications();
    QFileDevice::FileError saveAs(const QString &newFilePath);
    QFileDevice::FileError saveCopyAs(const QString &filePath);
    bool rename(const QString &newFilePath);
    ScintillaNext::FileStateChange checkFileForStateChange();
    bool moveToTrash();

    void toggleCommentSelection();
    void commentLineSelection();
    void uncommentLineSelection();

    void removeDuplicateLines();
    void removeConsecutiveDuplicateLines();

    void trimTrailingWhitespace();

signals:
    void aboutToSave();
    void saved();
    void closed();
    void renamed();

    void lexerChanged();
    void reloaded();
    void encodingChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString name;
    BufferType bufferType = BufferType::New;
    BomType bomType = BomType::None;
    QString codecName;          // legacy code page on disk; empty = UTF-8
    QString savedEncodingSpec = QStringLiteral("UTF-8");  // encoding() at the last read/save
    QString forcedReadSpec;     // one-shot override used by Reinterpret
    int decodeInvalidChars = 0; // bytes the read codec could not map to text
    QFileInfo fileInfo;
    QDateTime modifiedTime;
    RangeAllocator indicatorResources;

    bool temporary = false; // Temporary file loaded from a session. It can either be a 'New' file or actual 'File'

    void applyEncodingSpec(const QString &spec);
    QByteArray toFileBytes(const QByteArray &utf8) const;

    bool readFromDisk(QFile &file);
    QDateTime fileTimestamp();
    void updateTimestamp();

};

template<typename Func>
void ScintillaNext::forEachLineInSelection(int selection, Func callback)
{
    int lineStart = lineFromPosition(selectionNStart(selection));
    int lineEnd = lineFromPosition(selectionNEnd(selection));

    for (int curLine = lineStart; curLine <= lineEnd; ++curLine) {
        callback(curLine);
    }
}

// Stick this in the header file...because C++, that's why
template<typename Func>
void ScintillaNext::forEachMatchInRange(const QByteArray &text, Func callback, Sci_CharacterRange range)
{
    Sci_TextToFind ttf {range, text.constData(), {INVALID_POSITION, INVALID_POSITION}};
    int flags = searchFlags();

    while (send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != INVALID_POSITION) {
        if(ttf.chrgText.cpMin == ttf.chrgText.cpMax)
            break;
        ttf.chrg.cpMin = callback(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
    }
}

#endif // SCINTILLANEXT_H
