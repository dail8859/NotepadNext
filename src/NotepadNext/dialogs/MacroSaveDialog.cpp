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


#include "MacroSaveDialog.h"
#include "ui_MacroSaveDialog.h"

MacroSaveDialog::MacroSaveDialog(QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::MacroSaveDialog)
{
    ui->setupUi(this);

    connect(ui->editName, &QLineEdit::textChanged, [=](const QString &text) {
        ui->buttonOk->setEnabled(text.size() > 0);
    });

}

MacroSaveDialog::~MacroSaveDialog()
{
    delete ui;
}

QString MacroSaveDialog::getName() const
{
    return ui->editName->text();
}

QKeySequence MacroSaveDialog::getShortcut() const
{
    return ui->keySequenceEdit->keySequence();
}

int MacroSaveDialog::exec()
{
    ui->editName->clear();
    ui->keySequenceEdit->clear();

    return QDialog::exec();
}
