/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "BookMarkDecorator.h"
#include "ScintillaNext.h"
#include "MainWindow.h"
#include "SessionManager.h"
#include "EditorManager.h"
#include "NotepadNextApplication.h"

#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUuid>

#include <algorithm>


static constexpr int MAX_PERSISTED_LINE_MARKER = 20;

static QString RandomSessionFileName()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

static bool IsManagedSessionFileName(const QString &fileName)
{
    static const QRegularExpression uuidPattern(
        QStringLiteral("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"),
        QRegularExpression::CaseInsensitiveOption);
    return uuidPattern.match(fileName).hasMatch();
}

// QList<int> cannot be automatically serialized to/from QSettings (i.e. QVariant) so turn it to a QVariantList
static QVariantList QListToQVariantList(const QList<int> intList)
{
    QVariantList vl;
    for (const int i : intList){
        vl.append(i);
    }
    return vl;
}

// Do the opposite of the function above
static QList<int> QVariantListToQList(const QVariantList &variantList) {
    QList<int> intList;
    for (const QVariant &variant : variantList) {
        intList.append(variant.toInt());
    }
    return intList;
}

SessionManager::SessionManager(NotepadNextApplication *app, SessionFileTypes types)
    : app(app)
{
    setSessionFileTypes(types);
}

void SessionManager::setSessionFileTypes(SessionFileTypes types)
{
    fileTypes = types;
}

QDir SessionManager::sessionDirectory() const
{
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    const QString path = d.filePath("session");
    if (!d.mkpath("session")) {
        qWarning("Could not create session directory %s", qUtf8Printable(path));
    }
    return QDir(path);
}

bool SessionManager::saveIntoSessionDirectory(ScintillaNext *editor, const QString &sessionFileName) const
{
    const QFileDevice::FileError error = editor->saveCopyAs(sessionDirectory().filePath(sessionFileName));
    if (error != QFileDevice::NoError) {
        qWarning("Could not save session snapshot %s (error %d)", qUtf8Printable(sessionFileName), error);
        return false;
    }

    return true;
}

SessionManager::SessionFileType SessionManager::determineType(ScintillaNext *editor) const
{
    if (editor->isFile()) {
        if (editor->isSavedToDisk()) {
            return SessionManager::SavedFile;
        }
        else {
            return SessionManager::UnsavedFile;
        }
    }
    else {
        if (!editor->isSavedToDisk()) {
            return SessionManager::TempFile;
        }
        else {
            return SessionManager::None;
        }
    }
}

bool SessionManager::clear() const
{
    if (!clearSettings()) {
        return false;
    }

    clearDirectory();
    return true;
}

bool SessionManager::clearSettings() const
{
    ApplicationSettings settings;

    settings.beginGroup("CurrentSession");
    settings.remove("");
    settings.endGroup();
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qWarning("Could not clear previous session settings (status %d)", settings.status());
        return false;
    }
    return true;
}

void SessionManager::clearDirectory(const QStringList &retainedFiles) const
{
    QDir d = sessionDirectory();

    for (const QString &fileName : d.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        if (IsManagedSessionFileName(fileName) && !retainedFiles.contains(fileName)) {
            if (!d.remove(fileName)) {
                qWarning("Could not remove stale session snapshot %s", qUtf8Printable(fileName));
            }
        }
    }
}

