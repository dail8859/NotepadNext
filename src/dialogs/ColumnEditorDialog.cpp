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


#include "ColumnEditorDialog.h"
#include "UndoAction.h"
#include "ui_ColumnEditorDialog.h"


ColumnEditorDialog::ColumnEditorDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::ColumnEditorDialog),
    parent(parent)
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    ui->setupUi(this);

    ui->sbxStart->setMaximum(std::numeric_limits<int>::max());
    ui->sbxStart->setMinimum(std::numeric_limits<int>::min());

    ui->sbxStep->setMaximum(std::numeric_limits<int>::max());
    ui->sbxStep->setMinimum(std::numeric_limits<int>::min());

    connect(ui->gbxText, &QGroupBox::toggled, ui->gbxNumbers, [=](bool on) {
        ui->gbxNumbers->setChecked(!on);
    });

    connect(ui->gbxNumbers, &QGroupBox::toggled, ui->gbxText, [=](bool on) {
        ui->gbxText->setChecked(!on);
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        if (ui->gbxText->isChecked() && !ui->txtText->text().isEmpty()) {
            insertTextStartingAtCurrentColumn([=]() { return ui->txtText->text(); });
        }
        else if (ui->gbxNumbers->isChecked()) {
            int currentValue = ui->sbxStart->value();
            insertTextStartingAtCurrentColumn([&currentValue, this]() {
                const QString s = QString::number(currentValue);
                currentValue += ui->sbxStep->value();
                return s;
            });
        }
    });

     ui->txtText->setFocus();
}

ColumnEditorDialog::~ColumnEditorDialog()
{
    delete ui;
}

void ColumnEditorDialog::insertTextStartingAtCurrentColumn(const std::function<QString ()> &f)
{
    ScintillaNext *editor = parent->currentEditor();

    if (editor->selectionMode() == SC_SEL_STREAM && editor->selections() == 1 && editor->selectionEmpty()) {
        const int currentPos = editor->selectionNCaret(0);

        // If the cursor is in virtual space, the call to selectionNCaretVirtualSpace will be > 0
        const int currentColumn = editor->column(currentPos) + editor->selectionNCaretVirtualSpace(0);

        const UndoAction ua(editor);
        for (int line = editor->lineFromPosition(currentPos); line < editor->lineCount(); ++line) {
            insertTextAtColumn(editor, line, currentColumn, f());
        }
    }
    else/* if (editor->selectionMode() == SC_SEL_RECTANGLE || editor->selections() > 0)*/ {
        const int totalSelections = editor->selections();

        // TODO: sort selections from top to bottom?

        const UndoAction ua(editor);
        for(int selection = 0; selection < totalSelections; ++selection) {
            const int start = editor->selectionNStart(selection) + editor->selectionNStartVirtualSpace(selection);
            const int end = editor->selectionNEnd(selection) + editor->selectionNEndVirtualSpace(selection);

            editor->setTargetRange(start, end);
            editor->replaceTarget(-1, f().toUtf8().constData());
        }
    }
}

void ColumnEditorDialog::insertTextAtColumn(ScintillaNext *editor, int line, int column, const QString &str)
{
    const int lineEndPosition = editor->lineEndPosition(line);
    const int lineEndColumn = editor->column(lineEndPosition);

    // If the line does not end past the needed column, then use the left over as virtual space
    if (lineEndColumn < column) {
        editor->setTargetRange(lineEndPosition, lineEndPosition);

        editor->setTargetStartVirtualSpace(column - lineEndColumn);
        editor->setTargetEndVirtualSpace(column - lineEndColumn);
    }
    else {
        const int pos = editor->findColumn(line, column);

        editor->setTargetRange(pos, pos);
    }

    editor->replaceTarget(-1, str.toUtf8().constData());
}
