/*
 * This file is part of Notepad Next.
 * Copyright 2020 Justin Dailey
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


#include "Finder.h"
#include "QuickFindWidget.h"
#include "ScintillaNext.h"
#include "ui_QuickFindWidget.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QShortcut>
#include <QScrollBar>

QuickFindWidget::QuickFindWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuickFindWidget)
{
    ui->setupUi(this);

    this->setFocusProxy(ui->lineEdit);

    auto fw = new FocusWatcher(ui->lineEdit);
    ui->lineEdit->installEventFilter(this);

    connect(fw, &FocusWatcher::focusOut, [=]() {
        clearHighlights();
        hide();
    });

    connect(fw, &FocusWatcher::focusIn, [=]() {
        ui->lineEdit->selectAll();
        highlightAndNavigateToNextMatch();
    });

    connect(ui->lineEdit, &QLineEdit::textChanged, [=](const QString &) { this->highlightAndNavigateToNextMatch(); });
    connect(ui->lineEdit, &QLineEdit::returnPressed, [=]() {
        if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier) {
            navigateToPrevMatch();
        }
        else {
            navigateToNextMatch(true);
        }
    });
    connect(ui->buttonRegexp, &QToolButton::toggled, this, &QuickFindWidget::highlightAndNavigateToNextMatch);
    connect(ui->buttonMatchCase, &QToolButton::toggled, this, &QuickFindWidget::highlightAndNavigateToNextMatch);
}

QuickFindWidget::~QuickFindWidget()
{
    delete ui;
}

void QuickFindWidget::setEditor(ScintillaNext *editor)
{
    if (this->editor != Q_NULLPTR)
        disconnect(editor, &ScintillaNext::resized, this, &QuickFindWidget::positionWidget);
    connect(editor, &ScintillaNext::resized, this, &QuickFindWidget::positionWidget);

    this->editor = editor;

    editor->indicSetFore(28, 0xFF8000);
    editor->indicSetStyle(28, INDIC_FULLBOX);
    editor->indicSetOutlineAlpha(28, 150);
    editor->indicSetAlpha(28, 50);
    editor->indicSetUnder(28, true);

    positionWidget();
}

bool QuickFindWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Use escape key to close the quick find widget
        if (keyEvent->key() == Qt::Key_Escape) {
            clearHighlights();
            this->hide();
            editor->grabFocus();
        }
    }

    return QObject::eventFilter(obj, event);
}

void QuickFindWidget::highlightMatches()
{
    qInfo(Q_FUNC_INFO);

    clearHighlights();

    const QString text = ui->lineEdit->text();

    if (text.isEmpty()) {
        ui->lineEdit->setStyleSheet("border: 1px solid blue; padding: 2px;");
        return;
    }

    bool foundOne = false;
    editor->setIndicatorCurrent(28);
    editor->setSearchFlags(computeSearchFlags());
    editor->forEachMatch(text, [&](int start, int end) {
        foundOne = true;

        const int length = end - start;

        // Don't highlight 0 length matches
        if (length > 0)
            editor->indicatorFillRange(start, length);

        // Advance at least 1 character to prevent infinite loop
        return qMax(start + 1, end);
    });

    if (foundOne == false) {
        ui->lineEdit->setStyleSheet("border: 1px solid red; padding: 2px;");
    }
    else {
        ui->lineEdit->setStyleSheet("border: 1px solid blue; padding: 2px;");
    }
}

void QuickFindWidget::navigateToNextMatch(bool skipCurrent)
{
    qInfo(Q_FUNC_INFO);

    const QString text = ui->lineEdit->text();

    if (text.isEmpty()) {
        return;
    }

    Finder f = Finder(editor);
    f.setWrap(true);
    f.setSearchFlags(computeSearchFlags());
    f.setSearchText(text);

    int startPos = INVALID_POSITION;
    if (skipCurrent) {
        startPos = editor->selectionEnd();
    }
    else {
        startPos = editor->selectionStart();
    }

    auto range = f.findNext(startPos);
    if (range.cpMin == INVALID_POSITION)
        return;

    editor->setSel(range.cpMin, range.cpMax);
    editor->verticalCentreCaret();
}

void QuickFindWidget::navigateToPrevMatch()
{
    qInfo(Q_FUNC_INFO);

    const QString text = ui->lineEdit->text();

    if (text.isEmpty()) {
        return;
    }

    Finder f = Finder(editor);
    f.setWrap(true);
    f.setSearchFlags(computeSearchFlags());
    f.setSearchText(text);

    auto range = f.findPrev();
    if (range.cpMin == INVALID_POSITION)
        return;

    editor->setSel(range.cpMin, range.cpMax);
    editor->verticalCentreCaret();
}

void QuickFindWidget::highlightAndNavigateToNextMatch()
{
    highlightMatches();
    navigateToNextMatch(false);
}

int QuickFindWidget::computeSearchFlags()
{
    int searchFlags = 0;

    if (ui->buttonRegexp->isChecked()) {
        searchFlags |= SCFIND_REGEXP;
    }

    if (ui->buttonMatchCase->isChecked()) {
        searchFlags |= SCFIND_MATCHCASE;
    }

    return searchFlags;
}

void QuickFindWidget::positionWidget()
{
    int usableWidth = editor->width();

    // Account for the scrollbar as the quick find will be underneath if the scrollbar is visible
    if (editor->verticalScrollBar() && editor->verticalScrollBar()->isVisible()) {
        usableWidth -= editor->verticalScrollBar()->width();
    }

    QPoint position = QPoint(usableWidth - this->width(), 0);

    this->move(editor->mapTo(this->parentWidget(), position));
}

void QuickFindWidget::clearHighlights()
{
    editor->setIndicatorCurrent(28);
    editor->indicatorClearRange(0, editor->length());
}
