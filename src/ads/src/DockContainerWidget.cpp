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
/// \file   DockContainerWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockContainerWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

#include <QEvent>
#include <QList>
#include <QGridLayout>
#include <QPointer>
#include <QVariant>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QAbstractButton>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"
#include "DockingStateReader.h"
#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "ads_globals.h"
#include "DockSplitter.h"

#include <functional>
#include <iostream>

#if QT_VERSION < 0x050900

inline char toHexLower(uint value)
{
    return "0123456789abcdef"[value & 0xF];
}

QByteArray qByteArrayToHex(const QByteArray& src, char separator)
{
    if(src.size() == 0)
        return QByteArray();

    const int length = separator ? (src.size() * 3 - 1) : (src.size() * 2);
    QByteArray hex(length, Qt::Uninitialized);
    char *hexData = hex.data();
    const uchar *data = reinterpret_cast<const uchar *>(src.data());
    for (int i = 0, o = 0; i < src.size(); ++i) {
        hexData[o++] = toHexLower(data[i] >> 4);
        hexData[o++] = toHexLower(data[i] & 0xf);

        if ((separator) && (o < length))
            hexData[o++] = separator;
    }
    return hex;
}
#endif

namespace ads
{
static unsigned int zOrderCounter = 0;

enum eDropMode
{
	DropModeIntoArea,///< drop widget into a dock area
	DropModeIntoContainer,///< drop into container
	DropModeInvalid///< invalid mode - do not drop
};

/**
 * Converts dock area ID to an index for array access
 */
static int areaIdToIndex(DockWidgetArea area)
{
	switch (area)
	{
	case LeftDockWidgetArea: return 0;
	case RightDockWidgetArea: return 1;
	case TopDockWidgetArea: return 2;
	case BottomDockWidgetArea: return 3;
	case CenterDockWidgetArea: return 4;
	default:
		return 4;
	}
}

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void insertWidgetIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
{
	if (Append)
	{
		Splitter->addWidget(widget);
	}
	else
	{
		Splitter->insertWidget(0, widget);
	}
}

/**
 * Private data class of CDockContainerWidget class (pimpl)
 */
class DockContainerWidgetPrivate
{
public:
	CDockContainerWidget* _this;
	QPointer<CDockManager> DockManager;
	unsigned int zOrderIndex = 0;
	QList<CDockAreaWidget*> DockAreas;
	QGridLayout* Layout = nullptr;
	QSplitter* RootSplitter = nullptr;
	bool isFloating = false;
	CDockAreaWidget* LastAddedAreaCache[5];
	int VisibleDockAreaCount = -1;
	CDockAreaWidget* TopLevelDockArea = nullptr;

	/**
	 * Private data constructor
	 */
	DockContainerWidgetPrivate(CDockContainerWidget* _public);

	/**
	 * Adds dock widget to container and returns the dock area that contains
	 * the inserted dock widget
	 */
	CDockAreaWidget* dockWidgetIntoContainer(DockWidgetArea area, CDockWidget* Dockwidget);

	/**
	 * Adds dock widget to a existing DockWidgetArea
	 */
	CDockAreaWidget* dockWidgetIntoDockArea(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* TargetDockArea);

	/**
	 * Add dock area to this container
	 */
	void addDockArea(CDockAreaWidget* NewDockWidget, DockWidgetArea area = CenterDockWidgetArea);

	/**
	 * Drop floating widget into container
	 */
	void dropIntoContainer(CFloatingDockContainer* FloatingWidget, DockWidgetArea area);

	/**
	 * Drop floating widget into dock area
	 */
	void dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area);

	/**
	 * Moves the dock widget or dock area given in Widget parameter to a
	 * new dock widget area
	 */
	void moveToNewSection(QWidget* Widget, CDockAreaWidget* TargetArea, DockWidgetArea area);

	/**
	 * Moves the dock widget or dock area given in Widget parameter to a
	 * a dock area in container
	 */
	void moveToContainer(QWidget* Widgett, DockWidgetArea area);

