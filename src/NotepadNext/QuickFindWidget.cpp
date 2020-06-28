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


#include "QuickFindWidget.h"
#include "ScintillaNext.h"
#include "ui_QuickFindWidget.h"

#include <QLineEdit>
#include <QShortcut>


QuickFindWidget::QuickFindWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuickFindWidget)
{
    ui->setupUi(this);

    this->setFocusProxy(ui->lineEdit);

    auto fw = new FocusWatcher(ui->lineEdit);

    connect(fw, &FocusWatcher::focusOut, [=]() {
        clearHighlights();
        hide();
    });

    connect(fw, &FocusWatcher::focusIn, [=]() {
        ui->lineEdit->selectAll();
        performSearch();
    });

    connect(ui->lineEdit, &QLineEdit::textEdited, [=](const QString &) { this->performSearch(); });
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &QuickFindWidget::performSearch);
    connect(ui->buttonRegexp, &QToolButton::toggled, this, &QuickFindWidget::performSearch);
    connect(ui->buttonMatchCase, &QToolButton::toggled, this, &QuickFindWidget::performSearch);
}

QuickFindWidget::~QuickFindWidget()
{
    delete ui;
}

void QuickFindWidget::performSearch()
{
    clearHighlights();

    ScintillaNext *editor =  qobject_cast<ScintillaNext *>(this->parentWidget());
    QString text = ui->lineEdit->text();

    if (text.length() > 0) {
        int searchFlags = 0;

        if (ui->buttonRegexp->isChecked()) {
            searchFlags |= SCFIND_REGEXP;
        }

        if (ui->buttonMatchCase->isChecked()) {
            searchFlags |= SCFIND_MATCHCASE;
        }

        editor->setSearchFlags(searchFlags);

        int foundOne = false;
        editor->forEachMatch(text, [&](int start, int end) {
            foundOne = true;

            editor->indicatorFillRange(start, end - start);
            return end;
        });

        if (foundOne == false) {
            ui->lineEdit->setStyleSheet("border: 1px solid red; padding: 2px;");
        }
        else {
            ui->lineEdit->setStyleSheet("border: 1px solid blue; padding: 2px;");
        }
    }
    else {
        ui->lineEdit->setStyleSheet("border: 1px solid blue; padding: 2px;");
    }
}

void QuickFindWidget::clearHighlights()
{
    ScintillaNext *editor =  qobject_cast<ScintillaNext *>(this->parentWidget());

    editor->setIndicatorCurrent(29);
    editor->indicatorClearRange(0, editor->length());
}
