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
#include "EncodingSpecs.h"
#include "Finder.h"
#include "ScintillaCommenter.h"

#include "ByteArrayUtils.h"
#include "uchardet.h"
#include <cinttypes>

#include <QDir>
#include <QMouseEvent>
#include <QSaveFile>
#include <QTextCodec>


const int CHUNK_SIZE = 1024 * 1024 * 4; // Not sure what is best

inline const QByteArray BOM_UTF8    = QByteArray::fromHex("EFBBBF");
inline const QByteArray BOM_UTF16LE = QByteArray::fromHex("FFFE");
inline const QByteArray BOM_UTF16BE = QByteArray::fromHex("FEFF");

ScintillaNext::BomType detectBom(const QByteArray& data)
{
    if (data.startsWith(BOM_UTF8))    return ScintillaNext::BomType::Utf8;
    if (data.startsWith(BOM_UTF16LE)) return ScintillaNext::BomType::Utf16LE;
    if (data.startsWith(BOM_UTF16BE)) return ScintillaNext::BomType::Utf16BE;

    return ScintillaNext::BomType::None;
}

QByteArray bomData(ScintillaNext::BomType bom)
{
    switch (bom) {
    case ScintillaNext::BomType::Utf8:    return BOM_UTF8;
    case ScintillaNext::BomType::Utf16LE: return BOM_UTF16LE;
    case ScintillaNext::BomType::Utf16BE: return BOM_UTF16BE;
    case ScintillaNext::BomType::None:    return QByteArray();
    }
    return QByteArray();
}

int bomLength(ScintillaNext::BomType bom)
{
    if (bom == ScintillaNext::BomType::Utf8) return BOM_UTF8.length();
    else if (bom == ScintillaNext::BomType::Utf16LE) return BOM_UTF16LE.length();
    else if (bom == ScintillaNext::BomType::Utf16BE) return BOM_UTF16BE.length();

    return 0;
}

// QStringDecoder only understands the Unicode encodings, so legacy code pages
// are decoded with QTextCodec (Qt6::Core5Compat). uchardet is already linked
// and only *detects* a charset; it never converts one.
static bool looksSingleByteWestern(const QString &name)
{
    static const char *const prefixes[] = {
        "ISO-8859", "ISO 8859", "WINDOWS-125", "KOI8", "IBM", "TIS-620",
        "MACINTOSH", "X-MAC", "CP8", "CP4", "ASCII"
    };
    for (const char *prefix : prefixes) {
        if (name.startsWith(QLatin1String(prefix), Qt::CaseInsensitive))
            return true;
    }
    return false;
}

static bool hasHighBytes(const QByteArray &data)
{
    for (int i = 0; i < data.size(); ++i) {
        if (static_cast<unsigned char>(data.at(i)) >= 0x80)
            return true;
    }
    return false;
}

// uchardet tends to answer WINDOWS-1252 / ISO-8859-x for files that are almost
// pure ASCII with only a few CJK characters. Trusting that guess turns those
// characters into Latin garbage, which is worse than not detecting at all.
//
// The override has to stay narrow, because the opposite mistake is worse. An
// accented Latin letter followed by another letter is a valid GBK lead/trail
// pair, so Western prose decodes "cleanly" into scattered ideographs
// ("Hélène" -> 閘鑞, "Größenänderung" -> Gr鲞en鋘derung). Counting ideographs
// alone cannot separate the two. Real CJK text has runs of three or more
// ideographs back to back, while an accented Latin word yields runs of two at
// most, so the run length is what makes the override trustworthy.
static bool decodesAsGb18030WithCjk(const QByteArray &data)
{
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    if (!codec)
        return false;

    QTextCodec::ConverterState state(QTextCodec::DefaultConversion);
    const QString text = codec->toUnicode(data.constData(), data.size(), &state);
    if (state.invalidChars > 0 || text.contains(QChar(0xFFFD)))
        return false;

    int run = 0;
    int longestRun = 0;
    for (const QChar ch : text) {
        const ushort c = ch.unicode();
        if (c >= 0x4E00 && c <= 0x9FFF) {
            if (++run > longestRun)
                longestRun = run;
        }
        else {
            run = 0;
        }
    }
    return longestRun >= 3;
}

