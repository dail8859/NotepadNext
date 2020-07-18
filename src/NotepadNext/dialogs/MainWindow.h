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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>

#include "DockedEditor.h"

#include "ScintillaNext.h"
#include "BufferManager.h"
#include "StatusLabel.h"
#include "NppImporter.h"
#include "QuickFindWidget.h"

namespace Ui {
class MainWindow;
}

class NotepadNextApplication;
class ScintillaBuffer;
class FindReplaceDialog;
class RecentFilesListManager;
class LuaConsoleDock;
class MacroRecorder;
class MacroRunDialog;
class Macro;
class Settings;
class PreferencesDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(NotepadNextApplication *app, QWidget *parent = nullptr);
    ~MainWindow() override;

    void initialize(Settings *settings);

    bool isAnyUnsaved();

    void setupLanguageMenu();
    void setupEditor(ScintillaNext *editor);

public slots:
    void newFile();

    void openFileDialog();
    void openFile(const QString &filePath);

    void reloadFile();

    void closeCurrentFile();
    void closeFile(ScintillaBuffer *buffer);
    void closeAllFiles(bool forceClose);
    void closeAllExceptActive();
    void closeAllToLeft();
    void closeAllToRight();

    bool saveCurrentFile();
    bool saveFile(ScintillaBuffer *buffer);

    bool saveCurrentFileAsDialog();
    bool saveCurrentFileAs(const QString &fileName);
    bool saveFileAs(ScintillaBuffer *buffer, const QString &fileName);

    void saveCopyAsDialog();
    void saveCopyAs(const QString &fileName);
    void saveAll();

    void renameFile();
    void convertEOLs(int eolMode);

    void updateBufferFileStatusBasedUi(ScintillaBuffer *buffer);
    void updateEOLBasedUi(ScintillaNext *editor);
    void updateEncodingBasedUi(ScintillaNext *editor);
    void updateDocumentBasedUi(int updated);
    void updateSelectionBasedUi(ScintillaNext *editor);
    void updateContentBasedUi(ScintillaNext *editor);
    void updateSaveStatusBasedUi(bool isDirty);
    void updateBufferPositionBasedUi();
    void updateLanguageBasedUi(ScintillaNext *editor);
    void updateGui(ScintillaNext *editor);

    void detectLanguageFromExtension(ScintillaNext *editor);
    void bufferActivated(ScintillaNext *editor);

    void setLanguage(ScintillaNext *editor, const QString &languageName);

    void bringWindowToForeground();
    void focusIn();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void tabBarRightClicked(ScintillaNext *editor);
    void languageMenuTriggered();

private:
    Ui::MainWindow *ui = Q_NULLPTR;
    NotepadNextApplication *app = Q_NULLPTR;
    BufferManager *bufferManager = Q_NULLPTR;
    DockedEditor *dockedEditor = Q_NULLPTR;
    MacroRecorder *recorder = Q_NULLPTR;
    Settings *settings = Q_NULLPTR;

    bool isInInitialState();
    void openFileList(const QStringList &fileNames);
    bool checkBuffersBeforeClose(int startIndex, int endIndex);
    void setupStatusBar();
    bool checkBufferForModification(ScintillaBuffer *buffer);
    void setFoldMarkers(ScintillaNext *editor, const QString &type);

    QActionGroup *languageActionGroup;
    RecentFilesListManager *recentFilesListManager;

    QLabel *docType;
    QLabel *docSize;
    QLabel *docPos;
    QLabel *unicodeType;
    QLabel *eolFormat;
    QLabel *overType;

    //NppImporter *npp;

    // Dialogs
    FindReplaceDialog *frd = Q_NULLPTR;
    MacroRunDialog *mrd = Q_NULLPTR;
    PreferencesDialog *pd = Q_NULLPTR;
    QuickFindWidget *quickFind = Q_NULLPTR;

    QVector<Macro *> macros;
    Macro *currentMacro = Q_NULLPTR;

    // Docked widgets
    LuaConsoleDock *luaConsoleDock = Q_NULLPTR;
    QString getFileDialogFilter();
};

#endif // MAINWINDOW_H
