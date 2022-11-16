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
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QClipboard>
#include <QSettings>
#include <QStandardPaths>
#include <QWindow>
#include <QPushButton>
#include <QTimer>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>

#ifdef Q_OS_WIN
#include <QSimpleUpdater.h>
#include <Windows.h>
#endif

#include "DockAreaWidget.h"

#include "NotepadNextApplication.h"
#include "Settings.h"

#include "ScintillaNext.h"

#include "RecentFilesListManager.h"
#include "RecentFilesListMenuBuilder.h"
#include "EditorManager.h"

#include "LuaConsoleDock.h"
#include "LanguageInspectorDock.h"
#include "EditorInspectorDock.h"
#include "FolderAsWorkspaceDock.h"
#include "SearchResultsDock.h"
#include "DebugLogDock.h"
#include "HexViewerDock.h"

#include "FindReplaceDialog.h"
#include "MacroRunDialog.h"
#include "MacroSaveDialog.h"
#include "PreferencesDialog.h"

#include "QuickFindWidget.h"

#include "EditorPrintPreviewRenderer.h"
#include "MacroEditorDialog.h"

#include "ZoomEventWatcher.h"
#include "FileDialogHelpers.h"


MainWindow::MainWindow(NotepadNextApplication *app) :
    ui(new Ui::MainWindow),
    app(app),
    zoomEventWatcher(new ZoomEventWatcher(this))
{
    qInfo(Q_FUNC_INFO);

    ui->setupUi(this);

    qInfo("setupUi Completed");

    connect(this, &MainWindow::aboutToClose, this, &MainWindow::saveSettings);

    // Create and set up the connections to the docked editor
    dockedEditor = new DockedEditor(this);
    connect(dockedEditor, &DockedEditor::editorCloseRequested, this, [=](ScintillaNext *editor) { closeFile(editor); });
    connect(dockedEditor, &DockedEditor::editorActivated, this, &MainWindow::activateEditor);
    connect(dockedEditor, &DockedEditor::contextMenuRequestedForEditor, this, &MainWindow::tabBarRightClicked);
    connect(dockedEditor, &DockedEditor::titleBarDoubleClicked, this, &MainWindow::newFile);

    // Set up the menus
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::reloadFile);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeCurrentFile);
    connect(ui->actionCloseAll, &QAction::triggered, this, &MainWindow::closeAllFiles);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionOpenFolderasWorkspace, &QAction::triggered, this, &MainWindow::openFolderAsWorkspaceDialog);

    connect(ui->actionCloseAllExceptActive, &QAction::triggered, this, &MainWindow::closeAllExceptActive);
    connect(ui->actionCloseAllToLeft, &QAction::triggered, this, &MainWindow::closeAllToLeft);
    connect(ui->actionCloseAllToRight, &QAction::triggered, this, &MainWindow::closeAllToRight);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveCurrentFileAsDialog);
    connect(ui->actionSaveCopyAs, &QAction::triggered, this, &MainWindow::saveCopyAsDialog);
    connect(ui->actionSaveAll, &QAction::triggered, this, &MainWindow::saveAll);
    connect(ui->actionRename, &QAction::triggered, this, &MainWindow::renameFile);

    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);

    connect(ui->actionToggleSingleLineComment, &QAction::triggered, this, [=]() { currentEditor()->toggleCommentSelection(); });
    connect(ui->actionSingleLineComment, &QAction::triggered, this, [=]() { currentEditor()->commentLineSelection(); });
    connect(ui->actionSingleLineUncomment, &QAction::triggered, this, [=]() { currentEditor()->uncommentLineSelection(); });

    connect(ui->actionClearRecentFilesList, &QAction::triggered, app->getRecentFilesListManager(), &RecentFilesListManager::clear);

    connect(ui->actionMoveToTrash, &QAction::triggered, this, &MainWindow::moveCurrentFileToTrash);

    RecentFilesListMenuBuilder *recentFileListMenuBuilder = new RecentFilesListMenuBuilder(app->getRecentFilesListManager());
    connect(ui->menuRecentFiles, &QMenu::aboutToShow, this, [=]() {
        // NOTE: its unfortunate that this has to be hard coded, but there's no way
        // to easily determine what should or shouldn't be there
        while (ui->menuRecentFiles->actions().size() > 4) {
            delete ui->menuRecentFiles->actions().takeLast();
        }

        recentFileListMenuBuilder->populateMenu(ui->menuRecentFiles);
    });

    connect(ui->actionRestoreRecentlyClosedFile, &QAction::triggered, this, [=]() {
        if (app->getRecentFilesListManager()->count() > 0) {
            openFileList(QStringList() << app->getRecentFilesListManager()->mostRecentFile());
        }
    });

    connect(ui->actionOpenAllRecentFiles, &QAction::triggered, this, [=]() {
        openFileList(app->getRecentFilesListManager()->fileList());
    });

    connect(recentFileListMenuBuilder, &RecentFilesListMenuBuilder::fileOpenRequest, this, &MainWindow::openFile);

    QActionGroup *eolActionGroup = new QActionGroup(this);
    eolActionGroup->addAction(ui->actionWindows);
    eolActionGroup->addAction(ui->actionUnix);
    eolActionGroup->addAction(ui->actionMacintosh);

    connect(ui->actionWindows, &QAction::triggered, this, [=]() { convertEOLs(SC_EOL_CRLF); });
    connect(ui->actionUnix, &QAction::triggered, this, [=]() { convertEOLs(SC_EOL_LF); });
    connect(ui->actionMacintosh, &QAction::triggered, this, [=]() { convertEOLs(SC_EOL_CR); });

    connect(ui->actionUpperCase, &QAction::triggered, this, [=]() { currentEditor()->upperCase(); });
    connect(ui->actionLowerCase, &QAction::triggered, this, [=]() { currentEditor()->lowerCase(); });

    connect(ui->actionDuplicateCurrentLine, &QAction::triggered, this, [=]() { currentEditor()->lineDuplicate(); });
    connect(ui->actionMoveSelectedLinesUp, &QAction::triggered, this, [=]() { currentEditor()->moveSelectedLinesUp(); });
    connect(ui->actionMoveSelectedLinesDown, &QAction::triggered, this, [=]() { currentEditor()->moveSelectedLinesDown(); });
    connect(ui->actionSplitLines, &QAction::triggered, this, [=]() {
        currentEditor()->targetFromSelection();
        currentEditor()->linesSplit(0);
    });
    connect(ui->actionJoinLines, &QAction::triggered, this, [=]()  {
        currentEditor()->targetFromSelection();
        currentEditor()->linesJoin();
    });

    connect(ui->actionUndo, &QAction::triggered, this, [=]() { currentEditor()->undo(); });
    connect(ui->actionRedo, &QAction::triggered, this, [=]() { currentEditor()->redo(); });
    connect(ui->actionCut, &QAction::triggered, this, [=]() {
        if (currentEditor()->selectionEmpty()) {
            currentEditor()->copyAllowLine();
            currentEditor()->lineDelete();
        }
        else {
            currentEditor()->cut();
        }
    });
    connect(ui->actionCopy, &QAction::triggered, this, [=]() {
        currentEditor()->copyAllowLine();
    });
    connect(ui->actionDelete, &QAction::triggered, this, [=]() { currentEditor()->clear();});
    connect(ui->actionPaste, &QAction::triggered, this, [=]() { currentEditor()->paste();});
    connect(ui->actionSelect_All, &QAction::triggered, this, [=]() { currentEditor()->selectAll();});
    connect(ui->actionSelect_Next, &QAction::triggered, this, [=]() {
        // Set search flags here?
        currentEditor()->targetWholeDocument();
        currentEditor()->multipleSelectAddNext();
    });
    connect(ui->actionCopyFullPath, &QAction::triggered, this, [=]() {
        auto editor = currentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->getFilePath());
        }
    });
    connect(ui->actionCopyFileName, &QAction::triggered, this, [=]() {
        QApplication::clipboard()->setText(currentEditor()->getName());
    });
    connect(ui->actionCopyFileDirectory, &QAction::triggered, this, [=]() {
        auto editor = currentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->getPath());
        }
    });
    connect(ui->actionIncrease_Indent, &QAction::triggered, this, [=]() { currentEditor()->tab(); });
    connect(ui->actionDecrease_Indent, &QAction::triggered, this, [=]() { currentEditor()->backTab(); });

    SearchResultsDock *srDock = new SearchResultsDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, srDock);
    srDock->toggleViewAction()->setShortcut(Qt::Key_F7);
    ui->menuView->addAction(srDock->toggleViewAction());

    connect(srDock, &SearchResultsDock::searchResultActivated, this, [=](ScintillaNext *editor, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning) {
        dockedEditor->switchToEditor(editor);

        int linePos = editor->positionFromLine(lineNumber);
        editor->goToRange({linePos + startPositionFromBeginning, linePos + endPositionFromBeginning});
        editor->verticalCentreCaret();

        editor->grabFocus();
    });

    connect(ui->actionFind, &QAction::triggered, this, [=]() {
        showFindReplaceDialog(FindReplaceDialog::FIND_TAB);
    });

    connect(ui->actionFindNext, &QAction::triggered, this, [=]() {
        if (dialogs.contains("FindReplaceDialog")) {
            qobject_cast<FindReplaceDialog *>(dialogs["FindReplaceDialog"])->performLastSearch();
        }
    });

    connect(ui->actionQuickFind, &QAction::triggered, this, [=]() {
        if (quickFind == Q_NULLPTR) {
            quickFind = new QuickFindWidget(this);
        }
        quickFind->setEditor(currentEditor());
        quickFind->setFocus();

        quickFind->show();
    });

    connect(ui->actionReplace, &QAction::triggered, this, [=]() {
        showFindReplaceDialog(FindReplaceDialog::REPLACE_TAB);
    });

    connect(ui->actionGoToLine, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        const int currentLine = editor->lineFromPosition(editor->currentPos()) + 1;
        const int maxLine = editor->lineCount();
        bool ok;

        QInputDialog d = QInputDialog(this);
        Qt::WindowFlags flags = d.windowFlags() & ~Qt::WindowContextHelpButtonHint;
        int lineToGoTo = d.getInt(this, tr("Go to line"), tr("Line Number (1 - %1)").arg(maxLine), currentLine, 1, maxLine, 1, &ok, flags);

        if (ok) {
            editor->ensureVisible(lineToGoTo - 1);
            editor->gotoLine(lineToGoTo - 1);
            editor->verticalCentreCaret();
        }
    });
    ui->pushExitFullScreen->setParent(this); // This is important
    ui->pushExitFullScreen->setVisible(false);
    connect(ui->pushExitFullScreen, &QPushButton::clicked, ui->actionFullScreen, &QAction::trigger);
    connect(ui->actionFullScreen, &QAction::triggered, this, [=](bool b) {
        if (b) {
            // NOTE: don't hide() these as it will cancel their actions they hold
            ui->menuBar->setMaximumHeight(0);
            ui->mainToolBar->setMaximumHeight(0);

            showFullScreen();
            ui->pushExitFullScreen->setGeometry(width() - 20, 0, 20, 20);
            ui->pushExitFullScreen->show();
            ui->pushExitFullScreen->raise();
        }
        else {
            ui->menuBar->setMaximumHeight(QWIDGETSIZE_MAX);
            ui->mainToolBar->setMaximumHeight(QWIDGETSIZE_MAX);
            showNormal();

            ui->pushExitFullScreen->hide();
        }
    });

    connect(ui->actionShowAllCharacters, &QAction::triggered, this, [=](bool b) {
        ui->actionShowWhitespace->setChecked(b);
        ui->actionShowEndofLine->setChecked(b);
    });

    connect(ui->actionShowWhitespace, &QAction::toggled, this, [=](bool b) {
        // TODO: could make SCWS_VISIBLEALWAYS configurable via settings. Probably not worth
        // taking up menu space e.g. show all, show leading, show trailing
        for (auto &editor : editors()) {
            editor->setViewWS(b ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
        }

        ui->actionShowAllCharacters->setChecked(b && ui->actionShowEndofLine->isChecked());
    });

    connect(ui->actionShowEndofLine, &QAction::toggled, this, [=](bool b) {
        for (auto &editor : editors()) {
            editor->setViewEOL(b);
        }

        ui->actionShowAllCharacters->setChecked(b && ui->actionShowWhitespace->isChecked());
    });

    connect(ui->actionShowWrapSymbol, &QAction::triggered, this, [=](bool b) {
        for (auto &editor : editors()) {
            editor->setWrapVisualFlags(b ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
        }
    });

    connect(ui->actionShowIndentGuide, &QAction::triggered, this, [=](bool b) {
        currentEditor()->setIndentationGuides(b ? SC_IV_LOOKBOTH : SC_IV_NONE);
    });
    ui->actionShowIndentGuide->setChecked(true);

    connect(ui->actionWordWrap, &QAction::triggered, this, [=](bool b) {
        if (b) {
            for (auto &editor : editors()) {
                editor->setWrapMode(SC_WRAP_WORD);
            }
        }
        else {
            for (auto &editor : editors()) {
                // Store the top line and restore it after the lines have been unwrapped
                int topLine = editor->docLineFromVisible(editor->firstVisibleLine());
                editor->setWrapMode(SC_WRAP_NONE);
                editor->setFirstVisibleLine(topLine);
            }
        }
    });

    // Zooming controls all editors simulaneously
    connect(ui->actionZoomIn, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->zoomIn();
        }
        zoomLevel = currentEditor()->zoom();
    });
    connect(ui->actionZoomOut, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->zoomOut();
        }
        zoomLevel = currentEditor()->zoom();
    });
    connect(ui->actionZoomReset, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->setZoom(0);
        }
        zoomLevel = 0;
    });

    // Zoom watcher has detected a zoom event, so just trigger the UI action
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomIn, ui->actionZoomIn, &QAction::trigger);
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomOut, ui->actionZoomOut, &QAction::trigger);

    languageActionGroup = new QActionGroup(this);
    languageActionGroup->setExclusive(true);

    connect(ui->actionPreferences, &QAction::triggered, this, [=] {
        PreferencesDialog *pd = nullptr;

        if (!dialogs.contains("PreferencesDialog")) {
            pd = new PreferencesDialog(app->getSettings(), this);
            dialogs["PreferencesDialog"] = pd;
        }
        else {
            pd = qobject_cast<PreferencesDialog *>(dialogs["PreferencesDialog"]);
        }

        pd->show();
        pd->raise();
        pd->activateWindow();
    });

    // The macro manager has already loaded any saved macros, so it might have some already
    ui->actionRunMacroMultipleTimes->setEnabled(macroManager.availableMacros().size() > 0);
    ui->actionEditMacros->setEnabled(macroManager.availableMacros().size() > 0);

    connect(ui->actionMacroRecording, &QAction::triggered, this, [=](bool b) {
        if (b) {
            macroManager.startRecording(currentEditor());
        }
        else {
            macroManager.stopRecording();
        }
    });

    connect(&macroManager, &MacroManager::recordingStarted, this, [=]() {
        ui->actionMacroRecording->setText(tr("Stop Recording"));

        // A macro is being recorded so disable some macro options
        ui->actionPlayback->setEnabled(false);
        ui->actionRunMacroMultipleTimes->setEnabled(false);
        ui->actionSaveCurrentRecordedMacro->setEnabled(false);
    });

    connect(&macroManager, &MacroManager::recordingStopped, this, [=]() {
        ui->actionMacroRecording->setText(tr("Start Recording"));

        // Only enable these if the macro manager recorded a valid macro
        ui->actionPlayback->setEnabled(macroManager.hasCurrentUnsavedMacro());
        ui->actionSaveCurrentRecordedMacro->setEnabled(macroManager.hasCurrentUnsavedMacro());

        // The macro manager might have other macros
        ui->actionRunMacroMultipleTimes->setEnabled(macroManager.availableMacros().size() > 0 || macroManager.hasCurrentUnsavedMacro());
    });

    connect(ui->actionPlayback, &QAction::triggered, this, [=]() {
        macroManager.replayCurrentMacro(currentEditor());
    });

    connect(ui->actionSaveCurrentRecordedMacro, &QAction::triggered, this, [=]() {
        MacroSaveDialog macroSaveDialog;

        macroSaveDialog.show();
        macroSaveDialog.raise();
        macroSaveDialog.activateWindow();

        if (macroSaveDialog.exec() == QDialog::Accepted) {
            // We have at least 1 saved macro at this point
            ui->actionEditMacros->setEnabled(true);

            // The macro has been saved so disable save option
            ui->actionSaveCurrentRecordedMacro->setEnabled(false);

            // TODO: does the macro name already exist? Make the user retry

            macroManager.saveCurrentMacro(macroSaveDialog.getName());

            // TODO handle shortcuts
            if (!macroSaveDialog.getShortcut().isEmpty()) {
                // do something with msd.getShortcut().isEmpty()
            }
        }
    });

    connect(ui->actionRunMacroMultipleTimes, &QAction::triggered, this, [=]() {
        MacroRunDialog *macroRunDialog = nullptr;

        if (!dialogs.contains("MacroRunDialog")) {
            macroRunDialog = new MacroRunDialog(this, &macroManager);
            dialogs["MacroRunDialog"] = macroRunDialog;

            connect(macroRunDialog, &MacroRunDialog::execute, this, [=](Macro *macro, int times) {
                if (times > 0)
                    macro->replay(currentEditor(), times);
                else if (times == -1)
                    macro->replayTillEndOfFile(currentEditor());
            });
        }
        else {
            macroRunDialog = qobject_cast<MacroRunDialog *>(dialogs["MacroRunDialog"]);
        }

        macroRunDialog->show();
        macroRunDialog->raise();
        macroRunDialog->activateWindow();
    });

    connect(ui->actionEditMacros, &QAction::triggered, this, [=]() {
        MacroEditorDialog med(this, &macroManager);

        med.show();
        med.raise();
        med.activateWindow();

        med.exec();

        ui->actionEditMacros->setEnabled(macroManager.availableMacros().size() > 0);
    });

    connect(ui->menuMacro, &QMenu::aboutToShow, this, [=]() {
        // NOTE: its unfortunate that this has to be hard coded, but there's no way
        // to easily determine what should or shouldn't be there
        while (ui->menuMacro->actions().size() > 6) {
            delete ui->menuMacro->actions().takeLast();
        }

        for (const Macro *m : macroManager.availableMacros()) {
            ui->menuMacro->addAction(m->getName(), [=]() { m->replay(currentEditor()); });
        }
    });

    ui->actionAboutQt->setIcon(QPixmap(QLatin1String(":/qt-project.org/qmessagebox/images/qtlogo-64.png")));
    connect(ui->actionAboutQt, &QAction::triggered, &QApplication::aboutQt);

    ui->actionAboutNotepadNext->setShortcut(QKeySequence::HelpContents);
    connect(ui->actionAboutNotepadNext, &QAction::triggered, this, [=]() {
        QMessageBox::about(this, QString(),
                            QStringLiteral("<h3>%1 v%2%3</h3>"
                                    "<p>%4</p>"
                                    "<p>This program does stuff.</p>"
                                    R"(<p>This program is free software: you can redistribute it and/or modify
                                    it under the terms of the GNU General Public License as published by
                                    the Free Software Foundation, either version 3 of the License, or
                                    (at your option) any later version.</p>
                                    <p>This program is distributed in the hope that it will be useful,
                                    but WITHOUT ANY WARRANTY; without even the implied warranty of
                                    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                                    GNU General Public License for more details.</p>
                                    <p>You should have received a copy of the GNU General Public License
                                    along with this program. If not, see &lt;https://www.gnu.org/licenses/&gt;.</p>)")
                                .arg(QApplication::applicationDisplayName(), APP_VERSION, APP_DISTRIBUTION, QStringLiteral(APP_COPYRIGHT).toHtmlEscaped()));
    });

    EditorInspectorDock *editorInspectorDock = new EditorInspectorDock(this);
    editorInspectorDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, editorInspectorDock);

    LanguageInspectorDock *languageInspectorDock = new LanguageInspectorDock(this);
    languageInspectorDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, languageInspectorDock);

    LuaConsoleDock *luaConsoleDock = new LuaConsoleDock(app->getLuaState(), this);
    luaConsoleDock->hide();
    addDockWidget(Qt::BottomDockWidgetArea, luaConsoleDock);

    DebugLogDock *debugLogDock = new DebugLogDock(this);
    debugLogDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, debugLogDock);

    HexViewerDock *hexViewerDock = new HexViewerDock(this);
    hexViewerDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, hexViewerDock);

    ui->menuHelp->insertActions(ui->menuHelp->actions().at(0), {
                                    luaConsoleDock->toggleViewAction(),
                                    languageInspectorDock->toggleViewAction(),
                                    editorInspectorDock->toggleViewAction(),
                                    debugLogDock->toggleViewAction(),
                                    hexViewerDock->toggleViewAction()
                                });

    FolderAsWorkspaceDock *fawDock = new FolderAsWorkspaceDock(this);
    fawDock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, fawDock);
    ui->menuView->addAction(fawDock->toggleViewAction());
    connect(fawDock, &FolderAsWorkspaceDock::fileDoubleClicked, this, &MainWindow::openFile);

    connect(app->getSettings(), &Settings::showMenuBarChanged, this, [=](bool showMenuBar) {
        // Don't 'hide' it, else the actions won't be enabled
        ui->menuBar->setMaximumHeight(showMenuBar ? QWIDGETSIZE_MAX : 0);
    });
    connect(app->getSettings(), &Settings::showToolBarChanged, ui->mainToolBar, &QToolBar::setVisible);
    //connect(app->getSettings(), &Settings::showTabBarChanged, tabbedEditor->getTabBar(), &QTabBar::setVisible);
    connect(app->getSettings(), &Settings::showStatusBarChanged, ui->statusBar, &QStatusBar::setVisible);
    //connect(settings, &Settings::tabsClosableChanged, tabbedEditor->getTabBar(), &QTabBar::setTabsClosable);

    setupLanguageMenu();

    // Put the style sheet here for now
    QFile f(":/stylesheets/npp.css");
    f.open(QFile::ReadOnly);
    setStyleSheet(f.readAll());
    f.close();

    restoreSettings();

    initUpdateCheck();
}