// Return an Encoding-menu spec, or an empty string when the data is UTF-8.
// Strict UTF-8 must be tried first: uchardet happily reports a legacy code
// page for short or mostly-ASCII UTF-8 samples.
static QString detectCharset(const QByteArray &data)
{
    if (data.isEmpty())
        return QString();

    {
        QStringDecoder decoder(QStringDecoder::Utf8);
        // The result MUST be consumed. QStringDecoder only updates hasError()
        // when the decoded value is used, so discarding it silently disables
        // this probe under some optimizers.
        const QString probe = decoder(QByteArrayView(data));
        Q_UNUSED(probe);
        if (!decoder.hasError())
            return QString();
    }

    QString name;
    uchardet_t ud = uchardet_new();
    if (ud) {
        uchardet_handle_data(ud, data.constData(), static_cast<size_t>(data.size()));
        uchardet_data_end(ud);
        const char *charset = uchardet_get_charset(ud);
        if (charset && *charset)
            name = QString::fromLatin1(charset);
        uchardet_delete(ud);
    }

    if (name.isEmpty() || name.compare(QLatin1String("ASCII"), Qt::CaseInsensitive) == 0)
        return QString();
    if (name.startsWith(QLatin1String("UTF-"), Qt::CaseInsensitive))
        return QString();

    if (looksSingleByteWestern(name) && hasHighBytes(data) && decodesAsGb18030WithCjk(data)) {
        qInfo("uchardet guessed \"%s\" but the data decodes cleanly as GB18030 with CJK text", qUtf8Printable(name));
        return QStringLiteral("GB18030");
    }

    // Normalize uchardet's spelling to the Encoding menu's spelling so the
    // status bar and the checked menu item agree with the detected charset.
    // An unsupported name must not reach readFromDisk(): with no QTextCodec
    // it would silently append the raw legacy bytes as UTF-8.
    const QString spec = EncodingSpecs::canonical(name);
    if (spec.isEmpty()) {
        qWarning("uchardet reported \"%s\", which is not available as a QTextCodec; reading as UTF-8",
                 qUtf8Printable(name));
        return QString();
    }
    if (!QTextCodec::codecForName(spec.toLatin1())) {
        qWarning("Qt has no codec for \"%s\"; reading as UTF-8", qUtf8Printable(spec));
        return QString();
    }
    return spec;
}

static QFileDevice::FileError writeToDisk(const QByteArray &data, const QString &path, ScintillaNext::BomType bom)
{
    qInfo(Q_FUNC_INFO);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("writeToDisk() failed to open file %s: %s", qPrintable(path), qPrintable(file.errorString()));
        return file.error();
    }

    // Write BOM
    const QByteArray bomBytes = bomData(bom);
    if (!bomBytes.isEmpty() && file.write(bomBytes) == -1) {
        qWarning("writeToDisk() failed writing BOM: %s", qPrintable(file.errorString()));
        return file.error();
    }

    // TODO make this convert and write chunks rather than the entire thing
    if (bom == ScintillaNext::BomType::Utf16LE) {
        QStringEncoder encoder(QStringEncoder::Utf16LE);
        const QString text = QString::fromUtf8(data);
        const QByteArray encoded = encoder(text);

        file.write(encoded);
    }
    else if (bom == ScintillaNext::BomType::Utf16BE) {
        QStringEncoder encoder(QStringEncoder::Utf16BE);
        const QString text = QString::fromUtf8(data);
        const QByteArray encoded = encoder(text);

        file.write(encoded);
    }
    else {
        file.write(data);
    }

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

QString ScintillaNext::eolModeToString(int eolMode)
{
    if (eolMode == SC_EOL_CRLF)
        return QStringLiteral("crlf");
    else if (eolMode == SC_EOL_CR)
        return QStringLiteral("cr");
    else if (eolMode == SC_EOL_LF)
        return QStringLiteral("lf");
    else
        return QString(); // unknown
}

