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


#ifndef EDITORINFOSTATUSBAR_H
#define EDITORINFOSTATUSBAR_H

#include <QStatusBar>

#include "ScintillaTypes.h"


class QLabel;
class QMainWindow;
class ScintillaNext;

class EditorInfoStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit EditorInfoStatusBar(QMainWindow *window);

    void refresh(ScintillaNext *editor);

private slots:
    void connectToEditor(ScintillaNext *editor);

    void editorUpdated(Scintilla::Update updated);

    void updateDocumentSize(ScintillaNext *editor);
    void updateSelectionInfo(ScintillaNext *editor);
    void updateLanguage(ScintillaNext *editor);
    void updateEol(ScintillaNext *editor);
    void updateEncoding(ScintillaNext *editor);
    void updateOverType(ScintillaNext *editor);

private:
    QLabel *docType;
    QLabel *docSize;
    QLabel *docPos;
    QLabel *unicodeType;
    QLabel *eolFormat;
    QLabel *overType;

    QMetaObject::Connection editorUiUpdated;
    QMetaObject::Connection documentLexerChanged;
};

#endif // EDITORINFOSTATUSBAR_H
