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
#include "ScintillaCommenter.h"

#include "uchardet.h"
#include <cinttypes>

#include <QDebug>
#include <QDir>
#include <QMouseEvent>
#include <QSaveFile>
#include <QSettings>
#include <QTextCodec>
#include <QRegularExpression>

#include "NotepadNextApplication.h"
#include "MainWindow.h"

const int CHUNK_SIZE = 1024 * 1024 * 4; // Not sure what is best


static QFileDevice::FileError writeToDisk(const QByteArray &data, const QString &path)
{
    qInfo(Q_FUNC_INFO);

    QFile file(path);

    if (file.open(QIODevice::WriteOnly)) {
        if (file.write(data) != -1) {
            file.close();
            return QFileDevice::NoError;
        }
    }

    // If it got to this point there was an error
    qWarning("writeToDisk() failure code %d: %s", file.error(), qPrintable(file.errorString()));
    return file.error();
}

static bool isNewlineCharacter(char c)
{
    return c == '\n' || c == '\r';
}

ScintillaNext::ScintillaNext(QString name, QWidget *parent) :
    ScintillaEdit(parent),
    name(name),
    indicatorResources(INDICATOR_MAX + 1)
{
    // Per the scintilla documentation, some parts of the range are not generally available
    indicatorResources.disableRange(0, 7);
    indicatorResources.disableRange(INDICATOR_IME, INDICATOR_IME_MAX);
    indicatorResources.disableRange(INDICATOR_HISTORY_REVERTED_TO_ORIGIN_INSERTION, INDICATOR_HISTORY_REVERTED_TO_MODIFIED_DELETION);
}

ScintillaNext::~ScintillaNext()
{
}

ScintillaNext *ScintillaNext::fromFile(const QString &filePath, bool tryToCreate)
{
    QFile file(filePath);
    ScintillaNext *editor = new ScintillaNext(file.fileName());

    if(tryToCreate && !file.exists()) {
        qInfo("Trying to create %s", qUtf8Printable(filePath));
        QDir d;
        d.mkpath(QFileInfo(file).path());

        QFile f(filePath);
        f.open(QIODevice::WriteOnly);
        f.close();
    }

    bool readSuccessful = editor->readFromDisk(file);

    if (!readSuccessful) {
        delete editor;
        return Q_NULLPTR;
    }

    editor->setFileInfo(filePath);

    return editor;
}

int ScintillaNext::allocateIndicator(const QString &name)
{
    return indicatorResources.requestResource(name);
}

void ScintillaNext::goToRange(const Sci_CharacterRange &range)
{
    qInfo(Q_FUNC_INFO);

    if (isRangeValid(range)) {
        // Lines can be folded so make sure they are visible
        ensureVisible(lineFromPosition(range.cpMin));
        ensureVisible(lineFromPosition(range.cpMax));

        setSelection(range.cpMin, range.cpMax);
        scrollRange(range.cpMax, range.cpMin);
    }
}

QByteArray ScintillaNext::eolString() const
{
    const int eol = eOLMode();

    if (eol == SC_EOL_LF) return QByteArrayLiteral("\n");
    else if (eol == SC_EOL_CRLF) return QByteArrayLiteral("\r\n");
    else return QByteArrayLiteral("\r");
}

bool ScintillaNext::lineIsEmpty(int line)
{
    return (lineEndPosition(line) - positionFromLine(line)) == 0;
}

void ScintillaNext::deleteLine(int line)
{
    deleteRange(positionFromLine(line), lineLength(line));
}

void ScintillaNext::cutAllowLine()
{
    if (selectionEmpty()) {
        copyAllowLine();
        lineDelete();
    }
    else {
        cut();
    }
}

void ScintillaNext::deleteLeadingEmptyLines()
{
    while (lineCount() > 1 && lineIsEmpty(0)) {
        deleteLine(0);
    }
}

void ScintillaNext::deleteTrailingEmptyLines()
{
    const int docLength = length();
    int position = docLength;

    while (position > 0 && isNewlineCharacter(charAt(position - 1))) {
        position--;
    }

    deleteRange(position, docLength - position);
}

bool ScintillaNext::isSavedToDisk() const
{
    return !canSaveToDisk();
}

bool ScintillaNext::canSaveToDisk() const
{
    // The buffer can be saved if:
    // - It is marked as a temporary since as soon as it gets saved it is no longer a temporary buffer
    // - A modified file
    // - A missing file since as soon as it is saved it is no longer missing.
    return temporary ||
           (bufferType == ScintillaNext::New && modify()) ||
           (bufferType == ScintillaNext::File && modify()) ||
            (bufferType == ScintillaNext::FileMissing);
}

