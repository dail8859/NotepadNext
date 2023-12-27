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


#ifndef DOCKEDEDITOR_H
#define DOCKEDEDITOR_H

#include <QObject>

#include "DockManager.h"
#include "ScintillaNext.h"

class DockedEditor : public QObject
{
    Q_OBJECT

private:
    ads::CDockManager* dockManager = Q_NULLPTR;
    ScintillaNext *currentEditor = Q_NULLPTR;

public:
    explicit DockedEditor(QWidget *parent);

    ScintillaNext *getCurrentEditor() const;
    ads::CDockAreaWidget *currentDockArea() const;

    QVector<ScintillaNext *> editors() const;

    void switchToEditor(const ScintillaNext *editor);

    int count() const;

public slots:
    void addEditor(ScintillaNext *editor);

private slots:
    void dockWidgetCloseRequested();
    void editorRenamed(ScintillaNext *editor);

signals:
    void editorAdded(ScintillaNext *editor);
    void editorCloseRequested(ScintillaNext *editor);
    void editorClosed(ScintillaNext *editor);
    void editorActivated(ScintillaNext *editor);
    void editorOrderChanged();

    void contextMenuRequestedForEditor(ScintillaNext *editor);
    void titleBarDoubleClicked();
};

#endif // DOCKEDEDITOR_H
