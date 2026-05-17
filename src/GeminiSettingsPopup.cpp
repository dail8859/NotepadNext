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

#include "GeminiSettingsPopup.h"

#include "ApplicationSettings.h"
#include "GeminiCredentialStore.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShowEvent>
#include <QVBoxLayout>

namespace {

const char GeminiModelSetting[] = "Gemini/Model";
const char DefaultGeminiModel[] = "gemini-3.1-flash-lite";

}

GeminiSettingsPopup::GeminiSettingsPopup(ApplicationSettings *settings, GeminiCredentialStore *credentialStore, QWidget *parent)
    : QWidget(parent, Qt::Popup)
    , settings(settings)
    , credentialStore(credentialStore)
    , statusLabel(new QLabel(this))
    , apiKeyEdit(new QLineEdit(this))
    , modelEdit(new QLineEdit(this))
{
    apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyEdit->setPlaceholderText(tr("Enter a new API key"));

    modelEdit->setText(modelName());

    auto *formLayout = new QFormLayout;
    formLayout->addRow(tr("API key:"), apiKeyEdit);
    formLayout->addRow(tr("Model:"), modelEdit);

    auto *saveButton = new QPushButton(tr("Save"), this);
    auto *deleteButton = new QPushButton(tr("Delete"), this);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(saveButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(statusLabel);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &GeminiSettingsPopup::save);
    connect(deleteButton, &QPushButton::clicked, this, &GeminiSettingsPopup::remove);
    connect(modelEdit, &QLineEdit::editingFinished, this, [this]() {
        const QString model = modelEdit->text().trimmed();
        if (!model.isEmpty()) {
            this->settings->setValue(QLatin1String(GeminiModelSetting), model);
            emit modelChanged(model);
        }
    });

    refreshStatus();
}

QString GeminiSettingsPopup::modelName() const
{
    return settings->value(QLatin1String(GeminiModelSetting), QString::fromLatin1(DefaultGeminiModel)).toString();
}

void GeminiSettingsPopup::refreshStatus()
{
    QString errorMessage;
    const bool hasKey = credentialStore->hasApiKey(&errorMessage);

    if (!errorMessage.isEmpty()) {
        statusLabel->setText(errorMessage);
    }
    else if (hasKey) {
        statusLabel->setText(tr("Gemini API key is saved."));
    }
    else {
        statusLabel->setText(tr("No Gemini API key is saved."));
    }
}

void GeminiSettingsPopup::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    apiKeyEdit->clear();
    modelEdit->setText(modelName());
    refreshStatus();
}

void GeminiSettingsPopup::save()
{
    const QString apiKey = apiKeyEdit->text().trimmed();
    const QString model = modelEdit->text().trimmed();

    if (!model.isEmpty()) {
        settings->setValue(QLatin1String(GeminiModelSetting), model);
        emit modelChanged(model);
    }

    if (apiKey.isEmpty()) {
        refreshStatus();
        hide();
        return;
    }

    QString errorMessage;
    if (!credentialStore->saveApiKey(apiKey, &errorMessage)) {
        showError(errorMessage);
        return;
    }

    apiKeyEdit->clear();
    refreshStatus();
    emit credentialsChanged();
    hide();
}

void GeminiSettingsPopup::remove()
{
    QString errorMessage;
    if (!credentialStore->deleteApiKey(&errorMessage)) {
        showError(errorMessage);
        return;
    }

    apiKeyEdit->clear();
    refreshStatus();
    emit credentialsChanged();
}

void GeminiSettingsPopup::showError(const QString &message)
{
    QMessageBox::warning(this, tr("Gemini API"), message.isEmpty() ? tr("The Gemini API key could not be saved.") : message);
}
