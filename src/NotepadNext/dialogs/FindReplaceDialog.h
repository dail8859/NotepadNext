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


#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QEvent>
#include <QMetaEnum>
#include <QStatusBar>
#include <QTabBar>

#include "Finder.h"

class ScintillaNext;

namespace Ui {
class FindReplaceDialog;
}



class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    enum {
        FIND_TAB = 0,
        REPLACE_TAB = 1,
        FIND_IN_FILES_TAB = 2,
        MARK_TAB = 3
    };

    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog() override;

    void setFindText(const QString &string);
    void setTab(int tab);

protected:
    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void windowActivated();
    void windowDeactivated();

public slots:
    void setEditor(ScintillaNext *edit);
    void performLastSearch();

    void find();
    void count();
    void replace();
    void replaceAll();

private slots:
    void adjustOpacity(int value);
    void transparencyToggled(bool on);
    void adjustOpacityWhenLosingFocus(bool checked);
    void adjustOpacityAlways(bool on);

    void changeTab(int index);

private:
    void loadSettings();
    void saveSettings();

    void savePosition();
    void restorePosition();

    int computeSearchFlags();

    void goToMatch(const Sci_CharacterRange &range);
    void showMessage(const QString &message, const QString &color);

    void updateFindList(const QString &text);
    void updateReplaceList(const QString &text);
    void saveSearchTerm(const QString &text);

    bool isFirstTime = true;
    QPoint position;
    Ui::FindReplaceDialog *ui;

    QString searchTerm;
    ScintillaNext *editor;
    QStatusBar *statusBar;
    QTabBar *tabBar;

    Finder *finder = Q_NULLPTR;
};

#endif // FINDREPLACEDIALOG_H