MainWindow::~MainWindow()
{
    Q_ASSERT(dockedEditor->count() == 0);
}

void MainWindow::setupLanguageMenu()
{
    qInfo(Q_FUNC_INFO);

    QStringList language_names = app->getLanguages();

    int i = 0;
    while (i < language_names.size()) {
        QList<QAction *> actions;
        int j = i;

        // Get all consecutive names that start with the same letter
        // NOTE: this loop always runs once since i == j the first time
        while (j < language_names.size() && language_names[i][0].toUpper() == language_names[j][0].toUpper()) {
            const QString key = language_names[j];
            QAction *action = new QAction(key);
            action->setCheckable(true);
            action->setData(key);
            connect(action, &QAction::triggered, this, &MainWindow::languageMenuTriggered);
            languageActionGroup->addAction(action);
            actions.append(action);

            ++j;
        }

        if (actions.size() == 1) {
            ui->menuLanguage->addActions(actions);
        }
        else {
            // Create a sub menu with the actions
            QMenu *compactMenu = new QMenu(actions[0]->text().at(0).toUpper());
            compactMenu->addActions(actions);
            ui->menuLanguage->addMenu(compactMenu);
        }
        i = j;
    }
}

ScintillaNext *MainWindow::currentEditor() const
{
    return dockedEditor->getCurrentEditor();
}

