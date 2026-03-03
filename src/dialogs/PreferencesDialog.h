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


#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "ApplicationSettings.h"

#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>

namespace Ui {
class PreferencesDialog;
}

class Settings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(ApplicationSettings *settings, QWidget *parent = 0);
    ~PreferencesDialog();

    void showApplicationRestartRequired() const;

private:
    Ui::PreferencesDialog *ui;
    ApplicationSettings *settings;

    template <typename Func1, typename Func2, typename Func3>
    void MapSettingToCheckBox(QCheckBox *checkBox, Func1 getter, Func2 setter, Func3 notifier) const;

    template <typename Func1, typename Func2, typename Func3>
    void MapSettingToGroupBox(QGroupBox *groupBox, Func1 getter, Func2 setter, Func3 notifier) const;

    void populateTranslationComboBox();
};

#endif // PREFERENCESDIALOG_H
