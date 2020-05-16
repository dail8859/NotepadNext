%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockAreaTabBar : QScrollArea
{
    %TypeHeaderCode
    #include <DockAreaTabBar.h>
    %End
    
protected:
	virtual void wheelEvent(QWheelEvent* Event);

public:
	CDockAreaTabBar(ads::CDockAreaWidget* parent /TransferThis/);
	virtual ~CDockAreaTabBar();
	void insertTab(int Index, ads::CDockWidgetTab* Tab /Transfer/);
	void removeTab(ads::CDockWidgetTab* Tab) /TransferBack/;
	int count() const;
	int currentIndex() const;
	ads::CDockWidgetTab* currentTab() const;
	ads::CDockWidgetTab* tab(int Index) const;
	virtual bool eventFilter(QObject *watched, QEvent *event);
	bool isTabOpen(int Index) const;
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;
 	void elidedChanged(bool elided);

public slots:
	void setCurrentIndex(int Index);
	void closeTab(int Index);

signals:
	void currentChanging(int Index);
	void currentChanged(int Index);
	void tabBarClicked(int index);
	void tabCloseRequested(int index);
	void tabClosed(int index);
	void tabOpened(int index);
	void tabMoved(int from, int to);
	void removingTab(int index);
	void tabInserted(int index);
};

};

%End
