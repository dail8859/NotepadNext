#ifndef DockAreaTabBarH
#define DockAreaTabBarH
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
/// \file   DockAreaTabBar.h
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Declaration of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QScrollArea>
#include "ads_globals.h"

namespace ads
{
class CDockAreaWidget;
class CDockWidgetTab;
struct DockAreaTabBarPrivate;
class CDockAreaTitleBar;
class CFloatingDockContainer;
class IFloatingWidget;

/**
 * Custom tabbar implementation for tab area that is shown on top of a
 * dock area widget.
 * The tabbar displays the tab widgets of the contained dock widgets.
 * We cannot use QTabBar here because it does a lot of fancy animations
 * that will crash the application if a tab is removed while the animation
 * has not finished. And we need to remove a tab, if the user drags a
 * a dock widget out of a group of tabbed widgets
 */
class ADS_EXPORT CDockAreaTabBar : public QScrollArea
{
	Q_OBJECT
private:
	DockAreaTabBarPrivate* d; ///< private data (pimpl)
	friend struct DockAreaTabBarPrivate;
	friend class CDockAreaTitleBar;

private slots:
	void onTabClicked();
	void onTabCloseRequested();
	void onCloseOtherTabsRequested();
	void onTabWidgetMoved(const QPoint& GlobalPos);

protected:
	virtual void wheelEvent(QWheelEvent* Event) override;


public:
	using Super = QScrollArea;

	/**
	 * Default Constructor
	 */
	CDockAreaTabBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTabBar();

	/**
	 * Inserts the given dock widget tab at the given position.
	 * Inserting a new tab at an index less than or equal to the current index
	 * will increment the current index, but keep the current tab.
	 */
	void insertTab(int Index, CDockWidgetTab* Tab);

	/**
	 * Removes the given DockWidgetTab from the tabbar
	 */
	void removeTab(CDockWidgetTab* Tab);

	/**
	 * Returns the number of tabs in this tabbar
	 */
	int count() const;

	/**
	 * Returns the current index or -1 if no tab is selected
	 */
	int currentIndex() const;

	/**
	 * Returns the current tab or a nullptr if no tab is selected.
	 */
	CDockWidgetTab* currentTab() const;

	/**
	 * Returns the tab with the given index
	 */
	CDockWidgetTab* tab(int Index) const;

	/**
	 * Filters the tab widget events
	 */
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

	/**
	 * This function returns true if the tab is open, that means if it is
	 * visible to the user. If the function returns false, the tab is
	 * closed
	 */
	bool isTabOpen(int Index) const;

	/**
	 * Overrides the minimumSizeHint() function of QScrollArea
	 * The minimumSizeHint() is bigger than the sizeHint () for the scroll
	 * area because even if the scrollbars are invisible, the required speace
	 * is reserved in the minimumSizeHint(). This override simply returns
	 * sizeHint();
	 */
	virtual QSize minimumSizeHint() const override;

	/**
	 * The function provides a sizeHint that matches the height of the
	 * internal viewport.
	 */
	virtual QSize sizeHint() const override;

public slots:
	/**
	 * This property sets the index of the tab bar's visible tab
	 */
	void setCurrentIndex(int Index);

	/**
	 * This function will close the tab given in Index param.
	 * Closing a tab means, the tab will be hidden, it will not be removed
	 */
	void closeTab(int Index);

signals:
    /**
     * This signal is emitted when the tab bar's current tab is about to be changed. The new
     * current has the given index, or -1 if there isn't a new one.
     */
	void currentChanging(int Index);

	/**
	 * This signal is emitted when the tab bar's current tab changes. The new
	 * current has the given index, or -1 if there isn't a new one
	 */
	void currentChanged(int Index);

	/**
	 * This signal is emitted when user clicks on a tab
	 */
	void tabBarClicked(int index);

	/**
	 * This signal is emitted when the close button on a tab is clicked.
	 * The index is the index that should be closed.
	 */
	void tabCloseRequested(int index);

	/**
	 * This signal is emitted if a tab has been closed
	 */
	void tabClosed(int index);

	/**
	 * This signal is emitted if a tab has been opened.
	 * A tab is opened if it has been made visible
	 */
	void tabOpened(int index);

	/**
	 * This signal is emitted when the tab has moved the tab at index position
	 * from to index position to.
	 */
	void tabMoved(int from, int to);

	/**
	 * This signal is emitted, just before the tab with the given index is
	 * removed
	 */
	void removingTab(int index);

	/**
	 * This signal is emitted if a tab has been inserted
	 */
	void tabInserted(int index);

	/**
	 * This signal is emitted when a tab title elide state has been changed
	 */
	void elidedChanged(bool elided);
}; // class CDockAreaTabBar
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTabBarH

