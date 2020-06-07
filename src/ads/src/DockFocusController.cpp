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
	CDockAreaWidget* NewFocusedDockArea = nullptr;
	if (FocusedDockWidget)
	{
		updateDockWidgetFocusStyle(FocusedDockWidget, false);
	}

	CDockWidget* old = FocusedDockWidget;
	if (DockWidget != FocusedDockWidget)
	{
		std::cout << "!!!!!!!!!!!! focusedDockWidgetChanged " << (FocusedDockWidget ? FocusedDockWidget->objectName().toStdString() : "-")
			<< " -> " << (DockWidget ? DockWidget->objectName().toStdString() : "-") << std::endl;
	}
	FocusedDockWidget = DockWidget;
	updateDockWidgetFocusStyle(FocusedDockWidget, true);
	NewFocusedDockArea = FocusedDockWidget->dockAreaWidget();
	if (NewFocusedDockArea && (FocusedArea != NewFocusedDockArea))
	{
		if (FocusedArea)
		{
			std::cout << "FocusedArea" << std::endl;
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
    	std::cout << "NewFloatingWidget->setProperty(FocusedDockWidget)" << std::endl;
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

    if (old != DockWidget)
    {
    	emit DockManager->focusedDockWidgetChanged(old, DockWidget);
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
	std::cout << "CDockManager::onFocusChanged" << std::endl;
    std::cout << "focusedNow " << focusedNow << std::endl;
	Q_UNUSED(focusedOld)
	if (!focusedNow)
	{
		return;
	}

	CDockWidget* DockWidget = nullptr;
	auto DockWidgetTab = qobject_cast<CDockWidgetTab*>(focusedNow);
	std::cout << "FocuseNow " << focusedNow->metaObject()->className() << std::endl;
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
    	std::cout << "!DockWidget || !DockWidget->tabWidget()->isVisible() " << (DockWidget ? DockWidget->objectName().toStdString() : "0") << std::endl;
		std::cout << "DockWidget->tabWidget()->isHidden() " << (DockWidget ? DockWidget->tabWidget()->isHidden() : false) << std::endl;
    	return;
	}
#endif

	std::cout << "CDockManager::onFocusChanged " << DockWidget->tabWidget()->text().toStdString() << std::endl;
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
	std::cout << "\n\nCDockManager::notifyWidgetDrop" << std::endl;
	CDockWidget* DockWidget = qobject_cast<CDockWidget*>(DroppedWidget);
	if (DockWidget)
	{
		std::cout << "CDockManager::setWidgetFocus " << DockWidget->objectName().toStdString() << std::endl;
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
	std::cout << "\n\n" << std::endl;
}


//===========================================================================
void CDockFocusController::notifyFloatingWidgetDrop(CFloatingDockContainer* FloatingWidget)
{
	std::cout << "\n\nCDockManager::notifyFloatingWidgetDrop" << std::endl;
	if (!FloatingWidget || d->DockManager->isRestoringState())
	{
		return;
	}

	auto vDockWidget = FloatingWidget->property("FocusedDockWidget");
	if (!vDockWidget.isValid())
	{
		return;
	}
	std::cout << "vDockWidget.isValid()" << std::endl;
	auto DockWidget = vDockWidget.value<CDockWidget*>();
	if (DockWidget)
	{
		std::cout << "Dropped focus dock widget " << DockWidget->objectName().toStdString() << std::endl;
		DockWidget->dockAreaWidget()->setCurrentDockWidget(DockWidget);
		CDockManager::setWidgetFocus(DockWidget->tabWidget());
	}
	std::cout << "\n\n" << std::endl;
}


//==========================================================================
void CDockFocusController::onStateRestored()
{
	std::cout << "CDockFocusController::onStateRestored()" << std::endl;
	if (d->FocusedDockWidget)
	{
		updateDockWidgetFocusStyle(d->FocusedDockWidget, false);
	}
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockFocusController.cpp
