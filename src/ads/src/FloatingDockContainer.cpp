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
/// \file   FloatingDockContainer.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CFloatingDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDockContainer.h"

#include <iostream>

#include <QBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QDebug>
#include <QAbstractButton>
#include <QElapsedTimer>
#include <QTime>

#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockOverlay.h"

#ifdef Q_OS_LINUX
#include "linux/FloatingWidgetTitleBar.h"
#include <xcb/xcb.h>
#endif

namespace ads
{
static unsigned int zOrderCounter = 0;
/**
 * Private data class of CFloatingDockContainer class (pimpl)
 */
struct FloatingDockContainerPrivate
{
	CFloatingDockContainer *_this;
	CDockContainerWidget *DockContainer;
	unsigned int zOrderIndex = ++zOrderCounter;
	QPointer<CDockManager> DockManager;
	eDragState DraggingState = DraggingInactive;
	QPoint DragStartMousePosition;
	CDockContainerWidget *DropContainer = nullptr;
	CDockAreaWidget *SingleDockArea = nullptr;
	QPoint DragStartPos;
#ifdef Q_OS_LINUX
    QWidget* MouseEventHandler = nullptr;
    CFloatingWidgetTitleBar* TitleBar = nullptr;
#endif

	/**
	 * Private data constructor
	 */
	FloatingDockContainerPrivate(CFloatingDockContainer *_public);

	void titleMouseReleaseEvent();
	void updateDropOverlays(const QPoint &GlobalPos);

	/**
	 * Returns true if the given config flag is set
	 */
	static bool testConfigFlag(CDockManager::eConfigFlag Flag)
	{
		return CDockManager::configFlags().testFlag(Flag);
	}

	/**
	 * Tests is a certain state is active
	 */
	bool isState(eDragState StateId) const
	{
		return StateId == DraggingState;
	}

	void setState(eDragState StateId)
	{
		DraggingState = StateId;
	}

	void setWindowTitle(const QString &Text)
	{
#ifdef Q_OS_LINUX
        TitleBar->setTitle(Text);
#else
		_this->setWindowTitle(Text);
#endif
	}

	/**
	 * Reflect the current dock widget title in the floating widget windowTitle()
	 * depending on the CDockManager::FloatingContainerHasWidgetTitle flag
	 */
	void reflectCurrentWidget(CDockWidget* CurrentWidget)
	{
		// reflect CurrentWidget's title if configured to do so, otherwise display application name as window title
		if (testConfigFlag(CDockManager::FloatingContainerHasWidgetTitle))
		{
			setWindowTitle(CurrentWidget->windowTitle());
		}
		else
		{
			setWindowTitle(qApp->applicationDisplayName());
		}

		// reflect CurrentWidget's icon if configured to do so, otherwise display application icon as window icon
		QIcon CurrentWidgetIcon = CurrentWidget->icon();
		if (testConfigFlag(CDockManager::FloatingContainerHasWidgetIcon)
				&& !CurrentWidgetIcon.isNull())
		{
			_this->setWindowIcon(CurrentWidget->icon());
		}
		else
		{
			_this->setWindowIcon(QApplication::windowIcon());
		}
	}

	/**
	 * Handles escape key press when dragging around the floating widget
	 */
	void handleEscapeKey();
};
// struct FloatingDockContainerPrivate

//============================================================================
FloatingDockContainerPrivate::FloatingDockContainerPrivate(
    CFloatingDockContainer *_public) :
	_this(_public)
{

}

//============================================================================
void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setState(DraggingInactive);
	if (!DropContainer)
	{
		return;
	}

	if (DockManager->dockAreaOverlay()->dropAreaUnderCursor()
	    != InvalidDockWidgetArea
	    || DockManager->containerOverlay()->dropAreaUnderCursor()
	        != InvalidDockWidgetArea)
	{
		CDockOverlay *Overlay = DockManager->containerOverlay();
		if (!Overlay->dropOverlayRect().isValid())
		{
			Overlay = DockManager->dockAreaOverlay();
		}

		// Resize the floating widget to the size of the highlighted drop area
		// rectangle
		QRect Rect = Overlay->dropOverlayRect();
		int FrameWidth = (_this->frameSize().width() - _this->rect().width())
		    / 2;
		int TitleBarHeight = _this->frameSize().height()
		    - _this->rect().height() - FrameWidth;
		if (Rect.isValid())
		{
			QPoint TopLeft = Overlay->mapToGlobal(Rect.topLeft());
			TopLeft.ry() += TitleBarHeight;
			_this->setGeometry(
			    QRect(TopLeft,
			        QSize(Rect.width(), Rect.height() - TitleBarHeight)));
			QApplication::processEvents();
		}
		DropContainer->dropFloatingWidget(_this, QCursor::pos());
	}

	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}

