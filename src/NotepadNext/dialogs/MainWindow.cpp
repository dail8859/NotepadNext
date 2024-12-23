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
#include "BookMarkDecorator.h"
#include "URLFinder.h"
#include "SessionManager.h"
#include "UndoAction.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QClipboard>
#include <QStandardPaths>
#include <QWindow>
#include <QPushButton>
#include <QTimer>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QDirIterator>
#include <QProcess>
#include <QScreen>
#include <QFontDatabase>


#ifdef Q_OS_WIN
#include <QSimpleUpdater.h>
#include <Windows.h>
#endif

#include "DockAreaWidget.h"

#include "NotepadNextApplication.h"
#include "ApplicationSettings.h"

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
#include "FileListDock.h"

#include "FindReplaceDialog.h"
#include "MacroRunDialog.h"
#include "MacroSaveDialog.h"
#include "PreferencesDialog.h"
#include "ColumnEditorDialog.h"

#include "QuickFindWidget.h"

#include "EditorPrintPreviewRenderer.h"
#include "MacroEditorDialog.h"

#include "ZoomEventWatcher.h"
#include "FileDialogHelpers.h"

#include "HtmlConverter.h"
#include "RtfConverter.h"

#include "FadingIndicator.h"


MainWindow::MainWindow(NotepadNextApplication *app) :
    ui(new Ui::MainWindow),
    app(app),
    zoomEventWatcher(new ZoomEventWatcher(this))
{
    qInfo(Q_FUNC_INFO);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    applyCustomShortcuts();

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

#ifdef Q_OS_WIN
    ui->actionExit->setShortcut(QKeySequence("Alt+F4"));
#else
    ui->actionExit->setShortcut(QKeySequence::Quit);
#endif

    connect(ui->actionOpenFolderasWorkspace, &QAction::triggered, this, &MainWindow::openFolderAsWorkspaceDialog);

    connect(ui->actionCloseAllExceptActive, &QAction::triggered, this, &MainWindow::closeAllExceptActive);
    connect(ui->actionCloseAllToLeft, &QAction::triggered, this, &MainWindow::closeAllToLeft);
    connect(ui->actionCloseAllToRight, &QAction::triggered, this, &MainWindow::closeAllToRight);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveCurrentFileAsDialog);
    connect(ui->actionSaveCopyAs, &QAction::triggered, this, &MainWindow::saveCopyAsDialog);
    connect(ui->actionSaveAll, &QAction::triggered, this, &MainWindow::saveAll);
    connect(ui->actionRename, &QAction::triggered, this, &MainWindow::renameFile);

    connect(ui->actionExportHtml, &QAction::triggered, this, [=]() {
        HtmlConverter html(currentEditor());
        exportAsFormat(&html, QStringLiteral("HTML files (*.html)"));
    });

    connect(ui->actionExportRtf, &QAction::triggered, this, [=]() {
        RtfConverter rtf(currentEditor());
        exportAsFormat(&rtf, QStringLiteral("RTF Files (*.rtf)"));
    });

    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);

    connect(ui->actionToggleSingleLineComment, &QAction::triggered, this, [=]() { currentEditor()->toggleCommentSelection(); });
    connect(ui->actionSingleLineComment, &QAction::triggered, this, [=]() { currentEditor()->commentLineSelection(); });
    connect(ui->actionSingleLineUncomment, &QAction::triggered, this, [=]() { currentEditor()->uncommentLineSelection(); });

    connect(ui->actionBase64Encode,&QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = currentEditor()->getSelText();
        editor->replaceSel(selection.toBase64().constData());
    });
    connect(ui->actionURLEncode,&QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = currentEditor()->getSelText();
        editor->replaceSel(selection.toPercentEncoding().constData());
    });
    connect(ui->actionBase64Decode,&QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        if (auto result = QByteArray::fromBase64Encoding(selection)) {
            editor->replaceSel((*result).constData());
        }
    });
    connect(ui->actionURLDecode,&QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        editor->replaceSel(QByteArray::fromPercentEncoding(selection).constData());
    });
    connect(ui->actionCopyURL, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        URLFinder *urlFinder = editor->findChild<URLFinder *>(QString(), Qt::FindDirectChildrenOnly);
        if (urlFinder && urlFinder->isEnabled()) {
            urlFinder->copyURLToClipboard(contextMenuPos);
        }
    });

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
    connect(ui->actionRemoveEmptyLines, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        Finder f(editor);
        const UndoAction ua(editor);

        f.setSearchText(QStringLiteral("\\R\\R+"));
        f.setSearchFlags(SCFIND_REGEXP);
        f.replaceAll(editor->eolString());

        // The regex will not entirely remove a blank first line
        editor->deleteLeadingEmptyLines();

        // Regex will also not delete the final blank line
        editor->deleteTrailingEmptyLines();
    });

    connect(ui->actionColumnMode, &QAction::triggered, this, [=]() {
        ColumnEditorDialog *columnEditor = findChild<ColumnEditorDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (columnEditor == Q_NULLPTR) {
            columnEditor = new ColumnEditorDialog(this);
        }

        columnEditor->show();
        columnEditor->raise();
        columnEditor->activateWindow();
    });

    connect(ui->actionUndo, &QAction::triggered, this, [=]() { currentEditor()->undo(); });
    connect(ui->actionRedo, &QAction::triggered, this, [=]() { currentEditor()->redo(); });
    connect(ui->actionCut, &QAction::triggered, this, [=]() { currentEditor()->cutAllowLine(); });
    connect(ui->actionCopy, &QAction::triggered, this, [=]() { currentEditor()->copyAllowLine(); });
    connect(ui->actionDelete, &QAction::triggered, this, [=]() { currentEditor()->clear(); });
    connect(ui->actionPaste, &QAction::triggered, this, [=]() { currentEditor()->paste(); });
    connect(ui->actionSelectAll, &QAction::triggered, this, [=]() { currentEditor()->selectAll(); });
    connect(ui->actionSelectNext, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();

        editor->setSearchFlags(SCFIND_NONE);
        editor->targetWholeDocument();
        editor->multipleSelectAddNext();
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

    connect(ui->actionCopyAsHtml, &QAction::triggered, this, [=]() {
        HtmlConverter html(currentEditor());
        copyAsFormat(&html, "text/html");
    });

    connect(ui->actionCopyAsRtf, &QAction::triggered, this, [=]() {
        RtfConverter rtf(currentEditor());
        copyAsFormat(&rtf, "Rich Text Format");
    });

    connect(ui->actionIncrease_Indent, &QAction::triggered, this, [=]() { currentEditor()->tab(); });
    connect(ui->actionDecrease_Indent, &QAction::triggered, this, [=]() { currentEditor()->backTab(); });

    addAction(ui->actionToggleOverType);
    connect(ui->actionToggleOverType, &QAction::triggered, this, [=]() {
        currentEditor()->editToggleOvertype();
        ui->statusBar->refresh(currentEditor());
    });

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
        FindReplaceDialog *f = findChild<FindReplaceDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (f) {
            f->performLastSearch();
        }
    });

    connect(ui->actionQuickFind, &QAction::triggered, this, [=]() {
        QuickFindWidget *quickFind = findChild<QuickFindWidget *>(QString(), Qt::FindDirectChildrenOnly);

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

    connect(ui->actionToggleBookmark, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            editor->forEachLineInSelection(editor->mainSelection(), [&](int line) {
                bookMarkDecorator->toggleBookmark(line);
            });
        }
    });

    connect(ui->actionNextBookmark, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            int currentLine = editor->lineFromPosition(editor->currentPos());
            int nextBookmarkedLine = bookMarkDecorator->nextBookmarkAfter(currentLine + 1);

            if (nextBookmarkedLine != -1) {
                editor->ensureVisibleEnforcePolicy(nextBookmarkedLine);
                editor->gotoLine(nextBookmarkedLine);
            }
        }
    });

    connect(ui->actionClearBookmarks, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            bookMarkDecorator->clearAllBookmarks();
        }
    });

    connect(ui->actionInvertBookmarks, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            for (int line = 0; line < editor->lineCount(); line++) {
                bookMarkDecorator->toggleBookmark(line);
            }
        }
    });

    connect(ui->actionPreviousBookmark, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            int currentLine = editor->lineFromPosition(editor->currentPos());
            int prevBookmarkedLine = bookMarkDecorator->previousBookMarkBefore(currentLine - 1);

            if (prevBookmarkedLine != -1) {
                editor->ensureVisibleEnforcePolicy(prevBookmarkedLine);
                editor->gotoLine(prevBookmarkedLine);
            }
        }
    });

    connect(ui->actionCutBookmarkedLines, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            QString s = bookMarkDecorator->cutBookMarkedLines();

            if (!s.isEmpty()) {
                QApplication::clipboard()->setText(s);
            }
        }
    });

    connect(ui->actionCopyBookmarkedLines, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            QString s = bookMarkDecorator->copyBookMarkedLines();

            if (!s.isEmpty()) {
                QApplication::clipboard()->setText(s);
            }
        }
    });

    connect(ui->actionDeleteBookmarkedLines, &QAction::triggered, this, [=]() {
        ScintillaNext *editor = currentEditor();
        BookMarkDecorator *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            bookMarkDecorator->deleteBookMarkedLines();
        }
    });


    // The action needs added to the window so it can be triggered via the keyboard
    addAction(ui->actionNextTab);
    connect(ui->actionNextTab, &QAction::triggered, this, [=]() {
        int index = dockedEditor->currentDockArea()->currentIndex();
        int total = dockedEditor->currentDockArea()->dockWidgetsCount();

        index++;
        dockedEditor->currentDockArea()->setCurrentIndex(index < total ? index : 0);
    });

    // The action needs added to the window so it can be triggered via the keyboard
    addAction(ui->actionPreviousTab);
    connect(ui->actionPreviousTab, &QAction::triggered, this, [=]() {
        int index = dockedEditor->currentDockArea()->currentIndex();
        int total = dockedEditor->currentDockArea()->dockWidgetsCount();

        index--;
        dockedEditor->currentDockArea()->setCurrentIndex(index >= 0 ? index : total - 1);
    });

    ui->pushExitFullScreen->setParent(this); // This is important
    ui->pushExitFullScreen->setVisible(false);
    connect(ui->pushExitFullScreen, &QPushButton::clicked, ui->actionFullScreen, &QAction::trigger);
    connect(ui->actionFullScreen, &QAction::triggered, this, [=](bool b) {
        static bool wasMaximized;

        if (b) {
            // NOTE: don't hide() these as it will cancel their actions they hold
            ui->menuBar->setMaximumHeight(0);
            ui->mainToolBar->setMaximumHeight(0);

            wasMaximized = isMaximized();
            if (wasMaximized) {
                // By default when calling showMaximized() from a full screen state, the window will resize
                // to its "normal" size and then immediately resize to the "maximized" size which is very ugly.
                // By calling setGeometry() to the size of the screen, it at least alleviates the ugly animation
                // going from: fullscreen -> small "normal" size -> full size of screen
                setGeometry(screen()->availableGeometry());
            }

            showFullScreen();

            ui->pushExitFullScreen->setGeometry(width() - 20, 0, 20, 20);
            ui->pushExitFullScreen->show();
            ui->pushExitFullScreen->raise();
        }
        else {
            ui->menuBar->setMaximumHeight(QWIDGETSIZE_MAX);
            ui->mainToolBar->setMaximumHeight(QWIDGETSIZE_MAX);

            if (wasMaximized)
                showMaximized();
            else
                showNormal();

            ui->pushExitFullScreen->hide();
        }
    });


    // Show All Characters is just a short cut to toggle whitespace and EOL on
    ui->actionShowAllCharacters->setChecked(app->getSettings()->showWhitespace() && app->getSettings()->showEndOfLine());
    connect(ui->actionShowAllCharacters, &QAction::triggered, app->getSettings(), &ApplicationSettings::setShowWhitespace);
    connect(ui->actionShowAllCharacters, &QAction::triggered, app->getSettings(), &ApplicationSettings::setShowEndOfLine);

    // Show White Space
    ui->actionShowWhitespace->setChecked(app->getSettings()->showWhitespace());
    connect(app->getSettings(), &ApplicationSettings::showWhitespaceChanged, ui->actionShowWhitespace, &QAction::setChecked);
    connect(ui->actionShowWhitespace, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowWhitespace);
    // Update the "Show All Character" action
    connect(ui->actionShowWhitespace, &QAction::toggled, this, [=](bool b) {
        ui->actionShowAllCharacters->setChecked(b && ui->actionShowEndofLine->isChecked());
    });

    // Show EOL
    ui->actionShowEndofLine->setChecked(app->getSettings()->showEndOfLine());
    connect(app->getSettings(), &ApplicationSettings::showEndOfLineChanged, ui->actionShowEndofLine, &QAction::setChecked);
    connect(ui->actionShowEndofLine, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowEndOfLine);
    // Update the "Show All Character" action
    connect(ui->actionShowEndofLine, &QAction::toggled, this, [=](bool b) {
        ui->actionShowAllCharacters->setChecked(b && ui->actionShowWhitespace->isChecked());
    });

    // Show Wrap Symbol
    ui->actionShowWrapSymbol->setChecked(app->getSettings()->showWrapSymbol());
    connect(app->getSettings(), &ApplicationSettings::showWrapSymbolChanged, ui->actionShowWrapSymbol, &QAction::setChecked);
    connect(ui->actionShowWrapSymbol, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowWrapSymbol);

    // Show Indentation Guide
    ui->actionShowIndentGuide->setChecked(app->getSettings()->showIndentGuide());
    connect(app->getSettings(), &ApplicationSettings::showIndentGuideChanged, ui->actionShowIndentGuide, &QAction::setChecked);
    connect(ui->actionShowIndentGuide, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowIndentGuide);

    // Word Wrap
    ui->actionWordWrap->setChecked(app->getSettings()->wordWrap());
    connect(app->getSettings(), &ApplicationSettings::wordWrapChanged, ui->actionWordWrap, &QAction::setChecked);
    connect(ui->actionWordWrap, &QAction::toggled, app->getSettings(), &ApplicationSettings::setWordWrap);

    // Zooming controls all editors simulaneously
    connect(ui->actionZoomIn, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->zoomIn();
        }
        zoomLevel = currentEditor()->zoom();

        showEditorZoomLevelIndicator();
    });
    connect(ui->actionZoomOut, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->zoomOut();
        }
        zoomLevel = currentEditor()->zoom();

        showEditorZoomLevelIndicator();
    });
    connect(ui->actionZoomReset, &QAction::triggered, this, [=]() {
        for (ScintillaNext *editor : editors()) {
            editor->setZoom(0);
        }
        zoomLevel = 0;

        showEditorZoomLevelIndicator();
    });

    // Zoom watcher has detected a zoom event, so just trigger the UI action
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomIn, ui->actionZoomIn, &QAction::trigger);
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomOut, ui->actionZoomOut, &QAction::trigger);

    connect(ui->actionFoldAll, &QAction::triggered, this, [=]() { currentEditor()->foldAll(SC_FOLDACTION_CONTRACT | SC_FOLDACTION_CONTRACT_EVERY_LEVEL); });
    connect(ui->actionUnfoldAll, &QAction::triggered, this, [=]() { currentEditor()->foldAll(SC_FOLDACTION_EXPAND | SC_FOLDACTION_CONTRACT_EVERY_LEVEL); });

    connect(ui->actionFoldLevel1, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(0); });
    connect(ui->actionFoldLevel2, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(1); });
    connect(ui->actionFoldLevel3, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(2); });
    connect(ui->actionFoldLevel4, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(3); });
    connect(ui->actionFoldLevel5, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(4); });
    connect(ui->actionFoldLevel6, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(5); });
    connect(ui->actionFoldLevel7, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(6); });
    connect(ui->actionFoldLevel8, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(7); });
    connect(ui->actionFoldLevel9, &QAction::triggered, this, [=]() { currentEditor()->foldAllLevels(8); });

    connect(ui->actionUnfoldLevel1, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(0); });
    connect(ui->actionUnfoldLevel2, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(1); });
    connect(ui->actionUnfoldLevel3, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(2); });
    connect(ui->actionUnfoldLevel4, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(3); });
    connect(ui->actionUnfoldLevel5, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(4); });
    connect(ui->actionUnfoldLevel6, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(5); });
    connect(ui->actionUnfoldLevel7, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(6); });
    connect(ui->actionUnfoldLevel8, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(7); });
    connect(ui->actionUnfoldLevel9, &QAction::triggered, this, [=]() { currentEditor()->unFoldAllLevels(8); });

    languageActionGroup = new QActionGroup(this);
    languageActionGroup->setExclusive(true);

    connect(ui->actionPreferences, &QAction::triggered, this, [=] {
        PreferencesDialog *pd = findChild<PreferencesDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (pd == Q_NULLPTR) {
            pd = new PreferencesDialog(app->getSettings(), this);
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
        MacroRunDialog *macroRunDialog = findChild<MacroRunDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (macroRunDialog == Q_NULLPTR) {
            macroRunDialog = new MacroRunDialog(this, &macroManager);

            connect(macroRunDialog, &MacroRunDialog::execute, this, [=](Macro *macro, int times) {
                if (times > 0)
                    macro->replay(currentEditor(), times);
                else if (times == -1)
                    macro->replayTillEndOfFile(currentEditor());
            });
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
                            QStringLiteral("<h3>%1 v%2 %3</h3>"
                                    "<p>%4</p>"
                                    "<p><a href=\"https://github.com/dail8859/NotepadNext\">Notepad Next Home Page</a></p>"
                                    R"(<p>This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.</p> <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.</p> <p>You should have received a copy of the GNU General Public License along with this program. If not, see &lt;<a href="https://www.gnu.org/licenses/">https://www.gnu.org/licenses/</a>&gt;.</p>)")
                                .arg(QApplication::applicationDisplayName(), APP_VERSION, APP_DISTRIBUTION, QStringLiteral(APP_COPYRIGHT).toHtmlEscaped()));
    });

    connect(ui->actionDebugInfo, &QAction::triggered, this, [=]() {
        QMessageBox mb(QMessageBox::Information, tr("Debug Info"), app->debugInfo().join('\n'), QMessageBox::Ok, this);

        mb.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family());
        mb.setTextInteractionFlags(Qt::TextSelectableByMouse);

        mb.exec();
    });

#ifdef Q_OS_WIN
    connect(ui->actionShowInExplorer, &QAction::triggered, this, [=]() {
        QString filePath = QDir::toNativeSeparators(currentEditor()->getFileInfo().canonicalFilePath());
        QStringList arguments = {"/select,", filePath};
        QProcess::startDetached("explorer", arguments);
    });

    QString terminalName = app->getSettings()->value("App/TerminalName", "Command Prompt").toString();
    ui->actionOpenTerminalHere->setText(ui->actionOpenTerminalHere->text().arg(terminalName));

    connect(ui->actionOpenTerminalHere, &QAction::triggered, this, [=]() {
        QString command = app->getSettings()->value("App/TerminalCommand", "cmd").toString();
        QString filePath = QDir::toNativeSeparators(currentEditor()->getFileInfo().dir().canonicalPath());
        QStringList arguments = {"/c", "start", "/d", filePath, command};
        QProcess::startDetached("cmd", arguments);
    });
#endif

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

    FileListDock *fileListDock = new FileListDock(this);
    fileListDock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, fileListDock);
    ui->menuView->addAction(fileListDock->toggleViewAction());

    connect(app->getSettings(), &ApplicationSettings::showMenuBarChanged, this, [=](bool showMenuBar) {
        // Don't 'hide' it, else the actions won't be enabled
        ui->menuBar->setMaximumHeight(showMenuBar ? QWIDGETSIZE_MAX : 0);
    });
    connect(app->getSettings(), &ApplicationSettings::showToolBarChanged, ui->mainToolBar, &QToolBar::setVisible);
    connect(app->getSettings(), &ApplicationSettings::showStatusBarChanged, ui->statusBar, &QStatusBar::setVisible);

    // It seems restoreState() does not affect the status bar so set it manually
    ui->statusBar->setVisible(app->getSettings()->showStatusBar());

    setupLanguageMenu();

    applyStyleSheet();

    restoreSettings();

    initUpdateCheck();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyCustomShortcuts()
{
    ApplicationSettings *settings = app->getSettings();

    settings->beginGroup("Shortcuts");

    for (const QString &actionName : settings->childKeys()) {
        QAction *action = findChild<QAction *>(QStringLiteral("action") + actionName, Qt::FindDirectChildrenOnly);
        const QString shortcutString = settings->value(actionName).toString();

        if (action != Q_NULLPTR) {
            action->setShortcut(QKeySequence(shortcutString));
        }
        else {
            qWarning() << "Cannot find action" << actionName;
        }
    }

    settings->endGroup();
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
    // NOTE: this will need re-evaluated in the future.
    // So far it has been assumed 1 ScintillaNext instance is 1 DockedEditor widget instance.
    // If in the future a ScintillaNext can be cloned then the DockedEditor could return
    // the same ScintillaNext instance multiple times since 1 ScintillaNext could mean >= 1 DockedEditor widget instance
    return dockedEditor->editors();
}