int MainWindow::editorCount() const
{
    return dockedEditor->count();
}

QVector<ScintillaNext *> MainWindow::editors() const
{
    return dockedEditor->editors();
}

void MainWindow::newFile()
{
    qInfo(Q_FUNC_INFO);

    static int count = 1;

    app->getEditorManager()->createEmptyEditor(tr("New %1").arg(count++));
}

// One unedited, new blank document
bool MainWindow::isInInitialState()
{
    if (editorCount() == 1) {
        ScintillaNext *editor = currentEditor();
        return !editor->isFile() && editor->isSavedToDisk();
    }

    return false;
}

void MainWindow::openFileList(const QStringList &fileNames)
{
    qInfo(Q_FUNC_INFO);

    if (fileNames.size() == 0)
        return;

    bool wasInitialState = isInInitialState();
    const ScintillaNext *mostRecentEditor = Q_NULLPTR;

    for (const QString &filePath : fileNames) {
        qInfo("%s", qUtf8Printable(filePath));

        // Search currently open editors to see if it is already open
        ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);

        if (editor == Q_NULLPTR) {
            QFileInfo fileInfo(filePath);

            if (!fileInfo.isFile()) {
                auto reply = QMessageBox::question(this, tr("Create File"), tr("<b>%1</b> does not exist. Do you want to create it?").arg(filePath));

                if (reply == QMessageBox::Yes) {
                    editor = app->getEditorManager()->createEditorFromFile(filePath, true);
                }
                else {
                    // Make sure it is not still in the recent files list still.
                    // Normally when a file is opened it is removed from the file list,
                    // but if a user doesn't want to create the file, remove it explicitly.
                    app->getRecentFilesListManager()->removeFile(filePath);
                    continue;
                }
            }
            else {
                editor = app->getEditorManager()->createEditorFromFile(filePath);
            }
        }

        mostRecentEditor = editor;
    }

    // If any were successful, switch to the last one
    if (mostRecentEditor) {
        dockedEditor->switchToEditor(mostRecentEditor);
    }

    // TODO: reevaluate this now that the MainWindows doesn't deal with buffers any more
    /* This code breaks things on start up. During initial launch of the application, if a file is
     * specified via the command line, then the default new file would be closed. But if a buffer is closed
     * then the DockedEditor doesn't know about it, since focusedDockWidgetChanged is not emitted
     * leaving the currentEditor pointer pointing to a widget that is set to be deleted. Then during focusIn()
     * it needs the currentEditor pointer to check if the document has been modified
     *
     * if (wasInitialState) {
     *     QVector<ScintillaBuffer *> buffers = dockedEditor->buffers();
     *     ScintillaBuffer *bufferToClose = buffers.first();
     *     app->getBufferManager()->closeBuffer(bufferToClose);
     * }
    */
}

