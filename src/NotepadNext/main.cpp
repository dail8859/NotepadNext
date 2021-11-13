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

    // Set some attributes
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Default settings format
    QSettings::setDefaultFormat(QSettings::IniFormat);

    NotepadNextApplication app(argc, argv);

    // Log some debug info
    qInfo("=============================");
    qInfo("%s (v%s)", qUtf8Printable(QApplication::applicationName()), qUtf8Printable(QApplication::applicationVersion()));
    qInfo("OS: %s", qUtf8Printable(QSysInfo::prettyProductName()));
    qInfo("CPU: %s", qUtf8Printable(QSysInfo::currentCpuArchitecture()));
    qInfo("File Path: %s", qUtf8Printable(QApplication::applicationFilePath()));
    qInfo("Arguments: %s", qUtf8Printable(QApplication::arguments().join(' ')));
    qInfo("=============================");

    if(app.isPrimary()) {
        app.initGui();

        return app.exec();
    }
    else {
        // This eventually needs moved into the NotepadNextApplication to keep
        // sending/receiving logic in the same place
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);

        stream << app.arguments();

        qDebug() << "App already running...";
        qDebug() << "Primary instance PID: " << app.primaryPid();
        qDebug() << "Sending:" << buffer;

        app.sendMessage(buffer);

        qInfo("Secondary instance closing...");

        app.exit(0);
        return 0;
    }
}
