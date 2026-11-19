/*
 * This file is part of Notepad Next.
 * Copyright 2021 Justin Dailey
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


#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QObject>
#include <QPointer>
#include <QFileSystemWatcher>
#include <QHash>
#include <QSet>


class ApplicationSettings;
class ScintillaNext;

class EditorManager : public QObject
{
    Q_OBJECT

public:
    explicit EditorManager(ApplicationSettings *settings, QObject *parent = nullptr);

    ScintillaNext *createEditor(const QString &name);
    ScintillaNext *createEditorFromFile(const QString &filePath, bool tryToCreate=false);

    ScintillaNext *getEditorByFilePath(const QString &filePath);

    void manageEditor(ScintillaNext *editor);

signals:
    void editorCreated(ScintillaNext *editor);
    void editorClosed(ScintillaNext *editor);

    // Emitted asynchronously whenever the OS reports that an open file was
    // touched outside the application (write, permissions, delete, etc).
    // Does NOT get emitted for the application's own saves, since the
    // in-memory timestamp is refreshed synchronously before this signal
    // could ever be observed.
    void editorFileChangedOnDisk(ScintillaNext *editor);

private:
    void setupEditor(ScintillaNext *editor);
    void purgeOldEditorPointers();
    QList<QPointer<ScintillaNext>> getEditors();
    int detectEOLMode(ScintillaNext *editor) const;

    void watchEditorFile(ScintillaNext *editor);
    void unwatchEditorFile(ScintillaNext *editor);
    void processExternalFileChange(const QString &path);

    QList<QPointer<ScintillaNext>> editors;
    ApplicationSettings *settings;

    QFileSystemWatcher *fileWatcher;

    // Tracks the path currently registered with fileWatcher for each editor,
    // so a rename/"Save As" can drop the old path instead of leaking an
    // orphaned watch on it.
    QHash<ScintillaNext *, QString> watchedPaths;

    // A single external write commonly triggers 2+ fileChanged signals in a
    // row (this is a well-known QFileSystemWatcher/inotify quirk, not a bug
    // in this class). Paths in this set already have a debounce timer
    // pending, so further signals for them are ignored until it fires.
    QSet<QString> pendingFileChanges;
};

#endif // EDITORMANAGER_H
