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


#ifndef EDITORINSPECTORDOCK_H
#define EDITORINSPECTORDOCK_H

#include <QDockWidget>
#include <QTreeWidgetItem>

#include "ScintillaTypes.h"


class MainWindow;
class ScintillaNext;

typedef std::function<QString(ScintillaNext *)> EditorFunction;

namespace Ui {
class EditorInspectorDock;
}

class EditorInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit EditorInspectorDock(MainWindow *parent);
    ~EditorInspectorDock();

private slots:
    void attachToEditor(ScintillaNext *editor);
    void editorUIUpdated(Scintilla::Update updated);
    void updateEditorInfo(ScintillaNext *editor);

private:
    void newItem(QTreeWidgetItem *parent, const QString &label, EditorFunction func);

    Ui::EditorInspectorDock *ui;
    QTreeWidgetItem *selectionsInfo;
    QMetaObject::Connection editorConnection;
    QVector<QPair<QTreeWidgetItem *, EditorFunction>> items;
};

#endif // EDITORINSPECTORDOCK_H