bool SessionManager::saveSession(MainWindow *window)
{
    qInfo(Q_FUNC_INFO);

    // Early out if no flags are set
    if (fileTypes == SessionManager::None) {
        return clear();
    }

    const ScintillaNext *currentEditor = window->currentEditor();
    int currentEditorIndex = 0;
    QVariantList openedFiles;
    QStringList retainedFiles;

    for (const auto &editor : window->editors()) {
        const SessionFileType editorType = determineType(editor);

        if (fileTypes.testFlag(editorType)) {
            QVariantMap details;
            bool stored = true;

            if (editorType == SessionManager::SavedFile) {
                storeFileDetails(editor, details);
            }
            else if (editorType == SessionManager::UnsavedFile) {
                stored = storeUnsavedFileDetails(editor, details);
            }
            else if (editorType == SessionManager::TempFile) {
                stored = storeTempFile(editor, details);
            }
            else {
                qWarning("Unknown SessionFileType %d", editorType);
            }

            if (!stored) {
                for (const QString &fileName : retainedFiles) {
                    sessionDirectory().remove(fileName);
                }
                return false;
            }

            if (currentEditor == editor) {
                currentEditorIndex = openedFiles.size();
            }

            const QString sessionFileName = details.value("SessionFileName").toString();
            if (!sessionFileName.isEmpty()) {
                retainedFiles.append(sessionFileName);
            }

            openedFiles.append(details);
        }
    }

    ApplicationSettings settings;
    settings.beginGroup("CurrentSession");
    settings.remove("");
    settings.beginWriteArray("OpenedFiles", openedFiles.size());
    for (int index = 0; index < openedFiles.size(); ++index) {
        settings.setArrayIndex(index);
        const QVariantMap details = openedFiles.at(index).toMap();
        for (auto it = details.cbegin(); it != details.cend(); ++it) {
            settings.setValue(it.key(), it.value());
        }
    }
    settings.endArray();

    settings.setValue("CurrentEditorIndex", currentEditorIndex);
    settings.endGroup();

    settings.sync();
    if (settings.status() != QSettings::NoError) {
        qWarning("Could not publish session settings (status %d)", settings.status());
        return false;
    }

    clearDirectory(retainedFiles);
    return true;
}

void SessionManager::loadSession(MainWindow *window)
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;;

    settings.beginGroup("CurrentSession");

    ScintillaNext *currentEditor = Q_NULLPTR;
    const int currentEditorIndex = settings.value("CurrentEditorIndex").toInt();
    const int size = settings.beginReadArray("OpenedFiles");

    // NOTE: In theory the fileTypes should determine what is loaded, however if the session fileTypes
    // change from the last time it was saved then it means the settings were manually altered outside of the app,
    // which is non-standard behavior, so just load anything in the file

    for (int index = 0; index < size; ++index) {
        settings.setArrayIndex(index);

        ScintillaNext *editor = Q_NULLPTR;

        if (settings.contains("Type")) {
            const QString type = settings.value("Type").toString();

            if (type == QStringLiteral("File")) {
                editor = loadFileDetails(settings);
            }
            else if (type == QStringLiteral("UnsavedFile")) {
                editor = loadUnsavedFileDetails(settings);
            }
            else if (type == QStringLiteral("Temp")) {
                editor = loadTempFile(settings);
            }
            else {
                qDebug("Unknown session entry type: %s", qUtf8Printable(type));
            }

            if (editor) {
                 if (currentEditorIndex == index) {
                    currentEditor = editor;
                }
            }
        }
        else {
            qDebug("Unknown session entry type for index %d", index);
        }
    }

    settings.endArray();

    settings.endGroup();

    if (currentEditor) {
        window->switchToEditor(currentEditor);
    }
}

bool SessionManager::willFileGetStoredInSession(ScintillaNext *editor) const
{
    SessionFileType editorType = determineType(editor);

    // See if the editor type is in the currently supported file types
    return fileTypes.testFlag(editorType);
}

void SessionManager::storeFileDetails(ScintillaNext *editor, QVariantMap &details)
{
    details.insert("Type", "File");
    details.insert("FilePath", editor->getFilePath());

    storeEditorViewDetails(editor, details);
}

ScintillaNext* SessionManager::loadFileDetails(QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString filePath = settings.value("FilePath").toString();

    qDebug("Session file: \"%s\"", qUtf8Printable(filePath));

    ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);
    if (editor != Q_NULLPTR) {
        qDebug("  file is already open, ignoring");
        return Q_NULLPTR;
    }

    if (QFileInfo::exists(filePath)) {
        editor = ScintillaNext::fromFile(filePath);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        loadDockWidgetDetails(editor, settings);
        app->getEditorManager()->manageEditor(editor);

        loadEditorViewDetails(editor, settings);

        return editor;
    }
    else {
        qDebug("  no longer exists on disk, ignoring");
        return Q_NULLPTR;
    }
}

