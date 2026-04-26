#include <QToolButton>
#include <QMenu>

#include <map>

#include "TabsQuickActionsBar.h"

namespace {
    constexpr QLatin1StringView IconPlusPath("://icons/plus.svg");
    constexpr QLatin1StringView IconListPath("://icons/list_with_icons.svg");
    constexpr QLatin1StringView IconCrossPath("://icons/cross.svg");
}

struct TabsQuickActionsBar::Private
{
    TabsQuickActionsBar::Buttons visibileButtons = TabsQuickActionsBar::All;
    QAction *createNewTabAction = nullptr;
    QAction *showTabsMenuAction = nullptr;
    QAction *closeCurrentTabAction = nullptr;
};

TabsQuickActionsBar::TabsQuickActionsBar(const Buttons &visibileButtons, QWidget *parent)
    : QToolBar(parent),
      p(new Private)
{
    setIconSize({ 16, 16 });
    setStyleSheet(
        "QToolBar { margin: 0px; }"
        "QToolButton { padding: 0px; margin: 0px; }"
        "QToolButton::menu-indicator { image: none; }"
    );

    {
        p->createNewTabAction = new QAction(this);

        p->createNewTabAction->setIcon(QIcon(IconPlusPath));
        p->createNewTabAction->setToolTip(tr("Create a new file"));

        connect(p->createNewTabAction, &QAction::triggered,
                this,                  &TabsQuickActionsBar::createNewTabClicked);

        addAction(p->createNewTabAction);
    }

    {
        const auto tabsMenu = new QMenu(this);
        p->showTabsMenuAction = new QAction(this);

        p->showTabsMenuAction->setIcon(QIcon(IconListPath));
        p->showTabsMenuAction->setMenu(tabsMenu);

        connect(tabsMenu, &QMenu::aboutToShow, [this, tabsMenu]() {
            emit tabsMenuAboutToShow(tabsMenu);
        });

        addAction(p->showTabsMenuAction);

        { // Trick, cause addWidget will lost some style things
            const auto toolButton = qobject_cast<QToolButton*>(widgetForAction(p->showTabsMenuAction));

            if (toolButton) {
                toolButton->setPopupMode(QToolButton::InstantPopup);
            }
        }
    }

    {
        p->closeCurrentTabAction = new QAction(this);

        p->closeCurrentTabAction->setIcon(QIcon(IconCrossPath));
        p->closeCurrentTabAction->setToolTip(tr("Close the current file"));

        connect(p->closeCurrentTabAction, &QAction::triggered,
                this,                     &TabsQuickActionsBar::closeCurrentTabClicked);

        addAction(p->closeCurrentTabAction);
    }

    setVisibileButtons(visibileButtons);
}

TabsQuickActionsBar::~TabsQuickActionsBar()
{
    delete p;
}

TabsQuickActionsBar::Buttons TabsQuickActionsBar::visibileButtons() const
{
    return p->visibileButtons;
}

void TabsQuickActionsBar::setVisibileButtons(const Buttons &buttons)
{
    if (p->visibileButtons == buttons) return;

    p->visibileButtons = buttons;

    const std::map<QAction*, Button> mapping {
        { p->createNewTabAction,    CreateNewTab    },
        { p->showTabsMenuAction,    ShowTabsMenu    },
        { p->closeCurrentTabAction, CloseCurrentTab }
    };

    for (const auto &pair : mapping)
        pair.first->setVisible(buttons.testFlag(pair.second));

    emit visibileButtonsChanged(buttons);
}

void TabsQuickActionsBar::setVisibileButton(Button button, bool on)
{
    const auto& currentOptions = p->visibileButtons;
    setVisibileButtons(
        on ? (currentOptions | button)
           : (currentOptions & ~button)
    );
}
