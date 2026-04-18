#include <QToolButton>
#include <QMenu>

#include "QuickActionsBar.h"

namespace {
    constexpr char* IconPlusPath = "://icons/plus.svg";
    constexpr char* IconListPath = "://icons/list_with_icons.svg";
    constexpr char* IconCrossPath = "://icons/cross.svg";
}

struct QuickActionsBar::QuickActionsBarPrivate
{
    QuickActionsBar::Buttons visibilityOptions = QuickActionsBar::AllButtons;
    QAction *newTabAction = nullptr;
    QAction *showTabsListAction = nullptr;
    QAction *closeCurrentTabAction = nullptr;
};

QuickActionsBar::QuickActionsBar(const Buttons &visibilityOptions, QWidget *parent)
    : QToolBar(parent),
      p(new QuickActionsBarPrivate)
{
    setIconSize({ 16, 16 }); // TODO: adjustible
    setStyleSheet(
        "QToolBar { margin: 0px; } "
        "QToolButton { padding: 0px; margin: 0px; }"
        "QToolButton::menu-indicator { image:none; }"
    );

    {
        p->newTabAction = new QAction(this);

        p->newTabAction->setIcon(QIcon(IconPlusPath));
        p->newTabAction->setToolTip(tr("Create a new file"));

        connect(p->newTabAction, &QAction::triggered,
                this,            &QuickActionsBar::newTabClicked);

        addAction(p->newTabAction);
    }

    {
        const auto tabsMenu = new QMenu(this);
        p->showTabsListAction = new QAction(this);

        p->showTabsListAction->setIcon(QIcon(IconListPath));
        p->showTabsListAction->setMenu(tabsMenu);

        connect(tabsMenu, &QMenu::aboutToShow, [this, tabsMenu]() {
            emit tabsListAboutToShow(tabsMenu);
        });

        addAction(p->showTabsListAction);

        { // Trick cause addWidget will lost some style things
            const auto toolButton = qobject_cast<QToolButton*>(widgetForAction(p->showTabsListAction));

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
                this,                     &QuickActionsBar::closeCurrentTabClicked);

        addAction(p->closeCurrentTabAction);
    }

    setVisibilityOptions(visibilityOptions);
}

QuickActionsBar::~QuickActionsBar()
{
    delete p;
}

QuickActionsBar::Buttons QuickActionsBar::visibilityOptions() const
{
    return p->visibilityOptions;
}

void QuickActionsBar::setVisibilityOptions(const Buttons &options)
{
    if (p->visibilityOptions == options) return;

    p->visibilityOptions = options;

    p->newTabAction->setVisible(options.testFlag(NewTabButton));
    p->showTabsListAction->setVisible(options.testFlag(TabsListButton));
    p->closeCurrentTabAction->setVisible(options.testFlag(CloseCurrentTabButton));

    emit visibilityOptionsChanged(options);
}

void QuickActionsBar::setVisibilityOption(Button option, bool on)
{
    const auto& currentOptions = p->visibilityOptions;
    setVisibilityOptions(
        on ? (currentOptions | option)
           : (currentOptions & ~option)
    );
}
