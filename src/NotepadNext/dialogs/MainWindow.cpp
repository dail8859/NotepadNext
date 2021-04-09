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
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "DockAreaWidget.h"
#include "DockWidgetTab.h"

#include "NotepadNextApplication.h"
#include "Settings.h"

#include "ScintillaNext.h"
#include "ScintillaBuffer.h"

#include "RecentFilesListManager.h"
#include "EditorManager.h"

#include "LuaExtension.h"
#include "LuaState.h"
#include "MacroRecorder.h"

#include "LuaConsoleDock.h"
#include "LanguageInspectorDock.h"

#include "FindReplaceDialog.h"
#include "MacroRunDialog.h"
#include "MacroSaveDialog.h"
#include "PreferencesDialog.h"


MainWindow::MainWindow(NotepadNextApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app(app)
{
    qInfo(Q_FUNC_INFO);

    Q_INIT_RESOURCE(ads);

    ui->setupUi(this);

    qInfo("setupUi Completed");

    setupStatusBar();

    // The windows editor manager that supports docking
    dockedEditor = new DockedEditor(this);

    connect(dockedEditor, &DockedEditor::editorCloseRequested, [=](ScintillaNext *editor) {
        closeFile(editor);
    });
    connect(dockedEditor, &DockedEditor::editorActivated, this, &MainWindow::editorActivated);

    // Set up the lua state and the extension. Need to intialize it after the first file was created
    LuaExtension::Instance().Initialise(app->getLuaState()->L, Q_NULLPTR);
    connect(dockedEditor, &DockedEditor::editorActivated, [](ScintillaNext *editor) {
        LuaExtension::Instance().setEditor(editor);
    });

    // TODO: replace with + button on bar
    //connect(tabbedEditor->getTabBar(), &QTabBar::tabBarDoubleClicked, this, [=](int index) {
    //    if (index == TabbedEditor::INVALID_INDEX) {
    //        newFile();
    //    }
    //});
    connect(dockedEditor, &DockedEditor::contextMenuRequestedForEditor, this, &MainWindow::tabBarRightClicked);

    // Set up the menus
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::reloadFile);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeCurrentFile);
    connect(ui->actionCloseAll, &QAction::triggered, this, &MainWindow::closeAllFiles);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionCloseAllExceptActive, &QAction::triggered, this, &MainWindow::closeAllExceptActive);
    connect(ui->actionCloseAllToLeft, &QAction::triggered, this, &MainWindow::closeAllToLeft);
    connect(ui->actionCloseAllToRight, &QAction::triggered, this, &MainWindow::closeAllToRight);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveCurrentFileAsDialog);
    connect(ui->actionSaveCopyAs, &QAction::triggered, this, &MainWindow::saveCopyAsDialog);
    connect(ui->actionSaveAll, &QAction::triggered, this, &MainWindow::saveAll);
    connect(ui->actionRename, &QAction::triggered, this, &MainWindow::renameFile);

    connect(ui->actionClearRecentFilesList, &QAction::triggered, app->getRecentFilesListManager(), &RecentFilesListManager::clear);

    connect(ui->menuRecentFiles, &QMenu::aboutToShow, [=]() {
        // NOTE: its unfortunate that this has to be hard coded, but there's no way
        // to easily determine what should or shouldn't be there
        while (ui->menuRecentFiles->actions().size() > 4) {
            delete ui->menuRecentFiles->actions().takeLast();
        }

        app->getRecentFilesListManager()->populateMenu(ui->menuRecentFiles);
    });

    connect(ui->actionRestoreRecentlyClosedFile, &QAction::triggered, [=]() {
        if (app->getRecentFilesListManager()->count() > 0) {
            openFileList(QStringList() << app->getRecentFilesListManager()->mostRecentFile());
        }
    });

    connect(ui->actionOpenAllRecentFiles, &QAction::triggered, [=]() {
        openFileList(app->getRecentFilesListManager()->fileList());
    });
    connect(app->getRecentFilesListManager(), &RecentFilesListManager::fileOpenRequest, this, &MainWindow::openFile);

    QActionGroup *eolActionGroup = new QActionGroup(this);
    eolActionGroup->addAction(ui->actionWindows);
    eolActionGroup->addAction(ui->actionUnix);
    eolActionGroup->addAction(ui->actionMacintosh);

    connect(ui->actionWindows, &QAction::triggered, [=]() { convertEOLs(SC_EOL_CRLF); });
    connect(ui->actionUnix, &QAction::triggered, [=]() { convertEOLs(SC_EOL_LF); });
    connect(ui->actionMacintosh, &QAction::triggered, [=]() { convertEOLs(SC_EOL_CR); });

    connect(ui->actionUpperCase, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->upperCase();});
    connect(ui->actionLowerCase, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->lowerCase();});

    connect(ui->actionDuplicateCurrentLine, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->lineDuplicate();});
    connect(ui->actionMoveCurrentLineUp, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->moveSelectedLinesUp();});
    connect(ui->actionMoveCurrentLineDown, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->moveSelectedLinesDown();});
    connect(ui->actionSplitLines, &QAction::triggered, [=]() {
        dockedEditor->getCurrentEditor()->targetFromSelection();
        dockedEditor->getCurrentEditor()->linesSplit(0);
    });
    connect(ui->actionJoinLines, &QAction::triggered, [=]()  {
        dockedEditor->getCurrentEditor()->targetFromSelection();
        dockedEditor->getCurrentEditor()->linesJoin();
    });

    connect(ui->actionUndo, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->undo();});
    connect(ui->actionRedo, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->redo();});
    connect(ui->actionCut, &QAction::triggered, [=]() {
        if (dockedEditor->getCurrentEditor()->selectionEmpty()) {
            dockedEditor->getCurrentEditor()->copyAllowLine();
            dockedEditor->getCurrentEditor()->lineDelete();
        }
        else {
            dockedEditor->getCurrentEditor()->cut();
        }
    });
    connect(ui->actionCopy, &QAction::triggered, [=]() {
        dockedEditor->getCurrentEditor()->copyAllowLine();
    });
    connect(ui->actionPaste, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->paste();});
    connect(ui->actionSelect_All, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->selectAll();});
    connect(ui->actionSelect_Next, &QAction::triggered, [=]() {
        // Set search flags here?
        dockedEditor->getCurrentEditor()->targetWholeDocument();
        dockedEditor->getCurrentEditor()->multipleSelectAddNext();
    });
    connect(ui->actionCopyFullPath, &QAction::triggered, [=]() {
        auto editor = dockedEditor->getCurrentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->scintillaBuffer()->fileInfo.canonicalFilePath());
        }
    });
    connect(ui->actionCopyFileName, &QAction::triggered, [=]() {
        QApplication::clipboard()->setText(dockedEditor->getCurrentBuffer()->getName());
    });
    connect(ui->actionCopyFileDirectory, &QAction::triggered, [=]() {
        auto editor = dockedEditor->getCurrentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->scintillaBuffer()->fileInfo.canonicalFilePath());
        }
    });
    connect(ui->actionIncrease_Indent, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->tab();});
    connect(ui->actionDecrease_Indent, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->backTab();});

    connect(ui->actionFind, &QAction::triggered, [=]() {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();

        // Create it if it doesn't exist
        if (frd == Q_NULLPTR) {
            frd = new FindReplaceDialog(this);
        }

        frd->setEditor(editor);

        // TODO: if dockedEditor::editorActivated() is fired, or if the editor get closed
        // the FindReplaceDialog's editor pointer needs updated...

        // Get any selected text
        if (!editor->selectionEmpty()) {
            int selection = editor->mainSelection();
            int start = editor->selectionNStart(selection);
            int end = editor->selectionNEnd(selection);
            if (end > start) {
                auto selText = editor->get_text_range(start, end);
                frd->setFindText(QString::fromUtf8(selText));
            }
        }
        else {
            int start = editor->wordStartPosition(editor->currentPos(), true);
            int end = editor->wordEndPosition(editor->currentPos(), true);
            if (end > start) {
                editor->setSelectionStart(start);
                editor->setSelectionEnd(end);
                auto selText = editor->get_text_range(start, end);
                frd->setFindText(QString::fromUtf8(selText));
            }
        }

        frd->setTab(FindReplaceDialog::FIND_TAB);
        frd->show();
        frd->raise();
        frd->activateWindow();
    });

    connect(ui->actionFindNext, &QAction::triggered, [=]() {
        if (frd != Q_NULLPTR) {
            frd->performLastSearch();
        }
    });

    connect(ui->actionQuickFind, &QAction::triggered, [=]() {
        ScintillaNext *editor = this->dockedEditor->getCurrentEditor();

        if (quickFind == Q_NULLPTR) {
            quickFind = new QuickFindWidget(this);
        }
        quickFind->setEditor(editor);
        quickFind->setFocus();

        quickFind->show();
    });

    //connect(ui->actionReplace, &QAction::triggered, [=]() {
    //    // Create it if it doesn't exist
    //    if (frd == Q_NULLPTR) {
    //        frd = new FindReplaceDialog(this);
    //        frd->setEditor(editor);
    //    }
    //
    //    // Get any selected text
    //    if (!editor->selectionEmpty()) {
    //        int selection = editor->mainSelection();
    //        int start = editor->selectionNStart(selection);
    //        int end = editor->selectionNEnd(selection);
    //        if (end > start) {
    //            auto selText = editor->get_text_range(start, end);
    //            frd->setFindText(QString::fromUtf8(selText));
    //        }
    //    }
    //    else {
    //        int start = editor->wordStartPosition(editor->currentPos(), true);
    //        int end = editor->wordEndPosition(editor->currentPos(), true);
    //        if (end > start) {
    //            editor->setSelectionStart(start);
    //            editor->setSelectionEnd(end);
    //            auto selText = editor->get_text_range(start, end);
    //            frd->setFindText(QString::fromUtf8(selText));
    //        }
    //    }
    //
    //    frd->setTab(FindReplaceDialog::REPLACE_TAB);
    //    frd->show();
    //    frd->raise();
    //    frd->activateWindow();
    //});


    connect(ui->actionAlwaysOnTop, &QAction::triggered, [=](bool b) {
        const Qt::WindowFlags flags = this->windowFlags();
        if (b)
            this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
        else
            this->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
        this->show();
    });

    ui->pushExitFullScreen->setParent(this); // This is important
    ui->pushExitFullScreen->setVisible(false);
    connect(ui->pushExitFullScreen, &QPushButton::clicked, ui->actionFullScreen, &QAction::trigger);
    connect(ui->actionFullScreen, &QAction::triggered, [=](bool b) {
        if (b) {
            // NOTE: don't hide() these as it will cancel their actions they hold
            ui->menuBar->setMaximumHeight(0);
            ui->mainToolBar->setMaximumHeight(0);

            this->showFullScreen();
            ui->pushExitFullScreen->setGeometry(width() - 20, 0, 20, 20);
            ui->pushExitFullScreen->show();
            ui->pushExitFullScreen->raise();
        }
        else {
            ui->menuBar->setMaximumHeight(QWIDGETSIZE_MAX);
            ui->mainToolBar->setMaximumHeight(QWIDGETSIZE_MAX);
            this->showNormal();

            ui->pushExitFullScreen->hide();
        }
    });

    // The default behavior of QActionGroup cannot be used since it *must* have one set
    // but Notepad++ allows none to also be set.
    QActionGroup *showSymbolActionGroup = new QActionGroup(this);
    showSymbolActionGroup->addAction(ui->actionShowWhitespaceandTab);
    showSymbolActionGroup->addAction(ui->actionShowEndofLine);
    showSymbolActionGroup->addAction(ui->actionShowAllCharacters);
    showSymbolActionGroup->setExclusive(false);

    connect(showSymbolActionGroup, &QActionGroup::triggered, [=](QAction *action) {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();
        if (!action->isChecked()) {
            editor->setViewWS(SCWS_INVISIBLE);
            editor->setViewEOL(false);
        }
        else {
            // Uncheck all other actions
            foreach (QAction *otherAction, showSymbolActionGroup->actions()) {
                if (otherAction != action) {
                    otherAction->setChecked(false);
                }
            }

            if (action == ui->actionShowWhitespaceandTab) {
                editor->setViewWS(SCWS_VISIBLEALWAYS);
                editor->setViewEOL(false);
            }
            else if (action == ui->actionShowEndofLine) {
                editor->setViewWS(SCWS_INVISIBLE);
                editor->setViewEOL(true);
            }
            else if (action == ui->actionShowAllCharacters) {
                editor->setViewWS(SCWS_VISIBLEALWAYS);
                editor->setViewEOL(true);
            }
        }
    });

    connect(ui->actionShowWrapSymbol, &QAction::triggered, [=](bool b) {
        dockedEditor->getCurrentEditor()->setWrapVisualFlags(b ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    });
    connect(ui->actionShowIndentGuide, &QAction::triggered, [=](bool b) {
        dockedEditor->getCurrentEditor()->setIndentationGuides(b ? SC_IV_LOOKBOTH : SC_IV_NONE);
    });
    ui->actionShowIndentGuide->setChecked(true);

    connect(ui->actionWordWrap, &QAction::triggered, [=](bool b) {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();
        if (b) {
            dockedEditor->getCurrentEditor()->setWrapMode(SC_WRAP_WORD);
        }
        else {
            // Store the top line and restore it after the lines have been unwrapped
            int topLine = editor->docLineFromVisible(editor->firstVisibleLine());
            editor->setWrapMode(SC_WRAP_NONE);
            editor->setFirstVisibleLine(topLine);
        }
    });

    connect(ui->actionZoomIn, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->zoomIn();});
    connect(ui->actionZoomOut, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->zoomOut();});
    connect(ui->actionZoomReset, &QAction::triggered, [=]() { dockedEditor->getCurrentEditor()->setZoom(0); });

    languageActionGroup = new QActionGroup(this);
    languageActionGroup->setExclusive(true);

    connect(ui->actionPreferences, &QAction::triggered, [=] {
        if (pd == Q_NULLPTR)
            pd = new PreferencesDialog(app->getSettings(), this);
        pd->show();
        pd->raise();
        pd->activateWindow();
    });

    recorder = new MacroRecorder(this);
    connect(ui->actionMacroRecording, &QAction::triggered, [=](bool b) {
        if (b) {
            ui->actionMacroRecording->setText("Stop Recording");
            recorder->startRecording(dockedEditor->getCurrentEditor());

            // A macro is being recorded so disable some macro options
            ui->actionPlayback->setEnabled(false);
            ui->actionRunMacroMultipleTimes->setEnabled(false);
            ui->actionSaveCurrentRecordedMacro->setEnabled(false);
        }
        else {
            ui->actionMacroRecording->setText("Start Recording");
            Macro *m = recorder->stopRecording();

            if (m->size() > 0) {
                currentMacro = m;
                ui->actionPlayback->setEnabled(true);
                ui->actionRunMacroMultipleTimes->setEnabled(true);
                ui->actionSaveCurrentRecordedMacro->setEnabled(true);
            }
            else {
                // Don't need the macro returned from the recorder
                delete m;

                // A previously recorded one may still be valid
                if (currentMacro) {
                    ui->actionPlayback->setEnabled(true);
                    ui->actionRunMacroMultipleTimes->setEnabled(true);

                    // It may or may not be "saved" in the list
                    if (!macros.contains(currentMacro))
                        ui->actionSaveCurrentRecordedMacro->setEnabled(true);
                }
            }
        }
    });

    connect(ui->actionPlayback, &QAction::triggered, [=](bool b) {
        Q_UNUSED(b);

        currentMacro->replay(dockedEditor->getCurrentEditor());
    });

    connect(ui->actionRunMacroMultipleTimes, &QAction::triggered, [=](bool b) {
        Q_UNUSED(b);

        if (mrd == Q_NULLPTR) {
            mrd = new MacroRunDialog(this);
            connect(mrd, &MacroRunDialog::execute, [=](Macro *macro, int times) {
                if (times > 0)
                    macro->replay(dockedEditor->getCurrentEditor(), times);
                else if (times == -1)
                    macro->replayTillEndOfFile(dockedEditor->getCurrentEditor());
            });
        }

        if (!macros.contains(currentMacro))
            mrd->setMacros(QVector<Macro *>(macros) << currentMacro);
        else
            mrd->setMacros(macros);

        mrd->show();
        mrd->raise();
        mrd->activateWindow();
    });

    connect(ui->actionSaveCurrentRecordedMacro, &QAction::triggered, [=](bool b) {
        Q_UNUSED(b);

        MacroSaveDialog msd;

        msd.show();
        msd.raise();
        msd.activateWindow();

        if (msd.exec() == QDialog::Accepted) {
            // The macro has been saved so disable save option
            ui->actionSaveCurrentRecordedMacro->setEnabled(false);

            // TODO: does the macro name already exist?

            currentMacro->setName(msd.getName());
            if (!msd.getShortcut().isEmpty()) {
                // do something with msd.getShortcut().isEmpty()
            }

            macros.append(currentMacro);
        }
    });

    //connect(ui->menuWindows, &QMenu::aboutToShow, [=]() {
    //    auto actions = ui->menuWindows->actions();
    //
    //    // Keep the original 2 in the list (seperator and Window command)
    //    // delete the rest since nobody has ownership of them
    //    while (actions.size() > 2) {
    //        delete actions.takeFirst();
    //    }
    //
    //    ScintillaBuffer *currentBuffer = dockedEditor->getCurrentBuffer();
    //    foreach (ScintillaBuffer *buffer, dockedEditor->buffers()) {
    //        QAction *action = new QAction(buffer->getName());
    //        if (buffer == currentBuffer) {
    //            action->setCheckable(true);
    //            action->setChecked(true);
    //        }
    //
    //        //connect(action, &QAction::triggered, [=]() {
    //        //    tabbedEditor->switchToIndex(i);
    //        //});
    //
    //        // NOTE: the menu does not take ownership when using insertAction
    //        ui->menuWindows->insertAction(actions.at(0), action);
    //    }
    //});

    //connect(ui->actionWindowsList, &QAction::triggered, [=]() {
    //    WindowListDialog wld(this, dockedEditor->buffers().toList(), this);
    //    wld.show();
    //    wld.raise();
    //    wld.activateWindow();
    //    wld.exec();
    //});

    connect(ui->actionAboutQt, &QAction::triggered, &QApplication::aboutQt);
    connect(ui->actionAboutNotepadNext, &QAction::triggered, [=]() {
        QMessageBox::about(this, "About Notepad Next",
                            QString("<h3>Notepad Next v%1</h3><p>This program does stuff.</p><p>%2</p>")
                                .arg(APP_VERSION, APP_COPYRIGHT));
    });

    //connect(app->getBufferManager(), &BufferManager::bufferCreated, dockedEditor, &DockedEditor::addBuffer);
    //connect(app->getEditorManager(), &EditorManager::editorClosed, dockedEditor, &DockedEditor::removeEditor);
    //connect(app->getBufferManager(), &BufferManager::bufferRenamed, dockedEditor, &DockedEditor::renamedBuffer);

    // If the current file is saved update the window title incase the file was renamed
    connect(dockedEditor, &DockedEditor::editorAdded, this, &MainWindow::detectLanguageFromExtension);

    //connect(app->getBufferManager(), &BufferManager::bufferClosed, this, &MainWindow::updateBufferPositionBasedUi);

    //connect(app->getBufferManager(), &BufferManager::bufferRenamed, [=] (ScintillaBuffer *buffer) {
    //    updateBufferFileStatusBasedUi(buffer);
    //    //detectLanguageFromExtension(buffer);
    //    updateBufferFileStatusBasedUi(buffer);
    //});

    ui->actionAboutQt->setIcon(QPixmap(QLatin1String(":/qt-project.org/qmessagebox/images/qtlogo-64.png")));
    ui->actionAboutNotepadNext->setIcon(this->windowIcon());
    ui->actionAboutNotepadNext->setShortcut(QKeySequence::HelpContents);

    QSettings settings;
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());

    if (luaConsoleDock == Q_NULLPTR) {
        luaConsoleDock = new LuaConsoleDock(app->getLuaState(), this);
        luaConsoleDock->hide();
        addDockWidget(Qt::BottomDockWidgetArea, luaConsoleDock);
        ui->menuHelp->addSeparator()->setText("pickles");
        ui->menuHelp->addAction(luaConsoleDock->toggleViewAction());
    }

    if (languageInspectorDock == Q_NULLPTR) {
        languageInspectorDock = new LanguageInspectorDock(this);
        languageInspectorDock->hide();
        addDockWidget(Qt::RightDockWidgetArea, languageInspectorDock);
        ui->menuHelp->addAction(languageInspectorDock->toggleViewAction());
    }

    connect(app->getSettings(), &Settings::showMenuBarChanged, [=](bool showMenuBar) {
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
}

MainWindow::~MainWindow()
{
    Q_ASSERT(dockedEditor->count() == 0);
}

void MainWindow::setupLanguageMenu()
{
    qInfo(Q_FUNC_INFO);

    QStringList language_names = app->getLuaState()->executeAndReturn<QStringList>(
                R"(
                local names = {}
                for k in pairs(languages) do table.insert(names, k) end
                table.sort(names, function (a, b) return string.lower(a) < string.lower(b) end)
                return names
                )");

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

            //if (key == "normal")
            //    action->setChecked(true);
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

void MainWindow::connectEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    // Connect the editor to the UI
    // TODO: what if a non focused editor sends this signal

    connect(editor, &ScintillaNext::closed, [=]() { dockedEditor->removeEditor(editor); });

    connect(editor, &ScintillaNext::savePointChanged, this, &MainWindow::updateSaveStatusBasedUi);

    // These should only ever occur for the focused editor
    connect(editor, &ScintillaNext::updateUi, this, &MainWindow::updateDocumentBasedUi);
    connect(editor, &ScintillaNext::marginClicked, [editor](int position, int modifiers, int margin) {
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
}

ScintillaNext *MainWindow::currentEditor()
{
    return dockedEditor->getCurrentEditor();
}

DockedEditor *MainWindow::getDockedEditor()
{
    return dockedEditor;
}

void MainWindow::newFile()
{
    qInfo(Q_FUNC_INFO);

    static int count = 1;

    app->getEditorManager()->createEmptyEditor(QString("New %1").arg(count++));
}

// One unedited, new blank document
bool MainWindow::isInInitialState()
{
    if (dockedEditor->count() == 1) {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();
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

    foreach (const QString &filePath , fileNames) {
        qInfo(qUtf8Printable(filePath));

        // Search currently open buffers to see if it is already open
        ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);

        if (editor == Q_NULLPTR) {
            QFileInfo fileInfo(filePath);

            if (!fileInfo.isFile()) {
                auto reply = QMessageBox::question(this, "Create File", QString("<b>%1</b> does not exist. Do you want to create it?").arg(filePath));

                if (reply == QMessageBox::Yes) {
                    editor = app->getEditorManager()->createEditorFromFile(filePath);
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
    foreach (ScintillaNext *editor, editors) {
        auto buffer = editor->scintillaBuffer();

        if (!editor->isSavedToDisk()) {
            // Switch to it
            dockedEditor->switchToEditor(editor);

            // Ask the user what to do
            QString message = QString("Save file <b>%1</b>?").arg(buffer->getName());
            auto reply = QMessageBox::question(this, "Save File", message, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

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

void MainWindow::setupStatusBar()
{
    // Set up the status bar
    docType = new StatusLabel();
    ui->statusBar->addWidget(docType, 1);
    docSize = new StatusLabel(200);
    ui->statusBar->addPermanentWidget(docSize, 0);
    docPos = new StatusLabel(250);
    ui->statusBar->addPermanentWidget(docPos, 0);
    eolFormat = new StatusLabel(100);
    ui->statusBar->addPermanentWidget(eolFormat, 0);
    unicodeType = new StatusLabel(125);
    ui->statusBar->addPermanentWidget(unicodeType, 0);
    overType = new StatusLabel(25);
    ui->statusBar->addPermanentWidget(overType, 0);

    docType->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(docType, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        ui->menuLanguage->popup(docType->mapToGlobal(pos));
    });

    connect(qobject_cast<StatusLabel*>(overType), &StatusLabel::clicked, [=]() {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();
        bool ot = editor->overtype();
        if (ot)
            overType->setText("INS");
        else
            overType->setText("OVR");
        editor->setOvertype(!ot);
    });

    eolFormat->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(eolFormat, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        ui->menuEOLConversion->popup(eolFormat->mapToGlobal(pos));
    });
}

void MainWindow::openFileDialog()
{
    QString filter = app->getFileDialogFilter();

    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, // parent
        Q_NULLPTR, // caption
        Q_NULLPTR, // dir
        filter, // filter
        Q_NULLPTR // selected filter
        );

    openFileList(fileNames);
}

void MainWindow::openFile(const QString &filePath)
{
    openFileList(QStringList() << filePath);
}

void MainWindow::reloadFile()
{
    auto editor = dockedEditor->getCurrentEditor();

    if (!editor->isFile() && !editor->isSavedToDisk()) {
        return;
    }

    auto reply = QMessageBox::question(this, "Reload File", QString("Are you sure you want to reload <b>%1</b>? Any unsaved changes will be lost.").arg(editor->scintillaBuffer()->getName()));

    if (reply == QMessageBox::Yes) {
        editor->reload();
    }
}

void MainWindow::closeCurrentFile()
{
    closeFile(dockedEditor->getCurrentEditor());
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

        QString message = QString("Save file <b>%1</b>?").arg(editor->scintillaBuffer()->getName());
        auto reply = QMessageBox::question(this, "Save File", message, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

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
}

void MainWindow::closeAllFiles(bool forceClose = false)
{
    if (!forceClose) {
        if (!checkEditorsBeforeClose(dockedEditor->editors())) {
            return;
        }
    }

    // Ask the buffer manager to close the buffers the dockedEditor knows about
    foreach (ScintillaNext *editor, dockedEditor->editors()) {
        editor->close();
    }

    if (!forceClose)
        newFile();
}

void MainWindow::closeAllExceptActive()
{
    auto e = dockedEditor->getCurrentEditor();
    auto editors = dockedEditor->editors();

    editors.removeOne(e);

    if (checkEditorsBeforeClose(editors)) {
        foreach (ScintillaNext *editor, editors) {
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
        foreach (ScintillaNext *editor, editors) {
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
        foreach (ScintillaNext *editor, editors) {
            editor->close();
        }
    }
}

bool MainWindow::saveCurrentFile()
{
    return saveFile(dockedEditor->getCurrentEditor());
}

bool MainWindow::saveFile(ScintillaNext *editor)
{
    if (editor->modify())
        return true;

    if (!editor->isFile()) {
        // Switch to the buffer and show the saveas dialog
        dockedEditor->switchToEditor(editor);
        return saveCurrentFileAsDialog();
    }
    else {
        bool didItGetSaved = editor->save();
        if (didItGetSaved) {
            return true;
        }
    }

    return false;
}

bool MainWindow::saveCurrentFileAsDialog()
{
    QString dialogDir = QString();
    QString filter = app->getFileDialogFilter();
    auto editor = dockedEditor->getCurrentEditor();
    auto buffer = editor->scintillaBuffer();

    // Use the file path if possible
    if (editor->isFile()) {
        dialogDir = buffer->fileInfo.canonicalFilePath();
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, // parent
        Q_NULLPTR, // caption
        dialogDir, // dir
        filter, // filter
        Q_NULLPTR // selected filter
        );

    if (fileName.size() == 0) {
        return false;
    }

    // TODO: distinguish between the above case (i.e. the user cancels the dialog) and a failure
    // calling editor->saveAs() as it might fail.

    return saveFileAs(editor, fileName);
}

bool MainWindow::saveCurrentFileAs(const QString &fileName)
{
    return saveFileAs(dockedEditor->getCurrentEditor(), fileName);
}

bool MainWindow::saveFileAs(ScintillaNext *editor, const QString &fileName)
{
    qInfo("saveFileAs(%s)", qUtf8Printable(fileName));

    bool didItGetSaved = editor->saveAs(fileName);

    return didItGetSaved;
}

void MainWindow::saveCopyAsDialog()
{
    QString dialogDir = QString();
    QString filter = app->getFileDialogFilter();
    auto editor = dockedEditor->getCurrentEditor();

    // Use the file path if possible
    if (editor->isFile()) {
        dialogDir = editor->scintillaBuffer()->fileInfo.canonicalFilePath();
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, // parent
        Q_NULLPTR, // caption
        dialogDir, // dir
        filter, // filter
        Q_NULLPTR // selected filter
        );

    saveCopyAs(fileName);
}

void MainWindow::saveCopyAs(const QString &fileName)
{
    auto editor = dockedEditor->getCurrentEditor();
    editor->saveCopyAs(fileName);
}

void MainWindow::saveAll()
{
    foreach (ScintillaNext *editor, dockedEditor->editors()) {
        saveFile(editor);
    }
}

void MainWindow::renameFile()
{
    auto editor = dockedEditor->getCurrentEditor();

    Q_ASSERT(editor->isFile());

    QString fileName = QFileDialog::getSaveFileName(this, "", editor->scintillaBuffer()->fileInfo.canonicalFilePath());

    if (fileName.size() == 0) {
        return;
    }

    // TODO
    // The new fileName might be to one of the existing buffers.
    //auto otherBuffer = app->getBufferManager()->getBufferByFilePath(fileName);
    //bool renameSuccessful = app->getBufferManager()->renameBuffer(buffer, fileName);
    //if (renameSuccessful && otherBuffer) {
    //    app->getBufferManager()->closeBuffer(otherBuffer);
    //}
}

void MainWindow::convertEOLs(int eolMode)
{
    ScintillaNext *editor = dockedEditor->getCurrentEditor();
    editor->convertEOLs(eolMode);
    editor->setEOLMode(eolMode);
    updateEOLBasedUi(editor);
}

void MainWindow::updateFileStatusBasedUi(ScintillaNext *editor)
{
    bool isFile = editor->isFile();

    QString title = QString("[*]%1 - Notepad Next").arg(editor->scintillaBuffer()->getName());
    setWindowTitle(title);

    ui->actionReload->setEnabled(isFile);
    ui->actionRename->setEnabled(isFile);
    ui->actionCopyFullPath->setEnabled(isFile);
    ui->actionCopyFileDirectory->setEnabled(isFile);
}

bool MainWindow::isAnyUnsaved()
{
    foreach (ScintillaNext *editor, dockedEditor->editors()) {
        if (!editor->isSavedToDisk()) {
            return true;
        }
    }

    return false;
}

void MainWindow::updateEOLBasedUi(ScintillaNext *editor)
{
    switch(editor->eOLMode()) {
    case SC_EOL_CR:
        eolFormat->setText(ui->actionMacintosh->text());
        ui->actionMacintosh->setChecked(true);
        break;
    case SC_EOL_CRLF:
        eolFormat->setText(ui->actionWindows->text());
        ui->actionWindows->setChecked(true);
        break;
    case SC_EOL_LF:
        eolFormat->setText(ui->actionUnix->text());
        ui->actionUnix->setChecked(true);
        break;
    }
}

void MainWindow::updateEncodingBasedUi(ScintillaNext *editor)
{
    switch(editor->codePage()) {
    case 0:
        unicodeType->setText("ANSI");
        break;
    case SC_CP_UTF8:
        unicodeType->setText("UTF-8");
        break;
    default:
        unicodeType->setText(QString::number(editor->codePage()));
        break;
    }
}

void MainWindow::updateSaveStatusBasedUi(bool isDirty)
{
    setWindowModified(isDirty);

    ui->actionSave->setEnabled(isDirty);
    ui->actionSaveAll->setEnabled(isDirty || isAnyUnsaved());

    if (dockedEditor->count() == 1) {
        ScintillaNext *editor = dockedEditor->getCurrentEditor();
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
    ui->actionCloseAllExceptActive->setEnabled(dockedEditor->count() > 1);
}

void MainWindow::updateLanguageBasedUi(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    const QString language_name = editor->property("nn.meta.language");

    foreach (QAction *action, languageActionGroup->actions()) {
        if (action->data().toString() == language_name) {
            action->setChecked(true);
            docType->setText(action->text());
            break;
        }
    }
}

void MainWindow::updateGui(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    updateFileStatusBasedUi(editor);
    updateSaveStatusBasedUi(!editor->isSavedToDisk());
    updateEOLBasedUi(editor);
    updateEncodingBasedUi(editor);
    updateEditorPositionBasedUi();
    updateSelectionBasedUi(editor);
    updateContentBasedUi(editor);
    updateLanguageBasedUi(editor);
}

void MainWindow::updateDocumentBasedUi(int updated)
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());

    // TODO: what if this is triggered by an editor that is not the active editor?

    if (updated & SC_UPDATE_CONTENT) {
        updateSelectionBasedUi(editor);
    }

    if (updated & (SC_UPDATE_CONTENT | SC_UPDATE_SELECTION)) {
        updateContentBasedUi(editor);

    }
}

void MainWindow::updateSelectionBasedUi(ScintillaNext *editor)
{
    ui->actionUndo->setEnabled(editor->canUndo());
    ui->actionRedo->setEnabled(editor->canRedo());

    QString sizeText = QString("Length: %1    Lines: %2").arg(
            QLocale::system().toString(editor->length()),
            QLocale::system().toString(editor->lineCount()));
    docSize->setText(sizeText);
}

void MainWindow::updateContentBasedUi(ScintillaNext *editor)
{
    bool hasAnySelections = !editor->selectionEmpty();

    ui->actionDelete->setEnabled(hasAnySelections);
    ui->actionPaste->setEnabled(editor->canPaste());

    ui->actionLowerCase->setEnabled(hasAnySelections);
    ui->actionUpperCase->setEnabled(hasAnySelections);

    QString selectionText;
    if (editor->selections() > 1) {
        selectionText = "Sel: N/A";
    }
    else {
        int start = editor->selectionStart();
        int end = editor->selectionEnd();
        int lines = editor->lineFromPosition(end) - editor->lineFromPosition(start);

        if (end > start)
            lines++;

        selectionText = QString("Sel: %1 | %2").arg(
                QLocale::system().toString(editor->countCharacters(start, end)),
                QLocale::system().toString(lines));
    }

    const int pos = editor->currentPos();
    QString positionText = QString("Ln: %1    Col: %2    ").arg(
            QLocale::system().toString(editor->lineFromPosition(pos) + 1),
            QLocale::system().toString(editor->column(pos) + 1));
    docPos->setText(positionText + selectionText);
}

void MainWindow::detectLanguageFromExtension(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    // Only real files have extensions
    if (!editor->isFile()) {
        editor->setLexer(SCLEX_NULL);
        return;
    }

    // See if it already has a language
    if (editor->lexerLanguage() != "")
        return;

    const QString ext = editor->scintillaBuffer()->fileInfo.suffix();

    QString language_name = app->getLuaState()->executeAndReturn<QString>(QString(R"(
local ext = "%1"
for name, L in pairs(languages) do
    for _, v in ipairs(L.extensions) do
        if v == ext then
            return name
        end
    end
end
return "null"
)").arg(ext).toLatin1().constData());

    setLanguage(editor, language_name);

    return;
}

void MainWindow::editorActivated(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    checkFileForModification(editor);
    updateGui(editor);
}

void MainWindow::setLanguage(ScintillaNext *editor, const QString &languageName)
{
    qInfo(Q_FUNC_INFO);

    LuaExtension::Instance().setEditor(editor);

    app->getLuaState()->execute(QString("lexer = \"%1\"").arg(QString(languageName)).toLatin1().constData());
    app->getLuaState()->execute(R"(
        local L = languages[lexer]
        -- this resets the style definitions but keeps
        -- the "wanted" stuff, such as line numbers, etc
        -- resetEditorStyle()
        editor.LexerLanguage = L.lexer

        editor.UseTabs = (L.tabSettings or "tabs") == "tabs"
        editor.TabWidth = L.tabSize or 4
        if L.styles then
            for name, style in pairs(L.styles) do
                editor.StyleFore[style.id] = style.fgColor
                editor.StyleBack[style.id] = style.bgColor
                if style.fontStyle then
                    if style.fontStyle & 1 == 1 then
                        editor.StyleBold[style.id] = true
                    end
                    if style.fontStyle & 2 == 2 then
                        editor.StyleItalic[style.id] = true
                    end
                end
            end
        end
        if L.keywords then
            for id, kw in pairs(L.keywords) do
                editor.KeyWords[id] = kw
            end
        end
        if L.properties then
            for p,v in pairs(L.properties) do
                editor.Property[p] = v
            end
        end
        editor.Property["fold"] = "1"
        editor.Property["fold.compact"] = "0"

        -- The document needs redone, but don't force it to do the whole thing
        -- since SC_IDLESTYLING_TOVISIBLE is used
        editor:Colourise(0, 1);
        )");

    editor->setProperty("nn.meta.language", languageName.toLatin1().constData());
    updateLanguageBasedUi(editor);
}

void MainWindow::bringWindowToForeground()
{
    qInfo(Q_FUNC_INFO);

    //setWindowState(windowState() & ~Qt::WindowMinimized);
    //raise();
    //activateWindow();

    // Make sure the window isn't minimized
    // TODO: this always puts it in the "normal" state but it might have been maximized
    // before minimized...so either a flag needs stored or find a Qt call to do it appropriately
    if (isMinimized())
        showNormal();

#ifdef Q_OS_WIN
    // TODO: there doesn't seem to be a cross platform way to force the window
    // to the foreground. So this will need moved to a platform specific file

    HWND hWnd = reinterpret_cast<HWND>(effectiveWinId());
    if (hWnd) {
        // I have no idea what this does but it works mostly
        // https://www.codeproject.com/Articles/1724/Some-handy-dialog-box-tricks-tips-and-workarounds

        AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), nullptr), GetCurrentThreadId(), TRUE);

        SetForegroundWindow(hWnd);
        SetFocus(hWnd);

        AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), nullptr), GetCurrentThreadId(), FALSE);
    }
#endif
}

bool MainWindow::checkFileForModification(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    auto state = editor->scintillaBuffer()->checkForBufferStateChange();

    if (state == ScintillaBuffer::NoChange) {
        return false;
    }
    else if (state == ScintillaBuffer::Modified) {
        qInfo("ScintillaBuffer::Modified");
        editor->reload();
    }
    else if (state == ScintillaBuffer::Deleted) {
        qInfo("ScintillaBuffer::Deleted");
    }
    else if (state == ScintillaBuffer::Restored) {
        qInfo("ScintillaBuffer::Restored");
    }

    return true;
}

void MainWindow::focusIn()
{
    qInfo(Q_FUNC_INFO);

    if (checkFileForModification(dockedEditor->getCurrentEditor())) {
        updateGui(dockedEditor->getCurrentEditor());
    }
}

void MainWindow::addEditor(ScintillaNext *editor)
{
    dockedEditor->addEditor(editor);
    connectEditor(editor);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!checkEditorsBeforeClose(dockedEditor->editors())) {
        event->ignore();
        return;
    }

    // While tabs are being closed, turn off UI updates so the main window doesn't continuously refresh.
    disconnect(dockedEditor, &DockedEditor::editorActivated, this, &MainWindow::editorActivated);

    closeAllFiles(true);

    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());

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
        auto urls = event->mimeData()->urls();
        foreach(QUrl url, urls) {
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
        dockedEditor->getCurrentEditor()->setText(event->mimeData()->text().toLocal8Bit().constData());
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
    auto editor = dockedEditor->getCurrentEditor();
    QVariant v = act->data();

    setLanguage(editor, v.toString());
}
