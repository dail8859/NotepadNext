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

#include "EditorInspectorDock.h"
#include "ui_EditorInspectorDock.h"

#include "MainWindow.h"


EditorInspectorDock::EditorInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::EditorInspectorDock)
{
    ui->setupUi(this);

    QTreeWidgetItem *positionInfo = new QTreeWidgetItem(ui->treeWidget);
    positionInfo->setText(0, "Position Information");
    positionInfo->setExpanded(true);

    newItem(positionInfo, "Caret Position", [](ScintillaNext *editor) { return QString::number(editor->currentPos()); });
    newItem(positionInfo, "Anchor Position", [](ScintillaNext *editor) { return QString::number(editor->anchor()); });
    newItem(positionInfo, "Column", [](ScintillaNext *editor) { return QString::number(editor->column(editor->currentPos())); });
    newItem(positionInfo, "Caret (x, y)", [](ScintillaNext *editor) { return QString("(%1, %2)").arg(editor->pointXFromPosition(editor->currentPos())).arg(editor->pointYFromPosition(editor->currentPos())); });
    // SCI_GETCHARAT(position pos) → int
    newItem(positionInfo, "Current Style", [](ScintillaNext *editor) { return QString::number(editor->styleAt(editor->currentPos())); });
    newItem(positionInfo, "Current Line", [](ScintillaNext *editor) { return QString::number(editor->lineFromPosition(editor->currentPos()) + 1); });
    newItem(positionInfo, "Line Length", [](ScintillaNext *editor) { return QString::number(editor->lineLength(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, "Line End Position", [](ScintillaNext *editor) { return QString::number(editor->lineEndPosition(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, "Line Indentation", [](ScintillaNext *editor) { return QString::number(editor->lineIndentation(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, "Line Indent Position", [](ScintillaNext *editor) { return QString::number(editor->lineIndentPosition(editor->lineFromPosition(editor->currentPos()))); });


    QTreeWidgetItem *documentInfo = new QTreeWidgetItem(ui->treeWidget);
    documentInfo->setText(0, "Document Information");
    documentInfo->setExpanded(true);

    newItem(documentInfo, "Length ", [](ScintillaNext *editor) { return QString::number(editor->length()); });
    newItem(documentInfo, "Line Count", [](ScintillaNext *editor) { return QString::number(editor->lineCount()); });


    QTreeWidgetItem *viewInfo = new QTreeWidgetItem(ui->treeWidget);
    viewInfo->setText(0, "View Information");
    viewInfo->setExpanded(true);

    newItem(viewInfo, "Lines on Screen", [](ScintillaNext *editor) { return QString::number(editor->linesOnScreen()); });
    newItem(viewInfo, "First Visible Line", [](ScintillaNext *editor) { return QString::number(editor->firstVisibleLine() + 1); });
    newItem(viewInfo, "X Offset", [](ScintillaNext *editor) { return QString::number(editor->xOffset()); });


    QTreeWidgetItem *foldInfo = new QTreeWidgetItem(ui->treeWidget);
    foldInfo->setText(0, "Fold Information");
    foldInfo->setExpanded(true);

    newItem(foldInfo, "Visible From Doc Line", [](ScintillaNext *editor) { return QString::number(editor->visibleFromDocLine(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, "Doc Line From Visible", [](ScintillaNext *editor) { return QString::number(editor->docLineFromVisible(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, "Fold Level", [](ScintillaNext *editor) { return QString::number((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE); });
    newItem(foldInfo, "Is Fold Header", [](ScintillaNext *editor) { return QString::number((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG); });
    newItem(foldInfo, "Fold Parent", [](ScintillaNext *editor) { return QString::number(editor->foldParent(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, "Last Child", [](ScintillaNext *editor) { return QString::number(editor->lastChild(editor->lineFromPosition(editor->currentPos()), -1) + 1); });
    newItem(foldInfo, "Contracted Fold Next", [](ScintillaNext *editor) { return QString::number(editor->contractedFoldNext(editor->lineFromPosition(editor->currentPos())) + 1); });

    ui->treeWidget->resizeColumnToContents(0);

    connect(parent->getDockedEditor(), &DockedEditor::editorActivated, this, &EditorInspectorDock::attachToEditor);
    connect(this, &EditorInspectorDock::visibilityChanged, this, [=]() {
        MainWindow *mw = qobject_cast<MainWindow *>(this->parent());
        attachToEditor(mw->currentEditor());
    });
}

EditorInspectorDock::~EditorInspectorDock()
{
    delete ui;
}

void EditorInspectorDock::attachToEditor(ScintillaNext *editor)
{
    // If we were attached to another editor, disconnect that
    if (editorConnection) {
        disconnect(editorConnection);
    }

    // Don't update if the dock widget is not visible
    if (this->isHidden()) return;

    // Make sure we are connected to this editor already (only once)
    editorConnection = connect(editor, &ScintillaNext::updateUi, this, &EditorInspectorDock::editorUIUpdated, Qt::UniqueConnection);

    updateEditorInfo(editor);
}

void EditorInspectorDock::editorUIUpdated(Scintilla::Update updated)
{
    if (FlagSet(updated, Scintilla::Update::Content)
            || FlagSet(updated, Scintilla::Update::Selection)
            || FlagSet(updated, Scintilla::Update::VScroll)
            || FlagSet(updated, Scintilla::Update::HScroll)) {
        ScintillaNext *editor = qobject_cast<ScintillaNext*>(sender());

        updateEditorInfo(editor);
    }
}

void EditorInspectorDock::updateEditorInfo(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    for (const QPair<QTreeWidgetItem *, std::function<QString(ScintillaNext *)>> &item : qAsConst(items)) {
        QTreeWidgetItem *widget = item.first;

        if (widget->parent()->isExpanded()) {
            std::function<QString(ScintillaNext *)> func = item.second;
            widget->setText(1, func(editor));
        }
    }
}

void EditorInspectorDock::newItem(QTreeWidgetItem *parent, const QString &label, std::function<QString(ScintillaNext *)> func)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, label);
    items[label] = QPair<QTreeWidgetItem *, std::function<QString(ScintillaNext *)>>(item, func);
}
