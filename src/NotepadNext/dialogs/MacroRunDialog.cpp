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


#include "MacroManager.h"
#include "MacroRunDialog.h"

#include "ui_MacroRunDialog.h"


MacroRunDialog::MacroRunDialog(QWidget *parent, MacroManager *mm) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::MacroRunDialog),
    macroManager(mm)
{
    ui->setupUi(this);

    connect(ui->buttonRun, &QPushButton::clicked, this, [=]() {
        const int index = ui->comboBox->currentIndex();
        int times = -1; // for end of file

        if (ui->radioExecute->isChecked()) {
            times = ui->spinTimes->value();
        }

        emit execute(macroManager->availableMacros().at(index), times);
    });
}

MacroRunDialog::~MacroRunDialog()
{
    delete ui;
}

void MacroRunDialog::showEvent(QShowEvent *event)
{
    ui->comboBox->clear();

    for (const Macro *macro : macroManager->availableMacros()) {
        ui->comboBox->addItem(macro->getName());
    }

    QDialog::showEvent(event);
}
