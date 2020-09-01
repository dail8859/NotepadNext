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
/// \file   DockAreaWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaWidget.h"

#include <QStackedLayout>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QStyle>
#include <QPushButton>
#include <QDebug>
#include <QMenu>
#include <QSplitter>
#include <QXmlStreamWriter>
#include <QVector>
#include <QList>


#include "DockContainerWidget.h"
#include "DockWidget.h"
#include "FloatingDockContainer.h"
#include "DockManager.h"
#include "DockOverlay.h"
#include "DockAreaTabBar.h"
#include "DockSplitter.h"
#include "DockAreaTitleBar.h"
#include "DockComponentsFactory.h"
#include "DockWidgetTab.h"


namespace ads
{
static const char* const INDEX_PROPERTY = "index";
static const char* const ACTION_PROPERTY = "action";

/**
 * Internal dock area layout mimics stack layout but only inserts the current
 * widget into the internal QLayout object.
 * \warning Only the current widget has a parent. All other widgets
 * do not have a parent. That means, a widget that is in this layout may
 * return nullptr for its parent() function if it is not the current widget.
 */
class CDockAreaLayout
{
private:
	QBoxLayout* m_ParentLayout;
	QList<QWidget*> m_Widgets;
	int m_CurrentIndex = -1;
	QWidget* m_CurrentWidget = nullptr;

public:
	/**
	 * Creates an instance with the given parent layout
	 */
	CDockAreaLayout(QBoxLayout* ParentLayout)
		: m_ParentLayout(ParentLayout)
	{

	}

	/**
	 * Returns the number of widgets in this layout
	 */
	int count() const
	{
		return m_Widgets.count();
	}

	/**
	 * Inserts the widget at the given index position into the internal widget
	 * list
	 */
	void insertWidget(int index, QWidget* Widget)
	{
		Widget->setParent(nullptr);
		if (index < 0)
		{
			index = m_Widgets.count();
		}
		m_Widgets.insert(index, Widget);
		if (m_CurrentIndex < 0)
		{
			setCurrentIndex(index);
		}
		else
		{
			if (index <= m_CurrentIndex )
			{
				++m_CurrentIndex;
			}
		}
	}

	/**
	 * Removes the given widget from the layout
	 */
	void removeWidget(QWidget* Widget)
	{
		if (currentWidget() == Widget)
		{
			auto LayoutItem = m_ParentLayout->takeAt(1);
			if (LayoutItem)
			{
				LayoutItem->widget()->setParent(nullptr);
			}
			m_CurrentWidget = nullptr;
			m_CurrentIndex = -1;
		}
		else if (indexOf(Widget) < m_CurrentIndex)
		{
			--m_CurrentIndex;
		}
		m_Widgets.removeOne(Widget);
	}

	/**
	 * Returns the current selected widget
	 */
	QWidget* currentWidget() const
	{
		return m_CurrentWidget;
	}

	/**
	 * Activates the widget with the give index.
	 */
	void setCurrentIndex(int index)
	{
		QWidget *prev = currentWidget();
		QWidget *next = widget(index);
		if (!next || (next == prev && !m_CurrentWidget))
		{
			return;
		}

		bool reenableUpdates = false;
		QWidget *parent = m_ParentLayout->parentWidget();

		if (parent && parent->updatesEnabled())
		{
			reenableUpdates = true;
			parent->setUpdatesEnabled(false);
		}

		auto LayoutItem = m_ParentLayout->takeAt(1);
		if (LayoutItem)
		{
			LayoutItem->widget()->setParent(nullptr);
		}
		delete LayoutItem;

		m_ParentLayout->addWidget(next);
		if (prev)
		{
			prev->hide();
		}
		m_CurrentIndex = index;
		m_CurrentWidget = next;


		if (reenableUpdates)
		{
			parent->setUpdatesEnabled(true);
		}
	}

	/**
	 * Returns the index of the current active widget
	 */
	int currentIndex() const
	{
		return m_CurrentIndex;
	}

	/**
	 * Returns true if there are no widgets in the layout
	 */
	bool isEmpty() const
	{
		return m_Widgets.empty();
	}

