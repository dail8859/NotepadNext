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


#ifndef NOTEPADNEXTAPPLICATION_H
#define NOTEPADNEXTAPPLICATION_H

#include "ApplicationSettings.h"

#include "SingleApplication"

#include <QCommandLineParser>
#include <QPointer>


class MainWindow;
class LuaState;
class EditorManager;
class RecentFilesListManager;
class ScintillaNext;
class SessionManager;
class TranslationManager;


class NotepadNextApplication : public SingleApplication
{
    Q_OBJECT

public:
    NotepadNextApplication(int &argc, char **argv);

    bool init();

    RecentFilesListManager *getRecentFilesListManager() const { return recentFilesListManager; }
    EditorManager *getEditorManager() const { return editorManager; }
    SessionManager *getSessionManager() const;
    TranslationManager *getTranslationManager() const { return translationManager; };

    LuaState *getLuaState() const { return luaState; }
    QString getFileDialogFilter() const;
    QString getFileDialogFilterForLanguage(const QString &language) const;
    ApplicationSettings *getSettings() const { return settings; }

    QStringList getLanguages() const;
    void setEditorLanguage(ScintillaNext *editor, const QString &languageName) const;

    QString detectLanguage(ScintillaNext *editor) const;
    QString detectLanguageFromExtension(const QString &extension) const;
    QString detectLanguageFromContents(ScintillaNext *editor) const;

    void sendInfoToPrimaryInstance();

    bool isRunningAsAdmin() const;

    QStringList debugInfo() const;

protected:
    bool event(QEvent *event) override;

private slots:
    void saveSettings();
    void receiveInfoFromSecondaryInstance(quint32 instanceId, QByteArray message);

private:
    void openFiles(const QStringList &files);

    void loadSettings();

    EditorManager *editorManager;
    RecentFilesListManager *recentFilesListManager;
    ApplicationSettings *settings;
    SessionManager *sessionManager;
    TranslationManager *translationManager;

    LuaState *luaState = Q_NULLPTR;

    MainWindow *window = Q_NULLPTR;
    QPointer<QWidget> currentlyFocusedWidget; // Keep a weak pointer to the QWidget since we don't own it

    MainWindow *createNewWindow();

    QCommandLineParser parser;
};

#endif // NOTEPADNEXTAPPLICATION_H
