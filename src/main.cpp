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


#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QApplication>
#include <QDataStream>
#include <QString>
#include <QVector>

#include "ApplicationSettings.h"
#include "NotepadNextApplication.h"

#ifdef Q_OS_WIN
struct StartupArgumentStorage
{
    QVector<QByteArray> values;
    QVector<char *> pointers;
};

bool HasPlatformArgument(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        const QString argument = QString::fromLocal8Bit(argv[i]);
        if (argument == QLatin1String("-platform") || argument.startsWith(QLatin1String("-platform="))) {
            return true;
        }
    }

    return false;
}

void ApplyThemeModeArguments(int &argc, char **&argv, StartupArgumentStorage &storage)
{
    QSettings settings;
    const auto themeMode = static_cast<ApplicationSettings::ThemeModeEnum>(
        settings.value(QLatin1String("Gui/ThemeMode"), static_cast<int>(ApplicationSettings::LightTheme)).toInt());

    if (themeMode != ApplicationSettings::LightTheme || HasPlatformArgument(argc, argv)) {
        return;
    }

    storage.values.reserve(argc + 2);
    for (int i = 0; i < argc; i++) {
        storage.values.append(QByteArray(argv[i]));
    }
    storage.values.append(QByteArrayLiteral("-platform"));
    storage.values.append(QByteArrayLiteral("windows:darkmode=0"));

    storage.pointers.reserve(storage.values.size() + 1);
    for (QByteArray &value : storage.values) {
        storage.pointers.append(value.data());
    }
    storage.pointers.append(nullptr);

    argc = storage.values.size();
    argv = storage.pointers.data();
}
#endif

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{time process}] %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}: %{message}");

    // Set these since other parts of the app references these
    QApplication::setOrganizationName("NotepadNext");
    QApplication::setApplicationName("NotepadNext");
    QGuiApplication::setApplicationDisplayName("Notepad Next");
    QGuiApplication::setApplicationVersion(APP_VERSION);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // Default settings format
    QSettings::setDefaultFormat(QSettings::IniFormat);

    int applicationArgc = argc;
    char **applicationArgv = argv;
#ifdef Q_OS_WIN
    StartupArgumentStorage startupArgumentStorage;
    ApplyThemeModeArguments(applicationArgc, applicationArgv, startupArgumentStorage);
#endif

    NotepadNextApplication app(applicationArgc, applicationArgv);

    // Log some debug info
    qInfo("=============================");
    for(const auto &d : app.debugInfo()){
        qInfo("%s", qUtf8Printable(d));
    }
    qInfo("=============================");


    if(app.isPrimary()) {
        app.init();

        return app.exec();
    }
    else {
        qInfo() << "Primary instance already running. PID:" << app.primaryPid();

        app.sendInfoToPrimaryInstance();

        qInfo() << "Secondary instance closing...";

        app.exit(0);

        return 0;
    }
}
