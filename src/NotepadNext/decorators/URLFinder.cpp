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

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "URLFinder.h"

const int URL_INDIC = 10;

URLFinder::URLFinder(ScintillaNext *editor) :
    EditorDecorator(editor),
    timer(new QTimer(this))
{
    // Setup the indicator
    editor->indicSetStyle(URL_INDIC, INDIC_PLAIN);
    editor->indicSetFore(URL_INDIC, 0xFF0000);

    editor->indicSetHoverStyle(URL_INDIC, INDIC_DOTS);
    editor->indicSetHoverFore(URL_INDIC, 0xFF0000);

    // Resizing the window could reveal more text
    connect(editor, &ScintillaNext::resized, timer, qOverload<>(&QTimer::start));

    timer->setInterval(200);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &URLFinder::findURLs);
}

void URLFinder::findURLs()
{
    //qInfo(Q_FUNC_INFO);

    editor->setIndicatorCurrent(URL_INDIC);
    editor->indicatorClearRange(0, editor->length());

    int currentLine = editor->docLineFromVisible(editor->firstVisibleLine());
    int linesLeftToProcess = editor->linesOnScreen();
    const int flags = SCFIND_REGEXP;

    while(linesLeftToProcess >= 0 && currentLine < editor->lineCount()) {
        // Should only happen if the line is hidden
        if (!editor->lineVisible(currentLine)) {
            currentLine++;
            continue;
        }

        const int startPos = editor->positionFromLine(currentLine);
        const int endPos = editor->lineEndPosition(currentLine);
        QByteArray reg(R"(\bhttps?://[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b(?:[-a-zA-Z0-9()@:%_\+.~#?&\/=]*))");

        Sci_TextToFind ttf {{startPos, (Sci_PositionCR)endPos}, reg.constData(), {-1, -1}};
        while (editor->send(SCI_FINDTEXT, flags, (sptr_t)&ttf) != -1) {
            editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
            ttf.chrg.cpMin = ttf.chrgText.cpMax;
        }

        linesLeftToProcess -= editor->wrapCount(currentLine);

        // If the current line is a fold header and the fold is not expanded, skip
        if (((editor->foldLevel(currentLine) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG) && (!editor->foldExpanded(currentLine))) {
            currentLine = editor->lastChild(currentLine, -1) + 1;
        }
        else {
            currentLine++;
        }
    }
}

void URLFinder::notify(const Scintilla::NotificationData *pscn)
{
    // TODO: handle editor folding/unfolding
    // Currently there is no generic notification for this

    // Reprocess any time the content was updated or the editor was vertically scrolled
    if ((pscn->nmhdr.code == Scintilla::Notification::UpdateUI)) {
        if (FlagSet(pscn->updated, Scintilla::Update::VScroll)) {
            timer->start();
        }
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::Modified) {
        if (FlagSet(pscn->modificationType, Scintilla::ModificationFlags::InsertText) || FlagSet(pscn->modificationType, Scintilla::ModificationFlags::DeleteText)) {
            timer->start();
        }
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::Zoom) {
        timer->start();
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::IndicatorClick && FlagSet(pscn->modifiers, Scintilla::KeyMod::Ctrl)) {
        const int indicators = editor->indicatorAllOnFor(pscn->position);

        // TODO: make sure it is the indicator we want
        if (indicators & (1 << URL_INDIC)) {

            const int indicatorStart = editor->indicatorStart(URL_INDIC, pscn->position);
            const int indicatorEnd = editor->indicatorEnd(URL_INDIC, pscn->position);

            QUrl url(editor->get_text_range(indicatorStart, indicatorEnd));

            if (url.isValid()) {
                qInfo("URL hotspot click: \"%s\"", editor->get_text_range(indicatorStart, indicatorEnd).constData());
                QDesktopServices::openUrl(url);
            }
            else {
                qWarning("Invalid url \"%s\"", qUtf8Printable(url.errorString()));
            }
        }
    }
}