void ScintillaNext::setName(const QString &name)
{
    this->name = name;

    emit renamed();
}

bool ScintillaNext::isFile() const
{
    return bufferType == ScintillaNext::File || bufferType == ScintillaNext::FileMissing;
}

QFileInfo ScintillaNext::getFileInfo() const
{
    Q_ASSERT(isFile());

    return fileInfo;
}

QString ScintillaNext::getPath() const
{
    Q_ASSERT(isFile());

    return QDir::toNativeSeparators(fileInfo.canonicalPath());
}

QString ScintillaNext::getFilePath() const
{
    Q_ASSERT(isFile());

    return QDir::toNativeSeparators(fileInfo.canonicalFilePath());
}

void ScintillaNext::setFoldMarkers(const QString &type)
{
    QMap<QString, QList<int>> map{
        {"simple", {SC_MARK_MINUS, SC_MARK_PLUS, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY}},
        {"arrow",  {SC_MARK_ARROWDOWN, SC_MARK_ARROW, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY}},
        {"circle", {SC_MARK_CIRCLEMINUS, SC_MARK_CIRCLEPLUS, SC_MARK_VLINE, SC_MARK_LCORNERCURVE, SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE }},
        {"box",    {SC_MARK_BOXMINUS, SC_MARK_BOXPLUS, SC_MARK_VLINE, SC_MARK_LCORNER, SC_MARK_BOXPLUSCONNECTED, SC_MARK_BOXMINUSCONNECTED, SC_MARK_TCORNER }},
    };

    if (!map.contains(type))
        return;

    const auto types = map[type];
    markerDefine(SC_MARKNUM_FOLDEROPEN, types[0]);
    markerDefine(SC_MARKNUM_FOLDER, types[1]);
    markerDefine(SC_MARKNUM_FOLDERSUB, types[2]);
    markerDefine(SC_MARKNUM_FOLDERTAIL, types[3]);
    markerDefine(SC_MARKNUM_FOLDEREND, types[4]);
    markerDefine(SC_MARKNUM_FOLDEROPENMID, types[5]);
    markerDefine(SC_MARKNUM_FOLDERMIDTAIL, types[6]);
}

void ScintillaNext::close()
{
    emit closed();

    deleteLater();
}

QFileDevice::FileError ScintillaNext::save()
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(isFile());

    emit aboutToSave();

    QFileDevice::FileError writeSuccessful = writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), fileInfo.filePath());

    if (writeSuccessful == QFileDevice::NoError) {
        updateTimestamp();
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();
    }

    return writeSuccessful;
}

void ScintillaNext::reload()
{
    Q_ASSERT(isFile());

    // Ensure the file still exists.
    if (!QFile::exists(fileInfo.canonicalFilePath())) {
        return;
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
        updateTimestamp();
        setSavePoint();
    }

    return;
}

QFileDevice::FileError ScintillaNext::saveAs(const QString &newFilePath)
{
    bool isRenamed = bufferType == ScintillaNext::New || fileInfo.canonicalFilePath() != newFilePath;

    emit aboutToSave();

    QFileDevice::FileError saveSuccessful = writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), newFilePath);

    if (saveSuccessful == QFileDevice::NoError) {
        setFileInfo(newFilePath);
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();

        if (isRenamed) {
            emit renamed();
        }
    }

    return saveSuccessful;
}

QFileDevice::FileError ScintillaNext::saveCopyAs(const QString &filePath)
{
    return writeToDisk(QByteArray::fromRawData((char*)characterPointer(), textLength()), filePath);
}

bool ScintillaNext::rename(const QString &newFilePath)
{
    emit aboutToSave();

    // Write out the buffer to the new path
    if (saveCopyAs(newFilePath)) {
        // Remove the old file
        const QString oldPath = fileInfo.canonicalFilePath();
        QFile::remove(oldPath);

        // Everything worked fine, so update the buffer's info
        setFileInfo(newFilePath);
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();

        emit renamed();

        return true;
    }

    return false;
}

