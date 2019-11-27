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


#include "MainWindow.h"
#include "NotepadNextApplication.h"

#include <QSettings>

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


    NotepadNextApplication app("Notepad Next Application", argc, argv);

    // Log some debug info
    qInfo("Name: %s", QApplication::applicationName().toLatin1().constData());
    qInfo("Version: v%s", QApplication::applicationVersion().toLatin1().constData());
    qInfo("File Path: %s", QApplication::applicationFilePath().toLatin1().constData());
    qInfo("Arguments: %s", QApplication::arguments().join(' ').toLatin1().constData());

    if (app.isRunning()) {
        // Simply pass on the commandline arguments for now
        QStringList quotedArgs;

        foreach (const QString &arg, QtSingleApplication::arguments()) {
            quotedArgs << "\"" + arg + "\"";
        }

        app.sendMessage(quotedArgs.join(" "), 500);

        return 0;
    }
    else if (app.init()) {
        return app.exec();
    }
    else {
        return 0;
    }
}