bool SessionManager::storeUnsavedFileDetails(ScintillaNext *editor, QVariantMap &details)
{
    const QString sessionFileName = RandomSessionFileName();

    details.insert("Type", "UnsavedFile");
    details.insert("FilePath", editor->getFilePath());
    details.insert("SessionFileName", sessionFileName);

    storeEditorViewDetails(editor, details);

    return saveIntoSessionDirectory(editor, sessionFileName);
}

ScintillaNext *SessionManager::loadUnsavedFileDetails(QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString filePath = settings.value("FilePath").toString();
    const QString sessionFileName = settings.value("SessionFileName").toString();
    if (filePath.isEmpty() || !IsManagedSessionFileName(sessionFileName)) {
        qWarning("Invalid unsaved session entry; skipping");
        return Q_NULLPTR;
    }
    const QString sessionFilePath = sessionDirectory().filePath(sessionFileName);

    qDebug("Session file: \"%s\"", qUtf8Printable(filePath));
    qDebug("  temp loc: \"%s\"", qUtf8Printable(sessionFilePath));

    ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);
    if (editor != Q_NULLPTR) {
        qDebug("  file is already open, ignoring");
        return Q_NULLPTR;
    }

    if (QFileInfo::exists(sessionFilePath)) {
        ScintillaNext *editor = ScintillaNext::fromFile(sessionFilePath);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        // Since this editor has different file path info, treat this as a temporary buffer
        editor->setFileInfo(filePath);
        editor->setTemporary(true);

        loadDockWidgetDetails(editor, settings);
        app->getEditorManager()->manageEditor(editor);

        loadEditorViewDetails(editor, settings);

        return editor;
    }
    else {
        qDebug("  session snapshot no longer exists, ignoring this file");
        return Q_NULLPTR;
    }
}

bool SessionManager::storeTempFile(ScintillaNext *editor, QVariantMap &details)
{
    const QString sessionFileName = RandomSessionFileName();

    details.insert("Type", "Temp");
    details.insert("FileName", editor->getName());
    details.insert("SessionFileName", sessionFileName);
    details.insert("Language", editor->languageName);

    storeEditorViewDetails(editor, details);

    return saveIntoSessionDirectory(editor, sessionFileName);
}

ScintillaNext *SessionManager::loadTempFile(QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString fileName = settings.value("FileName").toString();
    const QString sessionFileName = settings.value("SessionFileName").toString();
    const QString languageName = settings.value("Language", QString()).toString();
    if (fileName.isEmpty() || !IsManagedSessionFileName(sessionFileName)) {
        qWarning("Invalid temporary session entry; skipping");
        return Q_NULLPTR;
    }
    const QString fullFilePath = sessionDirectory().filePath(sessionFileName);

    qDebug("Session temp file: \"%s\"", qUtf8Printable(fullFilePath));

    if (QFileInfo::exists(fullFilePath)) {
        ScintillaNext *editor = ScintillaNext::fromFile(fullFilePath, false);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        editor->detachFileInfo(fileName);
        editor->setTemporary(true);

        loadDockWidgetDetails(editor, settings);
        app->getEditorManager()->manageEditor(editor);

        if (!languageName.isEmpty()) {
            qDebug("Setting session file language to \"%s\"", qUtf8Printable(languageName));
            app->setEditorLanguage(editor, languageName);
        }

        loadEditorViewDetails(editor, settings);

        return editor;
    }
    else {
        qDebug("  no longer exists on disk, ignoring");
        return Q_NULLPTR;
    }
}