void MainWindow::newFile()
{
    qInfo(Q_FUNC_INFO);

    static int count = 1;

    // NOTE: in theory need to check all editors in the editorManager to future proof this.
    // If there is another window it would need to check those too to see if New X exists. The editor
    // manager would encompass all editors

    forever {
        QString newFileName = tr("New %1").arg(count++);
        bool canUseName = true;

        for (const ScintillaNext *editor : editors()) {
            if (!editor->isFile() && editor->getName() == newFileName) {
                canUseName = false;
                break;
            }
        }

        if (canUseName) {
            app->getEditorManager()->createEditor(newFileName);
            break;
        }
    }
}

// One unedited, new blank document
ScintillaNext *MainWindow::getInitialEditor()
{
    if (editorCount() == 1) {
        ScintillaNext *editor = currentEditor();

        // If the editor:
        //   is a temporary file
        //   is a 'real' file (or a 'missing' file)
        //   can undo any actions
        //   can redo any actions
        // Then do not treat it as an 'initial editor' that can be transparently closed for the user
        if (editor->isTemporary() || editor->isFile() || editor->canUndo() || editor->canRedo()) {
            return Q_NULLPTR;
        }

        return editor;
    }

    return Q_NULLPTR;
}

void MainWindow::openFileList(const QStringList &fileNames)
{
    qInfo(Q_FUNC_INFO);

    if (fileNames.size() == 0)
        return;

    ScintillaNext *initialEditor = getInitialEditor();
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

        if (editor) {
            mostRecentEditor = editor;
        }
    }

    // If any were successful, switch to the last one
    if (mostRecentEditor) {
        dockedEditor->switchToEditor(mostRecentEditor);
    }

    if (initialEditor) {
        initialEditor->close();
    }
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
    // Early out. If we aren't exiting on last tab closed, and it exists, there's no point in continuing
    if (!app->getSettings()->exitOnLastTabClosed() && getInitialEditor() != Q_NULLPTR) {
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

    // If the last document was closed, figure out what to do next
    if (editorCount() == 0) {
        if (app->getSettings()->exitOnLastTabClosed()) {
            close();
        }
        else {
            newFile();
        }
    }
}

