//============================================================================
/// \file   FloatingDragPreview.cpp
/// \author Uwe Kindler
/// \date   26.11.2019
/// \brief  Implementation of CFloatingDragPreview
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDragPreview.h"
#include <iostream>

#include <QEvent>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>

#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockContainerWidget.h"
#include "DockOverlay.h"

namespace ads
{

/**
 * Private data class (pimpl)
 */
struct FloatingDragPreviewPrivate
{
	CFloatingDragPreview *_this;
	QWidget* Content;
	CDockAreaWidget* ContentSourceArea = nullptr;
	QPoint DragStartMousePosition;
	CDockManager* DockManager;
	CDockContainerWidget *DropContainer = nullptr;
	qreal WindowOpacity;
	bool Hidden = false;
	QPixmap ContentPreviewPixmap;
	bool Canceled = false;


	/**
	 * Private data constructor
	 */
	FloatingDragPreviewPrivate(CFloatingDragPreview *_public);
	void updateDropOverlays(const QPoint &GlobalPos);

	void setHidden(bool Value)
	{
		Hidden = Value;
		_this->update();
	}

	/**
	 * Cancel dragging and emit the draggingCanceled event
	 */
	void cancelDragging()
	{
		Canceled = true;
		emit _this->draggingCanceled();
		DockManager->containerOverlay()->hideOverlay();
		DockManager->dockAreaOverlay()->hideOverlay();
		_this->close();
	}

	/**
	 * Creates the real floating widget in case the mouse is released outside
	 * outside of any drop area
	 */
	void createFloatingWidget();
};
// struct LedArrayPanelPrivate


//============================================================================
void FloatingDragPreviewPrivate::updateDropOverlays(const QPoint &GlobalPos)
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
	auto DockDropArea = DockAreaOverlay->dropAreaUnderCursor();
	auto ContainerDropArea = ContainerOverlay->dropAreaUnderCursor();

	if (!TopContainer)
	{
		ContainerOverlay->hideOverlay();
		DockAreaOverlay->hideOverlay();
		if (CDockManager::testConfigFlag(CDockManager::DragPreviewIsDynamic))
		{
			setHidden(false);
		}
		return;
	}

	int VisibleDockAreas = TopContainer->visibleDockAreaCount();
	ContainerOverlay->setAllowedAreas(
	    VisibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
	auto DockArea = TopContainer->dockAreaAt(GlobalPos);
	if (DockArea && DockArea->isVisible() && VisibleDockAreas >= 0 && DockArea != ContentSourceArea)
	{
		DockAreaOverlay->enableDropPreview(true);
		DockAreaOverlay->setAllowedAreas(
		    (VisibleDockAreas == 1) ? NoDockWidgetArea : DockArea->allowedAreas());
		DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

		// A CenterDockWidgetArea for the dockAreaOverlay() indicates that
		// the mouse is in the title bar. If the ContainerArea is valid
		// then we ignore the dock area of the dockAreaOverlay() and disable
		// the drop preview
		if ((Area == CenterDockWidgetArea) && (ContainerDropArea != InvalidDockWidgetArea))
		{
			DockAreaOverlay->enableDropPreview(false);
			ContainerOverlay->enableDropPreview(true);
		}
		else
		{
			ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
		}
		ContainerOverlay->showOverlay(TopContainer);
	}
	else
	{
		DockAreaOverlay->hideOverlay();
		// If there is only one single visible dock area in a container, then
		// it does not make sense to show a dock overlay because the dock area
		// would be removed and inserted at the same position
		if (VisibleDockAreas == 1)
		{
			ContainerOverlay->hideOverlay();
		}
		else
		{
			ContainerOverlay->showOverlay(TopContainer);
		}


		if (DockArea == ContentSourceArea && InvalidDockWidgetArea == ContainerDropArea)
		{
			DropContainer = nullptr;
		}
	}

	if (CDockManager::testConfigFlag(CDockManager::DragPreviewIsDynamic))
	{
		setHidden(DockDropArea != InvalidDockWidgetArea || ContainerDropArea != InvalidDockWidgetArea);
	}
}


//============================================================================
FloatingDragPreviewPrivate::FloatingDragPreviewPrivate(CFloatingDragPreview *_public) :
	_this(_public)
{

}


//============================================================================
void FloatingDragPreviewPrivate::createFloatingWidget()
{
	CDockWidget* DockWidget = qobject_cast<CDockWidget*>(Content);
	CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(Content);

	CFloatingDockContainer* FloatingWidget = nullptr;

	if (DockWidget && DockWidget->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		FloatingWidget = new CFloatingDockContainer(DockWidget);
	}
	else if (DockArea && DockArea->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		FloatingWidget = new CFloatingDockContainer(DockArea);
	}

	if (FloatingWidget)
	{
		FloatingWidget->setGeometry(_this->geometry());
		FloatingWidget->show();
		if (!CDockManager::testConfigFlag(CDockManager::DragPreviewHasWindowFrame))
		{
			QApplication::processEvents();
			int FrameHeight = FloatingWidget->frameGeometry().height() - FloatingWidget->geometry().height();
			QRect FixedGeometry = _this->geometry();
			FixedGeometry.adjust(0, FrameHeight, 0, 0);
			FloatingWidget->setGeometry(FixedGeometry);
		}
	}
}


//============================================================================
CFloatingDragPreview::CFloatingDragPreview(QWidget* Content, QWidget* parent) :
	QWidget(parent),
	d(new FloatingDragPreviewPrivate(this))
{
	d->Content = Content;
	setAttribute(Qt::WA_DeleteOnClose);
	if (CDockManager::testConfigFlag(CDockManager::DragPreviewHasWindowFrame))
	{
		setWindowFlags(
			Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	}
	else
	{
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);
	}

#ifdef Q_OS_LINUX
    auto Flags = windowFlags();
    Flags |= Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint;
    setWindowFlags(Flags);
#endif

	setWindowOpacity(0.6);

	// Create a static image of the widget that should get undocked
	// This is like some kind preview image like it is uses in drag and drop
	// operations
	if (CDockManager::testConfigFlag(CDockManager::DragPreviewShowsContentPixmap))
	{
		d->ContentPreviewPixmap = QPixmap(Content->size());
		Content->render(&d->ContentPreviewPixmap);
	}

	connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
		SLOT(onApplicationStateChanged(Qt::ApplicationState)));

