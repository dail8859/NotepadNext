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


#ifndef MACRORUNDIALOG_H
#define MACRORUNDIALOG_H

#include <QDialog>

class Macro;

namespace Ui {
class MacroRunDialog;
}

class MacroRunDialog : public QDialog
{
    Q_OBJECT

public:
    MacroRunDialog(QWidget *parent);
    ~MacroRunDialog();

    void setMacros(QVector<Macro*> macros);

signals:
    void execute(Macro *macro, int times);

private:
    Ui::MacroRunDialog *ui;
    QVector<Macro*> macros;
};

#endif // MACRORUNDIALOG_H
