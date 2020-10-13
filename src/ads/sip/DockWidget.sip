%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockWidget : QFrame
{
    %TypeHeaderCode
    #include <DockWidget.h>
    %End
    
protected:
    void setDockManager(ads::CDockManager* DockManager /Transfer/ );
    void setDockArea(ads::CDockAreaWidget* DockArea /Transfer/ );
    void setToggleViewActionChecked(bool Checked);
    void saveState(QXmlStreamWriter& Stream) const;
    void flagAsUnassigned();
    static void emitTopLevelEventForWidget(ads::CDockWidget* TopLevelDockWidget, bool Floating);
    void emitTopLevelChanged(bool Floating);
    void setClosedState(bool Closed);
    void toggleViewInternal(bool Open);
    bool closeDockWidgetInternal(bool ForceClose = false);
    
public:
	enum DockWidgetFeature
	{
		DockWidgetClosable,
		DockWidgetMovable,
		DockWidgetFloatable,
		DockWidgetDeleteOnClose,
        CustomCloseHandling,
        DockWidgetFocusable,
        DefaultDockWidgetFeatures,
        AllDockWidgetFeatures,
		NoDockWidgetFeatures
	};
    typedef QFlags<ads::CDockWidget::DockWidgetFeature> DockWidgetFeatures;

	enum eState
	{
		StateHidden,
		StateDocked,
		StateFloating
	};

	enum eInsertMode
	{
		AutoScrollArea,
		ForceScrollArea,
		ForceNoScrollArea
	};
	
    enum eMinimumSizeHintMode
    {
    	MinimumSizeHintFromDockWidget,
    	MinimumSizeHintFromContent
    };

	enum eToggleViewActionMode
	{
		ActionModeToggle,
		ActionModeShow
	};


	CDockWidget(const QString &title, QWidget* parent /TransferThis/ = 0);
	virtual ~CDockWidget();
	virtual QSize minimumSizeHint() const;
	void setWidget(QWidget* widget /Transfer/, ads::CDockWidget::eInsertMode InsertMode = AutoScrollArea);
	QWidget* takeWidget() /TransferBack/;
	QWidget* widget() const;
	ads::CDockWidgetTab* tabWidget() const;
	void setFeatures(ads::CDockWidget::DockWidgetFeatures features);
	void setFeature(ads::CDockWidget::DockWidgetFeature flag, bool on);
	ads::CDockWidget::DockWidgetFeatures features() const;
	ads::CDockManager* dockManager() const;
	ads::CDockContainerWidget* dockContainer() const;
	ads::CDockAreaWidget* dockAreaWidget() const;
	bool isFloating() const;
	bool isInFloatingContainer() const;
	bool isClosed() const;
	QAction* toggleViewAction() const;
	void setToggleViewActionMode(ads::CDockWidget::eToggleViewActionMode Mode);
	void setMinimumSizeHintMode(ads::CDockWidget::eMinimumSizeHintMode Mode);
    bool isCentralWidget() const;
	void setIcon(const QIcon& Icon);
	QIcon icon() const;
	QToolBar* toolBar() const;
	QToolBar* createDefaultToolBar();
	void setToolBar(QToolBar* ToolBar /Transfer/ );
	void setToolBarStyle(Qt::ToolButtonStyle Style, ads::CDockWidget::eState State);
	Qt::ToolButtonStyle toolBarStyle(ads::CDockWidget::eState State) const;
	void setToolBarIconSize(const QSize& IconSize, ads::CDockWidget::eState State);
	QSize toolBarIconSize(eState State) const;
    void setTitleBarActions(QList<QAction*> actions);
    virtual QList<QAction*> titleBarActions() const;

	void setTabToolTip(const QString &text);
    bool isFullScreen() const;
    bool isTabbed() const;
    bool isCurrentTab() const;

public:
	virtual bool event(QEvent *e);

public slots:
	void toggleView(bool Open = true);
	void setAsCurrentTab();
	void raise();
    void setFloating();
    void deleteDockWidget();
    void closeDockWidget();
    void showFullScreen();
    void showNormal();

signals:
	void viewToggled(bool Open);
	void closed();
	void titleChanged(const QString& Title);
	void topLevelChanged(bool topLevel);
    void closeRequested();
	void visibilityChanged(bool visible);
    void featuresChanged(ads::CDockWidget::DockWidgetFeatures features);
};

};

%End
