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
#include "LuaExtension.h"
#include "DebugManager.h"
#include "SessionManager.h"
#include "TranslationManager.h"
#include "ApplicationSettings.h"

#include "LuaState.h"
#include "lua.hpp"
#include "LuaBridge.h"

#include "EditorConfigAppDecorator.h"

#include "ILexer.h"
#include "Lexilla.h"

#include <QCommandLineParser>

#include <QDirIterator>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

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

void parseCommandLine(QCommandLineParser &parser, const QStringList &args)
{
    parser.setApplicationDescription("Notepad Next");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("files", "Files to open.");

    parser.addOptions({
        {"translation", "Overrides the system default translation.", "translation"},
        {"reset-settings", "Resets all application settings."},
        {"n", "Places the cursor on the line number for the first file specified", "line number"}
    });

    parser.process(args);
}

static QString toLocalFileName(const QString file)
{
    QUrl fileUrl(file);
    return fileUrl.isValid() && fileUrl.isLocalFile() ? fileUrl.toLocalFile() : file;
}

NotepadNextApplication::NotepadNextApplication(int &argc, char **argv)
    : SingleApplication(argc, argv, true, opts)
{
#ifdef Q_OS_WIN
    // Create a system-wide mutex so the installer can detect if it is running
    CreateMutex(NULL, false, L"NotepadNextMutex");
#endif
    parseCommandLine(parser, arguments());

    DebugManager::manageDebugOutput();
    DebugManager::pauseDebugOutput();
}

bool NotepadNextApplication::init()
{
    qInfo(Q_FUNC_INFO);

    setWindowIcon(QIcon(QStringLiteral(":/icons/NotepadNext.png")));

    settings = new ApplicationSettings(this);

    if (parser.isSet("reset-settings")) {
        QFileInfo original(settings->fileName());
        const QString backup = original.canonicalFilePath() + QStringLiteral(".backup");

        qInfo("Resetting application settings");
        qInfo("Backuping up %s to %s", qUtf8Printable(settings->fileName()), qUtf8Printable(backup));

        QFile::remove(backup);
        QFile::rename(settings->fileName(), backup);

        settings->clear();
    }

    // Translation files are stored as a qresource
    translationManager = new TranslationManager(this, QStringLiteral(":/i18n/"));

    // The command line overrides the settings
    if (!parser.value("translation").isEmpty()) {
        translationManager->loadTranslation(parser.value("translation"));
    }
    else if (!settings->translation().isEmpty()){
        translationManager->loadTranslation(settings->translation());
    }
    else {
        translationManager->loadSystemDefaultTranslation();
    }

    // This connection isn't needed since the application can not appropriately retranslate the UI at runtime
    //connect(settings, &ApplicationSettings::translationChanged, translationManager, &TranslationManager::loadTranslationByName);

    luaState = new LuaState();

    recentFilesListManager = new RecentFilesListManager(this);
    editorManager = new EditorManager(settings, this);
    sessionManager = new SessionManager(this);

    connect(editorManager, &EditorManager::editorCreated, recentFilesListManager, [=](ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->removeFile(editor->getFilePath());
        }
    });

    connect(editorManager, &EditorManager::editorClosed, recentFilesListManager, [=](ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->addFile(editor->getFilePath());
        }
    });

    loadSettings();

    connect(this, &NotepadNextApplication::aboutToQuit, this, &NotepadNextApplication::saveSettings);

    EditorConfigAppDecorator *ecad = new EditorConfigAppDecorator(this);
    ecad->setEnabled(true);

    luaState->executeFile(":/scripts/init.lua");
    LuaExtension::Instance().Initialise(luaState->L, Q_NULLPTR);

    // LuaBridge is not a long term solution
    // This is probably temporary, but it is quick and works
    luabridge::setHideMetatables(false);
    luabridge::getGlobalNamespace(luaState->L)
        .beginNamespace("nn")
            .beginClass<ApplicationSettings>("Settings")
                .addFunction("showMenuBar", &ApplicationSettings::setShowMenuBar)
                .addFunction("showToolBar", &ApplicationSettings::setShowToolBar)
                .addFunction("showTabBar", &ApplicationSettings::setShowTabBar)
                .addFunction("showStatusBar", &ApplicationSettings::setShowStatusBar)
            .endClass()
        .endNamespace();
    luabridge::setGlobal(luaState->L, settings, "settings");

    createNewWindow();
    connect(editorManager, &EditorManager::editorCreated, window, &MainWindow::addEditor);

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
    luabridge::setGlobal(luaState->L, window, "window");

    // If the application is activated (e.g. user switching to another program and them back) the focus
    // needs to be reset on whatever object previously had focus (e.g. the find dialog)
    connect(this, &NotepadNextApplication::focusChanged, this, [&](QWidget *old, QWidget *now) {
        Q_UNUSED(old);
        if (now) {
            currentlyFocusedWidget = now;
        }
    });

    connect(this, &SingleApplication::instanceStarted, window, &MainWindow::bringWindowToForeground);
    connect(this, &SingleApplication::receivedMessage, this, &NotepadNextApplication::receiveInfoFromSecondaryInstance, Qt::QueuedConnection);

    connect(this, &NotepadNextApplication::applicationStateChanged, this, [&](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {

            // Make sure it is active...
            // The application can be active without the main window being show e.g. if there is a
            // message box that pops up before the main window
            if (window->isActiveWindow()) {
                window->focusIn();
            }

            if (!currentlyFocusedWidget.isNull()) {
                currentlyFocusedWidget->activateWindow();
            }
        }
    });

    if (settings->restorePreviousSession()) {
        qInfo("Restoring previous session");

        sessionManager->loadSession(window);
    }

    openFiles(parser.positionalArguments());

    if (parser.isSet("n") && parser.positionalArguments().length() > 0) {
        QString firstFile = parser.positionalArguments()[0];
        ScintillaNext *editor = editorManager->getEditorByFilePath(toLocalFileName(firstFile));

        if (editor) {
            int n = parser.value("n").toInt();
            editor->gotoLine(n - 1);
        }
    }

    // If the window does not have any editors (meaning the no files were
    // specified on the command line) then create a new empty file
    if (window->editorCount() == 0) {
        window->newFile();
    }

    // Everything should be ready at this point

    window->restoreWindowState();
    window->show();

    DebugManager::resumeDebugOutput();

    return true;
}