	/**
	 * Creates a new tab for a widget dropped into the center of a section
	 */
	void dropIntoCenterOfSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea);

	/**
	 * Creates a new tab for a widget dropped into the center of a section
	 */
	void moveIntoCenterOfSection(QWidget* Widget, CDockAreaWidget* TargetArea);


	/**
	 * Adds new dock areas to the internal dock area list
	 */
	void addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas);

	/**
	 * Wrapper function for DockAreas append, that ensures that dock area signals
	 * are properly connected to dock container slots
	 */
	void appendDockAreas(const QList<CDockAreaWidget*> NewDockAreas);

	/**
	 * Save state of child nodes
	 */
	void saveChildNodesState(QXmlStreamWriter& Stream, QWidget* Widget);

	/**
	 * Restore state of child nodes.
	 * \param[in] Stream The data stream that contains the serialized state
	 * \param[out] CreatedWidget The widget created from parsed data or 0 if
	 * the parsed widget was an empty splitter
	 * \param[in] Testing If Testing is true, only the stream data is
	 * parsed without modifiying anything.
	 */
	bool restoreChildNodes(CDockingStateReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Restores a splitter.
	 * \see restoreChildNodes() for details
	 */
	bool restoreSplitter(CDockingStateReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Restores a dock area.
	 * \see restoreChildNodes() for details
	 */
	bool restoreDockArea(CDockingStateReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Helper function for recursive dumping of layout
	 */
	void dumpRecursive(int level, QWidget* widget);

	/**
	 * Calculate the drop mode from the given target position
	 */
	eDropMode getDropMode(const QPoint& TargetPos);

	/**
	 * Initializes the visible dock area count variable if it is not initialized
	 * yet
	 */
	void initVisibleDockAreaCount()
	{
		if (VisibleDockAreaCount > -1)
		{
			return;
		}

		VisibleDockAreaCount = 0;
		for (auto DockArea : DockAreas)
		{
			VisibleDockAreaCount += DockArea->isHidden() ? 0 : 1;
		}
	}

	/**
	 * Access function for the visible dock area counter
	 */
	int& visibleDockAreaCount()
	{
		// Lazy initialisation - we initialize the VisibleDockAreaCount variable
		// on first use
		initVisibleDockAreaCount();
		return VisibleDockAreaCount;
	}

	/**
	 * The visible dock area count changes, if dock areas are remove, added or
	 * when its view is toggled
	 */
	void onVisibleDockAreaCountChanged();

	void emitDockAreasRemoved()
	{
		onVisibleDockAreaCountChanged();
		emit _this->dockAreasRemoved();
	}

	void emitDockAreasAdded()
	{
		onVisibleDockAreaCountChanged();
		emit _this->dockAreasAdded();
	}

	/**
	 * Helper function for creation of new splitter
	 */
	CDockSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent = nullptr)
	{
		CDockSplitter* s = new CDockSplitter(orientation, parent);
		s->setOpaqueResize(CDockManager::configFlags().testFlag(CDockManager::OpaqueSplitterResize));
		s->setChildrenCollapsible(false);
		return s;
	}


// private slots: ------------------------------------------------------------
	void onDockAreaViewToggled(bool Visible)
	{
		CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(_this->sender());
		VisibleDockAreaCount += Visible ? 1 : -1;
		onVisibleDockAreaCountChanged();
		emit _this->dockAreaViewToggled(DockArea, Visible);
	}
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{
	std::fill(std::begin(LastAddedAreaCache),std::end(LastAddedAreaCache), nullptr);
}


//============================================================================
eDropMode DockContainerWidgetPrivate::getDropMode(const QPoint& TargetPos)
{
	CDockAreaWidget* DockArea = _this->dockAreaAt(TargetPos);
	auto dropArea = InvalidDockWidgetArea;
	auto ContainerDropArea = DockManager->containerOverlay()->dropAreaUnderCursor();

	if (DockArea)
	{
		auto dropOverlay = DockManager->dockAreaOverlay();
		dropOverlay->setAllowedAreas(DockArea->allowedAreas());
		dropArea = dropOverlay->showOverlay(DockArea);
		if (ContainerDropArea != InvalidDockWidgetArea &&
			ContainerDropArea != dropArea)
		{
			dropArea = InvalidDockWidgetArea;
		}

		if (dropArea != InvalidDockWidgetArea)
		{
            ADS_PRINT("Dock Area Drop Content: " << dropArea);
            return DropModeIntoArea;
		}
	}

	// mouse is over container
	if (InvalidDockWidgetArea == dropArea)
	{
		dropArea = ContainerDropArea;
        ADS_PRINT("Container Drop Content: " << dropArea);
		if (dropArea != InvalidDockWidgetArea)
		{
			return DropModeIntoContainer;
		}
	}

	return DropModeInvalid;
}


//============================================================================
void DockContainerWidgetPrivate::onVisibleDockAreaCountChanged()
{
	auto TopLevelDockArea = _this->topLevelDockArea();

	if (TopLevelDockArea)
	{
		this->TopLevelDockArea = TopLevelDockArea;
		TopLevelDockArea->titleBarButton(TitleBarButtonUndock)->setVisible(false || !_this->isFloating());
		TopLevelDockArea->titleBarButton(TitleBarButtonClose)->setVisible(false || !_this->isFloating());
	}
	else if (this->TopLevelDockArea)
	{
		this->TopLevelDockArea->titleBarButton(TitleBarButtonUndock)->setVisible(true);
		this->TopLevelDockArea->titleBarButton(TitleBarButtonClose)->setVisible(true);
		this->TopLevelDockArea = nullptr;
	}
}


//============================================================================
void DockContainerWidgetPrivate::dropIntoContainer(CFloatingDockContainer* FloatingWidget,
	DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	CDockContainerWidget* FloatingDockContainer = FloatingWidget->dockContainer();
	auto NewDockAreas = FloatingDockContainer->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	QSplitter* Splitter = RootSplitter;

	if (DockAreas.count() <= 1)
	{
		Splitter->setOrientation(InsertParam.orientation());
	}
	else if (Splitter->orientation() != InsertParam.orientation())
	{
		QSplitter* NewSplitter = newSplitter(InsertParam.orientation());
		QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
		NewSplitter->addWidget(Splitter);
		Splitter = NewSplitter;
		delete li;
	}

	// Now we can insert the floating widget content into this container
	auto FloatingSplitter = FloatingDockContainer->rootSplitter();
	if (FloatingSplitter->count() == 1)
	{
		insertWidgetIntoSplitter(Splitter, FloatingSplitter->widget(0), InsertParam.append());
	}
	else if (FloatingSplitter->orientation() == InsertParam.orientation())
	{
        int InsertIndex = InsertParam.append() ? Splitter->count() : 0;
		while (FloatingSplitter->count())
		{
            Splitter->insertWidget(InsertIndex++, FloatingSplitter->widget(0));
		}
	}
	else
	{
		insertWidgetIntoSplitter(Splitter, FloatingSplitter, InsertParam.append());
	}

	RootSplitter = Splitter;
	addDockAreasToList(NewDockAreas);

	// If we dropped the floating widget into the main dock container that does
	// not contain any dock widgets, then splitter is invisible and we need to
	// show it to display the docked widgets
	if (!Splitter->isVisible())
	{
		Splitter->show();
	}
	_this->dumpLayout();
}


//============================================================================
void DockContainerWidgetPrivate::dropIntoCenterOfSection(
	CFloatingDockContainer* FloatingWidget, CDockAreaWidget* TargetArea)
{
	CDockContainerWidget* FloatingContainer = FloatingWidget->dockContainer();
	auto NewDockWidgets = FloatingContainer->dockWidgets();
	auto TopLevelDockArea = FloatingContainer->topLevelDockArea();
	int NewCurrentIndex = -1;

	// If the floating widget contains only one single dock are, then the
	// current dock widget of the dock area will also be the future current
	// dock widget in the drop area.
	if (TopLevelDockArea)
	{
		NewCurrentIndex = TopLevelDockArea->currentIndex();
	}

	for (int i = 0; i < NewDockWidgets.count(); ++i)
	{
		CDockWidget* DockWidget = NewDockWidgets[i];
		TargetArea->insertDockWidget(i, DockWidget, false);
		// If the floating widget contains multiple visible dock areas, then we
		// simply pick the first visible open dock widget and make it
		// the current one.
		if (NewCurrentIndex < 0 && !DockWidget->isClosed())
		{
			NewCurrentIndex = i;
		}
	}
	TargetArea->setCurrentIndex(NewCurrentIndex);
	TargetArea->updateTitleBarVisibility();
	return;
}


//============================================================================
void DockContainerWidgetPrivate::dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area)
{
	// Dropping into center means all dock widgets in the dropped floating
	// widget will become tabs of the drop area
	if (CenterDockWidgetArea == area)
	{
		dropIntoCenterOfSection(FloatingWidget, TargetArea);
		return;
	}

	auto InsertParam = internal::dockAreaInsertParameters(area);
	auto NewDockAreas = FloatingWidget->dockContainer()->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetArea);

	if (!TargetAreaSplitter)
	{
		QSplitter* Splitter = newSplitter(InsertParam.orientation());
		Layout->replaceWidget(TargetArea, Splitter);
		Splitter->addWidget(TargetArea);
		TargetAreaSplitter = Splitter;
	}
	int AreaIndex = TargetAreaSplitter->indexOf(TargetArea);
	auto Widget = FloatingWidget->dockContainer()->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	auto FloatingSplitter = qobject_cast<QSplitter*>(Widget);

	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		auto Sizes = TargetAreaSplitter->sizes();
		int TargetAreaSize = (InsertParam.orientation() == Qt::Horizontal) ? TargetArea->width() : TargetArea->height();
		bool AdjustSplitterSizes = true;
		if ((FloatingSplitter->orientation() != InsertParam.orientation()) && FloatingSplitter->count() > 1)
		{
			TargetAreaSplitter->insertWidget(AreaIndex + InsertParam.insertOffset(), Widget);
		}
		else
		{
			AdjustSplitterSizes = (FloatingSplitter->count() == 1);
			int InsertIndex = AreaIndex + InsertParam.insertOffset();
			while (FloatingSplitter->count())
			{
				TargetAreaSplitter->insertWidget(InsertIndex++, FloatingSplitter->widget(0));
			}
		}

		if (AdjustSplitterSizes)
		{
			int Size = (TargetAreaSize - TargetAreaSplitter->handleWidth()) / 2;
			Sizes[AreaIndex] = Size;
			Sizes.insert(AreaIndex, Size);
			TargetAreaSplitter->setSizes(Sizes);
		}
	}
	else
	{
		QList<int> NewSplitterSizes;
		QSplitter* NewSplitter = newSplitter(InsertParam.orientation());
		int TargetAreaSize = (InsertParam.orientation() == Qt::Horizontal) ? TargetArea->width() : TargetArea->height();
		bool AdjustSplitterSizes = true;
		if ((FloatingSplitter->orientation() != InsertParam.orientation()) && FloatingSplitter->count() > 1)
		{
			NewSplitter->addWidget(Widget);
		}
		else
		{
			AdjustSplitterSizes = (FloatingSplitter->count() == 1);
			while (FloatingSplitter->count())
			{
				NewSplitter->addWidget(FloatingSplitter->widget(0));
			}
		}

		// Save the sizes before insertion and restore it later to prevent
		// shrinking of existing area
		auto Sizes = TargetAreaSplitter->sizes();
		insertWidgetIntoSplitter(NewSplitter, TargetArea, !InsertParam.append());
		if (AdjustSplitterSizes)
		{
			int Size = TargetAreaSize / 2;
			NewSplitter->setSizes({Size, Size});
		}
		TargetAreaSplitter->insertWidget(AreaIndex, NewSplitter);
		TargetAreaSplitter->setSizes(Sizes);
	}

	addDockAreasToList(NewDockAreas);
	_this->dumpLayout();
}


