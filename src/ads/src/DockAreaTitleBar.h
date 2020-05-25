#ifndef DockAreaTitleBarH
#define DockAreaTitleBarH
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
/// \file   DockAreaTitleBar.h
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Declaration of CDockAreaTitleBar class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QAbstractButton)

namespace ads
{
class CDockAreaTabBar;
class CDockAreaWidget;
struct DockAreaTitleBarPrivate;

/**
 * Title bar of a dock area.
 * The title bar contains a tabbar with all tabs for a dock widget group and
 * with a tabs menu button, a undock button and a close button.
 */
class ADS_EXPORT CDockAreaTitleBar : public QFrame
{
	Q_OBJECT
private:
	DockAreaTitleBarPrivate* d; ///< private data (pimpl)
	friend struct DockAreaTitleBarPrivate;

private slots:
	void onTabsMenuAboutToShow();
	void onCloseButtonClicked();
	void onUndockButtonClicked();
	void onTabsMenuActionTriggered(QAction* Action);
	void onCurrentTabChanged(int Index);

protected:
		/**
	 * Stores mouse position to detect dragging
	 */
	virtual void mousePressEvent(QMouseEvent* ev) override;

	/**
	 * Stores mouse position to detect dragging
	 */
	virtual void mouseReleaseEvent(QMouseEvent* ev) override;

	/**
	 * Starts floating the complete docking area including all dock widgets,
	 * if it is not the last dock area in a floating widget
	 */
	virtual void mouseMoveEvent(QMouseEvent* ev) override;

	/**
	 * Double clicking the title bar also starts floating of the complete area
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

	/**
	 * Show context menu
	 */
	virtual void contextMenuEvent(QContextMenuEvent *event) override;

public slots:
	/**
	 * Call this slot to tell the title bar that it should update the tabs menu
	 * the next time it is shown.
	 */
	void markTabsMenuOutdated();


public:
	using Super = QFrame;

	/**
	 * Default Constructor
	 */
	CDockAreaTitleBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTitleBar();

	/**
	 * Returns the pointer to the tabBar()
	 */
	CDockAreaTabBar* tabBar() const;

	/**
	 * Returns the button corresponding to the given title bar button identifier
	 */
	QAbstractButton* button(TitleBarButton which) const;

	/**
	 * Updates the visibility of the dock widget actions in the title bar
	 */
	void updateDockWidgetActionsButtons();

	/**
	 * Marks the tabs menu outdated before it calls its base class
	 * implementation
	 */
	virtual void setVisible(bool Visible) override;

	/**
	 * Inserts a custom widget at position index into this title bar.
	 * If index is negative, the widget is added at the end.
	 * You can use this function to insert custom widgets into the title bar.
	 */
	void insertWidget(int index, QWidget *widget);

	/**
	 * Searches for widget widget in this title bar.
	 * You can use this function, to get the position of the default
	 * widget in the tile bar.
	 * \code
	 * int tabBarIndex = TitleBar->indexOf(TitleBar->tabBar());
	 * int closeButtonIndex = TitleBar->indexOf(TitleBar->button(TitleBarButtonClose));
	 * \endcode
	 */
	int indexOf(QWidget *widget) const;

signals:
	/**
	 * This signal is emitted if a tab in the tab bar is clicked by the user
	 * or if the user clicks on a tab item in the title bar tab menu.
	 */
	void tabBarClicked(int index);
}; // class name

}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTitleBarH
