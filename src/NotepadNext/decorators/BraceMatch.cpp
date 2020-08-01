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


#include "Sci_Position.h"

#include "BraceMatch.h"

static const QList<char> braces = {'[', ']', '(', ')', '{', '}'};

BraceMatch::BraceMatch(ScintillaEdit *editor) :
    EditorDecorator(editor)
{
    setObjectName("BraceMatch");

    const int INDIC_BRACEHIGHLIGHT = 8;
    const int INDIC_BRACEBADLIGHT = 9;

    editor->indicSetStyle(INDIC_BRACEHIGHLIGHT, INDIC_FULLBOX);
    editor->indicSetFore(INDIC_BRACEHIGHLIGHT, 0x777777);
    editor->indicSetAlpha(INDIC_BRACEHIGHLIGHT, 100);
    editor->indicSetOutlineAlpha(INDIC_BRACEHIGHLIGHT, 200);
    editor->indicSetUnder(INDIC_BRACEHIGHLIGHT, true);
    editor->braceHighlightIndicator(true, INDIC_BRACEHIGHLIGHT);

    editor->indicSetStyle(INDIC_BRACEBADLIGHT, INDIC_FULLBOX);
    editor->indicSetFore(INDIC_BRACEBADLIGHT, 0x0000FF);
    editor->indicSetAlpha(INDIC_BRACEBADLIGHT, 100);
    editor->indicSetOutlineAlpha(INDIC_BRACEBADLIGHT, 200);
    editor->indicSetUnder(INDIC_BRACEBADLIGHT, true);
    editor->braceBadLightIndicator(true, INDIC_BRACEBADLIGHT);

    editor->setIndentationGuides(SC_IV_LOOKBOTH);

    connect(this, &EditorDecorator::stateChanged, [=](bool b) {
        if (b) {
            doHighlighting();
        }
        else {
            clearHighlighting();
        }
    });
}

void BraceMatch::doHighlighting()
{
    const Sci_Position pos = static_cast<Sci_Position>(editor->currentPos());

    // Check the character before the caret first
    int match = editor->braceMatch(pos - 1, 0);

    if (match != INVALID_POSITION) {
         editor->braceHighlight(pos - 1, match);
         editor->setHighlightGuide(editor->column(pos - 1));
    }
    else {
        // Check the character after the caret
        match = editor->braceMatch(pos, 0);
        if (match != INVALID_POSITION) {
             editor->braceHighlight(pos, match);
             editor->setHighlightGuide(editor->column(pos));
        }
        else {
            // Nothing was found, now check to see if we need to badlight something
            // by checking the characters
            char c = static_cast<char>(editor->charAt(pos - 1));
            if (braces.contains(c)) {
                editor->braceBadLight(pos - 1);
            }
            else {
                c = static_cast<char>(editor->charAt(pos));
                if (braces.contains(c)) {
                    editor->braceBadLight(pos);
                }
                else {
                    clearHighlighting();
                }
            }
        }
    }
}

void BraceMatch::clearHighlighting()
{
    editor->braceHighlight(INVALID_POSITION, INVALID_POSITION);
    editor->setHighlightGuide(0);
}

void BraceMatch::notify(const SCNotification *pscn)
{
    if (pscn->nmhdr.code == SCN_UPDATEUI && pscn->updated & (SC_UPDATE_CONTENT | SC_UPDATE_SELECTION)) {
        doHighlighting();
    }
}