	// The only safe way to receive escape key presses is to install an event
	// filter for the application object
	qApp->installEventFilter(this);
}


//============================================================================
CFloatingDragPreview::CFloatingDragPreview(CDockWidget* Content)
	: CFloatingDragPreview((QWidget*)Content, Content->dockManager())
{
	d->DockManager = Content->dockManager();
	if (Content->dockAreaWidget()->openDockWidgetsCount() == 1)
	{
		d->ContentSourceArea = Content->dockAreaWidget();
	}
	setWindowTitle(Content->windowTitle());
}


//============================================================================
CFloatingDragPreview::CFloatingDragPreview(CDockAreaWidget* Content)
	: CFloatingDragPreview((QWidget*)Content, Content->dockManager())
{
	d->DockManager = Content->dockManager();
	d->ContentSourceArea = Content;
	setWindowTitle(Content->currentDockWidget()->windowTitle());
}


//============================================================================
CFloatingDragPreview::~CFloatingDragPreview()
{
	delete d;
}


//============================================================================
void CFloatingDragPreview::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition
	    - QPoint(BorderSize, 0);
	move(moveToPos);
	d->updateDropOverlays(QCursor::pos());
}


//============================================================================
void CFloatingDragPreview::startFloating(const QPoint &DragStartMousePos,
    const QSize &Size, eDragState DragState, QWidget *MouseEventHandler)
{
	Q_UNUSED(MouseEventHandler)
	Q_UNUSED(DragState)
	resize(Size);
	d->DragStartMousePosition = DragStartMousePos;
	moveFloating();
	show();

}


//============================================================================
void CFloatingDragPreview::finishDragging()
{
	ADS_PRINT("CFloatingDragPreview::finishDragging");
	auto DockDropArea = d->DockManager->dockAreaOverlay()->visibleDropAreaUnderCursor();
	auto ContainerDropArea = d->DockManager->containerOverlay()->visibleDropAreaUnderCursor();
	if (!d->DropContainer)
	{
		d->createFloatingWidget();
	}
	else if (DockDropArea != InvalidDockWidgetArea)
	{
		d->DropContainer->dropWidget(d->Content, DockDropArea, d->DropContainer->dockAreaAt(QCursor::pos()));
	}
	else if (ContainerDropArea != InvalidDockWidgetArea)
	{
		// If there is only one single dock area, and we drop into the center
		// then we tabify the dropped widget into the only visible dock area
		if (d->DropContainer->visibleDockAreaCount() <= 1 && CenterDockWidgetArea == ContainerDropArea)
		{
			d->DropContainer->dropWidget(d->Content, ContainerDropArea, d->DropContainer->dockAreaAt(QCursor::pos()));
		}
		else
		{
			d->DropContainer->dropWidget(d->Content, ContainerDropArea, nullptr);
		}
	}
	else
	{
		d->createFloatingWidget();
	}

	this->close();
	d->DockManager->containerOverlay()->hideOverlay();
	d->DockManager->dockAreaOverlay()->hideOverlay();
}


//============================================================================
void CFloatingDragPreview::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	if (d->Hidden)
	{
		return;
	}

	QPainter painter(this);
	if (CDockManager::testConfigFlag(CDockManager::DragPreviewShowsContentPixmap))
	{
		painter.drawPixmap(QPoint(0, 0), d->ContentPreviewPixmap);
	}

	// If we do not have a window frame then we paint a QRubberBand like
	// frameless window
	if (!CDockManager::testConfigFlag(CDockManager::DragPreviewHasWindowFrame))
	{
		QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
		QPen Pen = painter.pen();
		Pen.setColor(Color.darker(120));
		Pen.setStyle(Qt::SolidLine);
		Pen.setWidth(1);
		Pen.setCosmetic(true);
		painter.setPen(Pen);
		Color = Color.lighter(130);
		Color.setAlpha(64);
		painter.setBrush(Color);
		painter.drawRect(rect().adjusted(0, 0, -1, -1));
	}
}

//============================================================================
void CFloatingDragPreview::onApplicationStateChanged(Qt::ApplicationState state)
{
	if (state != Qt::ApplicationActive)
	{
		disconnect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
			this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
		d->cancelDragging();
	}
}


//============================================================================
bool CFloatingDragPreview::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
    if (!d->Canceled && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_Escape)
        {
            watched->removeEventFilter(this);
            d->cancelDragging();
        }
    }

    return false;
}



} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDragPreview.cpp
