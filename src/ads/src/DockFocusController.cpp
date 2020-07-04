//============================================================================
/// \file   DockFocusController.cpp
/// \author Uwe Kindler
/// \date   05.06.2020
/// \brief  Implementation of CDockFocusController class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockFocusController.h"

#include <algorithm>
#include <iostream>

#include <QPointer>
#include <QApplication>

#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockWidgetTab.h"
#include "DockContainerWidget.h"
#include "FloatingDockContainer.h"
#include "DockManager.h"
#include "DockAreaTitleBar.h"

#ifdef Q_OS_LINUX
#include "linux/FloatingWidgetTitleBar.h"
#endif

namespace ads
{
/**
 * Private data class of CDockFocusController class (pimpl)
 */
struct DockFocusControllerPrivate
{
	CDockFocusController *_this;
	QPointer<CDockWidget> FocusedDockWidget = nullptr;
	QPointer<CDockAreaWidget> FocusedArea = nullptr;
	CDockWidget* OldFocusedDockWidget = nullptr;
#ifdef Q_OS_LINUX
        QPointer<CFloatingDockContainer> FloatingWidget = nullptr;
#endif
	CDockManager* DockManager;

	/**
	 * Private data constructor
	 */
	DockFocusControllerPrivate(CDockFocusController *_public);

	/**
	 * This function updates the focus style of the given dock widget and
	 * the dock area that it belongs to
	 */
	void updateDockWidgetFocus(CDockWidget* DockWidget);
};
// struct DockFocusControllerPrivate


//===========================================================================
static void updateDockWidgetFocusStyle(CDockWidget* DockWidget, bool Focused)
{
	DockWidget->setProperty("focused", Focused);
	DockWidget->tabWidget()->setProperty("focused", Focused);
	DockWidget->tabWidget()->updateStyle();
	internal::repolishStyle(DockWidget);
}


//===========================================================================
static void updateDockAreaFocusStyle(CDockAreaWidget* DockArea, bool Focused)
{
	DockArea->setProperty("focused", Focused);
	internal::repolishStyle(DockArea);
	internal::repolishStyle(DockArea->titleBar());
}


//===========================================================================
#ifdef Q_OS_LINUX
static void updateFloatingWidgetFocusStyle(CFloatingDockContainer* FloatingWidget, bool Focused)
{
    auto TitleBar = qobject_cast<CFloatingWidgetTitleBar*>(FloatingWidget->titleBarWidget());
    if (!TitleBar)
    {
        return;
    }
    TitleBar->setProperty("focused", Focused);
    TitleBar->updateStyle();
}
#endif


//============================================================================
DockFocusControllerPrivate::DockFocusControllerPrivate(
    CDockFocusController *_public) :
	_this(_public)
{

}


//============================================================================
void DockFocusControllerPrivate::updateDockWidgetFocus(CDockWidget* DockWidget)
{
	if (!DockWidget->features().testFlag(CDockWidget::DockWidgetFocusable))
	{
		return;
	}

	CDockAreaWidget* NewFocusedDockArea = nullptr;
	if (FocusedDockWidget)
	{
		updateDockWidgetFocusStyle(FocusedDockWidget, false);
	}

	CDockWidget* old = FocusedDockWidget;
	FocusedDockWidget = DockWidget;
	updateDockWidgetFocusStyle(FocusedDockWidget, true);
	NewFocusedDockArea = FocusedDockWidget->dockAreaWidget();
	if (NewFocusedDockArea && (FocusedArea != NewFocusedDockArea))
	{
		if (FocusedArea)
		{
			QObject::disconnect(FocusedArea, SIGNAL(viewToggled(bool)), _this, SLOT(onFocusedDockAreaViewToggled(bool)));
			updateDockAreaFocusStyle(FocusedArea, false);
		}

		FocusedArea = NewFocusedDockArea;
		updateDockAreaFocusStyle(FocusedArea, true);
		QObject::connect(FocusedArea, SIGNAL(viewToggled(bool)), _this, SLOT(onFocusedDockAreaViewToggled(bool)));
	}


    auto NewFloatingWidget = FocusedDockWidget->dockContainer()->floatingWidget();
    if (NewFloatingWidget)
    {
    	NewFloatingWidget->setProperty("FocusedDockWidget", QVariant::fromValue(DockWidget));
    }


#ifdef Q_OS_LINUX
	// This code is required for styling the floating widget titlebar for linux
	// depending on the current focus state
    if (FloatingWidget == NewFloatingWidget)
    {
        return;
    }

    if (FloatingWidget)
    {
        updateFloatingWidgetFocusStyle(FloatingWidget, false);
    }
    FloatingWidget = NewFloatingWidget;

    if (FloatingWidget)
    {
        updateFloatingWidgetFocusStyle(FloatingWidget, true);
    }
#endif

    if (old == DockWidget)
    {
    	return;
    }

    if (DockWidget->isVisible())
    {
    	emit DockManager->focusedDockWidgetChanged(old, DockWidget);
    }
    else
    {
    	OldFocusedDockWidget = old;
    	QObject::connect(DockWidget, SIGNAL(visibilityChanged(bool)), _this, SLOT(onDockWidgetVisibilityChanged(bool)));
    }
}



//============================================================================
void CDockFocusController::onDockWidgetVisibilityChanged(bool Visible)
{
    auto Sender = sender();
    auto DockWidget = qobject_cast<ads::CDockWidget*>(Sender);
    /*qDebug() << "sender: " << Sender->metaObject()->className();
    qDebug() << "onDockWidgetVisibilityChanged " << Sender->objectName() << " Visible " << Visible;*/
    disconnect(Sender, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisibilityChanged(bool)));
    if (DockWidget && Visible)
	{
        //qDebug() << "emit d->DockManager->focusedDockWidgetChanged";
        emit d->DockManager->focusedDockWidgetChanged(d->OldFocusedDockWidget, DockWidget);
	}
}


