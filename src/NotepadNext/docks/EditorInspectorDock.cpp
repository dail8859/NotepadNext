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


static inline QString toBool(int b) {
    return b ? QStringLiteral("True") : QStringLiteral("False");
}

EditorInspectorDock::EditorInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::EditorInspectorDock)
{
    ui->setupUi(this);

    QTreeWidgetItem *positionInfo = new QTreeWidgetItem(ui->treeWidget);
    positionInfo->setText(0, tr("Position Information"));
    positionInfo->setExpanded(true);

    newItem(positionInfo, tr("Current Position"), [](ScintillaNext *editor) { return QString::number(editor->currentPos()); });
    newItem(positionInfo, tr("Current Position (x, y)"), [](ScintillaNext *editor) { return QString("(%1, %2)").arg(editor->pointXFromPosition(editor->currentPos())).arg(editor->pointYFromPosition(editor->currentPos())); });
    newItem(positionInfo, tr("Column"), [](ScintillaNext *editor) { return QString::number(editor->column(editor->currentPos())); });
    // SCI_GETCHARAT(position pos) → int
    newItem(positionInfo, tr("Current Style"), [](ScintillaNext *editor) { return QString::number(editor->styleAt(editor->currentPos())); });
    newItem(positionInfo, tr("Current Line"), [](ScintillaNext *editor) { return QString::number(editor->lineFromPosition(editor->currentPos()) + 1); });
    newItem(positionInfo, tr("Line Length"), [](ScintillaNext *editor) { return QString::number(editor->lineLength(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line End Position"), [](ScintillaNext *editor) { return QString::number(editor->lineEndPosition(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line Indentation"), [](ScintillaNext *editor) { return QString::number(editor->lineIndentation(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line Indent Position"), [](ScintillaNext *editor) { return QString::number(editor->lineIndentPosition(editor->lineFromPosition(editor->currentPos()))); });


    QTreeWidgetItem *selInfo = new QTreeWidgetItem(ui->treeWidget);
    selInfo->setText(0, tr("Selection Information"));
    selInfo->setExpanded(true);

    newItem(selInfo, tr("Mode"), [](ScintillaNext *editor) {
        switch (editor->selectionMode()) {
            case SC_SEL_STREAM:
                return QStringLiteral("SC_SEL_STREAM");
            case SC_SEL_RECTANGLE:
                return QStringLiteral("SC_SEL_RECTANGLE");
            case SC_SEL_LINES:
                return QStringLiteral("SC_SEL_LINES");
            case SC_SEL_THIN:
                return QStringLiteral("SC_SEL_THIN");
            default:
                return QStringLiteral("");
        }
    });
    newItem(selInfo, tr("Is Rectangle"), [](ScintillaNext *editor) { return toBool(editor->selectionIsRectangle()); });
    newItem(selInfo, tr("Selection Empty"), [](ScintillaNext *editor) { return toBool(editor->selectionEmpty()); });
    newItem(selInfo, tr("Main Selection"), [](ScintillaNext *editor) { return QString::number(editor->mainSelection()); });
    newItem(selInfo, tr("# of Selections"), [](ScintillaNext *editor) { return QString::number(editor->selections()); });

    selectionsInfo = new QTreeWidgetItem(selInfo);
    selectionsInfo->setText(0, tr("Multiple Selections"));
    selectionsInfo->setExpanded(true);


    QTreeWidgetItem *documentInfo = new QTreeWidgetItem(ui->treeWidget);
    documentInfo->setText(0, tr("Document Information"));
    documentInfo->setExpanded(true);

    newItem(documentInfo, tr("Length"), [](ScintillaNext *editor) { return QString::number(editor->length()); });
    newItem(documentInfo, tr("Line Count"), [](ScintillaNext *editor) { return QString::number(editor->lineCount()); });


    QTreeWidgetItem *viewInfo = new QTreeWidgetItem(ui->treeWidget);
    viewInfo->setText(0, tr("View Information"));
    viewInfo->setExpanded(true);

    newItem(viewInfo, tr("Lines on Screen"), [](ScintillaNext *editor) { return QString::number(editor->linesOnScreen()); });
    newItem(viewInfo, tr("First Visible Line"), [](ScintillaNext *editor) { return QString::number(editor->firstVisibleLine() + 1); });
    newItem(viewInfo, tr("X Offset"), [](ScintillaNext *editor) { return QString::number(editor->xOffset()); });


    QTreeWidgetItem *foldInfo = new QTreeWidgetItem(ui->treeWidget);
    foldInfo->setText(0, tr("Fold Information"));
    foldInfo->setExpanded(true);

    newItem(foldInfo, tr("Visible From Doc Line"), [](ScintillaNext *editor) { return QString::number(editor->visibleFromDocLine(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Doc Line From Visible"), [](ScintillaNext *editor) { return QString::number(editor->docLineFromVisible(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Fold Level"), [](ScintillaNext *editor) { return QString::number((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE); });
    newItem(foldInfo, tr("Is Fold Header"), [](ScintillaNext *editor) { return toBool((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG); });
    newItem(foldInfo, tr("Fold Parent"), [](ScintillaNext *editor) { return QString::number(editor->foldParent(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Last Child"), [](ScintillaNext *editor) { return QString::number(editor->lastChild(editor->lineFromPosition(editor->currentPos()), -1) + 1); });
    newItem(foldInfo, tr("Contracted Fold Next"), [](ScintillaNext *editor) { return QString::number(editor->contractedFoldNext(editor->lineFromPosition(editor->currentPos())) + 1); });

    connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            connectToEditor(parent->currentEditor());
            connect(parent, &MainWindow::editorActivated, this, &EditorInspectorDock::connectToEditor);
        }
        else {
            disconnectFromEditor();
            disconnect(parent, &MainWindow::editorActivated, this, &EditorInspectorDock::connectToEditor);
        }
    });
}

EditorInspectorDock::~EditorInspectorDock()
{
    delete ui;
}

void EditorInspectorDock::connectToEditor(ScintillaNext *editor)
{
    disconnectFromEditor();

    editorConnection = connect(editor, &ScintillaNext::updateUi, this, &EditorInspectorDock::editorUIUpdated);

    updateEditorInfo(editor);
}


void EditorInspectorDock::disconnectFromEditor()
{
    if (editorConnection) {
        disconnect(editorConnection);
    }
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

    for (const QPair<QTreeWidgetItem *, EditorFunction> &pair : qAsConst(items)) {
        QTreeWidgetItem *item = pair.first;

        if (item->parent()->isExpanded()) {
            EditorFunction func = pair.second;
            item->setText(1, func(editor));
        }
    }

    qDeleteAll(selectionsInfo->takeChildren());

    for (int i = 0; i < editor->selections(); ++i) {
        QTreeWidgetItem *selection = new QTreeWidgetItem(selectionsInfo);
        selection->setText(0, QLatin1Char('#') + QString::number(i));
        selection->setExpanded(true);

        QTreeWidgetItem *caret = new QTreeWidgetItem(selection);
        caret->setText(0, tr("Caret"));
        caret->setText(1, QString::number(editor->selectionNCaret(i)));

        QTreeWidgetItem *anchor = new QTreeWidgetItem(selection);
        anchor->setText(0, tr("Anchor"));
        anchor->setText(1, QString::number(editor->selectionNAnchor(i)));

        QTreeWidgetItem *caretVirtual = new QTreeWidgetItem(selection);
        caretVirtual->setText(0, tr("Caret Virtual Space"));
        caretVirtual->setText(1, QString::number(editor->selectionNCaretVirtualSpace(i)));

        QTreeWidgetItem *anchorVirtual = new QTreeWidgetItem(selection);
        anchorVirtual->setText(0, tr("Anchor Virtual Space"));
        anchorVirtual->setText(1, QString::number(editor->selectionNAnchorVirtualSpace(i)));
    }

    ui->treeWidget->resizeColumnToContents(0);
}

void EditorInspectorDock::newItem(QTreeWidgetItem *parent, const QString &label, EditorFunction func)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, label);
    items.append(qMakePair(item, func));
}
