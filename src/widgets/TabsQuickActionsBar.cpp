#include <QApplication>
#include <QToolButton>
#include <QStyle>
#include <QMenu>

#include <map>

#include "TabsQuickActionsBar.h"

namespace
{
    constexpr QLatin1StringView IconPlusPath("://icons/plus.svg");
    constexpr QLatin1StringView IconListPath("://icons/list_with_icons.svg");
    constexpr QLatin1StringView IconCrossPath("://icons/cross.svg");
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

    // Trick, cause addWidget will lost some style things
    const auto toolButton = qobject_cast<QToolButton*>(widgetForAction(showTabsMenuAction));
    if (toolButton) toolButton->setPopupMode(QToolButton::InstantPopup);

    connect(createNewTabAction, &QAction::triggered,
            this,               &TabsQuickActionsBar::createNewTabClicked);
    connect(tabsMenu, &QMenu::aboutToShow,
            this,     [this, tabsMenu]() { emit tabsMenuAboutToShow(tabsMenu); });
    connect(closeCurrentTabAction, &QAction::triggered,
            this,                  &TabsQuickActionsBar::closeCurrentTabClicked);

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