//============================================================================
void DockContainerWidgetPrivate::moveIntoCenterOfSection(QWidget* Widget, CDockAreaWidget* TargetArea)
{
	auto DroppedDockWidget = qobject_cast<CDockWidget*>(Widget);
	auto DroppedArea = qobject_cast<CDockAreaWidget*>(Widget);

	if (DroppedDockWidget)
	{
		CDockAreaWidget* OldDockArea = DroppedDockWidget->dockAreaWidget();
		if (OldDockArea == TargetArea)
		{
			return;
		}

		if (OldDockArea)
		{
			OldDockArea->removeDockWidget(DroppedDockWidget);
		}
		TargetArea->insertDockWidget(0, DroppedDockWidget, true);
	}
	else
	{
		QList<CDockWidget*> NewDockWidgets = DroppedArea->dockWidgets();
		int NewCurrentIndex = DroppedArea->currentIndex();
		for (int i = 0; i < NewDockWidgets.count(); ++i)
		{
			CDockWidget* DockWidget = NewDockWidgets[i];
			TargetArea->insertDockWidget(i, DockWidget, false);
		}
		TargetArea->setCurrentIndex(NewCurrentIndex);
		DroppedArea->dockContainer()->removeDockArea(DroppedArea);
		DroppedArea->deleteLater();
	}

	TargetArea->updateTitleBarVisibility();
	return;
}


//============================================================================
void DockContainerWidgetPrivate::moveToNewSection(QWidget* Widget, CDockAreaWidget* TargetArea, DockWidgetArea area)
{
	// Dropping into center means all dock widgets in the dropped floating
	// widget will become tabs of the drop area
	if (CenterDockWidgetArea == area)
	{
		moveIntoCenterOfSection(Widget, TargetArea);
		return;
	}


	CDockWidget* DroppedDockWidget = qobject_cast<CDockWidget*>(Widget);
	CDockAreaWidget* DroppedDockArea = qobject_cast<CDockAreaWidget*>(Widget);
	CDockAreaWidget* NewDockArea;
	if (DroppedDockWidget)
	{
		NewDockArea = new CDockAreaWidget(DockManager, _this);
		CDockAreaWidget* OldDockArea = DroppedDockWidget->dockAreaWidget();
		if (OldDockArea)
		{
			OldDockArea->removeDockWidget(DroppedDockWidget);
		}
		NewDockArea->addDockWidget(DroppedDockWidget);
	}
	else
	{
		DroppedDockArea->dockContainer()->removeDockArea(DroppedDockArea);
		NewDockArea = DroppedDockArea;
	}

	auto InsertParam = internal::dockAreaInsertParameters(area);
	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetArea);
	int AreaIndex = TargetAreaSplitter->indexOf(TargetArea);
	auto Sizes = TargetAreaSplitter->sizes();
	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		int TargetAreaSize = (InsertParam.orientation() == Qt::Horizontal) ? TargetArea->width() : TargetArea->height();
		TargetAreaSplitter->insertWidget(AreaIndex + InsertParam.insertOffset(), NewDockArea);
		int Size = (TargetAreaSize - TargetAreaSplitter->handleWidth()) / 2;
		Sizes[AreaIndex] = Size;
		Sizes.insert(AreaIndex, Size);
	}
	else
	{
		auto Sizes = TargetAreaSplitter->sizes();
		int TargetAreaSize = (InsertParam.orientation() == Qt::Horizontal) ? TargetArea->width() : TargetArea->height();
		QSplitter* NewSplitter = newSplitter(InsertParam.orientation());
		NewSplitter->addWidget(TargetArea);
		insertWidgetIntoSplitter(NewSplitter, NewDockArea, InsertParam.append());
		int Size = TargetAreaSize / 2;
		NewSplitter->setSizes({Size, Size});
		TargetAreaSplitter->insertWidget(AreaIndex, NewSplitter);
	}
	TargetAreaSplitter->setSizes(Sizes);

	addDockAreasToList({NewDockArea});
}


