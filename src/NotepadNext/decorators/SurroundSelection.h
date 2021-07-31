/*
 * This file is part of Notepad Next.
 * Copyright 2020 Justin Dailey
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


#ifndef SURROUNDSELECTION_H
#define SURROUNDSELECTION_H

#include <QObject>
#include <QWidget>

#include "ScintillaEdit.h"
#include "EditorDecorator.h"


class SurroundSelection : public EditorDecorator
{
    Q_OBJECT
public:
    SurroundSelection(ScintillaEdit *editor);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private:
    void surroundSelections(const char ch1, const char ch2);
};

#endif // SURROUNDSELECTION_H