//============================================================================
void FloatingDockContainerPrivate::updateDropOverlays(const QPoint &GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

	auto Containers = DockManager->dockContainers();
	CDockContainerWidget *TopContainer = nullptr;
	for (auto ContainerWidget : Containers)
	{
		if (!ContainerWidget->isVisible())
		{
			continue;
		}

		if (DockContainer == ContainerWidget)
		{
			continue;
		}

		QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
		if (ContainerWidget->rect().contains(MappedPos))
		{
			if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
			{
				TopContainer = ContainerWidget;
			}
		}
	}

	DropContainer = TopContainer;
	auto ContainerOverlay = DockManager->containerOverlay();
	auto DockAreaOverlay = DockManager->dockAreaOverlay();

	if (!TopContainer)
	{
		ContainerOverlay->hideOverlay();
		DockAreaOverlay->hideOverlay();
		return;
	}

	int VisibleDockAreas = TopContainer->visibleDockAreaCount();
	ContainerOverlay->setAllowedAreas(
	    VisibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
	DockWidgetArea ContainerArea = ContainerOverlay->showOverlay(TopContainer);
	ContainerOverlay->enableDropPreview(ContainerArea != InvalidDockWidgetArea);
	auto DockArea = TopContainer->dockAreaAt(GlobalPos);
	if (DockArea && DockArea->isVisible() && VisibleDockAreas > 0)
	{
		DockAreaOverlay->enableDropPreview(true);
		DockAreaOverlay->setAllowedAreas(
		    (VisibleDockAreas == 1) ? NoDockWidgetArea : DockArea->allowedAreas());
		DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

		// A CenterDockWidgetArea for the dockAreaOverlay() indicates that
		// the mouse is in the title bar. If the ContainerArea is valid
		// then we ignore the dock area of the dockAreaOverlay() and disable
		// the drop preview
		if ((Area == CenterDockWidgetArea)
		    && (ContainerArea != InvalidDockWidgetArea))
		{
			DockAreaOverlay->enableDropPreview(false);
			ContainerOverlay->enableDropPreview(true);
		}
		else
		{
			ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
		}
	}
	else
	{
		DockAreaOverlay->hideOverlay();
	}
}


//============================================================================
void FloatingDockContainerPrivate::handleEscapeKey()
{
	ADS_PRINT("FloatingDockContainerPrivate::handleEscapeKey()");
	setState(DraggingInactive);
	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockManager *DockManager) :
	tFloatingWidgetBase(DockManager),
	d(new FloatingDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->DockContainer = new CDockContainerWidget(DockManager, this);
	connect(d->DockContainer, SIGNAL(dockAreasAdded()), this,
	    SLOT(onDockAreasAddedOrRemoved()));
	connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this,
	    SLOT(onDockAreasAddedOrRemoved()));

#ifdef Q_OS_LINUX
    d->TitleBar = new CFloatingWidgetTitleBar(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    QDockWidget::setWidget(d->DockContainer);
    QDockWidget::setFloating(true);
    QDockWidget::setFeatures(QDockWidget::AllDockWidgetFeatures);
    setTitleBarWidget(d->TitleBar);
    connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
#else
	setWindowFlags(
	    Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);
	l->addWidget(d->DockContainer);
#endif

	DockManager->registerFloatingWidget(this);
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget *DockArea) :
	CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);
#ifdef Q_OS_LINUX
    d->TitleBar->enableCloseButton(isClosable());
#endif
    auto TopLevelDockWidget = topLevelDockWidget();
    if (TopLevelDockWidget)
    {
    	TopLevelDockWidget->emitTopLevelChanged(true);
    }
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockWidget *DockWidget) :
	CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
#ifdef Q_OS_LINUX
    d->TitleBar->enableCloseButton(isClosable());
#endif
    auto TopLevelDockWidget = topLevelDockWidget();
    if (TopLevelDockWidget)
    {
    	TopLevelDockWidget->emitTopLevelChanged(true);
    }
}