//============================================================================
void DockContainerWidgetPrivate::moveToContainer(QWidget* Widget, DockWidgetArea area)
{
	CDockWidget* DroppedDockWidget = qobject_cast<CDockWidget*>(Widget);
	CDockAreaWidget* DroppedDockArea = qobject_cast<CDockAreaWidget*>(Widget);
	CDockAreaWidget* NewDockArea;

	if (DroppedDockWidget)
	{
		NewDockArea = new CDockAreaWidget(DockManager, _this);
		CDockAreaWidget* OldDockArea = DroppedDockWidget->dockAreaWidget();
		if (OldDockArea)
		{
			OldDockArea->removeDockWidget(DroppedDockWidget);
		}
		NewDockArea->addDockWidget(DroppedDockWidget);
	}
	else
	{
		// We check, if we insert the dropped widget into the same place that
		// it already has and do nothing, if it is the same place. It would
		// also work without this check, but it looks nicer with the check
		// because there will be no layout updates
		auto Splitter = internal::findParent<CDockSplitter*>(DroppedDockArea);
		auto InsertParam = internal::dockAreaInsertParameters(area);
		if (Splitter == RootSplitter && InsertParam.orientation() == Splitter->orientation())
		{
			if (InsertParam.append() && Splitter->lastWidget() == DroppedDockArea)
			{
				return;
			}
			else if (!InsertParam.append() && Splitter->firstWidget() == DroppedDockArea)
			{
				return;
			}
		}
		DroppedDockArea->dockContainer()->removeDockArea(DroppedDockArea);
		NewDockArea = DroppedDockArea;
	}

	addDockArea(NewDockArea, area);
	LastAddedAreaCache[areaIdToIndex(area)] = NewDockArea;
}


//============================================================================
void DockContainerWidgetPrivate::addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas)
{
	int CountBefore = DockAreas.count();
	int NewAreaCount = NewDockAreas.count();
	appendDockAreas(NewDockAreas);
	// If the user dropped a floating widget that contains only one single
	// visible dock area, then its title bar button TitleBarButtonUndock is
	// likely hidden. We need to ensure, that it is visible
	for (auto DockArea : NewDockAreas)
	{
		DockArea->titleBarButton(TitleBarButtonUndock)->setVisible(true);
		DockArea->titleBarButton(TitleBarButtonClose)->setVisible(true);
	}

	// We need to ensure, that the dock area title bar is visible. The title bar
	// is invisible, if the dock are is a single dock area in a floating widget.
	if (1 == CountBefore)
	{
		DockAreas.at(0)->updateTitleBarVisibility();
	}

	if (1 == NewAreaCount)
	{
		DockAreas.last()->updateTitleBarVisibility();
	}

	emitDockAreasAdded();
}


//============================================================================
void DockContainerWidgetPrivate::appendDockAreas(const QList<CDockAreaWidget*> NewDockAreas)
{
	DockAreas.append(NewDockAreas);
	for (auto DockArea : NewDockAreas)
	{
		QObject::connect(DockArea,
			&CDockAreaWidget::viewToggled,
			_this,
			std::bind(&DockContainerWidgetPrivate::onDockAreaViewToggled, this, std::placeholders::_1));
	}
}


//============================================================================
void DockContainerWidgetPrivate::saveChildNodesState(QXmlStreamWriter& s, QWidget* Widget)
{
	QSplitter* Splitter = qobject_cast<QSplitter*>(Widget);
	if (Splitter)
	{
		s.writeStartElement("Splitter");
		s.writeAttribute("Orientation", (Splitter->orientation() == Qt::Horizontal) ? "|" : "-");
		s.writeAttribute("Count", QString::number(Splitter->count()));
        ADS_PRINT("NodeSplitter orient: " << Splitter->orientation()
            << " WidgetCont: " << Splitter->count());
			for (int i = 0; i < Splitter->count(); ++i)
			{
				saveChildNodesState(s, Splitter->widget(i));
			}

			s.writeStartElement("Sizes");
			for (auto Size : Splitter->sizes())
			{
				s.writeCharacters(QString::number(Size) + " ");
			}
			s.writeEndElement();
		s.writeEndElement();
	}
	else
	{
		CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(Widget);
		if (DockArea)
		{
			DockArea->saveState(s);
		}
	}
}


