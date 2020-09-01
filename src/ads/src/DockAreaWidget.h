#ifndef DockAreaWidgetH
#define DockAreaWidgetH
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   DockAreaWidget.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"
#include "DockWidget.h"

QT_FORWARD_DECLARE_CLASS(QXmlStreamWriter)
QT_FORWARD_DECLARE_CLASS(QAbstractButton)

namespace ads
{
struct DockAreaWidgetPrivate;
class CDockManager;
class CDockContainerWidget;
class DockContainerWidgetPrivate;
class CDockAreaTitleBar;


/**
 * DockAreaWidget manages multiple instances of DockWidgets.
 * It displays a title tab, which is clickable and will switch to
 * the contents associated to the title when clicked.
 */
class ADS_EXPORT CDockAreaWidget : public QFrame
{
	Q_OBJECT
private:
	DockAreaWidgetPrivate* d; ///< private data (pimpl)
	friend struct DockAreaWidgetPrivate;
	friend class CDockContainerWidget;
	friend class DockContainerWidgetPrivate;
	friend class CDockWidgetTab;
	friend struct DockWidgetPrivate;
	friend class CDockWidget;
	friend struct DockManagerPrivate;
	friend class CDockManager;
	void onDockWidgetFeaturesChanged();

private slots:
	void onTabCloseRequested(int Index);

	/**
	 * Reorder the index position of DockWidget at fromIndx to toIndex
	 * if a tab in the tabbar is dragged from one index to another one
	 */
	void reorderDockWidget(int fromIndex, int toIndex);

protected:
	/**
	 * Inserts a dock widget into dock area.
	 * All dockwidgets in the dock area tabified in a stacked layout with tabs.
	 * The index indicates the index of the new dockwidget in the tabbar and
	 * in the stacked layout. If the Activate parameter is true, the new
	 * DockWidget will be the active one in the stacked layout
	 */
	void insertDockWidget(int index, CDockWidget* DockWidget, bool Activate = true);

	/**
	 * Add a new dock widget to dock area.
	 * All dockwidgets in the dock area tabified in a stacked layout with tabs
	 */
	void addDockWidget(CDockWidget* DockWidget);

	/**
	 * Removes the given dock widget from the dock area
	 */
	void removeDockWidget(CDockWidget* DockWidget);

	/**
	 * Called from dock widget if it is opened or closed
	 */
	void toggleDockWidgetView(CDockWidget* DockWidget, bool Open);

	/**
	 * This is a helper function to get the next open dock widget to activate
	 * if the given DockWidget will be closed or removed.
	 * The function returns the next widget that should be activated or
	 * nullptr in case there are no more open widgets in this area.
	 */
	CDockWidget* nextOpenDockWidget(CDockWidget* DockWidget) const;

	/**
	 * Returns the index of the given DockWidget in the internal layout
	 */
	int index(CDockWidget* DockWidget);

	/**
	 * Call this function, if you already know, that the dock does not
	 * contain any visible content (any open dock widgets).
	 */
	void hideAreaWithNoVisibleContent();

	/**
	 * Updates the dock area layout and components visibility
	 */
	void updateTitleBarVisibility();

	/**
	 * This is the internal private function for setting the current widget.
	 * This function is called by the public setCurrentDockWidget() function
	 * and by the dock manager when restoring the state
	 */
	void internalSetCurrentDockWidget(CDockWidget* DockWidget);

	/**
	 * Marks tabs menu to update
	 */
	void markTitleBarMenuOutdated();

protected slots:
	void toggleView(bool Open);

public:
	using Super = QFrame;

	/**
	 * Dock area related flags
	 */
	enum eDockAreaFlag
	{
		HideSingleWidgetTitleBar = 0x0001,
		DefaultFlags = 0x0000
	};
	Q_DECLARE_FLAGS(DockAreaFlags, eDockAreaFlag)

    /**
	 * Default Constructor
	 */
	CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaWidget();

	/**
	 * Returns the dock manager object this dock area belongs to
	 */
	CDockManager* dockManager() const;

	/**
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if there is no
	 */
	CDockContainerWidget* dockContainer() const;

    /**
     * Returns the largest minimumSizeHint() of the dock widgets in this
     * area.
     * The minimum size hint is updated if a dock widget is removed or added.
     */
    virtual QSize minimumSizeHint() const override;

	/**
	 * Returns the rectangle of the title area
	 */
	QRect titleBarGeometry() const;

	/**
	 * Returns the rectangle of the content
	 */
	QRect contentAreaGeometry() const;

