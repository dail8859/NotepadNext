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

#include <QFontDatabase>

#include "MainWindow.h"
#include "ScintillaNext.h"

#include "HexViewerDock.h"
#include "ui_HexViewerDock.h"

#include "EditorHexViewerTableModel.h"


HexViewerDock::HexViewerDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::HexViewerDock)
{
    ui->setupUi(this);

    // Set the font of the table to a monospaced font...not sure how best to do this
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
    ui->tblHexView->setFont(font);

    connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            connectToEditor(parent->currentEditor());

            connect(parent, &MainWindow::editorActivated, this, &HexViewerDock::connectToEditor);
        }
        else {
            disconnect(parent, &MainWindow::editorActivated, this, &HexViewerDock::connectToEditor);
        }
    });
}

HexViewerDock::~HexViewerDock()
{
    delete ui;
}

void HexViewerDock::connectToEditor(ScintillaNext *editor)
{
    if (auto model = ui->tblHexView->model()) {
        model->deleteLater();
    }

    EditorHexViewerTableModel *model = new EditorHexViewerTableModel(this);
    model->setEditor(editor);
    ui->tblHexView->setModel(model);
    ui->tblHexView->resizeColumnsToContents();
    ui->tblHexView->resizeRowsToContents();
}
