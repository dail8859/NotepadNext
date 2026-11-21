/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
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

#ifndef DOCUMENTLIFECYCLECONTROLLER_H
#define DOCUMENTLIFECYCLECONTROLLER_H

#include <QVector>
#include <QFileDevice>
#include <QStringList>

class MainWindow;
class NotepadNextApplication;
class ScintillaNext;

class DocumentLifecycleController
{
public:
    DocumentLifecycleController(MainWindow *window, NotepadNextApplication *app);

    void openFiles(const QStringList &fileNames);
    bool checkEditorsBeforeClose(const QVector<ScintillaNext *> &editors);
    void closeFile(ScintillaNext *editor);

    bool saveFile(ScintillaNext *editor);
    bool saveFileAs(ScintillaNext *editor, const QString &fileName);
    bool saveAllEditors(const QVector<ScintillaNext *> &editors);
    void showSaveErrorMessage(ScintillaNext *editor, QFileDevice::FileError error) const;

private:
    enum class UserSaveAction { SaveAll, DiscardAll, Cancel };

    ScintillaNext *getInitialEditor() const;
    UserSaveAction promptForSave(const QVector<ScintillaNext *> &editors) const;

    MainWindow *window;
    NotepadNextApplication *app;
};

#endif // DOCUMENTLIFECYCLECONTROLLER_H
