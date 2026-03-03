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


#include <QItemSelectionModel>
#include <QMessageBox>
#include <QModelIndex>

#include "MacroEditorDialog.h"
#include "ComboBoxDelegate.h"

#include "ui_MacroEditorDialog.h"


MacroEditorDialog::MacroEditorDialog(QWidget *parent, MacroManager *mm) :
    QDialog(parent),
    ui(new Ui::MacroEditorDialog),
    macroManager(mm),
    model(new MacroListModel(this, mm))
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    ui->setupUi(this);

    ui->listMacros->setModel(model);

    connect(ui->listMacros->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MacroEditorDialog::rowChanged);

    connect(ui->btnCopyMacro, &QPushButton::clicked, this, &MacroEditorDialog::copyCurrentMacro);
    connect(ui->btnDeleteMacro, &QPushButton::clicked, this, &MacroEditorDialog::deleteCurrentMacro);

    connect(ui->editMacroName, &QLineEdit::textChanged, this, &MacroEditorDialog::macroNameChanged);

    connect(ui->btnInsertMacroStep, &QPushButton::clicked, this, &MacroEditorDialog::insertMacroStep);
    connect(ui->btnDeleteMacroStep, &QPushButton::clicked, this, &MacroEditorDialog::deleteMacroStep);
    connect(ui->btnMoveMacroStepUp, &QPushButton::clicked, this, &MacroEditorDialog::moveMacroStepUp);
    connect(ui->btnMoveMacroStepDown, &QPushButton::clicked, this, &MacroEditorDialog::moveMacroStepDown);

    QList<ComboBoxItem> recordableMacroMessages;
    for (Scintilla::Message msg : MacroStep::RecordableMacroMessages()) {
        recordableMacroMessages.append(qMakePair(MacroStep::NameOfMessage(msg), static_cast<int>(msg)));
    }
    ui->tblMacroSteps->setItemDelegateForColumn(0, new ComboBoxDelegate(recordableMacroMessages, this));

    ui->listMacros->setCurrentIndex(model->index(0));
}

MacroEditorDialog::~MacroEditorDialog()
{
    delete ui;
}

void MacroEditorDialog::rowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if (!current.isValid()){
        ui->editMacroName->setReadOnly(true);
        ui->editMacroName->clear();
        ui->keyMacroShortcut->clear();
        if (auto model = ui->tblMacroSteps->model()) {
            model->deleteLater();
        }
        return;
    }
    else {
        ui->editMacroName->setReadOnly(false);

        Macro *macro = model->macro(current);

        ui->editMacroName->setText(macro->getName());

        // Replace the existing model
        if (auto model = ui->tblMacroSteps->model()) {
            model->deleteLater();
        }
        ui->tblMacroSteps->setModel(new MacroStepTableModel(macro, this));

        // Resize some stuff now that we have a new model
        ui->tblMacroSteps->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tblMacroSteps->resizeRowsToContents();
        ui->tblMacroSteps->resizeColumnToContents(0);
    }
}

void MacroEditorDialog::macroNameChanged(const QString &text)
{
    // Ensure there is a non-blank name
    if (text.trimmed().isEmpty()) {
        ui->editMacroName->setStyleSheet("QLineEdit{border: 2px solid red}");
        return;
    }
    else {
        ui->editMacroName->setStyleSheet("");
    }

    QModelIndex currentIndex = ui->listMacros->selectionModel()->currentIndex();

    if (currentIndex.isValid()) {
        Macro *macro = model->macro(currentIndex);
        macro->setName(text);

        // Since the macro was edited directly, need to notify the model this index changed
        emit model->dataChanged(currentIndex, currentIndex);
    }
}

void MacroEditorDialog::deleteCurrentMacro()
{
    QModelIndex currentIndex = ui->listMacros->selectionModel()->currentIndex();

    if (currentIndex.isValid()) {
        const QString macroName = currentIndex.data(Qt::DisplayRole).toString();
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Delete Macro"), tr("Are you sure you want to delete <b>%1</b>?").arg(macroName));

        if (reply == QMessageBox::Yes) {
            model->removeRow(currentIndex.row());
        }
    }
}

void MacroEditorDialog::copyCurrentMacro()
{
    QModelIndex currentIndex = ui->listMacros->selectionModel()->currentIndex();

    if (currentIndex.isValid()) {
        Macro *originalMacro = model->macro(currentIndex);

        model->insertRows(currentIndex.row() + 1, 1);

        Macro *newMacro = model->macro(model->index(currentIndex.row() + 1));

        // TODO: move this into a copy constructor in the Macro class?

        newMacro->setName(originalMacro->getName() + " " + tr("(Copy)"));

        for (const MacroStep &step : qAsConst(originalMacro->getSteps())) {
            newMacro->addMacroStep(step);
        }

        // Select the newly created macro
        ui->listMacros->setCurrentIndex(model->index(currentIndex.row() + 1));
    }
}

void MacroEditorDialog::insertMacroStep()
{
    QModelIndex currentIndex = ui->tblMacroSteps->selectionModel()->currentIndex();

    if (currentIndex.isValid()) {
        ui->tblMacroSteps->model()->insertRows(currentIndex.row() + 1, 1);
        ui->tblMacroSteps->setCurrentIndex(ui->tblMacroSteps->model()->index(currentIndex.row() + 1, 0));
    }
    else {
        ui->tblMacroSteps->model()->insertRows(0, 1);
        ui->tblMacroSteps->setCurrentIndex(ui->tblMacroSteps->model()->index(0, 0));
    }

    // Resize things if needed
    ui->tblMacroSteps->resizeRowsToContents();
    ui->tblMacroSteps->resizeColumnToContents(0);
}

void MacroEditorDialog::deleteMacroStep()
{
    QModelIndex currentIndex = ui->tblMacroSteps->selectionModel()->currentIndex();

    if (currentIndex.isValid()) {
        ui->tblMacroSteps->model()->removeRow(currentIndex.row());
    }
}

void MacroEditorDialog::moveMacroStepUp()
{
    QModelIndex currentIndex = ui->tblMacroSteps->selectionModel()->currentIndex();

    if (currentIndex.isValid() && currentIndex.row() != 0) {
        // This does not make sense to me, start at the previous index and move it to the next index, but it works
        ui->tblMacroSteps->model()->moveRow(QModelIndex(), currentIndex.row() - 1, QModelIndex(), currentIndex.row() + 1);
    }
}

void MacroEditorDialog::moveMacroStepDown()
{
    QModelIndex currentIndex = ui->tblMacroSteps->selectionModel()->currentIndex();

    if (currentIndex.isValid() && currentIndex.row() != ui->tblMacroSteps->model()->rowCount() - 1) {
        // This also does not make ense, start at the current index and skip two
        ui->tblMacroSteps->model()->moveRow(QModelIndex(), currentIndex.row(), QModelIndex(), currentIndex.row() + 2);
    }
}
