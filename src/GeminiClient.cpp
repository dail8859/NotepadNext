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

#include "GeminiClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace {

QString normalizeModelName(QString model)
{
    model = model.trimmed();
    if (model.startsWith(QLatin1String("models/"))) {
        model.remove(0, 7);
    }
    return model;
}

QJsonObject textPart(const QString &text)
{
    QJsonObject part;
    part.insert(QStringLiteral("text"), text);
    return part;
}

QString parseGeminiMarkdown(const QJsonDocument &document)
{
    const QJsonArray candidates = document.object().value(QStringLiteral("candidates")).toArray();
    for (const QJsonValue &candidateValue : candidates) {
        const QJsonObject candidate = candidateValue.toObject();
        const QJsonObject content = candidate.value(QStringLiteral("content")).toObject();
        const QJsonArray parts = content.value(QStringLiteral("parts")).toArray();

        QString markdown;
        for (const QJsonValue &partValue : parts) {
            markdown += partValue.toObject().value(QStringLiteral("text")).toString();
        }

        if (!markdown.trimmed().isEmpty()) {
            return markdown;
        }
    }

    return QString();
}

QString parseGeminiError(const QJsonDocument &document)
{
    const QJsonObject error = document.object().value(QStringLiteral("error")).toObject();
    return error.value(QStringLiteral("message")).toString();
}

}

GeminiClient::GeminiClient(QObject *parent)
    : QObject(parent)
    , networkAccessManager(new QNetworkAccessManager(this))
{
}

void GeminiClient::formatMarkdown(const QString &text, const QString &model, const QString &apiKey)
{
    const QString normalizedModel = normalizeModelName(model);
    if (normalizedModel.isEmpty()) {
        emit formatFailed(tr("Gemini model name is empty."));
        return;
    }

    const QUrl url(QStringLiteral("https://generativelanguage.googleapis.com/v1beta/models/%1:generateContent").arg(normalizedModel));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("x-goog-api-key", apiKey.toUtf8());

    QJsonObject systemInstruction;
    systemInstruction.insert(QStringLiteral("parts"), QJsonArray{
        textPart(QStringLiteral("Return only Markdown. Do not wrap the response in a code fence and do not add explanations. Preserve facts, language, URLs, quotes, and code blocks. Structure the text with headings, lists, tables, quotes, links, and other Markdown syntax only when it fits the content."))
    });

    QJsonObject content;
    content.insert(QStringLiteral("role"), QStringLiteral("user"));
    content.insert(QStringLiteral("parts"), QJsonArray{
        textPart(QStringLiteral("Format this text as clean Markdown:\n\n%1").arg(text))
    });

    QJsonObject body;
    body.insert(QStringLiteral("systemInstruction"), systemInstruction);
    body.insert(QStringLiteral("contents"), QJsonArray{content});

    QNetworkReply *reply = networkAccessManager->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleReply(reply);
    });
}

void GeminiClient::handleReply(QNetworkReply *reply)
{
    reply->deleteLater();

    const QByteArray responseBytes = reply->readAll();
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(responseBytes, &parseError);

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMessage;
        if (parseError.error == QJsonParseError::NoError) {
            errorMessage = parseGeminiError(document);
        }
        if (errorMessage.isEmpty()) {
            errorMessage = reply->errorString();
        }
        emit formatFailed(errorMessage);
        return;
    }

    if (parseError.error != QJsonParseError::NoError) {
        emit formatFailed(tr("Gemini returned an invalid JSON response."));
        return;
    }

    const QString markdown = parseGeminiMarkdown(document);
    if (markdown.trimmed().isEmpty()) {
        emit formatFailed(tr("Gemini returned an empty Markdown response."));
        return;
    }

    emit formatSucceeded(markdown);
}
