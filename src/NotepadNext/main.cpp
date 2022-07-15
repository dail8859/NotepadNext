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

#include "NotepadNextApplication.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{time process}] %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}: %{message}");

    // Set these since other parts of the app references these
    QApplication::setOrganizationName("NotepadNext");
    QApplication::setApplicationName("NotepadNext");
    QGuiApplication::setApplicationDisplayName("Notepad Next");
#ifdef Q_OS_UNIX
    // Unix doesn't provide an application version by default
    QGuiApplication::setApplicationVersion(APP_VERSION);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // Default settings format
    QSettings::setDefaultFormat(QSettings::IniFormat);

    NotepadNextApplication app(argc, argv);

    const QStringList args = app.arguments();
    QCommandLineParser parser;
    parseCommandLine(parser, args);

    // Log some debug info
    qInfo("=============================");
    qInfo("%s v%s%s", qUtf8Printable(QApplication::applicationName()), qUtf8Printable(QApplication::applicationVersion()), APP_DISTRIBUTION);
    qInfo("Build Date/Time: %s %s", __DATE__, __TIME__);
    qInfo("Qt: %s", qVersion());
    qInfo("OS: %s", qUtf8Printable(QSysInfo::prettyProductName()));
    qInfo("Locale: %s", qUtf8Printable(QLocale::system().name()));
    qInfo("CPU: %s", qUtf8Printable(QSysInfo::currentCpuArchitecture()));
    qInfo("File Path: %s", qUtf8Printable(QApplication::applicationFilePath()));
    qInfo("Arguments: %s", qUtf8Printable(QApplication::arguments().join(' ')));
    qInfo("=============================");

    if(app.isPrimary()) {
        app.init(parser);

        return app.exec();
    }
    else {
        qInfo() << "Primary instance already running. PID:" << app.primaryPid();

        // This eventually needs moved into the NotepadNextApplication to keep
        // sending/receiving logic in the same place
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);

        stream << args;
        app.sendMessage(buffer);

        qInfo() << "Secondary instance closing...";

        app.exit(0);

        return 0;
    }
}
