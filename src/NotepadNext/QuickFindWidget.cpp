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


#include "FocusWatcher.h"
#include "QuickFindWidget.h"
#include "ScintillaNext.h"
#include "FadingIndicator.h"
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

    // Move the focus to the line edit widget
    setFocusProxy(ui->lineEdit);

    ui->lineEdit->installEventFilter(this);

    FocusWatcher *fw = new FocusWatcher(ui->lineEdit);
    connect(fw, &FocusWatcher::focusOut, this, &QuickFindWidget::focusOut);
    connect(fw, &FocusWatcher::focusIn, this, &QuickFindWidget::focusIn);

    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &QuickFindWidget::returnPressed);

    // Any changes need to trigger a new search
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &QuickFindWidget::performNewSearch);
    connect(ui->buttonMatchCase, &QToolButton::toggled, this, &QuickFindWidget::performNewSearch);
    connect(ui->buttonWholeWord, &QToolButton::toggled, this, &QuickFindWidget::performNewSearch);
    connect(ui->buttonRegexp, &QToolButton::toggled, this, &QuickFindWidget::performNewSearch);
}

QuickFindWidget::~QuickFindWidget()
{
    if (finder) {
        delete finder;
    }

    delete ui;
}

void QuickFindWidget::setEditor(ScintillaNext *editor)
{
    if (this->editor != Q_NULLPTR) {
        disconnect(editor, &ScintillaNext::resized, this, &QuickFindWidget::positionWidget);
    }

    connect(editor, &ScintillaNext::resized, this, &QuickFindWidget::positionWidget);

    this->editor = editor;

    if (finder == Q_NULLPTR) {
        finder = new Finder(editor);
        finder->setWrap(true); // Always wrap the search
    }
    else {
        finder->setEditor(editor);
    }

    initializeEditorIndicator();

    positionWidget();
}

bool QuickFindWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Use escape key to close the quick find widget
        if (keyEvent->key() == Qt::Key_Escape) {
            clearHighlights();
            clearCachedMatches();
            hide();
            editor->grabFocus();
        }
    }

    return QObject::eventFilter(obj, event);
}

void QuickFindWidget::setSearchContextColorBad()
{
    setSearchContextColor(QStringLiteral("red"));
}

void QuickFindWidget::setSearchContextColorGood()
{
    setSearchContextColor(QStringLiteral("blue"));
}

void QuickFindWidget::performNewSearch()
{
    clearHighlights();
    clearCachedMatches();
    ui->lblInfo->hide();

    // Early out
    if (searchText().isEmpty()) {
        setSearchContextColorGood();
        return;
    }

    prepareSearch();
    finder->forEachMatch([&](int start, int end) {
        matches.append(qMakePair(start, end));
        return qMax(start + 1, end);
    });

    if (matches.empty()) {
        setSearchContextColorBad();
    }
    else {
        setSearchContextColorGood();
    }

    highlightMatches();
    navigateToNextMatch(false);
}

void QuickFindWidget::highlightMatches()
{
    qInfo(Q_FUNC_INFO);

    editor->setIndicatorCurrent(indicator);
    for (const auto &range : matches) {
        editor->indicatorFillRange(range.first, range.second - range.first);
    }
}

void QuickFindWidget::showWrapIndicator()
{
    FadingIndicator::showPixmap(editor, QStringLiteral(":/icons/wrapindicator.png"));
}

void QuickFindWidget::navigateToNextMatch(bool skipCurrent)
{
    qInfo(Q_FUNC_INFO);

    // Early out if there are no matches
    if (matches.length() == 0) {
        ui->lblInfo->hide();
        return;
    }

    if (currentMatchIndex != -1) {
        currentMatchIndex++;
        if (currentMatchIndex >= matches.length()) {
            currentMatchIndex = 0;
        }
    }
    else {
        int startPos = INVALID_POSITION;
        if (skipCurrent) {
            startPos = editor->selectionEnd();
        }
        else {
            startPos = editor->selectionStart();
        }

        auto it = std::lower_bound(matches.begin(), matches.end(), startPos, [](const QPair<int, int>& pair, int value) {
            return pair.first < value;
        });

        if (it != matches.end()) {
            currentMatchIndex = std::distance(matches.begin(), it);
        } else {
            // Wrap back around
            currentMatchIndex = 0;
        }
    }

    // Search wrapped around
    if (currentMatchIndex == 0) {
        showWrapIndicator();
    }

    goToCurrentMatch();
}

