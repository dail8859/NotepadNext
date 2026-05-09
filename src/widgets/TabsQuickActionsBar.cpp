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


#include <QApplication>
#include <QToolButton>
#include <QStyle>
#include <QMenu>

#include <map>

#include "TabsQuickActionsBar.h"

namespace
{
    constexpr QLatin1StringView IconPlusPath(":/icons/plus.svg");
    constexpr QLatin1StringView IconListPath(":/icons/list_with_icons.svg");
    constexpr QLatin1StringView IconCrossPath(":/icons/cross.svg");
}

TabsQuickActionsBar::TabsQuickActionsBar(const Buttons &visibileButtons, QWidget *parent)
    : QToolBar(parent)
{
    createNewTabAction = addAction(QIcon(IconPlusPath), "");
    createNewTabAction->setToolTip(tr("Create a new file"));

    showTabsMenuAction = addAction(QIcon(IconListPath), "");
    showTabsMenuAction->setToolTip(tr("Show opened files list"));

    const auto tabsMenu = new QMenu(this);
    showTabsMenuAction->setMenu(tabsMenu);

    closeCurrentTabAction = addAction(QIcon(IconCrossPath), "");
    closeCurrentTabAction->setToolTip(tr("Close the current file"));

    const auto iconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);
    setIconSize({ iconSize, iconSize });
    setStyleSheet(
        "QToolBar { padding: 0px; margin: 0px; }"
        "QToolButton::menu-indicator { image: none; }"
    );

    // Trick, cause addWidget will lose some style things
    const auto toolButton = qobject_cast<QToolButton*>(widgetForAction(showTabsMenuAction));
    if (toolButton) toolButton->setPopupMode(QToolButton::InstantPopup);

    connect(createNewTabAction, &QAction::triggered, this, &TabsQuickActionsBar::createNewTabClicked);
    connect(tabsMenu, &QMenu::aboutToShow, this, [this, tabsMenu]() { emit tabsMenuAboutToShow(tabsMenu); });
    connect(closeCurrentTabAction, &QAction::triggered, this, &TabsQuickActionsBar::closeCurrentTabClicked);

    setVisibileButtons(visibileButtons);
}

void TabsQuickActionsBar::setVisibileButtons(const Buttons &buttons)
{
    if (visibileButtons == buttons)
        return;

    visibileButtons = buttons;

    const std::map<QAction*, Button> mapping {
        { createNewTabAction,    CreateNewTab    },
        { showTabsMenuAction,    ShowTabsMenu    },
        { closeCurrentTabAction, CloseCurrentTab }
    };

    for (const auto &pair : mapping)
        pair.first->setVisible(buttons.testFlag(pair.second));

    emit visibileButtonsChanged(buttons);
}

void TabsQuickActionsBar::setVisibileButton(Button button, bool on)
{
    const auto &currentOptions = visibileButtons;
    setVisibileButtons(
        on ? (currentOptions |  button)
           : (currentOptions & ~button)
    );
}
