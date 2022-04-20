/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "EditorInfoStatusBar.h"
#include "MainWindow.h"
#include "StatusLabel.h"


EditorInfoStatusBar::EditorInfoStatusBar(QMainWindow *window) :
    QStatusBar(window)
{
    // Set up the status bar
    docType = new StatusLabel();
    addWidget(docType, 1);

    docSize = new StatusLabel(200);
    addPermanentWidget(docSize, 0);

    docPos = new StatusLabel(250);
    addPermanentWidget(docPos, 0);

    eolFormat = new StatusLabel(100);
    addPermanentWidget(eolFormat, 0);

    unicodeType = new StatusLabel(125);
    addPermanentWidget(unicodeType, 0);

    /*
    docType->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(docType, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        ui->menuLanguage->popup(docType->mapToGlobal(pos));
    });

    eolFormat->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(eolFormat, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        ui->menuEOLConversion->popup(eolFormat->mapToGlobal(pos));
    });
    */

    MainWindow *w = qobject_cast<MainWindow *>(window);

    connect(w, &MainWindow::editorActivated, this, &EditorInfoStatusBar::connectToEditor);
}

void EditorInfoStatusBar::refresh(ScintillaNext *editor)
{
    updateDocumentSize(editor);
    updateSelectionInfo(editor);
    updateLanguage(editor);
    updateEol(editor);
    updateEncoding(editor);
}

void EditorInfoStatusBar::connectToEditor(ScintillaNext *editor)
{
    // Remove any previous connections
    disconnect(editorUiUpdated);
    disconnect(documentLexerChanged);

    // Connect to the new editor
    editorUiUpdated = connect(editor, &ScintillaNext::updateUi, this, &EditorInfoStatusBar::editorUpdated);
    documentLexerChanged = connect(editor->get_doc(), &ScintillaDocument::lexer_changed, this, [=]() { updateLanguage(editor); });

    refresh(editor);
}

void EditorInfoStatusBar::editorUpdated(Scintilla::Update updated)
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());

    if (Scintilla::FlagSet(updated, Scintilla::Update::Content)) {
        updateDocumentSize(editor);
    }

    if (Scintilla::FlagSet(updated, Scintilla::Update::Content) || Scintilla::FlagSet(updated, Scintilla::Update::Selection)) {
        updateSelectionInfo(editor);
    }
}

void EditorInfoStatusBar::updateDocumentSize(ScintillaNext *editor)
{
    QString sizeText = tr("Length: %1    Lines: %2").arg(
            QLocale::system().toString(editor->length()),
            QLocale::system().toString(editor->lineCount()));
    docSize->setText(sizeText);
}

void EditorInfoStatusBar::updateSelectionInfo(ScintillaNext *editor)
{
    QString selectionText;

    if (editor->selections() > 1) {
        selectionText = tr("Sel: N/A");
    }
    else {
        int start = editor->selectionStart();
        int end = editor->selectionEnd();
        int lines = editor->lineFromPosition(end) - editor->lineFromPosition(start);

        if (end > start)
            lines++;

        selectionText = tr("Sel: %1 | %2").arg(
                QLocale::system().toString(editor->countCharacters(start, end)),
                QLocale::system().toString(lines));
    }

    const int pos = editor->currentPos();
    QString positionText = tr("Ln: %1    Col: %2    ").arg(
            QLocale::system().toString(editor->lineFromPosition(pos) + 1),
            QLocale::system().toString(editor->column(pos) + 1));
    docPos->setText(positionText + selectionText);
}

void EditorInfoStatusBar::updateLanguage(ScintillaNext *editor)
{
    docType->setText(editor->languageName);
}


void EditorInfoStatusBar::updateEol(ScintillaNext *editor)
{
    // No good way to keep these in sync with the Main Window menu items :(

    switch(editor->eOLMode()) {
    case SC_EOL_CR:
        eolFormat->setText(tr("Macintosh (CR)"));
        break;
    case SC_EOL_CRLF:
        eolFormat->setText(tr("Windows (CR LF)"));
        break;
    case SC_EOL_LF:
        eolFormat->setText(tr("Unix (LF)"));
        break;
    }
}

void EditorInfoStatusBar::updateEncoding(ScintillaNext *editor)
{
    switch(editor->codePage()) {
    case 0:
        unicodeType->setText(tr("ANSI"));
        break;
    case SC_CP_UTF8:
        unicodeType->setText(tr("UTF-8"));
        break;
    default:
        unicodeType->setText(QString::number(editor->codePage()));
        break;
    }
}