void QuickFindWidget::navigateToPrevMatch()
{
    qInfo(Q_FUNC_INFO);

    // Early out if there are no matches
    if (matches.length() == 0) {
        ui->lblInfo->hide();
        return;
    }

    if (currentMatchIndex != -1) {
        currentMatchIndex--;
        if (currentMatchIndex < 0) {
            currentMatchIndex = matches.length() - 1;
        }
    }
    else {
        qWarning("navigateToPrevMatch() with no valid index yet");
        return;
    }

    // Search wrapped around
    if (currentMatchIndex == matches.length() - 1) {
        showWrapIndicator();
    }

    goToCurrentMatch();
}

void QuickFindWidget::goToCurrentMatch()
{
    editor->setSel(matches[currentMatchIndex].first, matches[currentMatchIndex].second);
    editor->verticalCentreCaret();

    ui->lblInfo->show();
    ui->lblInfo->setText(tr("%L1/%L2").arg(currentMatchIndex + 1).arg(matches.length()));
}

int QuickFindWidget::computeSearchFlags() const
{
    int searchFlags = 0;

    if (ui->buttonMatchCase->isChecked()) {
        searchFlags |= SCFIND_MATCHCASE;
    }

    if (ui->buttonWholeWord->isChecked()) {
        searchFlags |= SCFIND_WHOLEWORD;
    }

    if (ui->buttonRegexp->isChecked()) {
        searchFlags |= SCFIND_REGEXP;
    }

    return searchFlags;
}

void QuickFindWidget::setSearchContextColor(const QString &color)
{
    ui->lineEdit->setStyleSheet(QStringLiteral("border: 1px solid %1; padding: 2px;").arg(color));
}

void QuickFindWidget::initializeEditorIndicator()
{
    indicator = editor->allocateIndicator(QStringLiteral("quick_find"));

    editor->indicSetFore(indicator, 0xFF8000);
    editor->indicSetStyle(indicator, INDIC_FULLBOX);
    editor->indicSetOutlineAlpha(indicator, 150);
    editor->indicSetAlpha(indicator, 50);
    editor->indicSetUnder(indicator, true);
}

QString QuickFindWidget::searchText() const
{
    return ui->lineEdit->text();
}

void QuickFindWidget::positionWidget()
{
    int usableWidth = editor->width();

    // Account for the scrollbar as the quick find will be underneath if the scrollbar is visible
    if (editor->verticalScrollBar() && editor->verticalScrollBar()->isVisible()) {
        usableWidth -= editor->verticalScrollBar()->width();
    }

    QPoint position = QPoint(usableWidth - width(), 0);

    move(editor->mapTo(parentWidget(), position));
}

void QuickFindWidget::prepareSearch()
{
    finder->setSearchText(searchText());
    finder->setSearchFlags(computeSearchFlags());
}

void QuickFindWidget::focusIn()
{
    ui->lineEdit->selectAll();
    ui->lblInfo->hide();
    performNewSearch();
}

void QuickFindWidget::focusOut()
{
    clearHighlights();
    clearCachedMatches();
    hide();
}

void QuickFindWidget::returnPressed()
{
    if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier) {
        navigateToPrevMatch();
    }
    else {
        navigateToNextMatch(true);
    }
}

void QuickFindWidget::clearHighlights()
{
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, editor->length());
}

void QuickFindWidget::clearCachedMatches()
{
    matches.clear();
    currentMatchIndex = -1;
}