	/**
	 * Returns the index of the given widget
	 */
	int indexOf(QWidget* w) const
	{
		return m_Widgets.indexOf(w);
	}

	/**
	 * Returns the widget for the given index
	 */
	QWidget* widget(int index) const
	{
		return (index < m_Widgets.size()) ? m_Widgets.at(index) : nullptr;
	}

	/**
	 * Returns the geometry of the current active widget
	 */
	QRect geometry() const
	{
		return m_Widgets.empty() ? QRect() : currentWidget()->geometry();
	}
};



using DockAreaLayout = CDockAreaLayout;
static const DockWidgetAreas DefaultAllowedAreas = AllDockAreas;


/**
 * Private data class of CDockAreaWidget class (pimpl)
 */
struct DockAreaWidgetPrivate
{
	CDockAreaWidget*	_this			= nullptr;
	QBoxLayout*			Layout			= nullptr;
	DockAreaLayout*		ContentsLayout	= nullptr;
	CDockAreaTitleBar*	TitleBar		= nullptr;
	CDockManager*		DockManager		= nullptr;
	bool UpdateTitleBarButtons = false;
	DockWidgetAreas		AllowedAreas	= DefaultAllowedAreas;
	QSize MinSizeHint;
	CDockAreaWidget::DockAreaFlags Flags{CDockAreaWidget::DefaultFlags};

	/**
	 * Private data constructor
	 */
	DockAreaWidgetPrivate(CDockAreaWidget* _public);

	/**
	 * Creates the layout for top area with tabs and close button
	 */
	void createTitleBar();

	/**
	 * Returns the dock widget with the given index
	 */
	CDockWidget* dockWidgetAt(int index)
	{
		return qobject_cast<CDockWidget*>(ContentsLayout->widget(index));
	}

	/**
	 * Convenience function to ease title widget access by index
	 */
	CDockWidgetTab* tabWidgetAt(int index)
	{
		return dockWidgetAt(index)->tabWidget();
	}


	/**
	 * Returns the tab action of the given dock widget
	 */
	QAction* dockWidgetTabAction(CDockWidget* DockWidget) const
	{
		return qvariant_cast<QAction*>(DockWidget->property(ACTION_PROPERTY));
	}

	/**
	 * Returns the index of the given dock widget
	 */
	int dockWidgetIndex(CDockWidget* DockWidget) const
	{
		return DockWidget->property(INDEX_PROPERTY).toInt();
	}

	/**
	 * Convenience function for tabbar access
	 */
	CDockAreaTabBar* tabBar() const
	{
		return TitleBar->tabBar();
	}

	/**
	 * Udpates the enable state of the close and detach button
	 */
	void updateTitleBarButtonStates();

	/**
	 * Scans all contained dock widgets for the max. minimum size hint
	 */
	void updateMinimumSizeHint()
	{
		MinSizeHint = QSize();
		for (int i = 0; i < ContentsLayout->count(); ++i)
		{
			auto Widget = ContentsLayout->widget(i);
			MinSizeHint.setHeight(qMax(MinSizeHint.height(), Widget->minimumSizeHint().height()));
			MinSizeHint.setWidth(qMax(MinSizeHint.width(), Widget->minimumSizeHint().width()));
		}
	}
};
// struct DockAreaWidgetPrivate


//============================================================================
DockAreaWidgetPrivate::DockAreaWidgetPrivate(CDockAreaWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaWidgetPrivate::createTitleBar()
{
	TitleBar = componentsFactory()->createDockAreaTitleBar(_this);
	Layout->addWidget(TitleBar);
	QObject::connect(tabBar(), &CDockAreaTabBar::tabCloseRequested, _this, &CDockAreaWidget::onTabCloseRequested);
	QObject::connect(TitleBar, &CDockAreaTitleBar::tabBarClicked, _this, &CDockAreaWidget::setCurrentIndex);
	QObject::connect(tabBar(), &CDockAreaTabBar::tabMoved, _this, &CDockAreaWidget::reorderDockWidget);
}


//============================================================================
void DockAreaWidgetPrivate::updateTitleBarButtonStates()
{
	if (_this->isHidden())
	{
		UpdateTitleBarButtons = true;
		return;
	}

	TitleBar->button(TitleBarButtonClose)->setEnabled(
		_this->features().testFlag(CDockWidget::DockWidgetClosable));
	TitleBar->button(TitleBarButtonUndock)->setEnabled(
		_this->features().testFlag(CDockWidget::DockWidgetFloatable));
	TitleBar->updateDockWidgetActionsButtons();
	UpdateTitleBarButtons = false;
}


//============================================================================
CDockAreaWidget::CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent) :
	QFrame(parent),
	d(new DockAreaWidgetPrivate(this))
{
	d->DockManager = DockManager;
	d->Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);