bool MainWindow::checkEditorsBeforeClose(const QVector<ScintillaNext *> &editors)
{
    for (ScintillaNext *editor : editors) {
        if (!editor->isSavedToDisk()) {
            // Switch to it
            dockedEditor->switchToEditor(editor);

            // Ask the user what to do
            QString message = tr("Save file <b>%1</b>?").arg(editor->getName());
            auto reply = QMessageBox::question(this, tr("Save File"), message, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

            if (reply == QMessageBox::Cancel) {
                // Stop checking and let the caller know
                return false;
            }
            else if (reply == QMessageBox::Save) {
                bool didFileGetSaved = saveFile(editor);

                // The user might have canceled the save file dialog so just stop now
                if (didFileGetSaved == false) {
                    // Stop checking and let the caller know
                    return false;
                }
            }
        }
    }

    // Everything is fine
    return true;
}

void MainWindow::openFileDialog()
{
    QString dialogDir;
    const QString filter = app->getFileDialogFilter();
    const ScintillaNext *editor = currentEditor();

    // Use the path if possible
    if (editor->isFile()) {
        dialogDir = editor->getPath();
    }

    QStringList fileNames = FileDialogHelpers::getOpenFileNames(this, QString(), dialogDir, filter);

    openFileList(fileNames);
}

void MainWindow::openFile(const QString &filePath)
{
    openFileList(QStringList() << filePath);
}

void MainWindow::openFolderAsWorkspaceDialog()
{
    QString dialogDir;
    const ScintillaNext *editor = currentEditor();

    // Use the path if possible
    if (editor->isFile()) {
        dialogDir = editor->getPath();
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Folder as Workspace"), dialogDir, QFileDialog::ShowDirsOnly);

    if (!dir.isEmpty()) {
        FolderAsWorkspaceDock *fawDock = findChild<FolderAsWorkspaceDock *>();
        fawDock->setRootPath(dir);
        fawDock->setVisible(true);
    }
}

void MainWindow::reloadFile()
{
    auto editor = currentEditor();

    if (!editor->isFile() && !editor->isSavedToDisk()) {
        return;
    }

    const QString filePath = editor->getFilePath();
    auto reply = QMessageBox::question(this, tr("Reload File"), tr("Are you sure you want to reload <b>%1</b>? Any unsaved changes will be lost.").arg(filePath));

    if (reply == QMessageBox::Yes) {
        editor->reload();
    }
}

void MainWindow::closeCurrentFile()
{
    closeFile(currentEditor());
}

void MainWindow::closeFile(ScintillaNext *editor)
{
    if (isInInitialState()) {
        // Don't close the last file
        return;
    }

    if(editor->isSavedToDisk()) {
        editor->close();
    }
    else {
        // The user needs be asked what to do about this file, so switch to it
        dockedEditor->switchToEditor(editor);

        QString message = tr("Save file <b>%1</b>?").arg(editor->getName());
        auto reply = QMessageBox::question(this, tr("Save File"), message, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if (reply == QMessageBox::Cancel) {
            return;
        }

        if (reply == QMessageBox::Save) {
            bool didFileGetSaved = saveFile(editor);

            // The user might have canceled the save file dialog so just stop now
            if (didFileGetSaved == false)
                return;
        }

        editor->close();
    }

    // If the last document was closed, start with a new one
    if (editorCount() == 0) {
        newFile();
    }
}

void MainWindow::closeAllFiles(bool forceClose = false)
{
    if (!forceClose) {
        if (!checkEditorsBeforeClose(editors())) {
            return;
        }
    }

    // Ask the manager to close the editors the dockedEditor knows about
    for (ScintillaNext *editor : editors()) {
        editor->close();
    }

    if (!forceClose)
        newFile();
}

void MainWindow::closeAllExceptActive()
{
    auto e = currentEditor();
    auto editor_list = editors();

    editor_list.removeOne(e);

    if (checkEditorsBeforeClose(editor_list)) {
        for (ScintillaNext *editor : editor_list) {
            editor->close();
        }
    }
}

void MainWindow::closeAllToLeft()
{
    const int index = dockedEditor->currentDockArea()->currentIndex();
    QVector<ScintillaNext *> editors;

    for (int i = 0; i < index; ++i) {
        auto editor = qobject_cast<ScintillaNext *>(dockedEditor->currentDockArea()->dockWidget(i)->widget());
        editors.append(editor);
    }

    if (checkEditorsBeforeClose(editors)) {
        for (ScintillaNext *editor : editors) {
            editor->close();
        }
    }
}

void MainWindow::closeAllToRight()
{
    const int index = dockedEditor->currentDockArea()->currentIndex();
    const int total = dockedEditor->currentDockArea()->dockWidgetsCount();
    QVector<ScintillaNext *> editors;

    for (int i = index + 1; i < total; ++i) {
        auto editor = qobject_cast<ScintillaNext *>(dockedEditor->currentDockArea()->dockWidget(i)->widget());
        editors.append(editor);
    }

    if (checkEditorsBeforeClose(editors)) {
        for (ScintillaNext *editor : editors) {
            editor->close();
        }
    }
}

bool MainWindow::saveCurrentFile()
{
    return saveFile(currentEditor());
}

bool MainWindow::saveFile(ScintillaNext *editor)
{
    if (!editor->modify())
        return true;

    if (!editor->isFile()) {
        // Switch to the editor and show the saveas dialog
        dockedEditor->switchToEditor(editor);
        return saveCurrentFileAsDialog();
    }
    else {
        bool didItGetSaved = editor->save();
        if (didItGetSaved) {
            return true;
        }
        else {
            QMessageBox::warning(this, tr("Error Saving File"),  tr("Something went wrong saving <b>%1</b>?").arg(editor->getName()));
        }
    }

    return false;
}

bool MainWindow::saveCurrentFileAsDialog()
{
    QString dialogDir;
    const QString filter = app->getFileDialogFilter();
    ScintillaNext *editor = currentEditor();

    // Use the file path if possible
    if (editor->isFile()) {
        dialogDir = editor->getFilePath();
    }

    QString fileName = FileDialogHelpers::getSaveFileName(this, QString(), dialogDir, filter);

    if (fileName.size() == 0) {
        return false;
    }

    // TODO: distinguish between the above case (i.e. the user cancels the dialog) and a failure
    // calling editor->saveAs() as it might fail.

    return saveFileAs(editor, fileName);
}

bool MainWindow::saveCurrentFileAs(const QString &fileName)
{
    return saveFileAs(currentEditor(), fileName);
}

bool MainWindow::saveFileAs(ScintillaNext *editor, const QString &fileName)
{
    qInfo("saveFileAs(%s)", qUtf8Printable(fileName));

    bool didItGetSaved = editor->saveAs(fileName);

    return didItGetSaved;
}

void MainWindow::saveCopyAsDialog()
{
    QString dialogDir;
    const QString filter = app->getFileDialogFilter();
    const ScintillaNext* editor = currentEditor();

    // Use the file path if possible
    if (editor->isFile()) {
        dialogDir = editor->getFilePath();
    }

    QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Save a Copy As"), dialogDir, filter);

    saveCopyAs(fileName);
}

void MainWindow::saveCopyAs(const QString &fileName)
{
    auto editor = currentEditor();
    editor->saveCopyAs(fileName);
}

void MainWindow::saveAll()
{
    for (ScintillaNext *editor : editors()) {
        saveFile(editor);
    }
}

void MainWindow::renameFile()
{
    ScintillaNext *editor = currentEditor();

    Q_ASSERT(editor->isFile());

    QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Rename"), editor->getFilePath());

    if (fileName.size() == 0) {
        return;
    }

    // TODO
    // The new fileName might be to one of the existing editors.
    //auto otherEditor = app->getEditorByFilePath(fileName);

    bool renameSuccessful = editor->rename(fileName);
    Q_UNUSED(renameSuccessful)
}

void MainWindow::moveCurrentFileToTrash()
{
    ScintillaNext *editor = currentEditor();

    moveFileToTrash(editor);
}

void MainWindow::moveFileToTrash(ScintillaNext *editor)
{
    Q_ASSERT(editor->isFile());

    const QString filePath = editor->getFilePath();
    auto reply = QMessageBox::question(this, tr("Delete File"), tr("Are you sure you want to move <b>%1</b> to the trash?").arg(filePath));;

    if (reply == QMessageBox::Yes) {
        if (editor->moveToTrash()) {
            closeCurrentFile();

            // Since the file no longer exists, specifically remove it from the recent files list
            app->getRecentFilesListManager()->removeFile(editor->getFilePath());
        }
        else {
            QMessageBox::warning(this, tr("Error Deleting File"),  tr("Something went wrong deleting <b>%1</b>?").arg(filePath));
        }
    }
}

void MainWindow::print()
{
    QPrintPreviewDialog printDialog(this, Qt::Window);
    EditorPrintPreviewRenderer renderer(currentEditor());

    connect(&printDialog, &QPrintPreviewDialog::paintRequested, &renderer, &EditorPrintPreviewRenderer::render);

    // TODO: load/save the page layout that was used and reload it next time
    //preview.printer()->setPageLayout( /* todo */ );

    printDialog.printer()->setPageMargins(QMarginsF(.5, .5, .5, .5), QPageLayout::Inch);

    connect(&printDialog, &QPrintPreviewDialog::accepted, this, [&]() {
        qInfo() << printDialog.printer()->pageLayout();
    });

    printDialog.exec();
}

void MainWindow::convertEOLs(int eolMode)
{
    ScintillaNext *editor = currentEditor();

    // TODO: does convertEOLs trigger SCN_MODIFIED notifications? If so can these be turned off to increase performance?
    editor->convertEOLs(eolMode);
    editor->setEOLMode(eolMode);

    updateEOLBasedUi(editor);

    // There's no simple Scintilla notification that the EOL mode has changed
    // So tell the status bar to refresh its info
    ui->statusBar->refresh(editor);
}

void MainWindow::showFindReplaceDialog(int index)
{
    ScintillaNext *editor = currentEditor();
    FindReplaceDialog *frd = nullptr;

    // Create it if it doesn't exist
    if (!dialogs.contains("FindReplaceDialog")) {
        frd = new FindReplaceDialog(findChild<SearchResultsDock *>(), this);
        dialogs["FindReplaceDialog"] = frd;
    }
    else {
        frd = qobject_cast<FindReplaceDialog *>(dialogs["FindReplaceDialog"]);
    }

    // TODO: if dockedEditor::editorActivated() is fired, or if the editor get closed
    // the FindReplaceDialog's editor pointer needs updated...

    // Get any selected text
    if (!editor->selectionEmpty()) {
        int selection = editor->mainSelection();
        int start = editor->selectionNStart(selection);
        int end = editor->selectionNEnd(selection);
        if (end > start) {
            auto selText = editor->get_text_range(start, end);
            frd->setFindString(QString::fromUtf8(selText));
        }
    }
    else {
        int start = editor->wordStartPosition(editor->currentPos(), true);
        int end = editor->wordEndPosition(editor->currentPos(), true);
        if (end > start) {
            editor->setSelectionStart(start);
            editor->setSelectionEnd(end);
            auto selText = editor->get_text_range(start, end);
            frd->setFindString(QString::fromUtf8(selText));
        }
    }

    frd->setTab(index);
    frd->show();
    frd->raise();
    frd->activateWindow();
}

void MainWindow::updateFileStatusBasedUi(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    bool isFile = editor->isFile();
    QString fileName;

    if (isFile) {
        fileName = editor->getFilePath();
    }
    else {
        fileName = editor->getName();
    }

    setWindowTitle(QStringLiteral("[*]%1").arg(fileName));

    ui->actionReload->setEnabled(isFile);
    ui->actionRename->setEnabled(isFile);
    ui->actionMoveToTrash->setEnabled(isFile);
    ui->actionCopyFullPath->setEnabled(isFile);
    ui->actionCopyFileDirectory->setEnabled(isFile);
}

bool MainWindow::isAnyUnsaved() const
{
    for (const ScintillaNext *editor : editors()) {
        if (!editor->isSavedToDisk()) {
            return true;
        }
    }

    return false;
}

void MainWindow::updateEOLBasedUi(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    switch(editor->eOLMode()) {
    case SC_EOL_CR:
        ui->actionMacintosh->setChecked(true);
        break;
    case SC_EOL_CRLF:
        ui->actionWindows->setChecked(true);
        break;
    case SC_EOL_LF:
        ui->actionUnix->setChecked(true);
        break;
    }
}

void MainWindow::updateSaveStatusBasedUi(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    bool isDirty = !editor->isSavedToDisk();

    setWindowModified(isDirty);

    ui->actionSave->setEnabled(isDirty);
    ui->actionSaveAll->setEnabled(isDirty || isAnyUnsaved());

    if (editorCount() == 1) {
        bool ableToClose = editor->isFile() || isDirty;
        ui->actionClose->setEnabled(ableToClose);
        ui->actionCloseAll->setEnabled(ableToClose);
    }
    else {
        ui->actionClose->setEnabled(true);
        ui->actionCloseAll->setEnabled(true);
    }
}

void MainWindow::updateEditorPositionBasedUi()
{
    const int index = dockedEditor->currentDockArea()->currentIndex();
    const int total = dockedEditor->currentDockArea()->dockWidgetsCount();

    ui->actionCloseAllToLeft->setEnabled(index > 0);
    ui->actionCloseAllToRight->setEnabled(index < (total - 1));
    ui->actionCloseAllExceptActive->setEnabled(editorCount() > 1);
}

void MainWindow::updateLanguageBasedUi(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    const QString language_name = editor->languageName;

    for (QAction *action : languageActionGroup->actions()) {
        if (action->data().toString() == language_name) {
            action->setChecked(true);

            // Found one, so we are completely done
            return;
        }
    }

    // The above loop did not set any action as checked, so make sure they are all unchecked now
    for (QAction *action : languageActionGroup->actions()) {
        if (action->isChecked()) {
            action->setChecked(false);
        }
    }
}

void MainWindow::updateGui(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    updateFileStatusBasedUi(editor);
    updateSaveStatusBasedUi(editor);
    updateEOLBasedUi(editor);
    updateEditorPositionBasedUi();
    updateSelectionBasedUi(editor);
    updateContentBasedUi(editor);
    updateLanguageBasedUi(editor);
}

void MainWindow::updateDocumentBasedUi(Scintilla::Update updated)
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());

    // TODO: what if this is triggered by an editor that is not the active editor?

    if (Scintilla::FlagSet(updated, Scintilla::Update::Content)) {
        updateSelectionBasedUi(editor);
    }

    if (Scintilla::FlagSet(updated, Scintilla::Update::Content) || Scintilla::FlagSet(updated, Scintilla::Update::Selection)) {
        updateContentBasedUi(editor);
    }
}

void MainWindow::updateSelectionBasedUi(ScintillaNext *editor)
{
    ui->actionUndo->setEnabled(editor->canUndo());
    ui->actionRedo->setEnabled(editor->canRedo());
}

void MainWindow::updateContentBasedUi(ScintillaNext *editor)
{
    bool hasAnySelections = !editor->selectionEmpty();

    ui->actionPaste->setEnabled(editor->canPaste());

    ui->actionLowerCase->setEnabled(hasAnySelections);
    ui->actionUpperCase->setEnabled(hasAnySelections);
}

void MainWindow::detectLanguage(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor->isFile()) {
        // Default to some specific language if it is not a file.
        setLanguage(editor, "Text");
        return;
    }
    else {
        const QString language_name = app->detectLanguage(editor);

        setLanguage(editor, language_name);
    }

    return;
}

