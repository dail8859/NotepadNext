/*
 * This file is part of Notepad Next.
 * Copyright 2024 Justin Dailey
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

#include "TranslationManager.h"

#include <QApplication>
#include <QDirIterator>
#include <QDebug>

#include <memory>

QString TranslationFileNameToLocaleName(const QString &baseName)
{
    // return "zh_CN" from "NotepadNext_zh_CN"
    return baseName.mid(QApplication::applicationName().length() + 1); // +1 for the underscore
}


TranslationManager::TranslationManager(QObject *parent, const QString &path)
    : QObject(parent), path(path)
{
}

QStringList TranslationManager::availableTranslations() const
{
    QStringList translations;
    QDirIterator it(path);

    while (it.hasNext()) {
        it.next();

        if (it.fileName().startsWith(QApplication::applicationName())) { // Some are Qt translation files, not for the application
            QString localeName = TranslationFileNameToLocaleName(it.fileInfo().baseName());

            translations.append(localeName);
        }
    }

    return translations;
}

QString TranslationManager::FormatLocaleTerritoryAndLanguage(QLocale &locale)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    const QString territory = QLocale::countryToString(locale.country());
#else
    const QString territory = QLocale::territoryToString(locale.territory());
#endif
    const QString language = QLocale::languageToString(locale.language());
    return QStringLiteral("%1 / %2").arg(language, territory);
}

void TranslationManager::loadSystemDefaultTranslation()
{
    // The wrong translation file may be loaded when passing Locale::system() to loadTranslation function, e.g. "zh_CN" translation file will be loaded when the locale is "en_US". It's probably a Qt bug.
    loadTranslation(QLocale(QLocale::system().name()));
}

void TranslationManager::loadTranslation(QLocale locale)
{
    qInfo(Q_FUNC_INFO);

    const QStringList filenames{QStringList{QApplication::applicationName(), "qt", "qtbase"}};

    for (const auto& filename : filenames) {
        std::unique_ptr<QTranslator> translator = std::make_unique<QTranslator>();

        if (translator->load(locale, filename, QStringLiteral("_"), path)) {
            if (QCoreApplication::installTranslator(translator.get())) {
                qInfo() << "Loaded translation" << translator.get()->filePath();
                translators.append(translator.release());
            }
        }
    }
}

void TranslationManager::loadTranslation(QString localeName)
{
    loadTranslation(QLocale(localeName));
}