//============================================================================
CDockFocusController::CDockFocusController(CDockManager* DockManager) :
	Super(DockManager),
	d(new DockFocusControllerPrivate(this))
{
	d->DockManager = DockManager;
	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)),
			this, SLOT(onApplicationFocusChanged(QWidget*, QWidget*)));
	connect(d->DockManager, SIGNAL(stateRestored()), SLOT(onStateRestored()));
}

//============================================================================
CDockFocusController::~CDockFocusController()
{
	delete d;
}


//===========================================================================
void CDockFocusController::onApplicationFocusChanged(QWidget* focusedOld, QWidget* focusedNow)
{
	if (d->DockManager->isRestoringState())
	{
		return;
	}

	Q_UNUSED(focusedOld)
	if (!focusedNow)
	{
		return;
	}

	CDockWidget* DockWidget = nullptr;
	auto DockWidgetTab = qobject_cast<CDockWidgetTab*>(focusedNow);
	if (DockWidgetTab)
	{
		DockWidget = DockWidgetTab->dockWidget();
	}

	if (!DockWidget)
	{
		DockWidget = qobject_cast<CDockWidget*>(focusedNow);
	}

	if (!DockWidget)
	{
		DockWidget = internal::findParent<CDockWidget*>(focusedNow);
	}

#ifdef Q_OS_LINUX
    if (!DockWidget)
	{
		return;
	}
#else
    if (!DockWidget || DockWidget->tabWidget()->isHidden())
	{
    	return;
	}
#endif

	d->updateDockWidgetFocus(DockWidget);
}


//===========================================================================
void CDockFocusController::setDockWidgetFocused(CDockWidget* focusedNow)
{
	d->updateDockWidgetFocus(focusedNow);
}


//===========================================================================
void CDockFocusController::onFocusedDockAreaViewToggled(bool Open)
{
	if (d->DockManager->isRestoringState())
	{
		return;
	}

	CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(sender());
	if (!DockArea || Open)
	{
		return;
	}
	auto Container = DockArea->dockContainer();
	auto OpenedDockAreas = Container->openedDockAreas();
	if (OpenedDockAreas.isEmpty())
	{
		return;
	}

	CDockManager::setWidgetFocus(OpenedDockAreas[0]->currentDockWidget()->tabWidget());
}


//===========================================================================
void CDockFocusController::notifyWidgetOrAreaRelocation(QWidget* DroppedWidget)
{
	if (d->DockManager->isRestoringState())
	{
		return;
	}

	CDockWidget* DockWidget = qobject_cast<CDockWidget*>(DroppedWidget);
	if (DockWidget)
	{
		CDockManager::setWidgetFocus(DockWidget->tabWidget());
		return;
	}

	CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(DroppedWidget);
	if (!DockArea)
	{
		return;
	}

	DockWidget = DockArea->currentDockWidget();
	CDockManager::setWidgetFocus(DockWidget->tabWidget());
}


//===========================================================================
void CDockFocusController::notifyFloatingWidgetDrop(CFloatingDockContainer* FloatingWidget)
{
	if (!FloatingWidget || d->DockManager->isRestoringState())
	{
		return;
	}

	auto vDockWidget = FloatingWidget->property("FocusedDockWidget");
	if (!vDockWidget.isValid())
	{
		return;
	}

	auto DockWidget = vDockWidget.value<CDockWidget*>();
	if (DockWidget)
	{
		DockWidget->dockAreaWidget()->setCurrentDockWidget(DockWidget);
		CDockManager::setWidgetFocus(DockWidget->tabWidget());
	}
}


//==========================================================================
void CDockFocusController::onStateRestored()
{
	if (d->FocusedDockWidget)
	{
		updateDockWidgetFocusStyle(d->FocusedDockWidget, false);
	}
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockFocusController.cpp