	d->createTitleBar();
	d->ContentsLayout = new DockAreaLayout(d->Layout);
	if (d->DockManager)
	{
		emit d->DockManager->dockAreaCreated(this);
	}
}

//============================================================================
CDockAreaWidget::~CDockAreaWidget()
{
    ADS_PRINT("~CDockAreaWidget()");
	delete d->ContentsLayout;
	delete d;
}


//============================================================================
CDockManager* CDockAreaWidget::dockManager() const
{
	return d->DockManager;
}


//============================================================================
CDockContainerWidget* CDockAreaWidget::dockContainer() const
{
	return internal::findParent<CDockContainerWidget*>(this);
}


//============================================================================
void CDockAreaWidget::addDockWidget(CDockWidget* DockWidget)
{
	insertDockWidget(d->ContentsLayout->count(), DockWidget);
}


//============================================================================
void CDockAreaWidget::insertDockWidget(int index, CDockWidget* DockWidget,
	bool Activate)
{
	d->ContentsLayout->insertWidget(index, DockWidget);
	DockWidget->setDockArea(this);
	DockWidget->tabWidget()->setDockAreaWidget(this);
	auto TabWidget = DockWidget->tabWidget();
	// Inserting the tab will change the current index which in turn will
	// make the tab widget visible in the slot
	d->tabBar()->blockSignals(true);
	d->tabBar()->insertTab(index, TabWidget);
	d->tabBar()->blockSignals(false);
	TabWidget->setVisible(!DockWidget->isClosed());
	DockWidget->setProperty(INDEX_PROPERTY, index);
	d->MinSizeHint.setHeight(qMax(d->MinSizeHint.height(), DockWidget->minimumSizeHint().height()));
	d->MinSizeHint.setWidth(qMax(d->MinSizeHint.width(), DockWidget->minimumSizeHint().width()));
	if (Activate)
	{
		setCurrentIndex(index);
	}
	// If this dock area is hidden, then we need to make it visible again
	// by calling DockWidget->toggleViewInternal(true);
	if (!this->isVisible() && d->ContentsLayout->count() > 1 && !dockManager()->isRestoringState())
	{
		DockWidget->toggleViewInternal(true);
	}
	d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
}


//============================================================================
void CDockAreaWidget::removeDockWidget(CDockWidget* DockWidget)
{
    ADS_PRINT("CDockAreaWidget::removeDockWidget");
    auto CurrentDockWidget = currentDockWidget();
  	auto NextOpenDockWidget = (DockWidget == CurrentDockWidget) ? nextOpenDockWidget(DockWidget) : nullptr;

	d->ContentsLayout->removeWidget(DockWidget);
	auto TabWidget = DockWidget->tabWidget();
	TabWidget->hide();
	d->tabBar()->removeTab(TabWidget);
	TabWidget->setParent(DockWidget);
	DockWidget->setDockArea(nullptr);
	CDockContainerWidget* DockContainer = dockContainer();
	if (NextOpenDockWidget)
	{
		setCurrentDockWidget(NextOpenDockWidget);
	}
	else if (d->ContentsLayout->isEmpty() && DockContainer->dockAreaCount() >= 1)
	{
        ADS_PRINT("Dock Area empty");
		DockContainer->removeDockArea(this);
		this->deleteLater();
		if(DockContainer->dockAreaCount() == 0)
		{
			if(CFloatingDockContainer*  FloatingDockContainer = DockContainer->floatingWidget())
			{
				FloatingDockContainer->hide();
				FloatingDockContainer->deleteLater();
			}
		}
	}
	else if (DockWidget == CurrentDockWidget)
	{
		// if contents layout is not empty but there are no more open dock
		// widgets, then we need to hide the dock area because it does not
		// contain any visible content
		hideAreaWithNoVisibleContent();
	}

	d->updateTitleBarButtonStates();
	updateTitleBarVisibility();
	d->updateMinimumSizeHint();
	auto TopLevelDockWidget = DockContainer->topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}