void MainWindow::closeAllFiles()
{
    if (!checkEditorsBeforeClose(editors())) {
        return;
    }

    // Ask the manager to close the editors the dockedEditor knows about
    for (ScintillaNext *editor : editors()) {
        editor->close();
    }

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
    if (editor->isSavedToDisk())
        return true;

    if (!editor->isFile()) {
        // Switch to the editor and show the saveas dialog
        dockedEditor->switchToEditor(editor);
        return saveCurrentFileAsDialog();
    }
    else {
        QFileDevice::FileError error = editor->save();
        if (error == QFileDevice::NoError) {
            return true;
        }
        else {
            showSaveErrorMessage(editor, error);
            return false;
        }
    }
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

    QString selectedFilter = app->getFileDialogFilterForLanguage(editor->languageName);
    QString fileName = FileDialogHelpers::getSaveFileName(this, QString(), dialogDir, filter, &selectedFilter);

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

    QFileDevice::FileError error = editor->saveAs(fileName);

    if (error == QFileDevice::NoError) {
        return true;
    }
    else {
        showSaveErrorMessage(editor, error);
        return false;
    }
}

bool MainWindow::saveCopyAsDialog()
{
    QString dialogDir;
    const QString filter = app->getFileDialogFilter();
    const ScintillaNext* editor = currentEditor();

    // Use the file path if possible
    if (editor->isFile()) {
        dialogDir = editor->getFilePath();
    }

    QString selectedFilter = app->getFileDialogFilterForLanguage(editor->languageName);
    QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Save a Copy As"), dialogDir, filter, &selectedFilter);

    if (fileName.size() == 0) {
        return false;
    }

    return saveCopyAs(fileName);
}

