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

#include "Settings.h"

#include "SingleApplication"

#include <QPointer>
#include <QTranslator>


class MainWindow;
class LuaState;
class EditorManager;
class RecentFilesListManager;
class ScintillaNext;

class NotepadNextApplication : public SingleApplication
{
    Q_OBJECT

public:
    NotepadNextApplication(int &argc, char **argv);

    bool init();

    RecentFilesListManager *getRecentFilesListManager() const { return recentFilesListManager; }
    EditorManager *getEditorManager() const { return editorManager; }

    LuaState *getLuaState() const { return luaState; }
    QString getFileDialogFilter() const;
    Settings *getSettings() const { return settings; }

    QStringList getLanguages() const;
    void setEditorLanguage(ScintillaNext *editor, const QString &languageName) const;
    QString detectLanguageFromExtension(const QString &extension) const;

    void loadSystemDefaultTranslation();

private:
    EditorManager *editorManager;
    RecentFilesListManager *recentFilesListManager;
    Settings *settings;

    LuaState *luaState = Q_NULLPTR;

    QList<MainWindow *> windows;
    QPointer<QWidget> currentlyFocusedWidget; // Keep a weak pointer to the QWidget since we don't own it

    void applyArguments(const QStringList &args);
    MainWindow *createNewWindow();
    QTranslator translator;
};

#endif // NOTEPADNEXTAPPLICATION_H
