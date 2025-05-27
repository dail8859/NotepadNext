/*
 * This file is part of Notepad Next.
 * Copyright 2025 Justin Dailey
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

#include "MarkerAppDecorator.h"
#include "EditorManager.h"
#include "ScintillaNext.h"


static QList<QColor> marker_colors = {
    QColor(0x00, 0xFF, 0xFF),
    QColor(0xFF, 0x80, 0x00),
    QColor(0xFF, 0xFF, 0x00)
};

static int QColorToScintillaColour(QColor c)
{
    return c.red() | (c.green() << 8) | (c.blue() << 16);
}

MarkerAppDecorator::MarkerAppDecorator(NotepadNextApplication *app)
    : ApplicationDecorator(app)
{
    // Any time an editor is created go ahead and allocate/set the required indicators
    connect(app->getEditorManager(), &EditorManager::editorCreated, this, [](ScintillaNext *editor) {
        for (int i = 0; i < marker_colors.size(); i++) {
            int indicator = editor->allocateIndicator(QString("marker_%1").arg(i));
            editor->indicSetFore(indicator, QColorToScintillaColour(marker_colors[i]));
            editor->indicSetStyle(indicator, INDIC_ROUNDBOX);
            editor->indicSetOutlineAlpha(indicator, 150);
            editor->indicSetAlpha(indicator, 100);
            editor->indicSetUnder(indicator, true);
        }
    });
}

QColor MarkerAppDecorator::markerColor(int i) const
{
    return marker_colors[i];
}

void MarkerAppDecorator::mark(ScintillaNext *editor, int i)
{
    //const int mainSelection = editor->mainSelection();
    //const int selectionStart = editor->selectionNStart(mainSelection);
    //const int selectionEnd = editor->selectionNEnd(mainSelection);

    const int curPos = editor->currentPos();
    const int wordStart = editor->wordStartPosition(curPos, true);
    const int wordEnd = editor->wordEndPosition(wordStart, true);

    // Make sure the selection is on word boundaries
    if (wordStart == wordEnd) {
        return;
    }

    int indicator = editor->allocateIndicator(QString("marker_%1").arg(i));
    editor->setIndicatorCurrent(indicator);

    const QByteArray selText = editor->get_text_range(wordStart, wordEnd);
    Sci_TextToFind ttf {{0, (Sci_PositionCR)editor->length()}, selText.constData(), {-1, -1}};
    const int flags = SCFIND_WHOLEWORD;

    while (editor->send(SCI_FINDTEXT, flags, (sptr_t)&ttf) != -1) {
        editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
        ttf.chrg.cpMin = ttf.chrgText.cpMax;
    }
}

void MarkerAppDecorator::clear(ScintillaNext *editor, int i)
{
    int indicator = editor->allocateIndicator(QString("marker_%1").arg(i));
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, editor->length());
}

void MarkerAppDecorator::clearAll(ScintillaNext *editor)
{
    for (int i = 0; i < marker_colors.size(); i++) {
        clear(editor, i);
    }
}
