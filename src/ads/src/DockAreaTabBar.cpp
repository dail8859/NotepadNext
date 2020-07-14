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
/// \file   DockAreaTabBar.cpp
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Implementation of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDragPreview.h"
#include "DockAreaTabBar.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>
#include <QBoxLayout>
#include <QApplication>
#include <QtGlobal>

#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"

#include <iostream>


namespace ads
{
/**
 * Private data class of CDockAreaTabBar class (pimpl)
 */
struct DockAreaTabBarPrivate
{
	CDockAreaTabBar* _this;
	CDockAreaWidget* DockArea;
	QWidget* TabsContainerWidget;
	QBoxLayout* TabsLayout;
	int CurrentIndex = -1;

	/**
	 * Private data constructor
	 */
	DockAreaTabBarPrivate(CDockAreaTabBar* _public);

	/**
	 * Update tabs after current index changed or when tabs are removed.
	 * The function reassigns the stylesheet to update the tabs
	 */
	void updateTabs();

	/**
	 * Convenience function to access first tab
	 */
	CDockWidgetTab* firstTab() const {return _this->tab(0);}

	/**
	 * Convenience function to access last tab
	 */
	CDockWidgetTab* lastTab() const {return _this->tab(_this->count() - 1);}
};
// struct DockAreaTabBarPrivate

//============================================================================
DockAreaTabBarPrivate::DockAreaTabBarPrivate(CDockAreaTabBar* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaTabBarPrivate::updateTabs()
{
	// Set active TAB and update all other tabs to be inactive
	for (int i = 0; i < _this->count(); ++i)
	{
		auto TabWidget = _this->tab(i);
		if (!TabWidget)
		{
			continue;
		}

		if (i == CurrentIndex)
		{
			TabWidget->show();
			TabWidget->setActiveTab(true);
			_this->ensureWidgetVisible(TabWidget);
		}
		else
		{
			TabWidget->setActiveTab(false);
		}
	}
}


//============================================================================
CDockAreaTabBar::CDockAreaTabBar(CDockAreaWidget* parent) :
	QScrollArea(parent),
	d(new DockAreaTabBarPrivate(this))
{
	d->DockArea = parent;
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setFrameStyle(QFrame::NoFrame);
	setWidgetResizable(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	d->TabsContainerWidget = new QWidget();
	d->TabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	d->TabsContainerWidget->setObjectName("tabsContainerWidget");
	d->TabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	d->TabsLayout->setContentsMargins(0, 0, 0, 0);
	d->TabsLayout->setSpacing(0);
	d->TabsLayout->addStretch(1);
	d->TabsContainerWidget->setLayout(d->TabsLayout);
	setWidget(d->TabsContainerWidget);

    setFocusPolicy(Qt::NoFocus);
}


//============================================================================
CDockAreaTabBar::~CDockAreaTabBar()
{
	delete d;
}


//============================================================================
void CDockAreaTabBar::wheelEvent(QWheelEvent* Event)
{
	Event->accept();
	const int direction = Event->angleDelta().y();
	if (direction < 0)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
	}
	else
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
	}
}


//============================================================================
void CDockAreaTabBar::setCurrentIndex(int index)
{
	if (index == d->CurrentIndex)
	{
		return;
	}

	if (index < -1 || index > (count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
    }

    emit currentChanging(index);
	d->CurrentIndex = index;
	d->updateTabs();
	updateGeometry();
	emit currentChanged(index);
}


//============================================================================
int CDockAreaTabBar::count() const
{
	// The tab bar contains a stretch item as last item
	return d->TabsLayout->count() - 1;
}


//===========================================================================
void CDockAreaTabBar::insertTab(int Index, CDockWidgetTab* Tab)
{
	d->TabsLayout->insertWidget(Index, Tab);
	connect(Tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
	connect(Tab, SIGNAL(closeRequested()), this, SLOT(onTabCloseRequested()));
	connect(Tab, SIGNAL(closeOtherTabsRequested()), this, SLOT(onCloseOtherTabsRequested()));
	connect(Tab, SIGNAL(moved(const QPoint&)), this, SLOT(onTabWidgetMoved(const QPoint&)));
	connect(Tab, SIGNAL(elidedChanged(bool)), this, SIGNAL(elidedChanged(bool)));
	Tab->installEventFilter(this);
	emit tabInserted(Index);
    if (Index <= d->CurrentIndex)
	{
		setCurrentIndex(d->CurrentIndex + 1);
    }
    else if (d->CurrentIndex == -1)
    {
    	setCurrentIndex(Index);
    }

	updateGeometry();
}


//===========================================================================
void CDockAreaTabBar::removeTab(CDockWidgetTab* Tab)
{
	if (!count())
	{
		return;
	}
    ADS_PRINT("CDockAreaTabBar::removeTab ");
	int NewCurrentIndex = currentIndex();
	int RemoveIndex = d->TabsLayout->indexOf(Tab);
	if (count() == 1)
	{
		NewCurrentIndex = -1;
	}
	if (NewCurrentIndex > RemoveIndex)
	{
		NewCurrentIndex--;
	}
	else if (NewCurrentIndex == RemoveIndex)
	{
		NewCurrentIndex = -1;
		// First we walk to the right to search for the next visible tab
		for (int i = (RemoveIndex + 1); i < count(); ++i)
		{
			if (tab(i)->isVisibleTo(this))
			{
				NewCurrentIndex = i - 1;
				break;
			}
		}

		// If there is no visible tab right to this tab then we walk to
		// the left to find a visible tab
		if (NewCurrentIndex < 0)
		{
			for (int i = (RemoveIndex - 1); i >= 0; --i)
			{
				if (tab(i)->isVisibleTo(this))
				{
					NewCurrentIndex = i;
					break;
				}
			}
		}
	}

	emit removingTab(RemoveIndex);
	d->TabsLayout->removeWidget(Tab);
	Tab->disconnect(this);
	Tab->removeEventFilter(this);
    ADS_PRINT("NewCurrentIndex " << NewCurrentIndex);
	if (NewCurrentIndex != d->CurrentIndex)
	{
		setCurrentIndex(NewCurrentIndex);
	}
	else
	{
		d->updateTabs();
	}

	updateGeometry();
}


//===========================================================================
int CDockAreaTabBar::currentIndex() const
{
	return d->CurrentIndex;
}


//===========================================================================
CDockWidgetTab* CDockAreaTabBar::currentTab() const
{
	if (d->CurrentIndex < 0)
	{
		return nullptr;
	}
	else
	{
		return qobject_cast<CDockWidgetTab*>(d->TabsLayout->itemAt(d->CurrentIndex)->widget());
	}
}


//===========================================================================
void CDockAreaTabBar::onTabClicked()
{
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(sender());
	if (!Tab)
	{
		return;
	}

	int index = d->TabsLayout->indexOf(Tab);
	if (index < 0)
	{
		return;
	}
	setCurrentIndex(index);
 	emit tabBarClicked(index);
}


//===========================================================================
void CDockAreaTabBar::onTabCloseRequested()
{
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(sender());
	int Index = d->TabsLayout->indexOf(Tab);
	closeTab(Index);
}


//===========================================================================
void CDockAreaTabBar::onCloseOtherTabsRequested()
{
	auto Sender = qobject_cast<CDockWidgetTab*>(sender());
	for (int i = 0; i < count(); ++i)
	{
		auto Tab = tab(i);
		if (Tab->isClosable() && !Tab->isHidden() && Tab != Sender)
		{
			// If the dock widget is deleted with the closeTab() call, its tab
			// it will no longer be in the layout, and thus the index needs to
			// be updated to not skip any tabs
			int Offset = Tab->dockWidget()->features().testFlag(
				CDockWidget::DockWidgetDeleteOnClose) ? 1 : 0;
			closeTab(i);

			// If the the dock widget blocks closing, i.e. if the flag
			// CustomCloseHandling is set, and the dock widget is still open,
			// then we do not need to correct the index
			if (Tab->dockWidget()->isClosed())
			{
				i -= Offset;
			}
		}
	}
}


//===========================================================================
CDockWidgetTab* CDockAreaTabBar::tab(int Index) const
{
	if (Index >= count() || Index < 0)
	{
		return nullptr;
	}
	return qobject_cast<CDockWidgetTab*>(d->TabsLayout->itemAt(Index)->widget());
}


//===========================================================================
void CDockAreaTabBar::onTabWidgetMoved(const QPoint& GlobalPos)
{
	CDockWidgetTab* MovingTab = qobject_cast<CDockWidgetTab*>(sender());
	if (!MovingTab)
	{
		return;
	}

	int fromIndex = d->TabsLayout->indexOf(MovingTab);
	auto MousePos = mapFromGlobal(GlobalPos);
	MousePos.rx() = qMax(d->firstTab()->geometry().left(), MousePos.x());
	MousePos.rx() = qMin(d->lastTab()->geometry().right(), MousePos.x());
	int toIndex = -1;
	// Find tab under mouse
	for (int i = 0; i < count(); ++i)
	{
		CDockWidgetTab* DropTab = tab(i);
		if (DropTab == MovingTab || !DropTab->isVisibleTo(this)
		    || !DropTab->geometry().contains(MousePos))
		{
			continue;
		}

		toIndex = d->TabsLayout->indexOf(DropTab);
		if (toIndex == fromIndex)
		{
			toIndex = -1;
		}
		break;
	}

	if (toIndex > -1)
	{
		d->TabsLayout->removeWidget(MovingTab);
		d->TabsLayout->insertWidget(toIndex, MovingTab);
        ADS_PRINT("tabMoved from " << fromIndex << " to " << toIndex);
		emit tabMoved(fromIndex, toIndex);
		setCurrentIndex(toIndex);
	}
	else
	{
		// Ensure that the moved tab is reset to its start position
		d->TabsLayout->update();
	}
}

//===========================================================================
void CDockAreaTabBar::closeTab(int Index)
{
	if (Index < 0 || Index >= count())
	{
		return;
	}

	auto Tab = tab(Index);
	if (Tab->isHidden())
	{
		return;
	}
	emit tabCloseRequested(Index);
}


//===========================================================================
bool CDockAreaTabBar::eventFilter(QObject *watched, QEvent *event)
{
	bool Result = Super::eventFilter(watched, event);
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(watched);
	if (!Tab)
	{
		return Result;
	}

	switch (event->type())
	{
	case QEvent::Hide:
		 emit tabClosed(d->TabsLayout->indexOf(Tab));
		 updateGeometry();
		 break;

	case QEvent::Show:
		 emit tabOpened(d->TabsLayout->indexOf(Tab));
		 updateGeometry();
		 break;

    // Setting the text of a tab will cause a LayoutRequest event
    case QEvent::LayoutRequest:
         updateGeometry();
         break;

	default:
		break;
	}

	return Result;
}


//===========================================================================
bool CDockAreaTabBar::isTabOpen(int Index) const
{
	if (Index < 0 || Index >= count())
	{
		return false;
	}

	return !tab(Index)->isHidden();
}


//===========================================================================
QSize CDockAreaTabBar::minimumSizeHint() const
{
	QSize Size = sizeHint();
	Size.setWidth(10);
	return Size;
}


//===========================================================================
QSize CDockAreaTabBar::sizeHint() const
{
	return d->TabsContainerWidget->sizeHint();
}

} // namespace ads


//---------------------------------------------------------------------------
// EOF DockAreaTabBar.cpp