void MainWindow::activateEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    checkFileForModification(editor);
    updateGui(editor);

    emit editorActivated(editor);
}

void MainWindow::setLanguage(ScintillaNext *editor, const QString &languageName)
{
    qInfo(Q_FUNC_INFO);
    qInfo("Language Name: %s", qUtf8Printable(languageName));

    app->setEditorLanguage(editor, languageName);
}

void MainWindow::bringWindowToForeground()
{
    qInfo(Q_FUNC_INFO);

    // There doesn't seem to be a cross platform way to force the window to the foreground

#ifdef Q_OS_WIN
    HWND hWnd = reinterpret_cast<HWND>(effectiveWinId());

    if (hWnd) {
        // I have no idea what this does, but it seems to work on Windows
        // References:
        // https://stackoverflow.com/questions/916259/win32-bring-a-window-to-top
        // https://github.com/notepad-plus-plus/notepad-plus-plus/blob/ebe7648ee1a5a560d4fc65297cbdcf08055e56e3/PowerEditor/src/winmain.cpp#L596

        HWND hCurWnd = GetForegroundWindow();
        DWORD threadId = GetCurrentThreadId();
        DWORD procId = GetWindowThreadProcessId(hCurWnd, NULL);

        int sw = 0;
        if (IsZoomed(hWnd)) {
            sw = SW_MAXIMIZE;
        } else if (IsIconic(hWnd)) {
            sw = SW_RESTORE;
        }

        if (sw != 0) {
            ShowWindow(hWnd, sw);
        }

        AttachThreadInput(procId, threadId, TRUE);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
        AttachThreadInput(procId, threadId, FALSE);
    }
#else
    // Make sure the window isn't minimized
    // TODO: this always puts it in the "normal" state but it might have been maximized
    // before minimized...so either a flag needs stored or find a Qt call to do it appropriately
    if (isMinimized())
        showNormal();
#endif
}

bool MainWindow::checkFileForModification(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    auto state = editor->checkFileForStateChange();

    if (state == ScintillaNext::NoChange) {
        return false;
    }
    else if (state == ScintillaNext::Modified) {
        qInfo("ScintillaNext::Modified");
        editor->reload();
    }
    else if (state == ScintillaNext::Deleted) {
        qInfo("ScintillaNext::Deleted");
    }
    else if (state == ScintillaNext::Restored) {
        qInfo("ScintillaNext::Restored");
    }

    return true;
}

void MainWindow::saveSettings() const
{
    qInfo(Q_FUNC_INFO);

    QSettings settings;

    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());

    settings.setValue("Gui/ShowMenuBar", app->getSettings()->showMenuBar());
    settings.setValue("Gui/ShowToolBar", app->getSettings()->showToolBar());
    settings.setValue("Gui/ShowStatusBar", app->getSettings()->showStatusBar());

    settings.setValue("Editor/ShowWhitespace", ui->actionShowWhitespace->isChecked());
    settings.setValue("Editor/ShowEndOfLine", ui->actionShowEndofLine->isChecked());
    settings.setValue("Editor/ShowWrapSymbol", ui->actionShowWrapSymbol->isChecked());

    settings.setValue("Editor/WordWrap", ui->actionWordWrap->isChecked());
    settings.setValue("Editor/IndentGuide", ui->actionShowIndentGuide->isChecked());
    settings.setValue("Editor/ZoomLevel", zoomLevel);

    FolderAsWorkspaceDock *fawDock = findChild<FolderAsWorkspaceDock *>();
    settings.setValue("FolderAsWorkspace/RootPath", fawDock->rootPath());
}

