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


#include "ScintillaNext.h"
#include "SessionManager.h"
#include "EditorManager.h"

#include <QSettings>

SessionManager::SessionManager()
{

}

void SessionManager::ClearSession()
{
    // Save session
    QSettings settings;
    settings.beginGroup("CurrentSession");

    // Clear everything out. There can be left over entries that are no longer needed
    settings.remove("");
}

void SessionManager::SaveSession(QVector<ScintillaNext *> editors)
{
    SessionManager::ClearSession();

    // Save session
    QSettings settings;
    settings.beginGroup("CurrentSession");

    settings.beginWriteArray("OpenedFiles");

    int i = 0;
    for (const auto &editor : editors) {
        if (editor->isFile()) {
            settings.setArrayIndex(i);
            settings.setValue("FilePath", editor->getFilePath());
            settings.setValue("FirstVisibleLine", static_cast<int>(editor->firstVisibleLine() + 1)); // Keep it 1-based in the settings just for human-readability
            //settings.setValue("XOffset", editor->xOffset());
            ++i;
        }
    }

    settings.endArray();
    settings.endGroup();
}

void SessionManager::LoadSession(EditorManager *editorManager)
{
    QSettings settings;

    settings.beginGroup("CurrentSession");

    int size = settings.beginReadArray("OpenedFiles");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString filePath = settings.value("FilePath").toString();
        int firstVisibleLine = settings.value("FirstVisibleLine").toInt() - 1;
        //int xOffset = settings.value("XOffset").toInt();

        // See if it is already opened, if so just ignore it
        ScintillaNext *editor = editorManager->getEditorByFilePath(filePath);

        if (editor == Q_NULLPTR) {
            QFileInfo fileInfo(filePath);

            if (fileInfo.isFile()) {
                editor = editorManager->createEditorFromFile(filePath);

                editor->setFirstVisibleLine(firstVisibleLine);
                //editor->setXOffset(xOffset);
            }
        }
    }

    settings.endArray();
    settings.endGroup();
}