bool MainWindow::saveCopyAs(const QString &fileName)
{
    auto editor = currentEditor();

    QFileDevice::FileError error = editor->saveCopyAs(fileName);

    if (error == QFileDevice::NoError) {
        return true;
    }
    else {
        showSaveErrorMessage(editor, error);
        return false;
    }
}

void MainWindow::saveAll()
{
    for (ScintillaNext *editor : editors()) {
        saveFile(editor);
    }
}

void MainWindow::exportAsFormat(Converter *converter, const QString &filter)
{
    const QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Export As"), QString(), filter + ";;All files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile f(fileName);

    f.open(QIODevice::WriteOnly);

    QTextStream s(&f);
    converter->convert(s);
    f.close();
}

void MainWindow::copyAsFormat(Converter *converter, const QString &mimeType)
{
    // This is not ideal as we are *assuming* the converter is currently associated with the currentEditor()
    ScintillaNext *editor = currentEditor();
    QByteArray buffer;
    QTextStream stream(&buffer);

    if (editor->selectionEmpty())
        converter->convert(stream);
    else {
        converter->convertRange(stream, editor->selectionStart(), editor->selectionEnd());
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setData(mimeType, buffer);

    QApplication::clipboard()->setMimeData(mimeData);
}

void MainWindow::renameFile()
{
    ScintillaNext *editor = currentEditor();

    if (editor->isFile()) {
        const QString filter = app->getFileDialogFilter();
        QString selectedFilter = app->getFileDialogFilterForLanguage(editor->languageName);
        QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Rename"), editor->getFilePath(), filter, &selectedFilter);

        if (fileName.isEmpty()) {
            return;
        }

        // TODO
        // The new fileName might be to one of the existing editors.
        //auto otherEditor = app->getEditorByFilePath(fileName);

        bool renameSuccessful = editor->rename(fileName);
        Q_UNUSED(renameSuccessful)
    }
    else {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename"), tr("Name:"), QLineEdit::Normal, editor->getName(), &ok);

        if (ok && !text.isEmpty()) {
            editor->setName(text);
        }
    }
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
    auto reply = QMessageBox::question(this, tr("Delete File"), tr("Are you sure you want to move <b>%1</b> to the trash?").arg(filePath));

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
    FindReplaceDialog *frd = findChild<FindReplaceDialog *>(QString(), Qt::FindDirectChildrenOnly);

    if (frd == Q_NULLPTR) {
        frd = new FindReplaceDialog(determineSearchResultsHandler(), this);
    }
    else {
        frd->setSearchResultsHandler(determineSearchResultsHandler());
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

    QString title = QStringLiteral("[*]%1").arg(fileName);
    if (app->isRunningAsAdmin()) {
        title += QStringLiteral(" - [%1]").arg(tr("Administrator"));
    }
    setWindowTitle(title);

    ui->actionReload->setEnabled(isFile);
    ui->actionMoveToTrash->setEnabled(isFile);
    ui->actionCopyFullPath->setEnabled(isFile);
    ui->actionCopyFileDirectory->setEnabled(isFile);
    ui->actionShowInExplorer->setEnabled(isFile);
    ui->actionOpenTerminalHere->setEnabled(isFile);
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

    ui->actionBase64Encode->setEnabled(hasAnySelections);
    ui->actionURLEncode->setEnabled(hasAnySelections);
    ui->actionBase64Decode->setEnabled(hasAnySelections);
    ui->actionURLDecode->setEnabled(hasAnySelections);
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

void MainWindow::applyStyleSheet()
{
    qInfo(Q_FUNC_INFO);

    QString sheet;
    QFile f(":/stylesheets/npp.css");
    qInfo() << "Loading stylesheet:" << f.fileName();

    f.open(QFile::ReadOnly);
    sheet = f.readAll();
    f.close();

    // If there is a "custom.css" file where the ini is located, load it as a style sheet addition
    QString directoryPath = QFileInfo(app->getSettings()->fileName()).absolutePath();
    QString fullPath = QDir(directoryPath).filePath("custom.css");
    if (QFile::exists(fullPath)) {
        QFile custom(fullPath);
        qInfo() << "Loading stylesheet:" << custom.fileName();

        custom.open(QFile::ReadOnly);
        sheet += custom.readAll();
        custom.close();
    }

    setStyleSheet(sheet);
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
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();
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

void MainWindow::showSaveErrorMessage(ScintillaNext *editor, QFileDevice::FileError error)
{
    const QString name = editor->isFile() ? editor->getFilePath() : editor->getName();
    QMessageBox::warning(this, tr("Error Saving File"), tr("An error occurred when saving <b>%1</b><br><br>Error: %2").arg(name, qt_error_string(error)));
}

void MainWindow::showEditorZoomLevelIndicator()
{
    // Not sure if Scintilla's zoom level matches up to an exact percentage, but visibly this is close
    FadingIndicator::showText(currentEditor(), tr("Zoom: %1%").arg(zoomLevel * 10 + 100));
}

void MainWindow::saveSettings() const
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings *settings = app->getSettings();

    settings->setValue("MainWindow/geometry", saveGeometry());
    settings->setValue("MainWindow/windowState", saveState());

    settings->setValue("Editor/ZoomLevel", zoomLevel);
}

void MainWindow::restoreSettings()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings *settings = app->getSettings();

    zoomLevel = settings->value("Editor/ZoomLevel", 0).toInt();
}

ISearchResultsHandler *MainWindow::determineSearchResultsHandler()
{
    // Determine what will get the search results
    if (app->getSettings()->combineSearchResults()) {
        searchResults.reset(new SearchResultsCollector(findChild<SearchResultsDock *>()));

        return searchResults.data();
    }
    else {
        return findChild<SearchResultsDock *>();
    }
}

void MainWindow::restoreWindowState()
{
    ApplicationSettings *settings = app->getSettings();

    restoreGeometry(settings->value("MainWindow/geometry").toByteArray());
    restoreState(settings->value("MainWindow/windowState").toByteArray());

    // Always hide the dock no matter how the application was closed
    SearchResultsDock *srDock = findChild<SearchResultsDock *>();
    srDock->hide();
}

void MainWindow::switchToEditor(const ScintillaNext *editor)
{
    dockedEditor->switchToEditor(editor);
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

    // Watch for any zoom events (Ctrl+Scroll or pinch-to-zoom (Qt translates it as Ctrl+Scroll)) so that the event
    // can be handled before the ScintillaEditBase widget, so that it can be applied to all editors to keep zoom level equal.
    // NOTE: Need to install this on the scroll area's viewport, not on the editor widget itself...that was painful to learn
    editor->viewport()->installEventFilter(zoomEventWatcher);

    editor->setZoom(zoomLevel);

    editor->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(editor, &ScintillaNext::customContextMenuRequested, this, [=](const QPoint &pos) {
        contextMenuPos = editor->send(SCI_POSITIONFROMPOINT, pos.x(), pos.y());
        QMenu *menu = new QMenu(this);
        menu->setAttribute(Qt::WA_DeleteOnClose);

        URLFinder *urlFinder = editor->findChild<URLFinder *>(QString(), Qt::FindDirectChildrenOnly);
        if (urlFinder && urlFinder->isEnabled() && urlFinder->isURL(contextMenuPos)) {
            menu->addAction(ui->actionCopyURL);
            menu->addSeparator();
        }
        menu->addAction(ui->actionCut);
        menu->addAction(ui->actionCopy);
        menu->addAction(ui->actionPaste);
        menu->addAction(ui->actionDelete);
        menu->addSeparator();
        menu->addAction(ui->actionSelectAll);
        menu->addSeparator();
        menu->addAction(ui->actionBase64Encode);
        menu->addAction(ui->actionURLEncode);
        menu->addSeparator();
        menu->addAction(ui->actionBase64Decode);
        menu->addAction(ui->actionURLDecode);
        menu->popup(QCursor::pos());
    });

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


    app->getSettings()->setValue("App/LastUpdateCheck", QDateTime::currentDateTime());
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
            ApplicationSettings settings;
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
    const SessionManager *sessionManager = app->getSessionManager();
    QVector<ScintillaNext *> e;

    // Check all editors to see if the session manager will not handle it
    for (auto editor : editors()) {
        if (!sessionManager->willFileGetStoredInSession(editor)) {
            e.append(editor);
        }
    }

    if (!checkEditorsBeforeClose(e)) {
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
                QFileInfo info(url.toLocalFile());

                if (info.exists()) {
                    if (info.isDir()) {
                        QDirIterator it(url.toLocalFile(), QDir::Files, QDirIterator::FollowSymlinks| QDirIterator::Subdirectories);
                        while (it.hasNext()) {
                            fileNames << it.next();
                        }
                    }
                    else {
                        fileNames.append(url.toLocalFile());
                    }
                }
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

    // Create the menu
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    // Default actions
    QStringList actionNames{
        "Close",
        "CloseAllExceptActive",
        "CloseAllToLeft",
        "CloseAllToRight",
        "",
        "Save",
        "SaveAs",
        "Rename",
        "",
        "Reload",
        "",
#ifdef Q_OS_WIN
        "ShowInExplorer",
        "OpenTerminalHere",
        "",
#endif
        "CopyFullPath",
        "CopyFileName",
        "CopyFileDirectory"
    };

    // If the entry exists in the settings, use that
    ApplicationSettings *settings = app->getSettings();
    if (settings->contains("Gui/TabBarContextMenu")) {
        actionNames = settings->value("Gui/TabBarContextMenu").toStringList();
    }

    // Populate the menu
    for (const QString &actionName : actionNames) {
        if (actionName.isEmpty()) {
            menu->addSeparator();
        }
        else {
            QAction *a = findChild<QAction *>(QStringLiteral("action") + actionName, Qt::FindDirectChildrenOnly);

            if (a != Q_NULLPTR) {
                menu->addAction(a);
            }
            else {
                qWarning() << "Cannot locate menu named" << actionName;
            }
        }
    }

    // Show it
    menu->popup(QCursor::pos());
}

void MainWindow::languageMenuTriggered()
{
    const QAction *act = qobject_cast<QAction *>(sender());
    auto editor = currentEditor();
    QVariant v = act->data();

    setLanguage(editor, v.toString());
}
