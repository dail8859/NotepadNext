/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
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

#include "DocumentLifecycleController.h"

#include "EditorManager.h"
#include "MainWindow.h"
#include "NotepadNextApplication.h"
#include "RecentFilesListManager.h"
#include "ScintillaNext.h"

#include <QFileInfo>
#include <QMessageBox>

DocumentLifecycleController::DocumentLifecycleController(MainWindow *window, NotepadNextApplication *app)
    : window(window),
      app(app)
{
}

ScintillaNext *DocumentLifecycleController::getInitialEditor() const
{
    if (window->editorCount() != 1) {
        return Q_NULLPTR;
    }

    ScintillaNext *editor = window->currentEditor();
    if (!editor || editor->isTemporary() || editor->isFile() || editor->canUndo() || editor->canRedo()) {
        return Q_NULLPTR;
    }

    return editor;
}

void DocumentLifecycleController::openFiles(const QStringList &fileNames)
{
    qInfo(Q_FUNC_INFO);

    if (fileNames.isEmpty()) {
        return;
    }

    QList<ScintillaNext *> openedEditors;
    ScintillaNext *initialEditor = getInitialEditor();

    for (const QString &filePath : fileNames) {
        qInfo("%s", qUtf8Printable(filePath));

        ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);
        if (!editor) {
            const QFileInfo fileInfo(filePath);
            if (!fileInfo.isFile()) {
                const auto reply = QMessageBox::question(
                    window, window->tr("Create File"),
                    window->tr("<b>%1</b> does not exist. Do you want to create it?").arg(filePath));

                if (reply == QMessageBox::Yes) {
                    editor = app->getEditorManager()->createEditorFromFile(filePath, true);
                }
                else {
                    app->getRecentFilesListManager()->removeFile(filePath);
                    continue;
                }
            }
            else {
                editor = app->getEditorManager()->createEditorFromFile(filePath);
            }
        }

        if (editor) {
            openedEditors.append(editor);
        }
    }

    if (!openedEditors.isEmpty()) {
        window->switchToEditor(openedEditors.last());
        if (initialEditor) {
            initialEditor->close();
        }
    }
}

DocumentLifecycleController::UserSaveAction
DocumentLifecycleController::promptForSave(const QVector<ScintillaNext *> &editors) const
{
    const int count = editors.count();
    QMessageBox msgBox(window);
    msgBox.setWindowTitle(window->tr("Save File"));
    msgBox.setIcon(QMessageBox::Question);

    const QString text = count == 1
        ? window->tr("Save changes to <b>%1</b>?").arg(editors.first()->getName())
        : window->tr("There are %n files with unsaved changes. Save them?", "", count);
    msgBox.setText(text);

    auto *saveBtn = msgBox.addButton(count > 1 ? window->tr("Save All") : window->tr("Save"), QMessageBox::AcceptRole);
    auto *discardBtn = msgBox.addButton(count > 1 ? window->tr("Discard All") : window->tr("Discard"), QMessageBox::DestructiveRole);
    msgBox.addButton(QMessageBox::Cancel);

    msgBox.setDefaultButton(saveBtn);
    msgBox.exec();

    if (msgBox.clickedButton() == saveBtn) {
        return UserSaveAction::SaveAll;
    }
    if (msgBox.clickedButton() == discardBtn) {
        return UserSaveAction::DiscardAll;
    }
    return UserSaveAction::Cancel;
}

bool DocumentLifecycleController::checkEditorsBeforeClose(const QVector<ScintillaNext *> &editors)
{
    QVector<ScintillaNext *> unsaved;
    for (ScintillaNext *editor : editors) {
        if (editor && !editor->isSavedToDisk()) {
            unsaved.append(editor);
        }
    }

    if (unsaved.isEmpty()) {
        return true;
    }

    window->getDockedEditor()->switchToEditor(unsaved.first());
    switch (promptForSave(unsaved)) {
        case UserSaveAction::DiscardAll:
            return true;
        case UserSaveAction::SaveAll:
            return saveAllEditors(unsaved);
        case UserSaveAction::Cancel:
        default:
            return false;
    }
}

void DocumentLifecycleController::closeFile(ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    if (!app->getSettings()->exitOnLastTabClosed() && getInitialEditor()) {
        return;
    }

    if (!checkEditorsBeforeClose({editor})) {
        return;
    }

    editor->close();
    if (window->editorCount() == 0) {
        if (app->getSettings()->exitOnLastTabClosed()) {
            window->close();
        }
        else {
            window->newFile();
        }
    }
}

void DocumentLifecycleController::showSaveErrorMessage(ScintillaNext *editor, QFileDevice::FileError error) const
{
    const QString name = editor->isFile() ? editor->getFilePath() : editor->getName();

    QString errorString;
    switch (error) {
        case QFileDevice::ReadError:        errorString = window->tr("Read error"); break;
        case QFileDevice::WriteError:       errorString = window->tr("Write error"); break;
        case QFileDevice::FatalError:       errorString = window->tr("Fatal error"); break;
        case QFileDevice::ResourceError:    errorString = window->tr("Resource error"); break;
        case QFileDevice::OpenError:        errorString = window->tr("Open error"); break;
        case QFileDevice::AbortError:       errorString = window->tr("Abort error"); break;
        case QFileDevice::TimeOutError:     errorString = window->tr("Timeout error"); break;
        case QFileDevice::UnspecifiedError: errorString = window->tr("Unspecified error"); break;
        case QFileDevice::RemoveError:      errorString = window->tr("Remove error"); break;
        case QFileDevice::RenameError:      errorString = window->tr("Rename error"); break;
        case QFileDevice::PositionError:    errorString = window->tr("Position error"); break;
        case QFileDevice::ResizeError:      errorString = window->tr("Resize error"); break;
        case QFileDevice::PermissionsError: errorString = window->tr("Permissions error"); break;
        case QFileDevice::CopyError:        errorString = window->tr("Copy error"); break;
        default:                            errorString = window->tr("Unknown error (%1)").arg(static_cast<int>(error)); break;
    }

    QMessageBox::warning(window, window->tr("Error Saving File"),
        window->tr("An error occurred when saving <b>%1</b><br><br>Error: %2").arg(name, errorString));
}

bool DocumentLifecycleController::saveFile(ScintillaNext *editor)
{
    if (!editor) {
        return false;
    }
    if (editor->isSavedToDisk()) {
        return true;
    }

    if (!editor->isFile()) {
        window->getDockedEditor()->switchToEditor(editor);
        return window->saveCurrentFileAsDialog();
    }

    const QFileDevice::FileError error = editor->save();
    if (error == QFileDevice::NoError) {
        return true;
    }

    showSaveErrorMessage(editor, error);
    return false;
}

bool DocumentLifecycleController::saveFileAs(ScintillaNext *editor, const QString &fileName)
{
    if (!editor) {
        return false;
    }

    qInfo("saveFileAs(%s)", qUtf8Printable(fileName));
    const QFileDevice::FileError error = editor->saveAs(fileName);
    if (error == QFileDevice::NoError) {
        return true;
    }

    showSaveErrorMessage(editor, error);
    return false;
}

bool DocumentLifecycleController::saveAllEditors(const QVector<ScintillaNext *> &editors)
{
    for (ScintillaNext *editor : editors) {
        if (!saveFile(editor)) {
            return false;
        }
    }

    return true;
}
