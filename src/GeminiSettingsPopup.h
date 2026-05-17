/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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

#pragma once

#include <QWidget>

class ApplicationSettings;
class GeminiCredentialStore;
class QLabel;
class QLineEdit;

class GeminiSettingsPopup : public QWidget
{
    Q_OBJECT

public:
    GeminiSettingsPopup(ApplicationSettings *settings, GeminiCredentialStore *credentialStore, QWidget *parent = nullptr);

    QString modelName() const;
    void refreshStatus();

signals:
    void modelChanged(const QString &model);
    void credentialsChanged();

protected:
    void showEvent(QShowEvent *event) override;

private:
    ApplicationSettings *settings;
    GeminiCredentialStore *credentialStore;

    QLabel *statusLabel;
    QLineEdit *apiKeyEdit;
    QLineEdit *modelEdit;

    void save();
    void remove();
    void showError(const QString &message);
};