SessionManager *NotepadNextApplication::getSessionManager() const
{
    SessionManager::SessionFileTypes fileTypes;

    if (settings->restorePreviousSession()) {
        fileTypes |= SessionManager::SavedFile;
    }

    if (settings->restoreUnsavedFiles()) {
        fileTypes |= SessionManager::UnsavedFile;
    }

    if (settings->restoreTempFiles()) {
        fileTypes |= SessionManager::TempFile;
    }

    // Update the file types supported in case something has changed in the settings
    sessionManager->setSessionFileTypes(fileTypes);

    return sessionManager;
}

QString NotepadNextApplication::getFileDialogFilter() const
{
    return getLuaState()->executeAndReturn<QString>("return DialogFilters()");
}

QString NotepadNextApplication::getFileDialogFilterForLanguage(const QString &language) const
{
    return getLuaState()->executeAndReturn<QString>(QString("return FilterForLanguage(\"%1\")").arg(language).toLatin1().constData());
}

QStringList NotepadNextApplication::getLanguages() const
{
    return getLuaState()->executeAndReturn<QStringList>(
                R"(
                local names = {}
                for k in pairs(languages) do table.insert(names, k) end
                table.sort(names, function (a, b) return string.lower(a) < string.lower(b) end)
                return names
                )");
}

void NotepadNextApplication::setEditorLanguage(ScintillaNext *editor, const QString &languageName) const
{
    LuaExtension::Instance().setEditor(editor);

    getLuaState()->execute(QString("languageName = \"%1\"").arg(languageName).toLatin1().constData());
    const QString lexer = getLuaState()->executeAndReturn<QString>("return languages[languageName].lexer");

    editor->languageName = languageName;
    editor->languageSingleLineComment = getLuaState()->executeAndReturn<QString>("return languages[languageName].singleLineComment or \"\"").toUtf8();

    auto lexerInstance = CreateLexer(lexer.toLatin1().constData());
    editor->setILexer((sptr_t) lexerInstance);
    editor->clearDocumentStyle(); // Remove all previous style information, setting the lexer does not guarantee styling information is cleared

    // Not ideal this has to be manually emitted but it works since setILexer() is not widely used
    emit editor->lexerChanged();

    // Dynamic properties can be used to skip part of the default initialization. The value in the
    // property doesn't currently matter, but may be used at a later point.
    getLuaState()->execute(QString("skip_tabs = %1").arg(editor->QObject::property("nn_skip_usetabs").isValid() ? "true" : "false").toLatin1().constData());
    getLuaState()->execute(QString("skip_tabwidth = %1").arg(editor->QObject::property("nn_skip_tabwidth").isValid() ? "true" : "false").toLatin1().constData());

    getLuaState()->execute(QString("SetLanguage(languageName)").toLatin1().constData());
}

QString NotepadNextApplication::detectLanguage(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    QString language_name = QStringLiteral("Text");

    if (editor->isFile()) {
        language_name = detectLanguageFromExtension(editor->getFileInfo().suffix());
    }

    if (language_name == QStringLiteral("Text")) {
        language_name = detectLanguageFromContents(editor);
    }

    return language_name;
}

