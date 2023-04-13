/*!
 *  \author Kang Lin <kl222@126.com>
 *  The file is modified from the [RabbitCommon](https://github.com/KangLin/RabbitCommon)
 */

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

/*!
 * \brief The cursom title bar for QWidget, QDockWidget etc.
 */
class CTitleBar : public QWidget
{
    Q_OBJECT
    
public:
    /*!
     * \param parent
     */
    explicit CTitleBar(QWidget *parent);

    /* The default button */
    int VisibleTitleButton(bool bVisible);
    int VisibleMaximizeButton(bool bVisible);
    int VisibleMinimizeButton(bool bVisible);
    int VisibleFloatButton(bool bVisible);
    int VisibleCloseButton(bool bVisible);
    
    //! Set up user-defined buttons
    int SetWidgets(QList<QWidget*> pLstWidget);
    
    static QPushButton* CreateSmallPushButton(QIcon icon, QWidget *parent);

private Q_SLOTS:
    void slotMinimize();
    void slotMaximize();
    void slotFloat();
    void slotTopLevelChanged(bool topLevel);

private:
    QLabel *m_pTitle;
    QPushButton *m_pCloseButton, *m_pFloatButton, *m_pMaxButton, *m_pMinButton;
    QList<QWidget*> m_pLstWdiget;    
};

#endif // TITLEBAR_H
