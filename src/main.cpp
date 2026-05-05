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

#ifdef Q_OS_LINUX
    // Qt Advanced Docking System positions its drop-indicator overlays as
    // top-level Qt::Tool windows using mapToGlobal() + move() to absolute
    // coordinates. Wayland does not let clients position top-level windows,
    // so during a tab drag the indicators appear at wrong locations. Run
    // through XWayland where the positioning works correctly. Honor an
    // explicit QT_QPA_PLATFORM and an opt-out escape hatch.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")
        && !qEnvironmentVariableIsSet("NOTEPADNEXT_FORCE_NATIVE_WAYLAND"))
    {
        const QByteArray sessionType = qgetenv("XDG_SESSION_TYPE");
        const bool isWayland = (sessionType == "wayland")
                            || qEnvironmentVariableIsSet("WAYLAND_DISPLAY");
        if (isWayland) {
            qputenv("QT_QPA_PLATFORM", "xcb;wayland");
        }
    }
#endif

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

    NotepadNextApplication app(argc, argv);

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