QString NotepadNextApplication::detectLanguageFromExtension(const QString &extension) const
{
    qInfo(Q_FUNC_INFO);

    return getLuaState()->executeAndReturn<QString>(QString(R"(
    local ext = "%1"
    for name, L in pairs(languages) do
        if L.extensions then
            for _, v in ipairs(L.extensions) do
                if v == ext then
                    return name
                end
            end
        end
    end
    return "Text"
    )").arg(extension).toLatin1().constData());
}

QString NotepadNextApplication::detectLanguageFromContents(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    LuaExtension::Instance().setEditor(editor);

    return getLuaState()->executeAndReturn<QString>(QString(R"(
    -- Grab a small chunk
    if editor.Length > 0 then
        editor:SetTargetRange(0, math.min(64, editor.Length))
        return DetectLanguageFromContents(editor.TargetText)
    end

    return "Text"
    )").toLatin1().constData());
}

void NotepadNextApplication::sendInfoToPrimaryInstance()
{
    qInfo(Q_FUNC_INFO);

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream << arguments();
    const bool success = sendMessage(buffer);

    if (!success) {
        qWarning("sendMessage() unsuccessful");
    }
}

void NotepadNextApplication::receiveInfoFromSecondaryInstance(quint32 instanceId, QByteArray message)
{
    qInfo(Q_FUNC_INFO);

    Q_UNUSED(instanceId)

    QDataStream stream(&message, QIODevice::ReadOnly);
    QStringList args;

    stream >> args;

    QCommandLineParser parser;
    parseCommandLine(parser, args);

    openFiles(parser.positionalArguments());
}

bool NotepadNextApplication::isRunningAsAdmin() const
{
    static bool initialized = false;
    static bool isAdmin = false;

    if (!initialized) {
        initialized = true;

#ifdef Q_OS_WIN
        BOOL isMember;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        PSID administratorsGroupSid = NULL;

        // Create a SID for the Administrators group
        if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroupSid)) {
            if (CheckTokenMembership(NULL, administratorsGroupSid, &isMember)) {
                isAdmin = isMember;
            }
            FreeSid(administratorsGroupSid);
        }
#endif
    }

    return isAdmin;
}

bool NotepadNextApplication::event(QEvent *event)
{
    // Handle the QFileOpenEvent to open files on MacOS X.
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent*>(event);

        qInfo("QFileOpenEvent %s", qUtf8Printable(fileOpenEvent->file()));

        openFiles(QStringList() << fileOpenEvent->file());

        return true;
    }

    return SingleApplication::event(event);
}

void NotepadNextApplication::openFiles(const QStringList &files)
{
    qInfo(Q_FUNC_INFO);

    for (const QString &file : files) {
        window->openFile(toLocalFileName(file));
    }
}

void NotepadNextApplication::loadSettings()
{
    recentFilesListManager->setFileList(getSettings()->value("App/RecentFilesList").toStringList());
}

void NotepadNextApplication::saveSettings()
{
    getSettings()->setValue("App/RecentFilesList", recentFilesListManager->fileList());
}

MainWindow *NotepadNextApplication::createNewWindow()
{
    Q_ASSERT(window == Q_NULLPTR);

    window = new MainWindow(this);

    // Keep Lua's editor reference up to date
    connect(window, &MainWindow::editorActivated, this, [](ScintillaNext *editor) {
        LuaExtension::Instance().setEditor(editor);
    });

    // Since these editors don't actually get "closed" go ahead and add them to the recent file list
    connect(window, &MainWindow::aboutToClose, this, [=]() {
        for (const auto &editor : window->editors()) {
            if (editor->isFile()) {
                recentFilesListManager->addFile(editor->getFilePath());
            }
        }

        getSessionManager()->saveSession(window);
    });

    return window;
}

QStringList NotepadNextApplication::debugInfo() const
{
    QStringList info;

    info.append(QStringLiteral("%1 v%2 %3").arg(applicationDisplayName(), applicationVersion(), APP_DISTRIBUTION));
    info.append(QStringLiteral("Build Date/Time: %1 %2").arg(__DATE__, __TIME__));
    info.append(QStringLiteral("Qt: %1").arg(qVersion()));
    info.append(QStringLiteral("OS: %1").arg(QSysInfo::prettyProductName()));
    info.append(QStringLiteral("Locale: %1").arg(QLocale::system().name()));
    info.append(QStringLiteral("CPU: %1").arg(QSysInfo::currentCpuArchitecture()));
    info.append(QStringLiteral("File Path: %1").arg(applicationFilePath()));
    info.append(QStringLiteral("Arguments: %1").arg(arguments().join(' ')));
    info.append(QStringLiteral("Config File: %1").arg(ApplicationSettings().fileName()));

    return info;
}