//============================================================================
bool DockContainerWidgetPrivate::restoreSplitter(CDockingStateReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Ok;
	QString OrientationStr = s.attributes().value("Orientation").toString();

	// Check if the orientation string is right
	if (!OrientationStr.startsWith("|") && !OrientationStr.startsWith("-"))
	{
		return false;
	}

	// The "|" shall indicate a vertical splitter handle which in turn means
	// a Horizontal orientation of the splitter layout.
	bool HorizontalSplitter = OrientationStr.startsWith("|");
	// In version 0 we had a small bug. The "|" indicated a vertical orientation,
	// but this is wrong, because only the splitter handle is vertical, the
	// layout of the splitter is a horizontal layout. We fix this here
	if (s.fileVersion() == 0)
	{
		HorizontalSplitter = !HorizontalSplitter;
	}

	int Orientation = HorizontalSplitter ? Qt::Horizontal : Qt::Vertical;
	int WidgetCount = s.attributes().value("Count").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}
    ADS_PRINT("Restore NodeSplitter Orientation: " <<  Orientation <<
            " WidgetCount: " << WidgetCount);
	QSplitter* Splitter = nullptr;
	if (!Testing)
	{
		Splitter = newSplitter(static_cast<Qt::Orientation>(Orientation));
	}
	bool Visible = false;
	QList<int> Sizes;
	while (s.readNextStartElement())
	{
		QWidget* ChildNode = nullptr;
		bool Result = true;
		if (s.name() == "Splitter")
		{
			Result = restoreSplitter(s, ChildNode, Testing);
		}
		else if (s.name() == "Area")
		{
			Result = restoreDockArea(s, ChildNode, Testing);
		}
		else if (s.name() == "Sizes")
		{
			QString sSizes = s.readElementText().trimmed();
            ADS_PRINT("Sizes: " << sSizes);
			QTextStream TextStream(&sSizes);
			while (!TextStream.atEnd())
			{
				int value;
				TextStream >> value;
				Sizes.append(value);
			}
		}
		else
		{
			s.skipCurrentElement();
		}

		if (!Result)
		{
			return false;
		}

		if (Testing || !ChildNode)
		{
			continue;
		}

        ADS_PRINT("ChildNode isVisible " << ChildNode->isVisible()
            << " isVisibleTo " << ChildNode->isVisibleTo(Splitter));
		Splitter->addWidget(ChildNode);
		Visible |= ChildNode->isVisibleTo(Splitter);
	}

	if (Sizes.count() != WidgetCount)
	{
		return false;
	}

	if (!Testing)
	{
		if (!Splitter->count())
		{
			delete Splitter;
			Splitter = nullptr;
		}
		else
		{
			Splitter->setSizes(Sizes);
			Splitter->setVisible(Visible);
		}
		CreatedWidget = Splitter;
	}
	else
	{
		CreatedWidget = nullptr;
	}

	return true;
}


//============================================================================
bool DockContainerWidgetPrivate::restoreDockArea(CDockingStateReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Ok;
#ifdef ADS_DEBUG_PRINT
	int Tabs = s.attributes().value("Tabs").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}
#endif

	QString CurrentDockWidget = s.attributes().value("Current").toString();
    ADS_PRINT("Restore NodeDockArea Tabs: " << Tabs << " Current: "
            << CurrentDockWidget);

	CDockAreaWidget* DockArea = nullptr;
	if (!Testing)
	{
		DockArea = new CDockAreaWidget(DockManager, _this);
	}

	while (s.readNextStartElement())
	{
		if (s.name() != "Widget")
		{
			continue;
		}

		auto ObjectName = s.attributes().value("Name");
		if (ObjectName.isEmpty())
		{
			return false;
		}

		bool Closed = s.attributes().value("Closed").toInt(&Ok);
		if (!Ok)
		{
			return false;
		}

		s.skipCurrentElement();
		CDockWidget* DockWidget = DockManager->findDockWidget(ObjectName.toString());
		if (!DockWidget || Testing)
		{
			continue;
		}

        ADS_PRINT("Dock Widget found - parent " << DockWidget->parent());
		// We hide the DockArea here to prevent the short display (the flashing)
		// of the dock areas during application startup
		DockArea->hide();
		DockArea->addDockWidget(DockWidget);
		DockWidget->setToggleViewActionChecked(!Closed);
		DockWidget->setClosedState(Closed);
		DockWidget->setProperty(internal::ClosedProperty, Closed);
		DockWidget->setProperty(internal::DirtyProperty, false);
	}

	if (Testing)
	{
		return true;
	}

	if (!DockArea->dockWidgetsCount())
	{
		delete DockArea;
		DockArea = nullptr;
	}
	else
	{
		DockArea->setProperty("currentDockWidget", CurrentDockWidget);
		appendDockAreas({DockArea});
	}

	CreatedWidget = DockArea;
	return true;
}


