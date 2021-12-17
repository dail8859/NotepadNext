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


class ScintillaNext;

class EditorManager : public QObject
{
    Q_OBJECT

public:
    explicit EditorManager(QObject *parent = nullptr);

    ScintillaNext *createEmptyEditor(const QString &name);
    ScintillaNext *createEditorFromFile(const QString &filePath);
    ScintillaNext *cloneEditor(ScintillaNext *editor);

    ScintillaNext *getEditorByFilePath(const QString &filePath);

signals:
    void editorCreated(ScintillaNext *editor);
    void editorClosed(ScintillaNext *editor);

private:
    void setupEditor(ScintillaNext *editor);
    void setFoldMarkers(ScintillaNext *editor, const QString &type);
    void purgeOldEditorPointers();

    QList<QPointer<ScintillaNext>> editors;
};

#endif // EDITORMANAGER_H