void MainWindow::restoreSettings()
{
    qInfo(Q_FUNC_INFO);

    QSettings settings;

    app->getSettings()->setShowMenuBar(settings.value("Gui/ShowMenuBar", true).toBool());
    app->getSettings()->setShowToolBar(settings.value("Gui/ShowToolBar", true).toBool());
    app->getSettings()->setShowStatusBar(settings.value("Gui/ShowStatusBar", true).toBool());

    ui->actionShowWhitespace->setChecked(settings.value("Editor/ShowWhitespace", false).toBool());
    ui->actionShowEndofLine->setChecked(settings.value("Editor/ShowEndOfLine", false).toBool());
    ui->actionShowWrapSymbol->setChecked(settings.value("Editor/ShowWrapSymbol", false).toBool());

    ui->actionWordWrap->setChecked(settings.value("Editor/WordWrap", false).toBool());
    ui->actionShowIndentGuide->setChecked(settings.value("Editor/IndentGuide", true).toBool());
    zoomLevel = settings.value("Editor/ZoomLevel", 0).toInt();
}


void MainWindow::restoreWindowState()
{
    QSettings settings;

    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());

    // Restore the path if it has one
    FolderAsWorkspaceDock *fawDock = findChild<FolderAsWorkspaceDock *>();
    fawDock->setRootPath(settings.value("FolderAsWorkspace/RootPath").toString());

    // Always hide the dock no matter how the application was closed
    SearchResultsDock *srDock = findChild<SearchResultsDock *>();
    srDock->hide();
}

void MainWindow::focusIn()
{
    qInfo(Q_FUNC_INFO);

    if (checkFileForModification(currentEditor())) {
        updateGui(currentEditor());
    }
}