#if (ADS_DEBUG_LEVEL > 0)
	DockContainer->dumpLayout();
#endif
}


//============================================================================
void CDockAreaWidget::hideAreaWithNoVisibleContent()
{
	this->toggleView(false);

	// Hide empty parent splitters
	auto Splitter = internal::findParent<CDockSplitter*>(this);
	internal::hideEmptyParentSplitters(Splitter);

	//Hide empty floating widget
	CDockContainerWidget* Container = this->dockContainer();
	if (!Container->isFloating() && !CDockManager::testConfigFlag(CDockManager::HideSingleCentralWidgetTitleBar))
	{
		return;
	}

	updateTitleBarVisibility();
	auto TopLevelWidget = Container->topLevelDockWidget();
	auto FloatingWidget = Container->floatingWidget();
	if (TopLevelWidget)
	{
		if (FloatingWidget)
		{
			FloatingWidget->updateWindowTitle();
		}
		CDockWidget::emitTopLevelEventForWidget(TopLevelWidget, true);
	}
	else if (Container->openedDockAreas().isEmpty() && FloatingWidget)
	{
		FloatingWidget->hide();
	}
}


//============================================================================
void CDockAreaWidget::onTabCloseRequested(int Index)
{
    ADS_PRINT("CDockAreaWidget::onTabCloseRequested " << Index);
    auto* DockWidget = dockWidget(Index);
    if (DockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose))
    {
    	DockWidget->closeDockWidgetInternal();
    }
    else
    {
    	DockWidget->toggleView(false);
    }
}


//============================================================================
CDockWidget* CDockAreaWidget::currentDockWidget() const
{
	int CurrentIndex = currentIndex();
	if (CurrentIndex < 0)
	{
		return nullptr;
	}

	return dockWidget(CurrentIndex);
}


//============================================================================
void CDockAreaWidget::setCurrentDockWidget(CDockWidget* DockWidget)
{
	if (dockManager()->isRestoringState())
	{
		return;
	}

	internalSetCurrentDockWidget(DockWidget);
}


//============================================================================
void CDockAreaWidget::internalSetCurrentDockWidget(CDockWidget* DockWidget)
{
	int Index = index(DockWidget);
	if (Index < 0)
	{
		return;
	}

	setCurrentIndex(Index);
}


//============================================================================
void CDockAreaWidget::setCurrentIndex(int index)
{
	auto TabBar = d->tabBar();
	if (index < 0 || index > (TabBar->count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
    }

	auto cw = d->ContentsLayout->currentWidget();
	auto nw = d->ContentsLayout->widget(index);
	if (cw == nw && !nw->isHidden())
	{
		return;
	}

    emit currentChanging(index);
    TabBar->setCurrentIndex(index);
	d->ContentsLayout->setCurrentIndex(index);
	d->ContentsLayout->currentWidget()->show();
	emit currentChanged(index);
}


//============================================================================
int CDockAreaWidget::currentIndex() const
{
	return d->ContentsLayout->currentIndex();
}


//============================================================================
QRect CDockAreaWidget::titleBarGeometry() const
{
	return d->TitleBar->geometry();
}

//============================================================================
QRect CDockAreaWidget::contentAreaGeometry() const
{
	return d->ContentsLayout->geometry();
}


//============================================================================
int CDockAreaWidget::index(CDockWidget* DockWidget)
{
	return d->ContentsLayout->indexOf(DockWidget);
}


//============================================================================
QList<CDockWidget*> CDockAreaWidget::dockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		DockWidgetList.append(dockWidget(i));
	}
	return DockWidgetList;
}


//============================================================================
int CDockAreaWidget::openDockWidgetsCount() const
{
	int Count = 0;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		if (!dockWidget(i)->isClosed())
		{
			++Count;
		}
	}
	return Count;
}


