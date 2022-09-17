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


#ifndef QUICKFINDWIDGET_H
#define QUICKFINDWIDGET_H

#include <QEvent>
#include <QFrame>
#include <QKeyEvent>
#include <QLineEdit>
#include <QObject>

#include "ScintillaNext.h"

namespace Ui {
class QuickFindWidget;
}


class QuickFindWidget : public QFrame
{
    Q_OBJECT

public:
    explicit QuickFindWidget(QWidget *parent = nullptr);
    ~QuickFindWidget();

    void setEditor(ScintillaNext *editor);

	void setText(const QString& text);

protected:
   bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void highlightMatches();
    void navigateToNextMatch(bool skipCurrent = true);
    void navigateToPrevMatch();
    void highlightAndNavigateToNextMatch();

private slots:
    void positionWidget();

private:
    void clearHighlights();
    int computeSearchFlags() const;
    void setSearchContextColor(QString color);

    Ui::QuickFindWidget *ui;
    ScintillaNext *editor = Q_NULLPTR;
};

#endif // QUICKFINDWIDGET_H