int ScintillaNext::stringToEolMode(QString eolMode)
{
    if (eolMode == QStringLiteral("crlf"))
        return SC_EOL_CRLF;
    else if (eolMode == QStringLiteral("cr"))
        return SC_EOL_CR;
    else if (eolMode == QStringLiteral("lf"))
        return SC_EOL_LF;
    else
        return -1;
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

        setSelection(range.cpMax, range.cpMin);
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

Sci_CharacterRange ScintillaNext::getContextText()
{
    if (!selectionEmpty()) {
        if (selections() == 1) {
            int start = selectionStart();
            int end = selectionEnd();

            if (end - start < 1024 &&
                lineFromPosition(start) == lineFromPosition(end)) {
                return { start, end };
            }
        }

        return { INVALID_POSITION, INVALID_POSITION };
    }

    return wordAtPosition(currentPos());
}

Sci_CharacterRange ScintillaNext::wordAtPosition(int pos)
{
    int start = wordStartPosition(pos, true);
    int end = wordEndPosition(pos, true);

    if (end > start) {
        return { start, end };
    }

    return { INVALID_POSITION, INVALID_POSITION };
}

void ScintillaNext::modifyFoldLevels(int level, int action)
{
    const int totalLines = lineCount();

    int line = 0;
    while (line < totalLines) {
        int foldFlags = foldLevel(line); // Even though its called fold level it contains several other flags
        bool isHeader = foldFlags & SC_FOLDLEVELHEADERFLAG;
        int actualLevel = (foldFlags & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE;

        if (isHeader && actualLevel == level) {
            foldLine(line, action);
            line = lastChild(line, -1) + 1;
        }
        else {
            ++line;
        }
    }
}

void ScintillaNext::foldAllLevels(int level)
{
    modifyFoldLevels(level, SC_FOLDACTION_CONTRACT);
}

void ScintillaNext::unFoldAllLevels(int level)
{
    modifyFoldLevels(level, SC_FOLDACTION_EXPAND);
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
    // - The encoding was changed since the file was last read or saved
    return temporary ||
           encoding() != savedEncodingSpec ||
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

QString ScintillaNext::getToolTip() const
{
    return isFile() ? getFilePath() : getName();
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

    const QByteArray data = toFileBytes(QByteArray::fromRawData((char*)characterPointer(), textLength()));
    const QString path = fileInfo.filePath();
    QFileDevice::FileError writeSuccessful = writeToDisk(data, path, bomType);

    if (writeSuccessful == QFileDevice::NoError) {
        updateTimestamp();
        savedEncodingSpec = encoding();
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();
    }

    return writeSuccessful;
}

bool ScintillaNext::reload()
{
    Q_ASSERT(isFile());

    // Ensure the file still exists.
    if (!QFile::exists(fileInfo.canonicalFilePath())) {
        return false;
    }

    const int line = firstVisibleLine();
    const int caret = selectionNCaret(mainSelection());
    const int anchor = selectionNAnchor(mainSelection());

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

    if (!readSuccessful) {
        return false;
    }

    updateTimestamp();
    setSavePoint();

    // If this was a temporary file, make sure it is not any more
    if (isTemporary())
        setTemporary(false);

    scrollVertical(line, 0);
    setSelection(caret, anchor);

    emit reloaded();
    return true;
}

void ScintillaNext::omitModifications()
{
    // If file modifications will be omitted just update file timestamp
    // so pop-up will be displayed only once per file modifications.
    updateTimestamp();
    setTemporary(true);

    return;
}

QFileDevice::FileError ScintillaNext::saveAs(const QString &newFilePath)
{
    bool isRenamed = bufferType == ScintillaNext::New || fileInfo.canonicalFilePath() != newFilePath;

    emit aboutToSave();

    const QByteArray data = toFileBytes(QByteArray::fromRawData((char*)characterPointer(), textLength()));
    QFileDevice::FileError saveSuccessful = writeToDisk(data, newFilePath, bomType);

    if (saveSuccessful == QFileDevice::NoError) {
        setFileInfo(newFilePath);
        savedEncodingSpec = encoding();
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
    const QByteArray data = toFileBytes(QByteArray::fromRawData((char*)characterPointer(), textLength()));
    return writeToDisk(data, filePath, bomType);
}

bool ScintillaNext::rename(const QString &newFilePath)
{
    emit aboutToSave();

    // Write out the buffer to the new path
    if (saveCopyAs(newFilePath) == QFileDevice::NoError) {
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

void ScintillaNext::removeDuplicateLines()
{
    QByteArray data = QByteArray::fromRawData((char*) characterPointer(), textLength());
    const QByteArray delim = eolString();

    auto lines = ByteArrayUtils::split(data, delim);
    int originalLineCount = lines.length();
    ByteArrayUtils::removeDuplicates(lines);

    if (originalLineCount == lines.length()){
        return; // No lines were removed
    }

    QByteArray result = ByteArrayUtils::join(lines, delim);

    const UndoAction ua(this);
    setTargetRange(0, textLength());
    replaceTarget(result.length(), result.constData());
}

void ScintillaNext::removeConsecutiveDuplicateLines()
{
    QByteArray data = QByteArray::fromRawData((char*) characterPointer(), textLength());
    const QByteArray delim = eolString();

    auto lines = ByteArrayUtils::split(data, delim);
    int originalLineCount = lines.length();
    ByteArrayUtils::removeConsecutiveDuplicates(lines);
    QByteArray result = ByteArrayUtils::join(lines, delim);

    if (originalLineCount == lines.length()){
        return; // No lines were removed
    }

    const UndoAction ua(this);
    setTargetRange(0, textLength());
    replaceTarget(result.length(), result.constData());
}

void ScintillaNext::trimTrailingWhitespace()
{
    Finder f(this);
    const UndoAction ua(this);

    // $ alone only matches before \n, so handle CRLF and CR line endings explicitly
    f.options().text = QStringLiteral("[ \\t]+(?=[\\r\\n]|$)");
    f.options().flags = Scintilla::FindOption::RegExp;
    f.replaceAll(QString());
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
    QStringDecoder decoder;
    QStringEncoder encoder = QStringEncoder(QStringEncoder::Utf8);

    // A ConverterState must survive across chunks: a 4 MB read boundary can
    // fall in the middle of a multi-byte character.
    QTextCodec *fileCodec = Q_NULLPTR;
    QTextCodec::ConverterState codecState;
    codecName.clear();
    decodeInvalidChars = 0;

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

        int offset = 0;
        if (first_read) {
            first_read = false;

            bomType = detectBom(chunk);
            offset = bomLength(bomType);

            if (!forcedReadSpec.isEmpty()) {
                // Reinterpret: honour the user's choice instead of detecting.
                applyEncodingSpec(forcedReadSpec);
            } else {
                switch (bomType) {
                case BomType::Utf16BE:
                    decoder = QStringDecoder(QStringDecoder::Utf16BE);
                    offset = 2;
                    break;

                case BomType::Utf16LE:
                    decoder = QStringDecoder(QStringDecoder::Utf16LE);
                    offset = 2;
                    break;

                case BomType::Utf8:
                    // No decoder needed since Scintilla expects UTF-8.
                    offset = 3;
                    break;

                default:
                    // No BOM: strict UTF-8 first, then uchardet.
                    applyEncodingSpec(detectCharset(chunk));
                    break;
                }
            }

            if (!codecName.isEmpty()) {
                fileCodec = QTextCodec::codecForName(codecName.toLatin1());
                if (!fileCodec)
                    qWarning("No QTextCodec for \"%s\"; reading as UTF-8", qUtf8Printable(codecName));
            }

            savedEncodingSpec = encoding();
        }

        QByteArrayView input(chunk.constData() + offset, chunk.size() - offset);

        if (bomType == BomType::Utf16BE || bomType == BomType::Utf16LE) {
            QString text = decoder(input);
            QByteArray utf8 = encoder(text);

            appendText(utf8.size(), utf8.constData());
        } else if (fileCodec) {
            // Legacy code page -> QString -> UTF-8 (Scintilla only speaks UTF-8).
            const QString text = fileCodec->toUnicode(input.data(), static_cast<int>(input.size()), &codecState);
            const QByteArray utf8 = encoder(text);

            appendText(utf8.size(), utf8.constData());
        } else {
            // No conversion necessary.
            appendText(input.size(), input.data());
        }
    } while (!file.atEnd() && status() == SC_STATUS_OK);

    // A byte sequence the codec cannot map becomes U+FFFD in the buffer, so the
    // original bytes are gone. Remember the count: saving re-encodes the text,
    // which would write replacement characters over those bytes.
    decodeInvalidChars = codecState.invalidChars;

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
    savedEncodingSpec = encoding();
}

QString ScintillaNext::encoding() const
{
    if (!codecName.isEmpty())
        return codecName;

    switch (bomType) {
    case BomType::Utf8:    return QStringLiteral("UTF-8 BOM");
    case BomType::Utf16LE: return QStringLiteral("UTF-16LE BOM");
    case BomType::Utf16BE: return QStringLiteral("UTF-16BE BOM");
    default:               return QStringLiteral("UTF-8");
    }
}

void ScintillaNext::applyEncodingSpec(const QString &spec)
{
    codecName.clear();

    if (spec.isEmpty() || spec == QStringLiteral("UTF-8")) {
        bomType = BomType::None;
    } else if (spec == QStringLiteral("UTF-8 BOM")) {
        bomType = BomType::Utf8;
    } else if (spec == QStringLiteral("UTF-16LE BOM")) {
        bomType = BomType::Utf16LE;
    } else if (spec == QStringLiteral("UTF-16BE BOM")) {
        bomType = BomType::Utf16BE;
    } else {
        bomType = BomType::None;
        codecName = spec;
    }
}

QByteArray ScintillaNext::toFileBytes(const QByteArray &utf8) const
{
    if (codecName.isEmpty())
        return utf8;

    QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
    if (!codec) {
        qWarning("No QTextCodec for \"%s\"; writing UTF-8 instead", qUtf8Printable(codecName));
        return utf8;
    }

    const QString text = QString::fromUtf8(utf8);
    QTextCodec::ConverterState state(QTextCodec::DefaultConversion);
    const QByteArray encoded = codec->fromUnicode(text.constData(), text.size(), &state);
    if (state.invalidChars > 0) {
        qWarning("%d characters could not be represented in %s",
                 state.invalidChars, qUtf8Printable(codecName));
    }
    return encoded;
}

bool ScintillaNext::canBeConvertedTo(const QString &spec)
{
    // The Unicode specs can represent anything Scintilla stores (UTF-8).
    const bool unicode = spec.isEmpty()
        || spec == QStringLiteral("UTF-8")
        || spec == QStringLiteral("UTF-8 BOM")
        || spec == QStringLiteral("UTF-16LE BOM")
        || spec == QStringLiteral("UTF-16BE BOM");
    if (unicode)
        return true;

    QTextCodec *codec = QTextCodec::codecForName(spec.toLatin1());
    if (!codec)
        return false;

    const QString text = QString::fromUtf8(QByteArray::fromRawData((char*)characterPointer(), textLength()));
    QTextCodec::ConverterState state(QTextCodec::DefaultConversion);
    codec->fromUnicode(text.constData(), text.size(), &state);
    return state.invalidChars == 0;
}

void ScintillaNext::setEncoding(const QString &spec)
{
    if (encoding() == spec)
        return;

    applyEncodingSpec(spec);
    emit encodingChanged();

    // The text did not change, but the file must be written again.
    emit savePointChanged(canSaveToDisk());
}

bool ScintillaNext::reinterpretWithEncoding(const QString &spec)
{
    Q_ASSERT(isFile());

    // Reinterpreting is only meaningful for encodings without a BOM.
    if (spec.contains(QStringLiteral("BOM")))
        return false;

    // reload() empties the buffer before it reads. Keep a copy of the text and
    // the encoding state so a failed read cannot silently destroy the document.
    const QByteArray previousText(characterPointer(), textLength());
    const QString previousEncoding = encoding();
    const QString previousSavedEncoding = savedEncodingSpec;

    forcedReadSpec = spec;
    const bool readSuccessful = reload();
    forcedReadSpec.clear();

    if (!readSuccessful) {
        // reload() may return before clearing the buffer (e.g. the file was
        // deleted), in which case there is nothing to restore and the caret
        // position and undo history should be left alone.
        const QByteArray currentText(characterPointer(), textLength());
        if (currentText != previousText) {
            const QSignalBlocker blocker(this);
            setUndoCollection(false);
            setText("");
            appendText(previousText.size(), previousText.constData());
            setUndoCollection(true);
            setSavePoint();
        }
        applyEncodingSpec(previousEncoding);
        savedEncodingSpec = previousSavedEncoding;
        emit savePointChanged(canSaveToDisk());
        return false;
    }

    emit encodingChanged();
    emit savePointChanged(canSaveToDisk());
    return true;
}

void ScintillaNext::setTemporary(bool temp)
{
    temporary = temp;

    // Fake this signal
    emit savePointChanged(temporary);
}