//============================================================================
bool DockContainerWidgetPrivate::restoreChildNodes(CDockingStateReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Result = true;
	while (s.readNextStartElement())
	{
		if (s.name() == "Splitter")
		{
			Result = restoreSplitter(s, CreatedWidget, Testing);
            ADS_PRINT("Splitter");
		}
		else if (s.name() == "Area")
		{
			Result = restoreDockArea(s, CreatedWidget, Testing);
            ADS_PRINT("DockAreaWidget");
		}
		else
		{
			s.skipCurrentElement();
            ADS_PRINT("Unknown element");
		}
	}

	return Result;
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoContainer(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	addDockArea(NewDockArea, area);
	NewDockArea->updateTitleBarVisibility();
	LastAddedAreaCache[areaIdToIndex(area)] = NewDockArea;
	return NewDockArea;
}


//============================================================================
void DockContainerWidgetPrivate::addDockArea(CDockAreaWidget* NewDockArea, DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	// As long as we have only one dock area in the splitter we can adjust
	// its orientation
	if (DockAreas.count() <= 1)
	{
		RootSplitter->setOrientation(InsertParam.orientation());
	}

	QSplitter* Splitter = RootSplitter;
	if (Splitter->orientation() == InsertParam.orientation())
	{
		insertWidgetIntoSplitter(Splitter, NewDockArea, InsertParam.append());
		if (Splitter->isHidden())
		{
			Splitter->show();
		}
	}
	else
	{
		QSplitter* NewSplitter = newSplitter(InsertParam.orientation());
		if (InsertParam.append())
		{
			QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
			NewSplitter->addWidget(Splitter);
			NewSplitter->addWidget(NewDockArea);
			delete li;
		}
		else
		{
			NewSplitter->addWidget(NewDockArea);
			QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
			NewSplitter->addWidget(Splitter);
			delete li;
		}
		RootSplitter = NewSplitter;
	}

	addDockAreasToList({NewDockArea});
}


//============================================================================
void DockContainerWidgetPrivate::dumpRecursive(int level, QWidget* widget)
{
#if defined(QT_DEBUG)
	QSplitter* Splitter = qobject_cast<QSplitter*>(widget);
	QByteArray buf;
    buf.fill(' ', level * 4);
	if (Splitter)
	{
#ifdef ADS_DEBUG_PRINT
		qDebug("%sSplitter %s v: %s c: %s",
			(const char*)buf,
			(Splitter->orientation() == Qt::Vertical) ? "--" : "|",
			 Splitter->isHidden() ? " " : "v",
			 QString::number(Splitter->count()).toStdString().c_str());
        std::cout << (const char*)buf << "Splitter "
            << ((Splitter->orientation() == Qt::Vertical) ? "--" : "|") << " "
            << (Splitter->isHidden() ? " " : "v") << " "
            << QString::number(Splitter->count()).toStdString() << std::endl;
#endif
		for (int i = 0; i < Splitter->count(); ++i)
		{
			dumpRecursive(level + 1, Splitter->widget(i));
		}
	}
	else
	{
		CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(widget);
		if (!DockArea)
		{
			return;
		}
#ifdef ADS_DEBUG_PRINT
		qDebug("%sDockArea", (const char*)buf);
		std::cout << (const char*)buf
			<< (DockArea->isHidden() ? " " : "v")
			<< (DockArea->openDockWidgetsCount() > 0 ? " " : "c")
			<< " DockArea" << std::endl;
		buf.fill(' ', (level + 1) * 4);
		for (int i = 0; i < DockArea->dockWidgetsCount(); ++i)
		{
			std::cout << (const char*)buf << (i == DockArea->currentIndex() ? "*" : " ");
			CDockWidget* DockWidget = DockArea->dockWidget(i);
			std::cout << (DockWidget->isHidden() ? " " : "v");
			std::cout << (DockWidget->isClosed() ? "c" : " ") << " ";
			std::cout << DockWidget->windowTitle().toStdString() << std::endl;
        }
#endif
	}
#else
	Q_UNUSED(level);
	Q_UNUSED(widget);
#endif
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoDockArea(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* TargetDockArea)
{
	if (CenterDockWidgetArea == area)
	{
		TargetDockArea->addDockWidget(Dockwidget);
		TargetDockArea->updateTitleBarVisibility();
		return TargetDockArea;
	}

	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	auto InsertParam = internal::dockAreaInsertParameters(area);

	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetDockArea);
	int index = TargetAreaSplitter ->indexOf(TargetDockArea);
	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
        ADS_PRINT("TargetAreaSplitter->orientation() == InsertParam.orientation()");
		TargetAreaSplitter->insertWidget(index + InsertParam.insertOffset(), NewDockArea);
	}
	else
	{
        ADS_PRINT("TargetAreaSplitter->orientation() != InsertParam.orientation()");
		QSplitter* NewSplitter = newSplitter(InsertParam.orientation());
		NewSplitter->addWidget(TargetDockArea);
		insertWidgetIntoSplitter(NewSplitter, NewDockArea, InsertParam.append());
		TargetAreaSplitter->insertWidget(index, NewSplitter);
	}

	appendDockAreas({NewDockArea});
	emitDockAreasAdded();
	return NewDockArea;
}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	d->DockManager = DockManager;
	d->isFloating = floatingWidget() != nullptr;

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 1);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);

	// The function d->newSplitter() accesses the config flags from dock
	// manager which in turn requires a properly constructed dock manager.
	// If this dock container is the dock manager, then it is not properly
	// constructed yet because this base class constructor is called before
	// the constructor of the DockManager private class
	if (DockManager != this)
	{
		d->DockManager->registerDockContainer(this);
		createRootSplitter();
	}
}

//============================================================================
CDockContainerWidget::~CDockContainerWidget()
{
	if (d->DockManager)
	{
		d->DockManager->removeDockContainer(this);
	}
	delete d;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	CDockAreaWidget* OldDockArea = Dockwidget->dockAreaWidget();
	if (OldDockArea)
	{
		OldDockArea->removeDockWidget(Dockwidget);
	}

	Dockwidget->setDockManager(d->DockManager);
	if (DockAreaWidget)
	{
		return d->dockWidgetIntoDockArea(area, Dockwidget, DockAreaWidget);
	}
	else
	{
		return d->dockWidgetIntoContainer(area, Dockwidget);
	}
}

//============================================================================
void CDockContainerWidget::removeDockWidget(CDockWidget* Dockwidget)
{
	CDockAreaWidget* Area = Dockwidget->dockAreaWidget();
	if (Area)
	{
		Area->removeDockWidget(Dockwidget);
	}
}

//============================================================================
unsigned int CDockContainerWidget::zOrderIndex() const
{
	return d->zOrderIndex;
}


//============================================================================
bool CDockContainerWidget::isInFrontOf(CDockContainerWidget* Other) const
{
	return this->zOrderIndex() > Other->zOrderIndex();
}


