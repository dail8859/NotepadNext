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

#include <QMessageBox>


PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::PreferencesDialog),
    settings(settings)
{
    ui->setupUi(this);

    MapSettingToCheckBox(ui->checkBoxMenuBar, &ApplicationSettings::showMenuBar, &ApplicationSettings::setShowMenuBar, &ApplicationSettings::showMenuBarChanged);
    MapSettingToCheckBox(ui->checkBoxToolBar, &ApplicationSettings::showToolBar, &ApplicationSettings::setShowToolBar, &ApplicationSettings::showToolBarChanged);
    MapSettingToCheckBox(ui->checkBoxStatusBar, &ApplicationSettings::showStatusBar, &ApplicationSettings::setShowStatusBar, &ApplicationSettings::showStatusBarChanged);

    MapSettingToGroupBox(ui->gbxRestorePreviousSession, &ApplicationSettings::restorePreviousSession, &ApplicationSettings::setRestorePreviousSession, &ApplicationSettings::restorePreviousSessionChanged);
    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, this, [=](bool checked) {
        if (!checked) {
            ui->checkBoxUnsavedFiles->setChecked(false);
            ui->checkBoxRestoreTempFiles->setChecked(false);
        }
        else {
            QMessageBox::warning(this, tr("Warning"), tr("This feature is experimental and it should not be considered safe for critically important work. It may lead to possible data loss. Use at your own risk."));
        }
    });

    MapSettingToCheckBox(ui->checkBoxUnsavedFiles, &ApplicationSettings::restoreUnsavedFiles, &ApplicationSettings::setRestoreUnsavedFiles, &ApplicationSettings::restoreUnsavedFilesChanged);
    MapSettingToCheckBox(ui->checkBoxRestoreTempFiles, &ApplicationSettings::restoreTempFiles, &ApplicationSettings::setRestoreTempFiles, &ApplicationSettings::restoreTempFilesChanged);

    MapSettingToCheckBox(ui->checkBoxCombineSearchResults, &ApplicationSettings::combineSearchResults, &ApplicationSettings::setCombineSearchResults, &ApplicationSettings::combineSearchResultsChanged);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToCheckBox(QCheckBox *checkBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    checkBox->setChecked(std::bind(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, checkBox, &QCheckBox::setChecked);
    connect(checkBox, &QCheckBox::toggled, settings, setter);
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToGroupBox(QGroupBox *groupBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    groupBox->setChecked(std::bind(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, groupBox, &QGroupBox::setChecked);
    connect(groupBox, &QGroupBox::toggled, settings, setter);
}
