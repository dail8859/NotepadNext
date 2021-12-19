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


#include "MacroRunDialog.h"
#include "ui_MacroRunDialog.h"

#include "MacroRecorder.h"

MacroRunDialog::MacroRunDialog(QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::MacroRunDialog)
{
    ui->setupUi(this);

    //connect(ui->buttonOk, &QPushButton::clicked, this, &MacroRunDialog::accept);
    connect(ui->buttonRun, &QPushButton::clicked, [=]() {
        const int index = ui->comboBox->currentIndex();
        int times = -1; // for end of file

        if (ui->radioExecute->isChecked()) {
            times = ui->spinTimes->value();
        }

        emit execute(macros.at(index), times);
    });
}

MacroRunDialog::~MacroRunDialog()
{
    delete ui;
}

void MacroRunDialog::setMacros(QVector<Macro *> macros)
{
    qInfo(Q_FUNC_INFO);

    this->macros = macros;

    ui->comboBox->clear();

    for (const Macro *macro : macros) {
        ui->comboBox->addItem(macro->getName());
    }
}