	/**
	 * Returns the number of dock widgets in this area
	 */
	int dockWidgetsCount() const;

	/**
	 * Returns a list of all dock widgets in this dock area.
	 * This list contains open and closed dock widgets.
	 */
	QList<CDockWidget*> dockWidgets() const;

	/**
	 * Returns the number of open dock widgets in this area
	 */
	int openDockWidgetsCount() const;

	/**
	 * Returns a list of dock widgets that are not closed.
	 */
	QList<CDockWidget*> openedDockWidgets() const;

	/**
	 * Returns a dock widget by its index
	 */
	CDockWidget* dockWidget(int Index) const;

	/**
	 * Returns the index of the current active dock widget or -1 if there
	 * are is no active dock widget (ie.e if all dock widgets are closed)
	 */
	int currentIndex() const;

	/**
	 * Returns the index of the first open dock widgets in the list of
	 * dock widgets.
	 * This function is here for performance reasons. Normally it would
	 * be possible to take the first dock widget from the list returned by
	 * openedDockWidgets() function. But that function enumerates all
	 * dock widgets while this functions stops after the first open dock widget.
	 * If there are no open dock widgets, the function returns -1.
	 */
	int indexOfFirstOpenDockWidget() const;

	/**
	 * Returns the current active dock widget or a nullptr if there is no
	 * active dock widget (i.e. if all dock widgets are closed)
	 */
	CDockWidget* currentDockWidget() const;

	/**
	 * Shows the tab with the given dock widget
	 */
	void setCurrentDockWidget(CDockWidget* DockWidget);

	/**
	 * Saves the state into the given stream
	 */
	void saveState(QXmlStreamWriter& Stream) const;

	/**
	 * This functions returns the dock widget features of all dock widget in
	 * this area.
	 * A bitwise and is used to combine the flags of all dock widgets. That
	 * means, if only one single dock widget does not support a certain flag,
	 * the whole dock are does not support the flag. I.e. if one single
	 * dock widget in this area is not closable, the whole dock are is not
	 * closable.
	 */
	CDockWidget::DockWidgetFeatures features(eBitwiseOperator Mode = BitwiseAnd) const;

	/**
	 * Returns the title bar button corresponding to the given title bar
	 * button identifier
	 */
	QAbstractButton* titleBarButton(TitleBarButton which) const;

	/**
	 * Update the close button if visibility changed
	 */
	virtual void setVisible(bool Visible) override;

	/**
	 * Configures the areas of this particular dock area that are allowed for docking
	 */
	void setAllowedAreas(DockWidgetAreas areas);

	/**
	 * Returns flags with all allowed drop areas of this particular dock area
	 */
	DockWidgetAreas allowedAreas() const;

	/**
	 * Returns the title bar of this dock area
	 */
	CDockAreaTitleBar* titleBar() const;

	/**
	 * Returns the dock area flags - a combination of flags that configure the
	 * appearance and features of the dock area.
	 * \see setDockAreaFlasg()
	 */
	DockAreaFlags dockAreaFlags() const;

	/**
	 * Sets the dock area flags - a combination of flags that configure the
	 * appearance and features of the dock area
	 */
	void setDockAreaFlags(DockAreaFlags Flags);

	/**
	 * Sets the dock area flag Flag on this widget if on is true; otherwise
	 * clears the flag.
	 */
	void setDockAreaFlag(eDockAreaFlag Flag, bool On);

    /**
     * Returns true if the area contains the central widget of it's manager.
     */
    bool isCentralWidgetArea() const;

public slots:
	/**
	 * This activates the tab for the given tab index.
	 * If the dock widget for the given tab is not visible, the this function
	 * call will make it visible.
	 */
	void setCurrentIndex(int index);

	/**
	 * Closes the dock area and all dock widgets in this area
	 */
	void closeArea();

	/**
	 * This function closes all other areas except of this area
	 */
	void closeOtherAreas();

signals:
	/**
	 * This signal is emitted when user clicks on a tab at an index.
	 */
	void tabBarClicked(int index);

    /**
     * This signal is emitted when the tab bar's current tab is about to be changed. The new
     * current has the given index, or -1 if there isn't a new one.
     * @param index
     */
    void currentChanging(int index);

	/**
	 * This signal is emitted when the tab bar's current tab changes. The new
	 * current has the given index, or -1 if there isn't a new one
	 * @param index
	 */
	void currentChanged(int index);

	/**
	 * This signal is emitted if the visibility of this dock area is toggled
	 * via toggle view function
	 */
	void viewToggled(bool Open);
}; // class DockAreaWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaWidgetH
