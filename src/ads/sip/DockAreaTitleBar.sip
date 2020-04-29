%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockAreaTitleBar : QFrame
{
    %TypeHeaderCode
    #include <DockAreaTitleBar.h>
    %End

protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void contextMenuEvent(QContextMenuEvent *event);

public slots:
	void markTabsMenuOutdated();


public:
	CDockAreaTitleBar(ads::CDockAreaWidget* parent /TransferThis/);
	virtual ~CDockAreaTitleBar();
	ads::CDockAreaTabBar* tabBar() const;
	QAbstractButton* button(ads::TitleBarButton which) const;
 	void updateDockWidgetActionsButtons();
	virtual void setVisible(bool Visible);
	void insertWidget(int index, QWidget *widget /Transfer/ );
	int indexOf(QWidget *widget) const;


signals:
	void tabBarClicked(int index);
};

};

%End
