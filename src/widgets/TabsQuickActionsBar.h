/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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


#ifndef TABSQUICKACTIONSBAR_H
#define TABSQUICKACTIONSBAR_H

#include <QToolBar>

class TabsQuickActionsBar : public QToolBar
{
    Q_OBJECT

    Q_PROPERTY(
        Buttons visibileButtons
        READ    getVisibileButtons
        WRITE   setVisibileButtons
        NOTIFY  visibileButtonsChanged
    )

public:
    enum Button {
        None            = 0x00,
        CreateNewTab    = 0x01,
        ShowTabsMenu    = 0x02,
        CloseCurrentTab = 0x04,

        All = CreateNewTab |
              ShowTabsMenu |
              CloseCurrentTab
    };

    Q_FLAG(Button)
    Q_DECLARE_FLAGS(Buttons, Button)

signals:
    void createNewTabClicked();
    // Tabs menu must be filled with actual data from outside
    void tabsMenuAboutToShow(QMenu *menu);
    void closeCurrentTabClicked();

    void visibileButtonsChanged(const TabsQuickActionsBar::Buttons &visibileButtons);

public:
    inline TabsQuickActionsBar(QWidget *parent = nullptr) : TabsQuickActionsBar(Button::All, parent) { }
    explicit TabsQuickActionsBar(const Buttons &visibileButtons = Button::All, QWidget *parent = nullptr);
    virtual ~TabsQuickActionsBar() = default;

    inline Buttons getVisibileButtons() const { return visibileButtons; }
    void setVisibileButtons(const Buttons &buttons);
    void setVisibileButton(Button button, bool on = true);

private:
    Buttons visibileButtons = TabsQuickActionsBar::All;

    QAction *createNewTabAction = nullptr;
    QAction *showTabsMenuAction = nullptr;
    QAction *closeCurrentTabAction = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TabsQuickActionsBar::Buttons);

#endif // TABSQUICKACTIONSBAR_H