//============================================================================
QList<CDockWidget*> CDockAreaWidget::openedDockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		CDockWidget* DockWidget = dockWidget(i);
		if (!DockWidget->isClosed())
		{
			DockWidgetList.append(dockWidget(i));
		}
	}
	return DockWidgetList;
}


//============================================================================
int CDockAreaWidget::indexOfFirstOpenDockWidget() const
{
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		if (!dockWidget(i)->isClosed())
		{
			return i;
		}
	}

	return -1;
}


//============================================================================
int CDockAreaWidget::dockWidgetsCount() const
{
	return d->ContentsLayout->count();
}


//============================================================================
CDockWidget* CDockAreaWidget::dockWidget(int Index) const
{
	return qobject_cast<CDockWidget*>(d->ContentsLayout->widget(Index));
}


//============================================================================
void CDockAreaWidget::reorderDockWidget(int fromIndex, int toIndex)
{
    ADS_PRINT("CDockAreaWidget::reorderDockWidget");
	if (fromIndex >= d->ContentsLayout->count() || fromIndex < 0
     || toIndex >= d->ContentsLayout->count() || toIndex < 0 || fromIndex == toIndex)
	{
        ADS_PRINT("Invalid index for tab movement" << fromIndex << toIndex);
		return;
	}

	auto Widget = d->ContentsLayout->widget(fromIndex);
	d->ContentsLayout->removeWidget(Widget);
	d->ContentsLayout->insertWidget(toIndex, Widget);
	setCurrentIndex(toIndex);
}


//============================================================================
void CDockAreaWidget::toggleDockWidgetView(CDockWidget* DockWidget, bool Open)
{
	Q_UNUSED(DockWidget);
	Q_UNUSED(Open);
	updateTitleBarVisibility();
}


//============================================================================
void CDockAreaWidget::updateTitleBarVisibility()
{
	CDockContainerWidget* Container = dockContainer();
	if (!Container)
	{
		return;
	}

    if (CDockManager::testConfigFlag(CDockManager::AlwaysShowTabs))
    {
        return;
    }

	if (d->TitleBar)
	{
		bool Hidden = Container->hasTopLevelDockWidget() && (Container->isFloating()
			|| CDockManager::testConfigFlag(CDockManager::HideSingleCentralWidgetTitleBar));
		Hidden |= (d->Flags.testFlag(HideSingleWidgetTitleBar) && openDockWidgetsCount() == 1);
		d->TitleBar->setVisible(!Hidden);
	}
}


//============================================================================
void CDockAreaWidget::markTitleBarMenuOutdated()
{
	if (d->TitleBar)
	{
		d->TitleBar->markTabsMenuOutdated();
	}
}



//============================================================================
void CDockAreaWidget::saveState(QXmlStreamWriter& s) const
{
	s.writeStartElement("Area");
	s.writeAttribute("Tabs", QString::number(d->ContentsLayout->count()));
	auto CurrentDockWidget = currentDockWidget();
	QString Name = CurrentDockWidget ? CurrentDockWidget->objectName() : "";
	s.writeAttribute("Current", Name);
	// To keep the saved XML data small, we only save the allowed areas and the
	// dock area flags if the values are different from the default values
	if (d->AllowedAreas != DefaultAllowedAreas)
	{
		s.writeAttribute("AllowedAreas", QString::number(d->AllowedAreas, 16));
	}

	if (d->Flags != DefaultFlags)
	{
		s.writeAttribute("Flags", QString::number(d->Flags, 16));
	}
    ADS_PRINT("CDockAreaWidget::saveState TabCount: " << d->ContentsLayout->count()
            << " Current: " << Name);
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		dockWidget(i)->saveState(s);
	}
	s.writeEndElement();
}