//============================================================================
CFloatingDockContainer::~CFloatingDockContainer()
{
	ADS_PRINT("~CFloatingDockContainer");
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}

//============================================================================
CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}

//============================================================================
void CFloatingDockContainer::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if ((event->type() == QEvent::ActivationChange) && isActiveWindow())
	{
		ADS_PRINT("FloatingWidget::changeEvent QEvent::ActivationChange ");
		d->zOrderIndex = ++zOrderCounter;
		return;
	}
}

//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	switch (d->DraggingState)
	{
	case DraggingMousePressed:
#ifdef Q_OS_WIN
		qApp->installEventFilter(this);
#endif
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
		break;

	case DraggingFloatingWidget:
		d->updateDropOverlays(QCursor::pos());
#ifdef Q_OS_MACOS
		// In OSX when hiding the DockAreaOverlay the application would set
		// the main window as the active window for some reason. This fixes
		// that by resetting the active window to the floating widget after
		// updating the overlays.
		QApplication::setActiveWindow(this);
#endif
		break;
	default:
		break;
	}


}

//============================================================================
void CFloatingDockContainer::closeEvent(QCloseEvent *event)
{
	ADS_PRINT("CFloatingDockContainer closeEvent");
	d->setState(DraggingInactive);
	event->ignore();

	if (isClosable())
	{
		auto TopLevelDockWidget = topLevelDockWidget();
		if (TopLevelDockWidget && TopLevelDockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose))
		{
			if (!TopLevelDockWidget->closeDockWidgetInternal())
			{
				return;
			}
		}

		// In Qt version after 5.9.2 there seems to be a bug that causes the
		// QWidget::event() function to not receive any NonClientArea mouse
		// events anymore after a close/show cycle. The bug is reported here:
		// https://bugreports.qt.io/browse/QTBUG-73295
		// The following code is a workaround for Qt versions > 5.9.2 that seems
		// to work
		// Starting from Qt version 5.12.2 this seems to work again. But
		// now the QEvent::NonClientAreaMouseButtonPress function returns always
		// Qt::RightButton even if the left button was pressed
        this->hide();
	}
}

//============================================================================
void CFloatingDockContainer::hideEvent(QHideEvent *event)
{
	Super::hideEvent(event);
    if (event->spontaneous())
    {
        return;
    }

    // Prevent toogleView() events during restore state
    if (d->DockManager->isRestoringState())
    {
        return;
    }

	for (auto DockArea : d->DockContainer->openedDockAreas())
	{
		for (auto DockWidget : DockArea->openedDockWidgets())
		{
			DockWidget->toggleView(false);
		}
	}
}

//============================================================================
void CFloatingDockContainer::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
}


//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	switch (d->DraggingState)
	{
	case DraggingInactive:
	{
		// Normally we would check here, if the left mouse button is pressed.
		// But from QT version 5.12.2 on the mouse events from
		// QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
		// The event always returns Qt::RightButton even if the left button
		// is clicked.
		// It is really great to work around the whole NonClientMouseArea
		// bugs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
		if (e->type() == QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
#else
		if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
#endif
		{
			ADS_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
			d->DragStartPos = pos();
			d->setState(DraggingMousePressed);
		}
	}
	break;

	case DraggingMousePressed:
		switch (e->type())
		{
		case QEvent::NonClientAreaMouseButtonDblClick:
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick");
			d->setState(DraggingInactive);
			break;

		case QEvent::Resize:
			// If the first event after the mouse press is a resize event, then
			// the user resizes the window instead of dragging it around.
			// But there is one exception. If the window is maximized,
			// then dragging the window via title bar will cause the widget to
			// leave the maximized state. This in turn will trigger a resize event.
			// To know, if the resize event was triggered by user via moving a
			// corner of the window frame or if it was caused by a windows state
			// change, we check, if we are not in maximized state.
			if (!isMaximized())
			{
				d->setState(DraggingInactive);
			}
			break;

		default:
			break;
		}
		break;

	case DraggingFloatingWidget:
		if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
		{
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease");
			d->titleMouseReleaseEvent();
		}
		break;

	default:
		break;
	}

#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << QTime::currentTime() << "CFloatingDockContainer::event " << e->type();
#endif
	return QWidget::event(e);
}


//============================================================================
bool CFloatingDockContainer::eventFilter(QObject *watched, QEvent *e)
{
	Q_UNUSED(watched);
	// I have not found a way to detect non client area key press events to
	// handle escape key presses. On Windows, if the escape key is pressed while
	// dragging around a widget, the widget position is reset to its start position
	// which in turn generates a QEvent::NonClientAreaMouseButtonRelease event
	// if the mouse is outside of the widget after the move to its initial position
	// or a QEvent::MouseButtonRelease event, if the mouse is inside of teh widget
	// after the position has been reset.
	// So we can install an event filter on the application to get these events
	// here to properly cancel dragging and hide the overlays.
	// If we are in DraggingFloatingWidget state, it means the widget
	// has been dragged already but if the position is the same like
	// the start position, then this is an indication that the escape
	// key has been pressed.
	if (e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::NonClientAreaMouseButtonRelease)
	{
		ADS_PRINT("CFloatingDockContainer::eventFilter QEvent::MouseButtonRelease or"
			"QEvent::NonClientAreaMouseButtonRelease" << "d->DragggingState " << d->DraggingState);
		qApp->removeEventFilter(this);
		if (d->DragStartPos == pos())
		{
			d->handleEscapeKey();
			return true;
		}
		return false;
	}

#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << QTime::currentTime() << "CFloatingDockContainer::eventFilter " << e->type();
#endif
	return false;
}


//============================================================================
void CFloatingDockContainer::startFloating(const QPoint &DragStartMousePos,
    const QSize &Size, eDragState DragState, QWidget *MouseEventHandler)
{
#ifndef Q_OS_LINUX
	Q_UNUSED(MouseEventHandler)
#endif
	resize(Size);
	d->setState(DragState);
	d->DragStartMousePosition = DragStartMousePos;
#ifdef Q_OS_LINUX
	if (DraggingFloatingWidget == DragState)
	{
        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
		d->MouseEventHandler = MouseEventHandler;
		if (d->MouseEventHandler)
		{
			d->MouseEventHandler->grabMouse();
		}
	}
#endif
	moveFloating();
	show();
}

//============================================================================
void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition
	    - QPoint(BorderSize, 0);
	move(moveToPos);
}

//============================================================================
bool CFloatingDockContainer::isClosable() const
{
	return d->DockContainer->features().testFlag(
	    CDockWidget::DockWidgetClosable);
}

//============================================================================
void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	ADS_PRINT("CFloatingDockContainer::onDockAreasAddedOrRemoved()");
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->SingleDockArea = TopLevelDockArea;
		CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
		d->reflectCurrentWidget(CurrentWidget);
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
		    SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
			    SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		d->setWindowTitle(qApp->applicationDisplayName());
		setWindowIcon(QApplication::windowIcon());
	}
}

//============================================================================
void CFloatingDockContainer::updateWindowTitle()
{
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		CDockWidget* CurrentWidget = TopLevelDockArea->currentDockWidget();
		d->reflectCurrentWidget(CurrentWidget);
	}
	else
	{
		d->setWindowTitle(qApp->applicationDisplayName());
		setWindowIcon(QApplication::windowIcon());
	}
}

//============================================================================
void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
	d->reflectCurrentWidget(CurrentWidget);
}

//============================================================================
bool CFloatingDockContainer::restoreState(CDockingStateReader &Stream,
    bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}

	onDockAreasAddedOrRemoved();
	return true;
}

//============================================================================
bool CFloatingDockContainer::hasTopLevelDockWidget() const
{
	return d->DockContainer->hasTopLevelDockWidget();
}

//============================================================================
CDockWidget* CFloatingDockContainer::topLevelDockWidget() const
{
	return d->DockContainer->topLevelDockWidget();
}

//============================================================================
QList<CDockWidget*> CFloatingDockContainer::dockWidgets() const
{
	return d->DockContainer->dockWidgets();
}

//============================================================================
void CFloatingDockContainer::finishDragging()
{
	ADS_PRINT("CFloatingDockContainer::finishDragging");
#ifdef Q_OS_LINUX
   setAttribute(Qt::WA_X11NetWmWindowTypeDock, false);
   setWindowOpacity(1);
   activateWindow();
   if (d->MouseEventHandler)
   {
       d->MouseEventHandler->releaseMouse();
       d->MouseEventHandler = nullptr;
   }
#endif
   d->titleMouseReleaseEvent();
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDockContainer.cpp
