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

signals: /*public*/
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
