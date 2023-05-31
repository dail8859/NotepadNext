/*!
 *  \author Kang Lin <kl222@126.com>
 *  The file is modified from the [RabbitCommon](https://github.com/KangLin/RabbitCommon)
 */

#include "TitleBar.h"
#include <QHBoxLayout>
#include <QDockWidget>

QPushButton* CTitleBar::CreateSmallPushButton(QIcon icon, QWidget *parent) {
    QPushButton *button = new QPushButton(icon, "", parent);
    if(!button) return button;
    
    button->setObjectName("TitleBarButton");
    button->setFlat(true);

    // avoid keyboard focus, this looks bad
    button->setFocusPolicy(Qt::NoFocus);
    
    return button;
}

CTitleBar::CTitleBar(QWidget *parent)
    : QWidget{parent},
    m_pTitle(nullptr),
    m_pCloseButton(nullptr),
    m_pFloatButton(nullptr),
    m_pMaxButton(nullptr),
    m_pMinButton(nullptr)
{
    bool check = false;
    Q_ASSERT(parent);

    // title bar horizontal layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    setLayout(layout);
    // ugly platform dependent layout fix:
    // this should be solved by the style sheet
    layout->setContentsMargins(10, 3, 10, 3);  // left, top, right, bottom
    #ifdef __APPLE__
        layout->setSpacing(10);
    #else
        layout->setSpacing(0);
    #endif

    m_pTitle = new QLabel(this);
    Q_ASSERT(m_pTitle);
    m_pTitle->setText(parent->windowTitle());
    check = connect(parent, SIGNAL(windowTitleChanged(const QString&)),
                           m_pTitle, SLOT(setText(const QString&)));
    Q_ASSERT(check);
    m_pTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(m_pTitle);

    m_pMinButton = CreateSmallPushButton(QIcon(":/icons/window_min.svg"), this);
    Q_ASSERT(m_pMinButton);
    m_pMinButton->setToolTip(tr("Minimize"));
    layout->addWidget(m_pMinButton);
    check = connect(m_pMinButton, SIGNAL(clicked()),
                    this, SLOT(slotMinimize()));
    Q_ASSERT(check);
    
    m_pMaxButton = CreateSmallPushButton(QIcon(":/icons/window_max.svg"), this);
    Q_ASSERT(m_pMaxButton);
    m_pMaxButton->setToolTip(tr("Maximize"));
    layout->addWidget(m_pMaxButton);
    check = connect(m_pMaxButton, SIGNAL(clicked()),
                    this, SLOT(slotMaximize()));
    Q_ASSERT(check);
    
    m_pFloatButton = CreateSmallPushButton(QIcon(":/icons/floating.svg"), this);
    Q_ASSERT(m_pFloatButton);
    m_pFloatButton->setToolTip(tr("Floating"));
    layout->addWidget(m_pFloatButton);
    check = connect(m_pFloatButton, SIGNAL(clicked()),
                    this, SLOT(slotFloat()));
    Q_ASSERT(check);
    
    m_pCloseButton = CreateSmallPushButton(
                QIcon(":/icons/window_close.svg"), this);
    Q_ASSERT(m_pCloseButton);
    m_pCloseButton->setToolTip(tr("Close"));
    layout->addWidget(m_pCloseButton);
    check = connect(m_pCloseButton, SIGNAL(clicked()),
                    parent, SLOT(close()));
    Q_ASSERT(check);
    
    QDockWidget* pParent = qobject_cast<QDockWidget*>(parent);
    if(pParent) {    
        QDockWidget::DockWidgetFeatures features = pParent->features();
        VisibleCloseButton(QDockWidget::DockWidgetClosable & features);
        VisibleFloatButton(QDockWidget::DockWidgetFloatable & features);
        bool bFloating = pParent->isFloating();
        if(m_pFloatButton) {
            if(bFloating)
                m_pFloatButton->setIcon(QIcon(":/icons/unfloating.svg"));
            else
                m_pFloatButton->setIcon(QIcon(":/icons/floating.svg"));
        }
        bool check = connect(parent, SIGNAL(topLevelChanged(bool)),
                             this, SLOT(slotTopLevelChanged(bool)));
        Q_ASSERT(check);
        if(!pParent->isFloating())
        {
            VisibleMaximizeButton(false);
            VisibleMinimizeButton(false);
        }
    } else {
        VisibleFloatButton(false);
    }
}

int CTitleBar::SetWidgets(QList<QWidget*> pLstWidget)
{
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(this->layout());
    foreach (auto w, pLstWidget) {
        if(layout)
            layout->insertWidget(1, w);
    }
    return 0;
}

int CTitleBar::VisibleTitleButton(bool bVisible)
{
    if(m_pTitle)
        m_pTitle->setVisible(bVisible);
    return 0;
}

int CTitleBar::VisibleMaximizeButton(bool bVisible)
{
    if(m_pMaxButton)
        m_pMaxButton->setVisible(bVisible);
    return 0;
}

int CTitleBar::VisibleMinimizeButton(bool bVisible)
{
    if(m_pMaxButton)
        m_pMinButton->setVisible(bVisible);
    return 0;
}

int CTitleBar::VisibleFloatButton(bool bVisible)
{
    if(m_pFloatButton)
        m_pFloatButton->setVisible(bVisible);
    return 0;
}

int CTitleBar::VisibleCloseButton(bool bVisible)
{
    if(m_pCloseButton)
        m_pCloseButton->setVisible(bVisible);
    return 0;
}

void CTitleBar::slotMinimize()
{
    QWidget* pParent = qobject_cast<QWidget*>(parent());
    if(!pParent) return;
    if(pParent->isMinimized())
        pParent->showNormal();
    else
        pParent->showMinimized();
}

void CTitleBar::slotMaximize()
{
    QWidget* pParent = qobject_cast<QWidget*>(parent());
    if(!pParent) return;
    if(pParent->isMaximized())
        pParent->showNormal();
    else
        pParent->showMaximized();
}

void CTitleBar::slotFloat()
{
    QDockWidget* pParent = qobject_cast<QDockWidget*>(parent());
    if(pParent) {
        pParent->setFloating(!pParent->isFloating());
    }
}

void CTitleBar::slotTopLevelChanged(bool topLevel)
{
    if(m_pFloatButton) {
        if(topLevel) {
            VisibleMaximizeButton(true);
            //VisibleMinimizeButton(true);
            m_pFloatButton->setIcon(QIcon(":/icons/unfloating.svg"));
        } else {
            m_pFloatButton->setIcon(QIcon(":/icons/floating.svg"));
            VisibleMaximizeButton(false);
            VisibleMinimizeButton(false);
        }
    }
}