ScintillaNext::FileStateChange ScintillaNext::checkFileForStateChange()
{
    if (bufferType == BufferType::New) {
        return FileStateChange::NoChange;
    }
    else if (bufferType == BufferType::File) {
        // refresh else exists() fails to notice missing file
        fileInfo.refresh();

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

bool ScintillaNext::moveToTrash()
{
    if (QFile::exists(fileInfo.canonicalFilePath())) {
        QFile f(fileInfo.canonicalFilePath());

        return f.moveToTrash();
    }

    return false;
}

void ScintillaNext::toggleCommentSelection()
{
    ScintillaCommenter sc(this);
    sc.toggleSelection();
}

void ScintillaNext::commentLineSelection()
{
    ScintillaCommenter sc(this);
    sc.commentSelection();
}

void ScintillaNext::uncommentLineSelection()
{
    ScintillaCommenter sc(this);
    sc.uncommentSelection();
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
        qWarning("QFile::open() failed when opening \"%s\" - error code %d: %s", qUtf8Printable(file.fileName()), file.error(), qUtf8Printable(file.errorString()));
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
    QTextCodec *codec = Q_NULLPTR;
    QTextCodec::ConverterState state;

    bool first_read = true;
    do {
        // Try to read as much as possible
        chunk.resize(CHUNK_SIZE);
        bytesRead = file.read(chunk.data(), CHUNK_SIZE);
        chunk.resize(bytesRead);

        qDebug("Read %lld bytes", bytesRead);

        // TODO: this needs moved out of here. Would make much more sense to have a class (or classes)
        // responsible for handling low level situations like this to do things like:
        // - determine encoding
        // - determine space vs tabs
        // - determine indentation size

        if (first_read) {
            first_read = false;

            // Search for a BOM mark
            codec = QTextCodec::codecForUtfText(chunk, Q_NULLPTR);

            if (codec != Q_NULLPTR) {
                qDebug("BOM mark found");
            }
            else {
                qDebug("BOM mark not found, using uchardet");

                // Limit decoding to the first 64 kilobytes
                int detectionSize = qMin(chunk.size(), 64 * 1024);

                // Use uchardet to try and detect file encoding since no BOM was found
                uchardet_t encodingDetector = uchardet_new();
                if (uchardet_handle_data(encodingDetector, chunk.data(), detectionSize) == 0) {
                    uchardet_data_end(encodingDetector);

                    qDebug("uchardet detected encoding as: '%s'", uchardet_get_charset(encodingDetector));
                    codec = QTextCodec::codecForName(uchardet_get_charset(encodingDetector));
                }
                else {
                    qDebug("uchardet failure");
                }
                uchardet_delete(encodingDetector);
            }

            qDebug("Using codec: '%s'", codec ? codec->name().constData() : "");
        }

        if (codec) {
            const QByteArray utf8_data = codec->toUnicode(chunk.constData(), chunk.size(), &state).toUtf8();
            appendText(utf8_data.size(), utf8_data.constData());
        }
        else {
            appendText(chunk.size(), chunk.constData());
        }
    } while (!file.atEnd() && status() == SC_STATUS_OK);

    file.close();

    // Restore it back
    this->blockSignals(false);
    setUndoCollection(true);
    // modEventMask(SC_MODEVENTMASKALL)?

    if (status() != SC_STATUS_OK) {
        qWarning("something bad happened in document->add_data() %ld", status());
        return false;
    }

    if (bytesRead == -1) {
        qWarning("Something bad happened when reading disk %d %s", file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    if (!QFileInfo(file).isWritable()) {
        qInfo("Setting file as read-only");
        setReadOnly(true);
    }

    return true;
}

QDateTime ScintillaNext::fileTimestamp()
{
    Q_ASSERT(bufferType != ScintillaNext::New);

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

    updateTimestamp();
}

void ScintillaNext::detachFileInfo(const QString &newName)
{
    setName(newName);

    bufferType = ScintillaNext::New;
}

void ScintillaNext::setTemporary(bool temp)
{
    temporary = temp;

    // Fake this signal
    emit savePointChanged(temporary);
}

void ScintillaNext::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        tinyexprCalc(EVAL_ENTER);
        break;
    case Qt::Key_Question:
        if(tinyexprCalc(EVAL_QUESTION))
            return;
        break;
    case Qt::Key_0 :case Qt::Key_1 :case Qt::Key_2 :case Qt::Key_3 :case Qt::Key_4:
    case Qt::Key_5 :case Qt::Key_6 :case Qt::Key_7 :case Qt::Key_8 :case Qt::Key_9:
    case Qt::Key_ParenRight :case Qt::Key_Backspace :case Qt::Key_End:
        ScintillaEdit::keyPressEvent(event);
        tinyexprCalc(EVAL_JIT);
        return;
    default:
        break;
    }

    ScintillaEdit::keyPressEvent(event);
}

inline void ScintillaNext::position(int &line, int &index)
{
    int curpos = currentPos();
    int li = lineFromPosition(curpos);
    int lipos = positionFromLine(li);
    int indx = 0;
    while (lipos < curpos)
    {
        int new_pos = positionAfter(lipos);
        if (new_pos == lipos)
            break;
        lipos = new_pos;
        ++indx;
    }
    line = li;
    index = indx;
}

inline QString ScintillaNext::getEditText(int line)
{
    if (line < 0 || line >= lineCount())
        return QString();
    return QString::fromUtf8(getLine(line));
}

inline int ScintillaNext::getPos(int line, int index){
    int pos = positionFromLine(line);
    for (int i = 0; i < index; i++)
        pos = positionAfter(pos);
    return pos;
}

inline void ScintillaNext::insertAt(const QString &text, int line, int index)
{
    const bool isr = readOnly();
    if (isr) setReadOnly(false);
    beginUndoAction();
    insertText(getPos(line, index), text.toUtf8().constData());
    endUndoAction();
    if (isr) setReadOnly(true);
}

inline void ScintillaNext::setCursor(int line, int index) {
    gotoPos(getPos(line, index));
}

inline QString& removeTail0andDot(QString& s)
{
    if (s.contains(".")) {
        while (s.back() == '0') s.chop(1);
        if (s.back() == '.') s.chop(1);
    }
    return s;
}

static inline bool timeExprCalc(const QString& expr, QString& retstr) {
    QString tmstr = expr;
    tmstr.replace(" ", "");
    static const auto reTimefilter = QRegularExpression(R"(^\d[:hms][\d:hms\.]*[+\-]\d[\d:hms\.]*$)");
    auto mtf = reTimefilter.match(tmstr);
    if (!mtf.hasMatch())
        return false;
    static const auto reTimeformat = QRegularExpression(
        R"(^(((\d+)[h:])?((\d+)[m:])?((\d+(\.\d+)?)s?)?)([+\-])(((\d+)[h:])?((\d+)[m:])?((\d+(\.\d+)?)s?)?)$)");
    auto re = reTimeformat.match(tmstr);
    if (!re.hasMatch())
        return false;
    //for (int i = 1; i <= 17; ++i)
    //    qDebug() << i << re.captured(i);
    bool tmadd = bool(re.captured(9) == "+");
    auto tm1 = re.captured(1);
    auto tm2 = re.captured(10);
    static const auto exl = QRegularExpression(R"(^(?!.*[hms].*:)(?!.*:.*[hms]).*$)");
    if (!(exl.match(tm1).hasMatch() && exl.match(tm2).hasMatch()))
        return false; // [hms] and colon cannot be mixed in the same time string, "10:32m11" or "2h33:14" invalid.
    int tm_type = 0;
    QString tmfmt1;
    if (tm1.contains(':')) {
        tm_type = 1;
        tmfmt1 = "s"; // h:m:s.z 01:02:03.4 format
        if (re.captured(7).contains('.'))
            tmfmt1 += ".z";
        if (re.capturedLength(5)) {
            tmfmt1 = "h:m:" + tmfmt1;
        } else if (re.capturedLength(3)) {
            tmfmt1 = "m:" + tmfmt1;
        }
    } else {         // h'h'm'm's.z's' 01h02m03.4s format
        if (re.capturedLength(3))
            tmfmt1 = "h'h'";
        if (re.capturedLength(5))
            tmfmt1 += "m'm'";
        if (re.capturedLength(7))
            tmfmt1 += "s";
        if (re.capturedLength(8))
            tmfmt1 += ".z";
        if (re.captured(6).contains('s'))
            tmfmt1 += "'s'";
    }
    QString tmfmt2;
    if (tm2.contains(':')) {
        tmfmt2 = "s"; // h:m:s.z 01:02:03.4 format
        if (re.captured(16).contains('.'))
            tmfmt2 += ".z";
        if (re.capturedLength(14)) {
            tmfmt2 = "h:m:" + tmfmt2;
        } else if (re.capturedLength(12)) {
            tmfmt2 = "m:" + tmfmt2;
        }
    } else {          // h'h'm'm's.z's' 01h02m03.4s format
        if (re.capturedLength(12))
            tmfmt2 = "h'h'";
        if (re.capturedLength(14))
            tmfmt2 += "m'm'";
        if (re.capturedLength(16))
            tmfmt2 += "s";
        if (re.capturedLength(17))
            tmfmt2 += ".z";
        if (re.captured(15).contains('s'))
            tmfmt2 += "'s'";
    }
    const auto qtm1 = QTime::fromString(tm1, tmfmt1);
    const auto qtm2 = QTime::fromString(tm2, tmfmt2);
    //qDebug() << qtm1.msecsSinceStartOfDay() << qtm2.msecsSinceStartOfDay();
    QTime res;
    bool neg = false;
    if (tmadd) {
        res = QTime::fromMSecsSinceStartOfDay(qtm1.msecsSinceStartOfDay() + qtm2.msecsSinceStartOfDay());
    } else {
        if (qtm1 >= qtm2) {
            res = QTime::fromMSecsSinceStartOfDay(qtm1.msecsSinceStartOfDay() - qtm2.msecsSinceStartOfDay());
        } else {
            res = QTime::fromMSecsSinceStartOfDay(qtm2.msecsSinceStartOfDay() - qtm1.msecsSinceStartOfDay());
            neg = true;
        }
    }
    retstr = neg? "-": "";
    retstr += res.toString(res.msecsSinceStartOfDay() > 3600*1000 ?
        (tm_type ? "hh:mm:ss.z" : "h'h'm'm's.z's'"):
        (tm_type ? "mm:ss.z" : "m'm's.z's'")
    );
    removeTail0andDot(retstr);
    return true;
}

bool ScintillaNext::tinyexprCalc(evltype evt) {
    const NotepadNextApplication *app = qobject_cast<NotepadNextApplication *>(qApp);
    if (app == Q_NULLPTR)
        return false;
    const auto settings = app->getSettings();
    if (settings == Q_NULLPTR)
        return false;
    if ((evt == EVAL_ENTER && !settings->useEnter()) ||
        (evt == EVAL_QUESTION && !settings->useQuestion()) ||
        (evt == EVAL_JIT && !settings->useJITEval())
    )
        return false;
    const auto mainWnd = app->getMainWindow();
    if (mainWnd == Q_NULLPTR)
        return false;
    const int eval_accuracy = settings->accuracy();
    if (eval_accuracy <= 0 || eval_accuracy > 15) {
        qDebug() << "Bad eval_accuracy value" << eval_accuracy << "in config file";
        return false;
    }
    int line, index;
    position(line, index);
    if (index <= 0)
        return false;
    int idx = index - 1;
    QString linetxt = getLine(line);
    if (linetxt.length() < 1)
        return false;

    switch(evt)
    {
    case EVAL_QUESTION:             // =?
        if (linetxt.at(idx) != '=')
            return false;
        break;
    case EVAL_ENTER:                // =ENTER
        if (linetxt.at(idx) != '=')
            return false;
        break;
    case EVAL_JIT:
    default:
        ++idx; // JIT no =
        break;
    }
    QString exprline = linetxt.left(idx);
    bool is_eq = false;
    int i = 0;
    for (int ii = idx - 1; ii > -1; --ii) {
        auto c = exprline.at(ii);
        if (c == '=') {    // mark =, accept ==
            is_eq = !is_eq;
        }
        else if (c == '<' || c == '>') { // accept <= and >=
            is_eq = false;
        }
        else if (is_eq) {  // find left bound
            i = ii + 2;
            break;
        }
    }
    while (i < idx) {
        auto expr = exprline.mid(i);
        if (evt != EVAL_JIT) {
            QString retstr;
            if (timeExprCalc(expr, retstr)) {
                insertAt(retstr, line, idx + 1);
                setCursor(line, idx + 1 + retstr.length());
                return true;
            }
        }
        auto v = m_tinyEpr.evaluate(expr.toStdString());
        if (!std::isnan(v)) {
            QString res = QString::number(v, 'g', eval_accuracy);
            removeTail0andDot(res);
            if(evt == EVAL_JIT) {
                res = expr + "=" + res;
                mainWnd->updateEvalStatus(res);
            } else {
                insertAt(res, line, idx + 1);
                setCursor(line, idx + 1 + res.length());
                if (settings->useJITEval()) {
                    mainWnd->updateEvalStatus(expr);
                }
            }
            //qDebug() << "tinyexpr: " << expr << " = " << res;
            return true;
        }
        static const QSet<QChar> s_teSymbols = {
            '+','-','*','/',':','^','%','!','<','>','(',')',
            ',',' ','\t','m','h','s',
            '0','1','2','3','4','5','6','7','8','9'
        };
        static const QSet<QChar> s_teLookfwdSymbols = {'|','&'};
        while (++i && i < idx - 1 &&
               !s_teSymbols.contains(exprline.at(i)) &&
               !(s_teLookfwdSymbols.contains(exprline.at(i)) &&
                 exprline.at(i) == exprline.at(i+1))
               ) {}
    }
    return false;
}
