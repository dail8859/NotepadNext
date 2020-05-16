#ifndef DockAreaTitleBar_pH
#define DockAreaTitleBar_pH
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
/// \file   DockAreaTitleBar_p.h
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Declaration of classes CTitleBarButton and CSpacerWidget
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>
#include <QToolButton>

#include "ads_globals.h"

namespace ads
{
using tTitleBarButton = QToolButton;

/**
* Title bar button of a dock area that customizes tTitleBarButton appearance/behaviour
* according to various config flags such as:
* CDockManager::DockAreaHas_xxx_Button - if set to 'false' keeps the button always invisible
* CDockManager::DockAreaHideDisabledButtons - if set to 'true' hides button when it is disabled
*/
class CTitleBarButton : public tTitleBarButton
{
	Q_OBJECT
		
private:
	bool Visible = true;
	bool HideWhenDisabled = false;

public:
	using Super = tTitleBarButton;
	CTitleBarButton(bool visible = true, QWidget* parent = nullptr);

	/**
	* Adjust this visibility change request with our internal settings:
	*/
	virtual void setVisible(bool visible) override;

protected:
	/**
	* Handle EnabledChanged signal to set button invisible if the configured
	*/
	bool event(QEvent *ev) override;
};


/**
* This spacer widget is here because of the following problem.
* The dock area title bar handles mouse dragging and moving the floating widget.
* The  problem is, that if the title bar becomes invisible, i.e. if the dock
* area contains only one single dock widget and the dock area is moved
* into a floating widget, then mouse events are not handled anymore and dragging
* of the floating widget stops.
*/
class CSpacerWidget : public QWidget
{
	Q_OBJECT
public:
	using Super = QWidget;
	CSpacerWidget(QWidget* Parent = 0);
	virtual QSize sizeHint() const override {return QSize(0, 0);}
	virtual QSize minimumSizeHint() const override {return QSize(0, 0);}
};

}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTitleBar_pH