//============================================================================
CDockWidget* CDockAreaWidget::nextOpenDockWidget(CDockWidget* DockWidget) const
{
	auto OpenDockWidgets = openedDockWidgets();
	if (OpenDockWidgets.count() > 1 || (OpenDockWidgets.count() == 1 && OpenDockWidgets[0] != DockWidget))
	{
		CDockWidget* NextDockWidget;
		if (OpenDockWidgets.last() == DockWidget)
		{
			NextDockWidget = OpenDockWidgets[OpenDockWidgets.count() - 2];
		}
		else
		{
			int NextIndex = OpenDockWidgets.indexOf(DockWidget) + 1;
			NextDockWidget = OpenDockWidgets[NextIndex];
		}

		return NextDockWidget;
	}
	else
	{
		return nullptr;
	}
}


//============================================================================
CDockWidget::DockWidgetFeatures CDockAreaWidget::features(eBitwiseOperator Mode) const
{
	if (BitwiseAnd == Mode)
	{
		CDockWidget::DockWidgetFeatures Features(CDockWidget::AllDockWidgetFeatures);
		for (const auto DockWidget : dockWidgets())
		{
			Features &= DockWidget->features();
		}
		return Features;
	}
	else
	{
		CDockWidget::DockWidgetFeatures Features(CDockWidget::NoDockWidgetFeatures);
		for (const auto DockWidget : dockWidgets())
		{
			Features |= DockWidget->features();
		}
		return Features;
	}
}


//============================================================================
void CDockAreaWidget::toggleView(bool Open)
{
	setVisible(Open);

	emit viewToggled(Open);
}


//============================================================================
void CDockAreaWidget::setVisible(bool Visible)
{
	Super::setVisible(Visible);
	if (d->UpdateTitleBarButtons)
	{
		d->updateTitleBarButtonStates();
	}
}


//============================================================================
void CDockAreaWidget::setAllowedAreas(DockWidgetAreas areas)
{
	d->AllowedAreas = areas;
}


//============================================================================
DockWidgetAreas CDockAreaWidget::allowedAreas() const
{
	return d->AllowedAreas;
}


//============================================================================
CDockAreaWidget::DockAreaFlags CDockAreaWidget::dockAreaFlags() const
{
	return d->Flags;
}


//============================================================================
void CDockAreaWidget::setDockAreaFlags(DockAreaFlags Flags)
{
	auto ChangedFlags = d->Flags ^ Flags;
	d->Flags = Flags;
	if (ChangedFlags.testFlag(HideSingleWidgetTitleBar))
	{
		updateTitleBarVisibility();
	}
}


//============================================================================
void CDockAreaWidget::setDockAreaFlag(eDockAreaFlag Flag, bool On)
{
	auto flags = dockAreaFlags();
	internal::setFlag(flags, Flag, On);
	setDockAreaFlags(flags);
}


//============================================================================
QAbstractButton* CDockAreaWidget::titleBarButton(TitleBarButton which) const
{
	return d->TitleBar->button(which);
}


//============================================================================
void CDockAreaWidget::closeArea()
{
	// If there is only one single dock widget and this widget has the
	// DeleteOnClose feature, then we delete the dock widget now
	auto OpenDockWidgets = openedDockWidgets();
	if (OpenDockWidgets.count() == 1 && OpenDockWidgets[0]->features().testFlag(CDockWidget::DockWidgetDeleteOnClose))
	{
		OpenDockWidgets[0]->closeDockWidgetInternal();
	}
	else
	{
		for (auto DockWidget : openedDockWidgets())
		{
			DockWidget->toggleView(false);
		}
	}
}


//============================================================================
void CDockAreaWidget::closeOtherAreas()
{
	dockContainer()->closeOtherAreas(this);
}


//============================================================================
CDockAreaTitleBar* CDockAreaWidget::titleBar() const
{
	return d->TitleBar;
}


//============================================================================
bool CDockAreaWidget::isCentralWidgetArea() const
{
    if (dockWidgetsCount()!= 1)
    {
        return false;
    }

    return dockManager()->centralWidget() == dockWidgets()[0];
}


//============================================================================
QSize CDockAreaWidget::minimumSizeHint() const
{
	return d->MinSizeHint.isValid() ? d->MinSizeHint : Super::minimumSizeHint();
}


//============================================================================
void CDockAreaWidget::onDockWidgetFeaturesChanged()
{
	if (d->TitleBar)
	{
		d->updateTitleBarButtonStates();
	}
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaWidget.cpp
