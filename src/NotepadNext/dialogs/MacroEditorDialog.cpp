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


#include "MacroEditorDialog.h"
#include "MacroManager.h"
#include "ComboBoxDelegate.h"

#include "ui_MacroEditorDialog.h"

MacroEditorDialog::MacroEditorDialog(QWidget *parent, MacroManager *mm) :
    QDialog(parent),
    ui(new Ui::MacroEditorDialog),
    macroManager(mm)
{
    ui->setupUi(this);

    for (const Macro *macro : macroManager->availableMacros()) {
        ui->listMacros->addItem(macro->getName());
    }

    connect(ui->listMacros, &QListWidget::itemSelectionChanged, this, &MacroEditorDialog::selectionChanged);

    connect(ui->editMacroName, &QLineEdit::textChanged, this, [=](const QString &text) {
        Macro *macro = currentSelectedMacro();
        macro->setName(text);
        ui->listMacros->currentItem()->setText(macro->getName());
    });

    //QList<ComboBoxItem> caseItems{
    //    {"SC_CASE_MIXED", SC_CASE_MIXED},
    //    {"SC_CASE_UPPER", SC_CASE_UPPER},
    //    {"SC_CASE_LOWER", SC_CASE_LOWER},
    //    {"SC_CASE_CAMEL", SC_CASE_CAMEL}
    //};
    //ComboBoxDelegate *caseComoboDelegate = new ComboBoxDelegate(caseItems, this);
    //ui->tblMacroSteps->setItemDelegateForColumn(0, caseComoboDelegate);

    ui->listMacros->setCurrentRow(0);
}

MacroEditorDialog::~MacroEditorDialog()
{
    delete ui;
}

Macro *MacroEditorDialog::currentSelectedMacro() const
{
    return macroManager->availableMacros()[ui->listMacros->currentRow()];
}

void MacroEditorDialog::selectionChanged()
{
    const Macro *m = currentSelectedMacro();

    ui->editMacroName->setText(m->getName());

    ui->tblMacroSteps->clearContents();
    ui->tblMacroSteps->setRowCount(m->steps().size());

    int i = 0;
    for (const MacroAction &ma : m->steps()) {
        ui->tblMacroSteps->setItem(i, 0, new QTableWidgetItem(ma.name()));

        if (MacroAction::MessageHasString(ma.message)) {
            ui->tblMacroSteps->setItem(i, 1, new QTableWidgetItem(QString(ma.str)));
        }
        ++i;
    }

    ui->tblMacroSteps->resizeRowsToContents();
}
