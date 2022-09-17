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

#include <QFileDialog>

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "Settings.h"

PreferencesDialog::PreferencesDialog(Settings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::PreferencesDialog),
    settings(settings)
{
    ui->setupUi(this);

    ui->checkBoxMenuBar->setChecked(settings->showMenuBar());
    connect(settings, &Settings::showMenuBarChanged, ui->checkBoxMenuBar, &QCheckBox::setChecked);
    connect(ui->checkBoxMenuBar, &QCheckBox::clicked, settings, &Settings::setShowMenuBar);

    ui->checkBoxToolBar->setChecked(settings->showToolBar());
    connect(settings, &Settings::showToolBarChanged, ui->checkBoxToolBar, &QCheckBox::setChecked);
    connect(ui->checkBoxToolBar, &QCheckBox::clicked, settings, &Settings::setShowToolBar);

    ui->checkBoxStatusBar->setChecked(settings->showStatusBar());
    connect(settings, &Settings::showStatusBarChanged, ui->checkBoxStatusBar, &QCheckBox::setChecked);
    connect(ui->checkBoxStatusBar, &QCheckBox::clicked, settings, &Settings::setShowStatusBar);

	ui->ctagsCmdEdit->setText(settings->getCtagsCmd());
    connect(settings, &Settings::notifyCtagsCmd, ui->ctagsCmdEdit, &QLineEdit::setText);
    connect(ui->ctagsCmdEdit, &QLineEdit::textChanged, settings, &Settings::setCtagsCmd);

	connect(ui->ctagsCmdButton, &QPushButton::clicked, this, [=]() {
		QString fileName = QFileDialog::getOpenFileName(this, "choose ctags command");
		if (fileName.size() > 0) { ui->ctagsCmdEdit->setText(fileName); }
	});

}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}
