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


#ifndef MACROEDITORDIALOG_H
#define MACROEDITORDIALOG_H

#include <QDialog>

#include "MacroListModel.h"
#include "MacroStepTableModel.h"
#include "MacroManager.h"

namespace Ui {
class MacroEditorDialog;
}

class MacroEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MacroEditorDialog(QWidget *parent, MacroManager *mm);
    ~MacroEditorDialog();

private slots:
    void rowChanged(const QModelIndex &current, const QModelIndex &previous);
    void macroNameChanged(const QString &text);

    void deleteCurrentMacro();
    void copyCurrentMacro();

    void insertMacroStep();
    void deleteMacroStep();
    void moveMacroStepUp();
    void moveMacroStepDown();

private:
    Ui::MacroEditorDialog *ui;
    MacroManager *macroManager;
    MacroListModel *model;
};

#endif // MACROEDITORDIALOG_H