void MainWindow::addEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    detectLanguage(editor);

    // These should only ever occur for the focused editor??
    // TODO: look at editor inspector as an example to ensure updates are only coming from one editor.
    // Can save the connection objects and disconnected from them and only connect to the editor as it is activated.
    connect(editor, &ScintillaNext::savePointChanged, this, [=]() { updateSaveStatusBasedUi(editor); });
    connect(editor, &ScintillaNext::renamed, this, [=]() { detectLanguage(editor); });
    connect(editor, &ScintillaNext::renamed, this, [=]() { updateFileStatusBasedUi(editor); });
    connect(editor, &ScintillaNext::updateUi, this, &MainWindow::updateDocumentBasedUi);
    connect(editor, &ScintillaNext::marginClicked, [editor](Scintilla::Position position, Scintilla::KeyMod modifiers, int margin) {
        Q_UNUSED(modifiers);

        if (margin == 1) {
            int line = editor->lineFromPosition(position);

            if (editor->markerGet(line) & (1 << 24)) {
                while (editor->markerGet(line) & (1 << 24)) {
                    editor->markerDelete(line, 24);
                }
            }
            else {
                editor->markerAdd(line, 24);
            }
        }
    });

    // Watch for any zoom events (Ctrl+Scroll or pinch-to-zoom (Qt translates it as Ctrl+Scroll)) so that the event
    // can be handled before the ScintillaEditBase widget, so that it can be applied to all editors to keep zoom level equal.
    // NOTE: Need to install this on the scroll area's viewport, not on the editor widget itself...that was painful to learn
    editor->viewport()->installEventFilter(zoomEventWatcher);

    if (ui->actionWordWrap->isChecked())
        editor->setWrapMode(SC_WRAP_WHITESPACE);

    if (ui->actionShowIndentGuide->isChecked())
        editor->setIndentationGuides(SC_IV_LOOKBOTH);

    editor->setViewWS(ui->actionShowWhitespace->isChecked() ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    editor->setViewEOL(ui->actionShowEndofLine->isChecked());
    editor->setWrapVisualFlags(ui->actionShowWrapSymbol->isChecked() ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    editor->setZoom(zoomLevel);

    // The editor has been entirely configured at this point, so add it to the docked editor
    dockedEditor->addEditor(editor);
}

void MainWindow::checkForUpdates(bool silent)
{
#ifdef Q_OS_WIN
    qInfo(Q_FUNC_INFO);

    QString url = "https://github.com/dail8859/NotepadNext/raw/master/updates.json";
    QSimpleUpdater::getInstance()->checkForUpdates(url);

    if (!silent) {
        connect(QSimpleUpdater::getInstance(), &QSimpleUpdater::checkingFinished, this, &MainWindow::checkForUpdatesFinished, Qt::UniqueConnection);
    }
    else {
        disconnect(QSimpleUpdater::getInstance(), &QSimpleUpdater::checkingFinished, this, &MainWindow::checkForUpdatesFinished);
    }

    QSettings settings;
    settings.setValue("App/LastUpdateCheck", QDateTime::currentDateTime());
#else
    Q_UNUSED(silent);
#endif
}

void MainWindow::checkForUpdatesFinished(QString url)
{
#ifdef Q_OS_WIN
    if (!QSimpleUpdater::getInstance()->getUpdateAvailable(url)) {
        QMessageBox::information(this, QString(), tr("No updates are available at this time."));
    }
#endif
}

void MainWindow::initUpdateCheck()
{
#ifdef Q_OS_WIN
#ifdef QT_DEBUG
    if (true) {
#else
    QSettings registry(QSettings::NativeFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    const bool autoUpdatesEnabled = registry.value("AutoUpdate", 0).toBool();
    qInfo("AutoUpdates: %d", autoUpdatesEnabled);

    if (autoUpdatesEnabled) {
#endif
        connect(ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);

        // A bit after startup, see if we need to automatically check for an update
        QTimer::singleShot(15000, this, [=]() {
            QSettings settings;
            QDateTime dt = settings.value("App/LastUpdateCheck", QDateTime::currentDateTime()).toDateTime();

            if (dt.isValid()) {
                qInfo("Last checked for updates at: %s", qUtf8Printable(dt.toString()));

                if (dt.addDays(7) < QDateTime::currentDateTime()) {
                    checkForUpdates(true);
                }
            }
        });
    }
    else {
        ui->actionCheckForUpdates->setDisabled(true);
        ui->actionCheckForUpdates->setVisible(false);
    }
#else
    ui->actionCheckForUpdates->setDisabled(true);
    ui->actionCheckForUpdates->setVisible(false);
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!checkEditorsBeforeClose(editors())) {
        event->ignore();
        return;
    }

    emit aboutToClose();

    event->accept();

    QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qInfo(Q_FUNC_INFO);

    // NOTE: for urls these can be dragged anywhere in the application, editor, tabs, menu
    // because the ScintillaNext editor ignores urls so they can be handled by the main
    // application
    // Text dragging within the editor object itself is handled by Scintilla, but if the text
    // is dragged to other parts (tabs, menu, etc) it will be handled by the application to
    // create a new editor from the text.

    // Accept urls and text
    if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qInfo(Q_FUNC_INFO);

    if (event->mimeData()->hasUrls()) {
        // Get the urls into a stringlist
        QStringList fileNames;
        for (const QUrl &url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                fileNames.append(url.toLocalFile());
            }
        }

        openFileList(fileNames);
        bringWindowToForeground();
        event->acceptProposedAction();
    }
    else if (event->mimeData()->hasText()) {
        if (event->source()) {
            // if it is from an editor, remove the text
            ScintillaNext *sn = qobject_cast<ScintillaNext *>(event->source());
            if (sn) {
                sn->replaceSel("");
            }
        }

        newFile();
        currentEditor()->setText(event->mimeData()->text().toLocal8Bit().constData());
        bringWindowToForeground();
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void MainWindow::tabBarRightClicked(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    // Focus on the correct tab
    dockedEditor->switchToEditor(editor);

    // Create the menu and show it
    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionClose);
    menu->addAction(ui->actionCloseAllExceptActive);
    menu->addAction(ui->actionCloseAllToLeft);
    menu->addAction(ui->actionCloseAllToRight);
    menu->addSeparator();
    menu->addAction(ui->actionSave);
    menu->addAction(ui->actionSaveAs);
    menu->addAction(ui->actionRename);
    menu->addSeparator();
    menu->addAction(ui->actionReload);
    menu->addSeparator();
    menu->addAction(ui->actionCopyFullPath);
    menu->addAction(ui->actionCopyFileName);
    menu->addAction(ui->actionCopyFileDirectory);
    menu->popup(QCursor::pos());
}

void MainWindow::languageMenuTriggered()
{
    const QAction *act = qobject_cast<QAction *>(sender());
    auto editor = currentEditor();
    QVariant v = act->data();

    setLanguage(editor, v.toString());
}
