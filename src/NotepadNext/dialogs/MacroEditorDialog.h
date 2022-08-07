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

#include <QAbstractItemModel>
#include <QDialog>

namespace Ui {
class MacroEditorDialog;
}
class MacroManager;
class Macro;
class MacroModel;

class MacroEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MacroEditorDialog(QWidget *parent, MacroManager *mm);
    ~MacroEditorDialog();

private:
    Macro *currentSelectedMacro() const;

private slots:
    void rowChanged(const QModelIndex &current, const QModelIndex &previous);
    void macroNameChanged(const QString &text);

    void deleteCurrentMacro();
    void copyCurrentMacro();

private:
    Ui::MacroEditorDialog *ui;
    MacroManager *macroManager;
    MacroModel *model;
};


class MacroModel : public QAbstractListModel
{
    Q_OBJECT

public :
    MacroModel(QObject * parent, MacroManager *mm);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;

    Macro *macro(const QModelIndex &index);

private:
   MacroManager *macroManager;
};

#endif // MACROEDITORDIALOG_H
