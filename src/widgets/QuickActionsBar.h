#ifndef QUICKACTIONSBAR_H
#define QUICKACTIONSBAR_H

#include <QToolBar>

class QuickActionsBar : public QToolBar
{
    Q_OBJECT

    Q_PROPERTY(
        Buttons visibilityOptions
        READ    visibilityOptions
        WRITE   setVisibilityOptions
        NOTIFY  visibilityOptionsChanged
    )

public:
    enum Button {
        NoButtons             = 0x00,
        NewTabButton          = 0x01,
        TabsListButton        = 0x02,
        CloseCurrentTabButton = 0x04,

        AllButtons = NewTabButton |
                     TabsListButton |
                     CloseCurrentTabButton
    };

    Q_FLAG(Button)
    Q_DECLARE_FLAGS(Buttons, Button)

signals: /*public*/
    void newTabClicked();
    void tabsListAboutToShow(QMenu *list);
    void closeCurrentTabClicked();

    void visibilityOptionsChanged(const QuickActionsBar::Buttons &visibleOptions);

public:
    inline QuickActionsBar(QWidget *parent = nullptr) : QuickActionsBar(AllButtons, parent) { }
    explicit QuickActionsBar(const Buttons &visibilityOptions = Button::AllButtons, QWidget *parent = nullptr);
    virtual ~QuickActionsBar();

    Buttons visibilityOptions() const;
    void setVisibilityOptions(const Buttons &options);
    void setVisibilityOption(Button option, bool on = true);

private:
    struct QuickActionsBarPrivate;
    QuickActionsBarPrivate *p = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QuickActionsBar::Buttons);

#endif // QUICKACTIONSBAR_H
