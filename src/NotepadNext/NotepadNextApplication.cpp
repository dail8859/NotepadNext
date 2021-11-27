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
#include "RecentFilesListManager.h"
#include "EditorManager.h"

#include "LuaState.h"
#include "lua.hpp"
#include "LuaBridge.h"

#include "EditorConfigAppDecorator.h"

#include <QCommandLineParser>
#include <QSettings>

const SingleApplication::Options opts = SingleApplication::ExcludeAppPath | SingleApplication::ExcludeAppVersion | SingleApplication::SecondaryNotification;

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


NotepadNextApplication::NotepadNextApplication(int &argc, char **argv)
    : SingleApplication(argc, argv, true, opts)
{
    qInfo(Q_FUNC_INFO);

    recentFilesListManager = new RecentFilesListManager(this);
    editorManager = new EditorManager(this);
    settings = new Settings(this);

    connect(editorManager, &EditorManager::editorCreated, [=](ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->removeFile(editor->canonicalFilePath());
        }
    });

    connect(editorManager, &EditorManager::editorClosed, [=](ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->addFile(editor->canonicalFilePath());
        }
    });

    // Restore some settings and schedule saving the settings on quit
    QSettings qsettings;

    recentFilesListManager->setFileList(qsettings.value("App/RecentFilesList").toStringList());

    connect(this, &NotepadNextApplication::aboutToQuit, [=]() {
        QSettings qsettings;
        qsettings.setValue("App/RecentFilesList", recentFilesListManager->fileList());
    });

    EditorConfigAppDecorator *ecad = new EditorConfigAppDecorator(this);
    ecad->setEnabled(true);
}

bool NotepadNextApplication::initGui()
{
    qInfo(Q_FUNC_INFO);

    luaState = new LuaState();
    luaState->executeFile(":/scripts/init.lua");

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

    createNewWindow();
    connect(editorManager, &EditorManager::editorCreated, windows.first(), &MainWindow::addEditor);

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

    QObject::connect(this, &SingleApplication::instanceStarted, this->windows.first(), &MainWindow::bringWindowToForeground);

    QObject::connect(this, &SingleApplication::receivedMessage, [&] (quint32 instanceId, QByteArray message) {
        Q_UNUSED(instanceId)

        QDataStream stream(&message, QIODevice::ReadOnly);
        QStringList args;

        stream >> args;

        applyArguments(args);
    });

    QObject::connect(this, &NotepadNextApplication::applicationStateChanged, [&](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {
            windows.first()->focusIn();

            if (!currentlyFocusedWidget.isNull()) {
                currentlyFocusedWidget->activateWindow();
            }
        }
    });

    applyArguments(SingleApplication::arguments());

    // Everything should be ready at this point

    // If the window does not have any editors (meaning the applyArguments() did not
    // have any files to open) then create a new empty file
    if (windows.first()->getDockedEditor()->count() == 0) {
        windows.first()->newFile();
    }
    windows.first()->show();
    windows.first()->bringWindowToForeground();

    return true;
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
                    filter[#filter + 1] = name .. " Files (" .. table.concat(extensions, " ") .. ")"
                end
                return table.concat(filter, ";;")
                )=");

    return filter;
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
