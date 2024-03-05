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

QString TranslationFileNameToLocaleName(const QString &baseName)
{
    // return "zh_CN" from "NotepadNext_zh_CN"
    return baseName.mid(QApplication::applicationName().length() + 1); // +1 for the underscore
}


TranslationManager::TranslationManager(QCoreApplication *app, const QString &path)
    : QObject(app), path(path), app(app)
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

    // Load translation for NotepadNext e.g. ":/i18n/NotepadNext_en.qm"
    if (translatorApp.load(locale, QApplication::applicationName(), QString("_"), path)) {
        app->installTranslator(&translatorApp);
        qInfo("Loaded %s translation %s for Notepad Next", qUtf8Printable(locale.name()), qUtf8Printable(translatorApp.filePath()));
    } else {
        qInfo("%s translation not found for Notepad Next", qUtf8Printable(locale.name()));
    }

    // Load translation for Qt components e.g. ":/i18n/qt_en.qm"
    if (translatorQt.load(locale, QString("qt"), QString("_"), path)) {
        app->installTranslator(&translatorQt);
        qInfo("Loaded %s translation %s for Qt components", qUtf8Printable(locale.name()), qUtf8Printable(translatorQt.filePath()));
    } else {
        qInfo("%s translation not found for Qt components", qUtf8Printable(locale.name()));
    }
}

void TranslationManager::loadTranslationByName(QString localeName)
{
    if (localeName.isEmpty()) {
        loadSystemDefaultTranslation();
    }
    else {
        loadTranslation(QLocale(localeName));
    }
}