//============================================================================
bool CDockContainerWidget::event(QEvent *e)
{
	bool Result = QWidget::event(e);
	if (e->type() == QEvent::WindowActivate)
    {
        d->zOrderIndex = ++zOrderCounter;
    }
	else if (e->type() == QEvent::Show && !d->zOrderIndex)
	{
		d->zOrderIndex = ++zOrderCounter;
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::addDockArea(CDockAreaWidget* DockAreaWidget,
	DockWidgetArea area)
{
	CDockContainerWidget* Container = DockAreaWidget->dockContainer();
	if (Container && Container != this)
	{
		Container->removeDockArea(DockAreaWidget);
	}

	d->addDockArea(DockAreaWidget, area);
}


//============================================================================
void CDockContainerWidget::removeDockArea(CDockAreaWidget* area)
{
    ADS_PRINT("CDockContainerWidget::removeDockArea");
	area->disconnect(this);
	d->DockAreas.removeAll(area);
	CDockSplitter* Splitter = internal::findParent<CDockSplitter*>(area);

	// Remove are from parent splitter and recursively hide tree of parent
	// splitters if it has no visible content
	area->setParent(nullptr);
	internal::hideEmptyParentSplitters(Splitter);

	// Remove this area from cached areas
	const auto& cache = d->LastAddedAreaCache;
	if (auto p = std::find(cache, cache+sizeof(cache)/sizeof(cache[0]), area)) {
		d->LastAddedAreaCache[std::distance(cache, p)] = nullptr;
	}

	// If splitter has more than 1 widgets, we are finished and can leave
	if (Splitter->count() >  1)
	{
		goto emitAndExit;
	}

	// If this is the RootSplitter we need to remove empty splitters to
	// avoid too many empty splitters
	if (Splitter == d->RootSplitter)
	{
        ADS_PRINT("Removed from RootSplitter");
		// If splitter is empty, we are finished
		if (!Splitter->count())
		{
			Splitter->hide();
			goto emitAndExit;
		}

		QWidget* widget = Splitter->widget(0);
		QSplitter* ChildSplitter = qobject_cast<QSplitter*>(widget);
		// If the one and only content widget of the splitter is not a splitter
		// then we are finished
		if (!ChildSplitter)
		{
			goto emitAndExit;
		}

		// We replace the superfluous RootSplitter with the ChildSplitter
		ChildSplitter->setParent(nullptr);
		QLayoutItem* li = d->Layout->replaceWidget(Splitter, ChildSplitter);
		d->RootSplitter = ChildSplitter;
		delete li;
        ADS_PRINT("RootSplitter replaced by child splitter");
	}
	else if (Splitter->count() == 1)
	{
        ADS_PRINT("Replacing splitter with content");
		QSplitter* ParentSplitter = internal::findParent<QSplitter*>(Splitter);
		auto Sizes = ParentSplitter->sizes();
		QWidget* widget = Splitter->widget(0);
		widget->setParent(this);
		internal::replaceSplitterWidget(ParentSplitter, Splitter, widget);
		ParentSplitter->setSizes(Sizes);
	}

	delete Splitter;

emitAndExit:
	CDockWidget* TopLevelWidget = topLevelDockWidget();

	// Updated the title bar visibility of the dock widget if there is only
    // one single visible dock widget
	CDockWidget::emitTopLevelEventForWidget(TopLevelWidget, true);
	dumpLayout();
	d->emitDockAreasRemoved();
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockAreaAt(const QPoint& GlobalPos) const
{
	for (const auto& DockArea : d->DockAreas)
	{
		if (DockArea->isVisible() && DockArea->rect().contains(DockArea->mapFromGlobal(GlobalPos)))
		{
			return DockArea;
		}
	}

	return nullptr;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockArea(int Index) const
{
	return (Index < dockAreaCount()) ? d->DockAreas[Index] : nullptr;
}


//============================================================================
bool CDockContainerWidget::isFloating() const
{
	return d->isFloating;
}


//============================================================================
int CDockContainerWidget::dockAreaCount() const
{
	return d->DockAreas.count();
}


//============================================================================
int CDockContainerWidget::visibleDockAreaCount() const
{
	int Result = 0;
	for (auto DockArea : d->DockAreas)
	{
		Result += DockArea->isHidden() ? 0 : 1;
	}

	return Result;

	// TODO Cache or precalculate this to speed it up because it is used during
	// movement of floating widget
	//return d->visibleDockAreaCount();
}


//============================================================================
void CDockContainerWidget::dropFloatingWidget(CFloatingDockContainer* FloatingWidget,
	const QPoint& TargetPos)
{
    ADS_PRINT("CDockContainerWidget::dropFloatingWidget");
	CDockWidget* SingleDroppedDockWidget = FloatingWidget->topLevelDockWidget();
	CDockWidget* SingleDockWidget = topLevelDockWidget();
	CDockAreaWidget* DockArea = dockAreaAt(TargetPos);
	auto dropArea = InvalidDockWidgetArea;
	auto ContainerDropArea = d->DockManager->containerOverlay()->dropAreaUnderCursor();
	bool Dropped = false;

	if (DockArea)
	{
		auto dropOverlay = d->DockManager->dockAreaOverlay();
		dropOverlay->setAllowedAreas(DockArea->allowedAreas());
		dropArea = dropOverlay->showOverlay(DockArea);
		if (ContainerDropArea != InvalidDockWidgetArea &&
			ContainerDropArea != dropArea)
		{
			dropArea = InvalidDockWidgetArea;
		}

		if (dropArea != InvalidDockWidgetArea)
		{
            ADS_PRINT("Dock Area Drop Content: " << dropArea);
			d->dropIntoSection(FloatingWidget, DockArea, dropArea);
			Dropped = true;
		}
	}

	// mouse is over container
	if (InvalidDockWidgetArea == dropArea)
	{
		dropArea = ContainerDropArea;
        ADS_PRINT("Container Drop Content: " << dropArea);
		if (dropArea != InvalidDockWidgetArea)
		{
			d->dropIntoContainer(FloatingWidget, dropArea);
			Dropped = true;
		}
	}

	if (Dropped)
	{
		FloatingWidget->deleteLater();

		// If we dropped a floating widget with only one single dock widget, then we
		// drop a top level widget that changes from floating to docked now
		CDockWidget::emitTopLevelEventForWidget(SingleDroppedDockWidget, false);

		// If there was a top level widget before the drop, then it is not top
		// level widget anymore
		CDockWidget::emitTopLevelEventForWidget(SingleDockWidget, false);
	}

	window()->activateWindow();
	if (SingleDroppedDockWidget)
	{
		d->DockManager->notifyWidgetOrAreaRelocation(SingleDroppedDockWidget);
	}
	d->DockManager->notifyFloatingWidgetDrop(FloatingWidget);
}


//============================================================================
void CDockContainerWidget::dropWidget(QWidget* Widget, DockWidgetArea DropArea, CDockAreaWidget* TargetAreaWidget)
{
    CDockWidget* SingleDockWidget = topLevelDockWidget();
	if (TargetAreaWidget)
	{
		d->moveToNewSection(Widget, TargetAreaWidget, DropArea);
	}
	else
	{
		d->moveToContainer(Widget, DropArea);
	}

	// If there was a top level widget before the drop, then it is not top
	// level widget anymore
	CDockWidget::emitTopLevelEventForWidget(SingleDockWidget, false);
	CDockWidget* DockWidget = qobject_cast<CDockWidget*>(Widget);
	if (!DockWidget)
	{
		CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(Widget);
		auto OpenDockWidgets = DockArea->openedDockWidgets();
		if (OpenDockWidgets.count() == 1)
		{
			DockWidget = OpenDockWidgets[0];
		}
	}

	window()->activateWindow();
	d->DockManager->notifyWidgetOrAreaRelocation(Widget);
}


//============================================================================
QList<CDockAreaWidget*> CDockContainerWidget::openedDockAreas() const
{
	QList<CDockAreaWidget*> Result;
	for (auto DockArea : d->DockAreas)
	{
		if (!DockArea->isHidden())
		{
			Result.append(DockArea);
		}
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::saveState(QXmlStreamWriter& s) const
{
    ADS_PRINT("CDockContainerWidget::saveState isFloating "
        << isFloating());

	s.writeStartElement("Container");
	s.writeAttribute("Floating", QString::number(isFloating() ? 1 : 0));
	if (isFloating())
	{
		CFloatingDockContainer* FloatingWidget = floatingWidget();
		QByteArray Geometry = FloatingWidget->saveGeometry();
#if QT_VERSION < 0x050900
        s.writeTextElement("Geometry", qByteArrayToHex(Geometry, ' '));
#else
		s.writeTextElement("Geometry", Geometry.toHex(' '));
#endif
	}
	d->saveChildNodesState(s, d->RootSplitter);
	s.writeEndElement();
}


//============================================================================
bool CDockContainerWidget::restoreState(CDockingStateReader& s, bool Testing)
{
	bool IsFloating = s.attributes().value("Floating").toInt();
    ADS_PRINT("Restore CDockContainerWidget Floating" << IsFloating);

	QWidget*NewRootSplitter {};
	if (!Testing)
	{
		d->VisibleDockAreaCount = -1;// invalidate the dock area count
		d->DockAreas.clear();
		std::fill(std::begin(d->LastAddedAreaCache),std::end(d->LastAddedAreaCache), nullptr);
	}

	if (IsFloating)
	{
        ADS_PRINT("Restore floating widget");
		if (!s.readNextStartElement() || s.name() != "Geometry")
		{
			return false;
		}

		QByteArray GeometryString = s.readElementText(CDockingStateReader::ErrorOnUnexpectedElement).toLocal8Bit();
		QByteArray Geometry = QByteArray::fromHex(GeometryString);
		if (Geometry.isEmpty())
		{
			return false;
		}

		if (!Testing)
		{
			CFloatingDockContainer* FloatingWidget = floatingWidget();
			FloatingWidget->restoreGeometry(Geometry);
		}
	}

	if (!d->restoreChildNodes(s, NewRootSplitter, Testing))
	{
		return false;
	}

	if (Testing)
	{
		return true;
	}

	// If the root splitter is empty, rostoreChildNodes returns a 0 pointer
	// and we need to create a new empty root splitter
	if (!NewRootSplitter)
	{
		NewRootSplitter = d->newSplitter(Qt::Horizontal);
	}

	d->Layout->replaceWidget(d->RootSplitter, NewRootSplitter);
	QSplitter* OldRoot = d->RootSplitter;
	d->RootSplitter = qobject_cast<QSplitter*>(NewRootSplitter);
	OldRoot->deleteLater();

	return true;
}


//============================================================================
QSplitter* CDockContainerWidget::rootSplitter() const
{
	return d->RootSplitter;
}


//============================================================================
void CDockContainerWidget::createRootSplitter()
{
	if (d->RootSplitter)
	{
		return;
	}
	d->RootSplitter = d->newSplitter(Qt::Horizontal);
	d->Layout->addWidget(d->RootSplitter);
}


//============================================================================
void CDockContainerWidget::dumpLayout()
{
#if (ADS_DEBUG_LEVEL > 0)
	qDebug("\n\nDumping layout --------------------------");
	std::cout << "\n\nDumping layout --------------------------" << std::endl;
	d->dumpRecursive(0, d->RootSplitter);
	qDebug("--------------------------\n\n");
	std::cout << "--------------------------\n\n" << std::endl;
#endif
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::lastAddedDockAreaWidget(DockWidgetArea area) const
{
	return d->LastAddedAreaCache[areaIdToIndex(area)];
}


//============================================================================
bool CDockContainerWidget::hasTopLevelDockWidget() const
{
	auto DockAreas = openedDockAreas();
	if (DockAreas.count() != 1)
	{
		return false;
	}

	return DockAreas[0]->openDockWidgetsCount() == 1;
}


//============================================================================
CDockWidget* CDockContainerWidget::topLevelDockWidget() const
{
	auto TopLevelDockArea = topLevelDockArea();
	if (!TopLevelDockArea)
	{
		return nullptr;
	}

	auto DockWidgets = TopLevelDockArea->openedDockWidgets();
	if (DockWidgets.count() != 1)
	{
		return nullptr;
	}

	return DockWidgets[0];

}


//============================================================================
CDockAreaWidget* CDockContainerWidget::topLevelDockArea() const
{
	auto DockAreas = openedDockAreas();
	if (DockAreas.count() != 1)
	{
		return nullptr;
	}

	return DockAreas[0];
}


//============================================================================
QList<CDockWidget*> CDockContainerWidget::dockWidgets() const
{
	QList<CDockWidget*> Result;
	for (const auto DockArea : d->DockAreas)
	{
		Result.append(DockArea->dockWidgets());
	}

	return Result;
}


//============================================================================
CDockWidget::DockWidgetFeatures CDockContainerWidget::features() const
{
	CDockWidget::DockWidgetFeatures Features(CDockWidget::AllDockWidgetFeatures);
	for (const auto DockArea : d->DockAreas)
	{
		Features &= DockArea->features();
	}

	return Features;
}


//============================================================================
CFloatingDockContainer* CDockContainerWidget::floatingWidget() const
{
	return internal::findParent<CFloatingDockContainer*>(this);
}


//============================================================================
void CDockContainerWidget::closeOtherAreas(CDockAreaWidget* KeepOpenArea)
{
	for (const auto DockArea : d->DockAreas)
	{
		if (DockArea == KeepOpenArea)
		{
			continue;
		}

		if (!DockArea->features(BitwiseAnd).testFlag(CDockWidget::DockWidgetClosable))
		{
			continue;
		}

		// We do not close areas with widgets with custom close handling
		if (DockArea->features(BitwiseOr).testFlag(CDockWidget::CustomCloseHandling))
		{
			continue;
		}

		DockArea->closeArea();
	}
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
