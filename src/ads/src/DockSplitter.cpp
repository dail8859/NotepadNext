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
/// \file   DockSplitter.cpp
/// \author Uwe Kindler
/// \date   24.03.2017
/// \brief  Implementation of CDockSplitter
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockSplitter.h"

#include <QDebug>
#include <QChildEvent>

#include "DockAreaWidget.h"

namespace ads
{
/**
 * Private dock splitter data
 */
struct DockSplitterPrivate
{
	CDockSplitter* _this;
	int VisibleContentCount = 0;

	DockSplitterPrivate(CDockSplitter* _public) : _this(_public) {}
};

//============================================================================
CDockSplitter::CDockSplitter(QWidget *parent)
	: QSplitter(parent),
	  d(new DockSplitterPrivate(this))
{
	setProperty("ads-splitter", true);
	setChildrenCollapsible(false);
}


//============================================================================
CDockSplitter::CDockSplitter(Qt::Orientation orientation, QWidget *parent)
	: QSplitter(orientation, parent),
	  d(new DockSplitterPrivate(this))
{

}

//============================================================================
CDockSplitter::~CDockSplitter()
{
    ADS_PRINT("~CDockSplitter");
	delete d;
}


//============================================================================
bool CDockSplitter::hasVisibleContent() const
{
	// TODO Cache or precalculate this to speed up
	for (int i = 0; i < count(); ++i)
	{
		if (!widget(i)->isHidden())
		{
			return true;
		}
	}

	return false;
}


//============================================================================
QWidget* CDockSplitter::firstWidget() const
{
	return (count() > 0) ? widget(0) : nullptr;
}


//============================================================================
QWidget* CDockSplitter::lastWidget() const
{
	return (count() > 0) ? widget(count() - 1) : nullptr;
}

//============================================================================
bool CDockSplitter::isResizingWithContainer() const
{
    for (auto area : findChildren<CDockAreaWidget*>())
    {
        if(area->isCentralWidgetArea())
        {
            return true;
        }
    }

    return false;
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockSplitter.cpp
