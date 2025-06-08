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


#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H


#include <QDir>
#include <QSettings>


class ScintillaNext;
class MainWindow;
class NotepadNextApplication;
class Settings;

class SessionManager: public QObject
{
    Q_OBJECT

public:
    enum SessionFileType {
        None = 0,
        SavedFile = 1,
        UnsavedFile = 2,
        TempFile = 4,
    };
    Q_DECLARE_FLAGS(SessionFileTypes, SessionFileType)


    SessionManager(NotepadNextApplication *app, SessionFileTypes types = SessionFileTypes());

    void setSessionFileTypes(SessionFileTypes types);

    void clear() const;

    bool saveSessionTo(MainWindow *window, const QString &path);
    void saveCurrentSession(MainWindow *window);
    void loadDefaultSession(MainWindow *window);
    bool loadSessionFrom(MainWindow *window, const QString &path);

    bool willFileGetStoredInSession(ScintillaNext *editor) const;

    QDir sessionDirectory() const;
    bool isCustomSessionFolder() const { return useCustomSessionFolder; }

signals:
    void onSessionSaved(Settings &settings);
    void onSessionLoaded(Settings &settings);

private:
    void saveSession(MainWindow *window, QSettings &settings);
    void saveDefaultSession(MainWindow *window);
    ScintillaNext *loadSession(MainWindow *window, QSettings &settings);

    void saveIntoSessionDirectory(ScintillaNext *editor, const QString &sessionFileName) const;

    SessionFileType determineType(ScintillaNext *editor) const;

    void clearSettings(QSettings *settings) const;
    void clearDirectory() const;

    void storeFileDetails(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadFileDetails(QSettings &settings);

    void storeUnsavedFileDetails(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadUnsavedFileDetails(QSettings &settings);

    void storeTempFile(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadTempFile(QSettings &settings);

    void storeEditorViewDetails(ScintillaNext *editor, QSettings &settings);
    void loadEditorViewDetails(ScintillaNext *editor, QSettings &settings);

    NotepadNextApplication *app;
    SessionFileTypes fileTypes;

    bool useCustomSessionFolder;
    QString customSessionPath;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SessionManager::SessionFileTypes)

#endif // SESSIONMANAGER_H
