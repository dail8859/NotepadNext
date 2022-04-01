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

    ui->groupTabBar->setChecked(settings->showTabBar());
    connect(settings, &Settings::showTabBarChanged, ui->groupTabBar, &QGroupBox::setChecked);
    connect(ui->groupTabBar, &QGroupBox::clicked, settings, &Settings::setShowTabBar);

    ui->checkBoxStatusBar->setChecked(settings->showStatusBar());
    connect(settings, &Settings::showStatusBarChanged, ui->checkBoxStatusBar, &QCheckBox::setChecked);
    connect(ui->checkBoxStatusBar, &QCheckBox::clicked, settings, &Settings::setShowStatusBar);

    ui->checkBoxTabsClosable->setChecked(settings->tabsClosable());
    connect(settings, &Settings::tabsClosableChanged, ui->checkBoxTabsClosable, &QCheckBox::setChecked);
    connect(ui->checkBoxTabsClosable, &QCheckBox::clicked, settings, &Settings::setTabsClosable);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}
