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
#include "MacroManager.h"

#include "ui_MacroEditorDialog.h"


MacroEditorDialog::MacroEditorDialog(QWidget *parent, MacroManager *mm) :
    QDialog(parent),
    ui(new Ui::MacroEditorDialog),
    macroManager(mm),
    model(new MacroModel(this, mm))
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    ui->setupUi(this);

    ui->listMacros->setModel(model);

    connect(ui->listMacros->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MacroEditorDialog::rowChanged);

    connect(ui->editMacroName, &QLineEdit::textChanged, this, &MacroEditorDialog::macroNameChanged);

    connect(ui->btnDeleteMacro, &QPushButton::clicked, this, &MacroEditorDialog::deleteCurrentMacro);

    connect(ui->btnCopyMacro, &QPushButton::clicked, this, &MacroEditorDialog::copyCurrentMacro);

   ui->listMacros->setCurrentIndex(model->index(0));
}

MacroEditorDialog::~MacroEditorDialog()
{
    delete ui;
}

Macro *MacroEditorDialog::currentSelectedMacro() const
{
    return Q_NULLPTR;
}

void MacroEditorDialog::rowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if (!current.isValid()){
        ui->editMacroName->setReadOnly(true);
        ui->editMacroName->clear();
        ui->keyMacroShortcut->clear();
        ui->tblMacroSteps->clearContents();
        return;
    }
    else {
        ui->editMacroName->setReadOnly(false);

        Macro *macro = model->macro(current);

        ui->editMacroName->setText(macro->getName());

        ui->tblMacroSteps->clearContents();
        ui->tblMacroSteps->setRowCount(macro->getSteps().size());

        int i = 0;
        for (const MacroStep &step : macro->getSteps()) {
            ui->tblMacroSteps->setItem(i, 0, new QTableWidgetItem(step.name()));

            if (MacroStep::MessageHasString(step.message)) {
                ui->tblMacroSteps->setItem(i, 1, new QTableWidgetItem(QString(step.str)));
            }
            ++i;
        }

        ui->tblMacroSteps->resizeRowsToContents();
    }
}

void MacroEditorDialog::macroNameChanged(const QString &text)
{
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

        newMacro->setName(originalMacro->getName() + " " + tr("(Copy)"));

        for (const MacroStep &step : originalMacro->getSteps()) {
            newMacro->addMacroStep(step);
        }
    }
}


MacroModel::MacroModel(QObject *parent, MacroManager *mm) :
    QAbstractListModel(parent),
    macroManager(mm)
{
}

int MacroModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return macroManager->availableMacros().size();
}

QVariant MacroModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

   if (role == Qt::DisplayRole) {
       return macroManager->availableMacros()[index.row()]->getName();
   }

   return QVariant();
}

bool MacroModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    while (count--) {
        delete macroManager->availableMacros().takeAt(row);
    }

    endRemoveRows();

    return true;
}

bool MacroModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r) {
        macroManager->availableMacros().insert(row, new Macro());
    }

    endInsertRows();

    return true;
}

Macro *MacroModel::macro(const QModelIndex &index)
{
    if (index.isValid())
        return macroManager->availableMacros()[index.row()];
    else
        return Q_NULLPTR;
}
