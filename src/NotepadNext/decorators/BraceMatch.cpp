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

using namespace Scintilla;


BraceMatch::BraceMatch(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    setObjectName("BraceMatch");

    const int braceHighlight = editor->allocateIndicator("brace_highlight");
    const int braceBadlight = editor->allocateIndicator("brace_badlight");

    editor->indicSetStyle(braceHighlight, INDIC_FULLBOX);
    editor->indicSetFore(braceHighlight, 0x999999);
    editor->indicSetAlpha(braceHighlight, 100);
    editor->indicSetOutlineAlpha(braceHighlight, 200);
    editor->indicSetUnder(braceHighlight, true);
    editor->braceHighlightIndicator(true, braceHighlight);

    editor->indicSetStyle(braceBadlight, INDIC_FULLBOX);
    editor->indicSetFore(braceBadlight, 0x0000FF);
    editor->indicSetAlpha(braceBadlight, 100);
    editor->indicSetOutlineAlpha(braceBadlight, 200);
    editor->indicSetUnder(braceBadlight, true);
    editor->braceBadLightIndicator(true, braceBadlight);

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
    static const QList<char> braces = {'[', ']', '(', ')', '{', '}'};

    const Sci_Position pos = static_cast<Sci_Position>(editor->currentPos());

    // Check the character before the caret first
    int match = editor->braceMatch(pos - 1, 0);

    if (match != INVALID_POSITION) {
         editor->braceHighlight(pos - 1, match);
         editor->setHighlightGuide(editor->column(editor->lineIndentPosition(editor->lineFromPosition(pos - 1))));
    }
    else {
        // Check the character after the caret
        match = editor->braceMatch(pos, 0);
        if (match != INVALID_POSITION) {
             editor->braceHighlight(pos, match);
             editor->setHighlightGuide(editor->column(editor->lineIndentPosition(editor->lineFromPosition(pos))));
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

void BraceMatch::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::UpdateUI) {
        if (FlagSet(pscn->updated, Update::Content) || FlagSet(pscn->updated, Update::Selection)) {
            doHighlighting();
        }
    }
}
