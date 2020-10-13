%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

/**
 * Container that manages a number of dock areas with single dock widgets
 * or tabyfied dock widgets in each area.
 * Each window that support docking has a DockContainerWidget. That means
 * the main application window and all floating windows are ore contain
 * an DockContainerWidget.
 */
class CDockContainerWidget : QFrame
{
    %TypeHeaderCode
    #include <DockContainerWidget.h>
    %End

protected:
	virtual bool event(QEvent *e);
	QSplitter* rootSplitter() const;
	void createRootSplitter();
	void dropFloatingWidget(ads::CFloatingDockContainer* FloatingWidget, const QPoint& TargetPos);
    void dropWidget(QWidget* Widget, DockWidgetArea DropArea, CDockAreaWidget* TargetAreaWidget);
	void addDockArea(ads::CDockAreaWidget* DockAreaWidget /Transfer/, ads::DockWidgetArea area = ads::CenterDockWidgetArea);
	void removeDockArea(ads::CDockAreaWidget* area /TransferBack/);
	void saveState(QXmlStreamWriter& Stream) const;
	bool restoreState(CDockingStateReader& Stream, bool Testing);
	ads::CDockAreaWidget* lastAddedDockAreaWidget(ads::DockWidgetArea area) const;
	ads::CDockWidget* topLevelDockWidget() const;
	ads::CDockAreaWidget* topLevelDockArea() const;
    QList<ads::CDockWidget*> dockWidgets() const;
    void updateSplitterHandles(QSplitter* splitter);
    
public:
	/**
	 * Default Constructor
	 */
	CDockContainerWidget(ads::CDockManager* DockManager /TransferThis/, QWidget* parent /TransferThis/ = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockContainerWidget();

	/**
	 * Adds dockwidget into the given area.
	 * If DockAreaWidget is not null, then the area parameter indicates the area
	 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
	 * be dropped into the container.
	 * \return Returns the dock area widget that contains the new DockWidget
	 */
	ads::CDockAreaWidget* addDockWidget(ads::DockWidgetArea area, ads::CDockWidget* Dockwidget /Transfer/,
		ads::CDockAreaWidget* DockAreaWidget /Transfer/ = 0);

	/**
	 * Removes dockwidget
	 */
	void removeDockWidget(ads::CDockWidget* Dockwidget) /TransferBack/;

	/**
	 * Returns the current zOrderIndex
	 */
	virtual unsigned int zOrderIndex() const;

	/**
	 * This function returns true if this container widgets z order index is
	 * higher than the index of the container widget given in Other parameter
	 */
	bool isInFrontOf(ads::CDockContainerWidget* Other) const;

	/**
	 * Returns the dock area at teh given global position or 0 if there is no
	 * dock area at this position
	 */
	ads::CDockAreaWidget* dockAreaAt(const QPoint& GlobalPos) const;

	/**
	 * Returns the dock area at the given Index or 0 if the index is out of
	 * range
	 */
	ads::CDockAreaWidget* dockArea(int Index) const;

	/**
	 * Returns the list of dock areas that are not closed
	 * If all dock widgets in a dock area are closed, the dock area will be closed
	 */
	QList<ads::CDockAreaWidget*> openedDockAreas() const;
	bool hasTopLevelDockWidget() const;

	/**
	 * Returns the number of dock areas in this container
	 */
	int dockAreaCount() const;

	/**
	 * Returns the number of visible dock areas
	 */
	int visibleDockAreaCount() const;

	/**
	 * This function returns true, if this container is in a floating widget
	 */
	bool isFloating() const;

	/**
	 * Dumps the layout for debugging purposes
	 */
	void dumpLayout();

	/**
	 * This functions returns the dock widget features of all dock widget in
	 * this container.
	 * A bitwise and is used to combine the flags of all dock widgets. That
	 * means, if only dock widget does not support a certain flag, the whole
	 * dock are does not support the flag.
	 */
	ads::CDockWidget::DockWidgetFeatures features() const;

	/**
	 * If this dock container is in a floating widget, this function returns
	 * the floating widget.
	 * Else, it returns a nullptr.
	 */
	ads::CFloatingDockContainer* floatingWidget() const;

	/**
	 * Call this function to close all dock areas except the KeepOpenArea
	 */
	void closeOtherAreas(ads::CDockAreaWidget* KeepOpenArea);

signals:
	/**
	 * This signal is emitted if one or multiple dock areas has been added to
	 * the internal list of dock areas.
	 * If multiple dock areas are inserted, this signal is emitted only once
	 */
	void dockAreasAdded();

	/**
	 * This signal is emitted if one or multiple dock areas has been removed
	 */
	void dockAreasRemoved();

	/**
	 * This signal is emitted if a dock area is opened or closed via
	 * toggleView() function
	 */
	void dockAreaViewToggled(ads::CDockAreaWidget* DockArea, bool Open);
}; // class DockContainerWidget
};
 // namespace ads

%End