void SessionManager::storeEditorViewDetails(ScintillaNext *editor, QVariantMap &details)
{
    details.insert("FirstVisibleLine", static_cast<int>(editor->firstVisibleLine() + 1)); // Keep it 1-based in the settings just for human-readability
    details.insert("CurrentPosition", static_cast<int>(editor->currentPos()));

    if (QWidget *dockWidget = editor->parentWidget()) {
        details.insert("DockWidgetObjectName", dockWidget->objectName());
    }

    BookMarkDecorator *decorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);
    if (decorator == Q_NULLPTR) {
        qWarning("Editor has no BookMarkDecorator, unable to store bookmarks");
    }
    else {
        const QList<int> bookMarkedLines = decorator->bookMarkedLines();
        if (!bookMarkedLines.isEmpty()) {
            details.insert("BookMarks", QListToQVariantList(bookMarkedLines));
        }
    }

    QVariantList foldedLines;
    for (int line = editor->contractedFoldNext(0); line != -1; line = editor->contractedFoldNext(line + 1)) {
        foldedLines.append(line);
    }
    if (!foldedLines.isEmpty()) {
        details.insert("FoldedLines", foldedLines);
    }

    // Keep user/breakpoint markers while excluding app-reserved markers and fold controls.
    QVariantList lineMarkers;
    for (int marker = 0; marker <= MAX_PERSISTED_LINE_MARKER; ++marker) {
        for (int line = editor->markerNext(0, 1 << marker); line != -1; line = editor->markerNext(line + 1, 1 << marker)) {
            QVariantMap markerDetails;
            markerDetails.insert("Line", line);
            markerDetails.insert("Marker", marker);
            lineMarkers.append(markerDetails);
        }
    }
    if (!lineMarkers.isEmpty()) {
        details.insert("LineMarkers", lineMarkers);
    }
}

void SessionManager::loadDockWidgetDetails(ScintillaNext *editor, QSettings &settings)
{
    const QString objectName = settings.value("DockWidgetObjectName").toString();
    if (IsManagedSessionFileName(objectName)) {
        editor->QObject::setProperty("nn_session_dock_widget_name", objectName);
    }
}

void SessionManager::loadEditorViewDetails(ScintillaNext *editor, QSettings &settings)
{
    const int lineCount = qMax(1, editor->lineCount());

    if (settings.contains("BookMarks")) {
        QList<int> bookMarkedLines = QVariantListToQList(settings.value("BookMarks").toList()); // just using .value<QList<int>>() does not work...possibly a Qt bug?
        bookMarkedLines.erase(std::remove_if(bookMarkedLines.begin(), bookMarkedLines.end(), [lineCount](int line) {
            return line < 0 || line >= lineCount;
        }), bookMarkedLines.end());

        BookMarkDecorator *decorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);
        if (decorator != Q_NULLPTR) {
            decorator->setBookMarkedLines(bookMarkedLines);
        }
        else {
            qWarning("Editor has no BookMarkDecorator, unable to restore bookmarks");
        }
    }

    const QVariantList lineMarkers = settings.value("LineMarkers").toList();
    for (const QVariant &value : lineMarkers) {
        const QVariantMap markerDetails = value.toMap();
        const int line = markerDetails.value("Line").toInt();
        const int marker = markerDetails.value("Marker").toInt();
        if (line >= 0 && line < lineCount && marker >= 0 && marker <= MAX_PERSISTED_LINE_MARKER) {
            editor->markerAdd(line, marker);
        }
    }

    const QVariantList foldedLines = settings.value("FoldedLines").toList();
    for (const QVariant &value : foldedLines) {
        const int line = value.toInt();
        if (line >= 0 && line < lineCount &&
            (editor->foldLevel(line) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG) {
            editor->setFoldExpanded(line, false);
        }
    }

    const int currentPosition = qBound(0, settings.value("CurrentPosition").toInt(), static_cast<int>(editor->length()));
    const int firstVisibleLine = qBound(0, settings.value("FirstVisibleLine").toInt() - 1, lineCount - 1);
    editor->setEmptySelection(currentPosition);
    editor->setFirstVisibleLine(firstVisibleLine);
}
