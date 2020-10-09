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

#include "LuaState.h"
#include "lua.hpp"
#include "LuaBridge.h"

#include <QCommandLineParser>
#include <QSettings>

template <>
struct luabridge::Stack <QString const&>
{
    static void push (lua_State* L, QString const &s)
    {
        lua_pushlstring(L, s.toLatin1().constData(), s.toLatin1().size());
    }

    static QString get (lua_State* L, int index)
    {
        return QString(luaL_checkstring (L, index));
    }
};


NotepadNextApplication::NotepadNextApplication(const QString &id, int &argc, char **argv)
    : QtSingleApplication(id, argc, argv)
{
    qInfo(Q_FUNC_INFO);

    bufferManager = new BufferManager(this);

    luaState = new LuaState();
    luaState->executeFile(":/scripts/init.lua");

    settings = new Settings(this);

    // LuaBridge is not a long term solution
    // This is probably temporary, but it is quick and works
    luabridge::setHideMetatables(false);
    luabridge::getGlobalNamespace(luaState->L)
        .beginNamespace("nn")
            .beginClass<Settings>("Settings")
                .addFunction("showMenuBar", &Settings::setShowMenuBar)
                .addFunction("showToolBar", &Settings::setShowToolBar)
                .addFunction("showTabBar", &Settings::setShowTabBar)
                .addFunction("showStatusBar", &Settings::setShowStatusBar)
            .endClass()
        .endNamespace();
    luabridge::setGlobal(luaState->L, settings, "settings");
}

bool NotepadNextApplication::initGui()
{
    qInfo(Q_FUNC_INFO);

    luabridge::getGlobalNamespace(luaState->L)
        .beginNamespace("nn")
            .beginClass<QWidget>("QWidget")
                .addFunction("exit", &QWidget::close)
            .endClass()
            .deriveClass<MainWindow, QWidget>("MainWindow")
                .addFunction("newFile", &MainWindow::newFile)
                .addFunction("openFile", &MainWindow::openFile)
                .addFunction("openFileDialog", &MainWindow::openFileDialog)
                .addFunction("reloadFile", &MainWindow::reloadFile)
                .addFunction("saveFile", &MainWindow::saveCurrentFile)
                .addFunction("saveFileAs", &MainWindow::saveCurrentFileAs)
                .addFunction("closeFile", &MainWindow::closeCurrentFile)
            .endClass()
        .endNamespace();
    luabridge::setGlobal(luaState->L, windows.first(), "window");

    // If the application is activated (e.g. user switching to another program and them back) the focus
    // needs to be reset on whatever object previously had focus (e.g. the find dialog)
    QObject::connect(this, &NotepadNextApplication::focusChanged, [&] (QWidget *old, QWidget *now) {
        Q_UNUSED(old);
        if (now) {
            currentlyFocusedWidget = now;
        }
    });

    QObject::connect(this, &NotepadNextApplication::messageReceived, [&](const QString &message) {
        QStringList args = message.split("\" \"");
        args.first().remove(0,1); // remove beginning quote from first arg
        args.last().chop(1); // remove trailing quote from last arg
        applyArguments(args);

        // Received a message from another NN instance, so bring the existing one to the foreground
        windows.first()->bringWindowToForeground();
    });

    QObject::connect(this, &NotepadNextApplication::applicationStateChanged, [&](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {
            windows.first()->focusIn();

            if (currentlyFocusedWidget) {
                currentlyFocusedWidget->activateWindow();
            }
        }
    });

    applyArguments(QtSingleApplication::arguments());

    // Everything should be ready, so do it!
    windows.first()->show();
    windows.first()->bringWindowToForeground();

    return true;
}

LuaState *NotepadNextApplication::getLuaState() const
{
    return luaState;
}

QString NotepadNextApplication::getFileDialogFilter() const
{
    // TODO: cache the result?

    QString filter = getLuaState()->executeAndReturn<QString>(
                R"=(
                local filter = {"All files (*)"}
                for name, L in pairs(languages) do
                    local extensions = {}
                    for _, ext in ipairs(L.extensions) do
                        extensions[#extensions + 1] = "*." .. ext
                    end
                    filter[#filter + 1] = L.name .. " Files (" .. table.concat(extensions, " ") .. ")"
                end
                return table.concat(filter, ";;")
                )=");

    return filter;
}

Settings *NotepadNextApplication::getSettings() const
{
    return settings;
}

void NotepadNextApplication::applyArguments(const QStringList &args)
{
    qInfo(Q_FUNC_INFO);

    QCommandLineParser parser;
    parser.setApplicationDescription("Notepad Next");
    parser.addHelpOption();
    parser.addVersionOption();

    // TODO: add more options
    parser.addPositionalArgument("files", "The files to open.");

    parser.process(args);

    foreach (const QString &file, parser.positionalArguments()) {
        windows.first()->openFile(file);
    }
}

MainWindow *NotepadNextApplication::createNewWindow()
{
    MainWindow *w = new MainWindow(this);

    windows.append(w);

    return w;
}
