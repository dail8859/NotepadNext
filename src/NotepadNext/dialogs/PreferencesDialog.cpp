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

#include <QMessageBox>

PreferencesDialog::PreferencesDialog(Settings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::PreferencesDialog),
    settings(settings)
{
    ui->setupUi(this);

    ui->checkBoxMenuBar->setChecked(settings->showMenuBar());
    connect(settings, &Settings::showMenuBarChanged, ui->checkBoxMenuBar, &QCheckBox::setChecked);
    connect(ui->checkBoxMenuBar, &QCheckBox::toggled, settings, &Settings::setShowMenuBar);

    ui->checkBoxToolBar->setChecked(settings->showToolBar());
    connect(settings, &Settings::showToolBarChanged, ui->checkBoxToolBar, &QCheckBox::setChecked);
    connect(ui->checkBoxToolBar, &QCheckBox::toggled, settings, &Settings::setShowToolBar);

    ui->checkBoxStatusBar->setChecked(settings->showStatusBar());
    connect(settings, &Settings::showStatusBarChanged, ui->checkBoxStatusBar, &QCheckBox::setChecked);
    connect(ui->checkBoxStatusBar, &QCheckBox::toggled, settings, &Settings::setShowStatusBar);

    ui->gbxRestorePreviousSession->setChecked(settings->restorePreviousSession());
    connect(settings, &Settings::restorePreviousSessionChanged, ui->gbxRestorePreviousSession, &QGroupBox::setChecked);
    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, settings, &Settings::setRestorePreviousSession);

    ui->checkBoxUnsavedFiles->setChecked(settings->restoreUnsavedFiles());
    ui->checkBoxRestoreTempFiles->setChecked(settings->restoreTempFiles());

    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, ui->checkBoxRestoreTempFiles, &QCheckBox::setEnabled);

    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, this, [=](bool checked) {
        if (!checked) {
            ui->checkBoxUnsavedFiles->setChecked(false);
            ui->checkBoxRestoreTempFiles->setChecked(false);
        }
        else {
            QMessageBox::warning(this, tr("Warning"), tr("This feature is experimental and it should not be considered safe for critically important work. It may lead to possible data loss. Use at your own risk."));
        }
    });

    connect(settings, &Settings::restoreUnsavedFilesChanged, ui->checkBoxUnsavedFiles, &QCheckBox::setChecked);
    connect(ui->checkBoxUnsavedFiles, &QCheckBox::toggled, settings, &Settings::setRestoreUnsavedFiles);

    connect(settings, &Settings::restoreTempFilesChanged, ui->checkBoxRestoreTempFiles, &QCheckBox::setChecked);
    connect(ui->checkBoxRestoreTempFiles, &QCheckBox::toggled, settings, &Settings::setRestoreTempFiles);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}
