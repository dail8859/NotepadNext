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
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockWidgetTab.h"
#include "DockManager.h"

#include <algorithm>
#include <iostream>

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QAction>
#include <QXmlStreamWriter>
#include <QSettings>
#include <QMenu>
#include <QApplication>
#include <QGuiApplication>
#include <QPointer>

#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockWidget.h"
#include "ads_globals.h"
#include "DockAreaWidget.h"
#include "IconProvider.h"
#include "DockingStateReader.h"
#include "DockAreaTitleBar.h"

#ifdef Q_OS_LINUX
#include "linux/FloatingWidgetTitleBar.h"
#endif


/**
 * Initializes the resources specified by the .qrc file with the specified base
 * name. Normally, when resources are built as part of the application, the
 * resources are loaded automatically at startup. The Q_INIT_RESOURCE() macro
 * is necessary on some platforms for resources stored in a static library.
 * Because GCC caues a linker error if we put Q_INIT_RESOURCE into the
 * loadStyleSheet() function, we place it into a function outside of the ads
 * namespace
 */
static void initResource()
{
	Q_INIT_RESOURCE(ads);
}


namespace ads
{
static CDockManager::ConfigFlags StaticConfigFlags = CDockManager::DefaultNonOpaqueConfig;

/**
 * Private data class of CDockManager class (pimpl)
 */
struct DockManagerPrivate
{
	CDockManager* _this;
	QList<CFloatingDockContainer*> FloatingWidgets;
	QList<CDockContainerWidget*> Containers;
	CDockOverlay* ContainerOverlay;
	CDockOverlay* DockAreaOverlay;
	QMap<QString, CDockWidget*> DockWidgetsMap;
	QMap<QString, QByteArray> Perspectives;
	QMap<QString, QMenu*> ViewMenuGroups;
	QMenu* ViewMenu;
	CDockManager::eViewMenuInsertionOrder MenuInsertionOrder = CDockManager::MenuAlphabeticallySorted;
	bool RestoringState = false;
	QVector<CFloatingDockContainer*> UninitializedFloatingWidgets;
	QPointer<CDockWidget> FocusedDockWidget = nullptr;
	QPointer<CDockAreaWidget> FocusedArea = nullptr;
    QPointer<CFloatingDockContainer> FloatingWidget = nullptr;

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);

	/**
	 * Checks if the given data stream is a valid docking system state
	 * file.
	 */
	bool checkFormat(const QByteArray &state, int version);

	/**
	 * Restores the state
	 */
	bool restoreStateFromXml(const QByteArray &state, int version, bool Testing = internal::Restore);

	/**
	 * Restore state
	 */
	bool restoreState(const QByteArray &state, int version);

	void restoreDockWidgetsOpenState();
	void restoreDockAreasIndices();
	void emitTopLevelEvents();

	void hideFloatingWidgets()
	{
		// Hide updates of floating widgets from user
		for (auto FloatingWidget : FloatingWidgets)
		{
			FloatingWidget->hide();
		}
	}

	void markDockWidgetsDirty()
	{
		for (auto DockWidget : DockWidgetsMap)
		{
			DockWidget->setProperty("dirty", true);
		}
	}

	/**
	 * Restores the container with the given index
	 */
	bool restoreContainer(int Index, CDockingStateReader& stream, bool Testing);

	/**
	 * Loads the stylesheet
	 */
	void loadStylesheet();

	/**
	 * Adds action to menu - optionally in sorted order
	 */
	void addActionToMenu(QAction* Action, QMenu* Menu, bool InsertSorted);

	/**
	 * This function updates the focus style of the given dock widget and
	 * the dock area that it belongs to
	 */
	void updateDockWidgetFocus(CDockWidget* DockWidget);
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
void DockManagerPrivate::loadStylesheet()
{
	initResource();
	QString Result;
#ifdef Q_OS_LINUX
    QFile StyleSheetFile(":ads/stylesheets/default_linux.css");
#else
	QFile StyleSheetFile(":ads/stylesheets/default.css");
#endif
	StyleSheetFile.open(QIODevice::ReadOnly);
	QTextStream StyleSheetStream(&StyleSheetFile);
	Result = StyleSheetStream.readAll();
	StyleSheetFile.close();
	_this->setStyleSheet(Result);
}


//============================================================================
bool DockManagerPrivate::restoreContainer(int Index, CDockingStateReader& stream, bool Testing)
{
	if (Testing)
	{
		Index = 0;
	}

	bool Result = false;
	if (Index >= Containers.count())
	{
		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
		Result = FloatingWidget->restoreState(stream, Testing);
	}
	else
	{
        ADS_PRINT("d->Containers[i]->restoreState ");
		auto Container = Containers[Index];
		if (Container->isFloating())
		{
			Result = Container->floatingWidget()->restoreState(stream, Testing);
		}
		else
		{
			Result = Container->restoreState(stream, Testing);
		}
	}

	return Result;
}


//============================================================================
bool DockManagerPrivate::checkFormat(const QByteArray &state, int version)
{
    return restoreStateFromXml(state, version, internal::RestoreTesting);
}


//============================================================================
bool DockManagerPrivate::restoreStateFromXml(const QByteArray &state,  int version,
	bool Testing)
{
	Q_UNUSED(version);

    if (state.isEmpty())
    {
        return false;
    }
    CDockingStateReader s(state);
    s.readNextStartElement();
    if (s.name() != "QtAdvancedDockingSystem")
    {
    	return false;
    }
    ADS_PRINT(s.attributes().value("Version"));
    bool ok;
    int v = s.attributes().value("Version").toInt(&ok);
    if (!ok || v > CurrentVersion)
    {
    	return false;
    }

    s.setFileVersion(v);
    bool Result = true;
#ifdef ADS_DEBUG_PRINT
    int  DockContainers = s.attributes().value("Containers").toInt();
#endif
    ADS_PRINT(DockContainers);
    int DockContainerCount = 0;
    while (s.readNextStartElement())
    {
    	if (s.name() == "Container")
    	{
    		Result = restoreContainer(DockContainerCount, s, Testing);
			if (!Result)
			{
				break;
			}
			DockContainerCount++;
    	}
    }

    if (!Testing)
    {
		// Delete remaining empty floating widgets
		int FloatingWidgetIndex = DockContainerCount - 1;
		int DeleteCount = FloatingWidgets.count() - FloatingWidgetIndex;
		for (int i = 0; i < DeleteCount; ++i)
		{
			FloatingWidgets[FloatingWidgetIndex + i]->deleteLater();
			_this->removeDockContainer(FloatingWidgets[FloatingWidgetIndex + i]->dockContainer());
		}
    }

    return Result;
}


//============================================================================
void DockManagerPrivate::restoreDockWidgetsOpenState()
{
    // All dock widgets, that have not been processed in the restore state
    // function are invisible to the user now and have no assigned dock area
    // They do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto DockWidget : DockWidgetsMap)
    {
    	if (DockWidget->property(internal::DirtyProperty).toBool())
    	{
    		DockWidget->flagAsUnassigned();
            emit DockWidget->viewToggled(false);
    	}
    	else
    	{
    		DockWidget->toggleViewInternal(!DockWidget->property(internal::ClosedProperty).toBool());
    	}
    }
}


//============================================================================
void DockManagerPrivate::restoreDockAreasIndices()
{
    // Now all dock areas are properly restored and we setup the index of
    // The dock areas because the previous toggleView() action has changed
    // the dock area index
    int Count = 0;
    for (auto DockContainer : Containers)
    {
    	Count++;
    	for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
    	{
    		CDockAreaWidget* DockArea = DockContainer->dockArea(i);
    		QString DockWidgetName = DockArea->property("currentDockWidget").toString();
    		CDockWidget* DockWidget = nullptr;
    		if (!DockWidgetName.isEmpty())
    		{
    			DockWidget = _this->findDockWidget(DockWidgetName);
    		}

    		if (!DockWidget || DockWidget->isClosed())
    		{
    			int Index = DockArea->indexOfFirstOpenDockWidget();
    			if (Index < 0)
    			{
    				continue;
    			}
    			DockArea->setCurrentIndex(Index);
    		}
    		else
    		{
    			DockArea->internalSetCurrentDockWidget(DockWidget);
    		}
    	}
    }
}



//============================================================================
void DockManagerPrivate::emitTopLevelEvents()
{
    // Finally we need to send the topLevelChanged() signals for all dock
    // widgets if top level changed
    for (auto DockContainer : Containers)
    {
    	CDockWidget* TopLevelDockWidget = DockContainer->topLevelDockWidget();
    	if (TopLevelDockWidget)
    	{
    		TopLevelDockWidget->emitTopLevelChanged(true);
    	}
    	else
    	{
			for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
			{
				auto DockArea = DockContainer->dockArea(i);
				for (auto DockWidget : DockArea->dockWidgets())
				{
					DockWidget->emitTopLevelChanged(false);
				}
			}
    	}
    }
}


//============================================================================
bool DockManagerPrivate::restoreState(const QByteArray& State, int version)
{
	QByteArray state = State.startsWith("<?xml") ? State : qUncompress(State);
    if (!checkFormat(state, version))
    {
        ADS_PRINT("checkFormat: Error checking format!!!!!!!");
    	return false;
    }

    // Hide updates of floating widgets from use
    hideFloatingWidgets();
    markDockWidgetsDirty();

    if (!restoreStateFromXml(state, version))
    {
        ADS_PRINT("restoreState: Error restoring state!!!!!!!");
    	return false;
    }

    restoreDockWidgetsOpenState();
    restoreDockAreasIndices();
    emitTopLevelEvents();

    return true;
}


//============================================================================
void DockManagerPrivate::addActionToMenu(QAction* Action, QMenu* Menu, bool InsertSorted)
{
	if (InsertSorted)
	{
		auto Actions = Menu->actions();
		auto it = std::find_if(Actions.begin(), Actions.end(),
			[&Action](const QAction* a)
			{
				return a->text().compare(Action->text(), Qt::CaseInsensitive) > 0;
			});

		if (it == Actions.end())
		{
			Menu->addAction(Action);
		}
		else
		{
			Menu->insertAction(*it, Action);
		}
	}
	else
	{
		Menu->addAction(Action);
	}
}


//===========================================================================
void updateDockWidgetFocusStyle(CDockWidget* DockWidget, bool Focused)
{
	DockWidget->setProperty("focused", Focused);
	DockWidget->tabWidget()->setProperty("focused", Focused);
	DockWidget->tabWidget()->updateStyle();
	internal::repolishStyle(DockWidget);
}


//===========================================================================
void updateDockAreaFocusStyle(CDockAreaWidget* DockArea, bool Focused)
{
	DockArea->setProperty("focused", Focused);
	internal::repolishStyle(DockArea);
	internal::repolishStyle(DockArea->titleBar());
}


//===========================================================================
void updateFloatingWidgetFocusStyle(CFloatingDockContainer* FloatingWidget, bool Focused)
{
#ifdef Q_OS_LINUX
    auto TitleBar = qobject_cast<CFloatingWidgetTitleBar*>(FloatingWidget->titleBarWidget());
    if (!TitleBar)
    {
        return;
    }
    TitleBar->setProperty("focused", Focused);
    TitleBar->updateStyle();
#else
    Q_UNUSED(FloatingWidget)
    Q_UNUSED(Focused)
#endif
}


//============================================================================
void DockManagerPrivate::updateDockWidgetFocus(CDockWidget* DockWidget)
{
	CDockAreaWidget* NewFocusedDockArea = nullptr;
	if (FocusedDockWidget)
	{
		updateDockWidgetFocusStyle(FocusedDockWidget, false);
	}
	FocusedDockWidget = DockWidget;
	updateDockWidgetFocusStyle(FocusedDockWidget, true);
	NewFocusedDockArea = FocusedDockWidget->dockAreaWidget();
	if (!NewFocusedDockArea || (FocusedArea == NewFocusedDockArea))
	{
		return;
	}

	if (FocusedArea)
	{
		std::cout << "FocusedArea" << std::endl;
		QObject::disconnect(FocusedArea, SIGNAL(viewToggled(bool)), _this, SLOT(onFocusedDockAreaViewToggled(bool)));
		updateDockAreaFocusStyle(FocusedArea, false);
	}

	FocusedArea = NewFocusedDockArea;
	updateDockAreaFocusStyle(FocusedArea, true);
	QObject::connect(FocusedArea, SIGNAL(viewToggled(bool)), _this, SLOT(onFocusedDockAreaViewToggled(bool)));

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
}


//============================================================================
CDockManager::CDockManager(QWidget *parent) :
	CDockContainerWidget(this, parent),
	d(new DockManagerPrivate(this))
{
	createRootSplitter();
	QMainWindow* MainWindow = qobject_cast<QMainWindow*>(parent);
	if (MainWindow)
	{
		MainWindow->setCentralWidget(this);
	}

	d->ViewMenu = new QMenu(tr("Show View"), this);
	d->DockAreaOverlay = new CDockOverlay(this, CDockOverlay::ModeDockAreaOverlay);
	d->ContainerOverlay = new CDockOverlay(this, CDockOverlay::ModeContainerOverlay);
	d->Containers.append(this);
	d->loadStylesheet();

	if (CDockManager::configFlags().testFlag(CDockManager::FocusStyling))
	{
		connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)),
			this, SLOT(onFocusChanged(QWidget*, QWidget*)));
	}
}

//============================================================================
CDockManager::~CDockManager()
{
	auto FloatingWidgets = d->FloatingWidgets;
	for (auto FloatingWidget : FloatingWidgets)
	{
		delete FloatingWidget;
	}
	delete d;
}


//============================================================================
void CDockManager::registerFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.append(FloatingWidget);
	emit floatingWidgetCreated(FloatingWidget);
    ADS_PRINT("d->FloatingWidgets.count() " << d->FloatingWidgets.count());
}


//============================================================================
void CDockManager::removeFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.removeAll(FloatingWidget);
}


//============================================================================
void CDockManager::registerDockContainer(CDockContainerWidget* DockContainer)
{
	d->Containers.append(DockContainer);
}


//============================================================================
void CDockManager::removeDockContainer(CDockContainerWidget* DockContainer)
{
	if (this != DockContainer)
	{
		d->Containers.removeAll(DockContainer);
	}
}


//============================================================================
CDockOverlay* CDockManager::containerOverlay() const
{
	return d->ContainerOverlay;
}


//============================================================================
CDockOverlay* CDockManager::dockAreaOverlay() const
{
	return d->DockAreaOverlay;
}


//============================================================================
const QList<CDockContainerWidget*> CDockManager::dockContainers() const
{
	return d->Containers;
}


//============================================================================
const QList<CFloatingDockContainer*> CDockManager::floatingWidgets() const
{
	return d->FloatingWidgets;
}


//============================================================================
unsigned int CDockManager::zOrderIndex() const
{
	return 0;
}


//============================================================================
QByteArray CDockManager::saveState(int version) const
{
    QByteArray xmldata;
    QXmlStreamWriter s(&xmldata);
    auto ConfigFlags = CDockManager::configFlags();
	s.setAutoFormatting(ConfigFlags.testFlag(XmlAutoFormattingEnabled));
    s.writeStartDocument();
		s.writeStartElement("QtAdvancedDockingSystem");
		s.writeAttribute("Version", QString::number(version));
		s.writeAttribute("Containers", QString::number(d->Containers.count()));
		for (auto Container : d->Containers)
		{
			Container->saveState(s);
		}

		s.writeEndElement();
    s.writeEndDocument();

    return ConfigFlags.testFlag(XmlCompressionEnabled)
    	? qCompress(xmldata, 9) : xmldata;
}


//============================================================================
bool CDockManager::restoreState(const QByteArray &state, int version)
{
	// Prevent multiple calls as long as state is not restore. This may
	// happen, if QApplication::processEvents() is called somewhere
	if (d->RestoringState)
	{
		return false;
	}

	// We hide the complete dock manager here. Restoring the state means
	// that DockWidgets are removed from the DockArea internal stack layout
	// which in turn  means, that each time a widget is removed the stack
	// will show and raise the next available widget which in turn
	// triggers show events for the dock widgets. To avoid this we hide the
	// dock manager. Because there will be no processing of application
	// events until this function is finished, the user will not see this
	// hiding
	bool IsHidden = this->isHidden();
	if (!IsHidden)
	{
		hide();
	}
	d->RestoringState = true;
	emit restoringState();
	bool Result = d->restoreState(state, version);
	d->RestoringState = false;
	emit stateRestored();
	if (!IsHidden)
	{
		show();
	}

	return Result;
}


//============================================================================
CFloatingDockContainer* CDockManager::addDockWidgetFloating(CDockWidget* Dockwidget)
{
	d->DockWidgetsMap.insert(Dockwidget->objectName(), Dockwidget);
	CDockAreaWidget* OldDockArea = Dockwidget->dockAreaWidget();
	if (OldDockArea)
	{
		OldDockArea->removeDockWidget(Dockwidget);
	}

	Dockwidget->setDockManager(this);
	CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(Dockwidget);
	FloatingWidget->resize(Dockwidget->size());
	if (isVisible())
	{
		FloatingWidget->show();
	}
	else
	{
		d->UninitializedFloatingWidgets.append(FloatingWidget);
	}
	return FloatingWidget;
}


//============================================================================
void CDockManager::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
	if (d->UninitializedFloatingWidgets.empty())
	{
		return;
	}

	for (auto FloatingWidget : d->UninitializedFloatingWidgets)
	{
		FloatingWidget->show();
	}
	d->UninitializedFloatingWidgets.clear();
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidget(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget)
{
	d->DockWidgetsMap.insert(Dockwidget->objectName(), Dockwidget);
	return CDockContainerWidget::addDockWidget(area, Dockwidget, DockAreaWidget);
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidgetTab(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* AreaWidget = lastAddedDockAreaWidget(area);
	if (AreaWidget)
	{
		return addDockWidget(ads::CenterDockWidgetArea, Dockwidget, AreaWidget);
	}
	else if (!openedDockAreas().isEmpty())
	{
		return addDockWidget(area, Dockwidget, openedDockAreas().last());
	}
	else
	{
		return addDockWidget(area, Dockwidget, nullptr);
	}
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidgetTabToArea(CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	return addDockWidget(ads::CenterDockWidgetArea, Dockwidget, DockAreaWidget);
}


//============================================================================
CDockWidget* CDockManager::findDockWidget(const QString& ObjectName) const
{
	return d->DockWidgetsMap.value(ObjectName, nullptr);
}

//============================================================================
void CDockManager::removeDockWidget(CDockWidget* Dockwidget)
{
	emit dockWidgetAboutToBeRemoved(Dockwidget);
	d->DockWidgetsMap.remove(Dockwidget->objectName());
	CDockContainerWidget::removeDockWidget(Dockwidget);
	emit dockWidgetRemoved(Dockwidget);
}

//============================================================================
QMap<QString, CDockWidget*> CDockManager::dockWidgetsMap() const
{
	return d->DockWidgetsMap;
}


//============================================================================
void CDockManager::addPerspective(const QString& UniquePrespectiveName)
{
	d->Perspectives.insert(UniquePrespectiveName, saveState());
	emit perspectiveListChanged();
}


//============================================================================
void CDockManager::removePerspective(const QString& Name)
{
	removePerspectives({Name});
}


//============================================================================
void CDockManager::removePerspectives(const QStringList& Names)
{
	int Count = 0;
	for (auto Name : Names)
	{
		Count += d->Perspectives.remove(Name);
	}

	if (Count)
	{
		emit perspectivesRemoved();
		emit perspectiveListChanged();
	}
}


//============================================================================
QStringList CDockManager::perspectiveNames() const
{
	return d->Perspectives.keys();
}


//============================================================================
void CDockManager::openPerspective(const QString& PerspectiveName)
{
	const auto Iterator = d->Perspectives.find(PerspectiveName);
	if (d->Perspectives.end() == Iterator)
	{
		return;
	}

	emit openingPerspective(PerspectiveName);
	restoreState(Iterator.value());
	emit perspectiveOpened(PerspectiveName);
}


//============================================================================
void CDockManager::savePerspectives(QSettings& Settings) const
{
	Settings.beginWriteArray("Perspectives", d->Perspectives.size());
	int i = 0;
	for (auto it = d->Perspectives.constBegin(); it != d->Perspectives.constEnd(); ++it)
	{
		Settings.setArrayIndex(i);
		Settings.setValue("Name", it.key());
		Settings.setValue("State", it.value());
		++i;
	}
	Settings.endArray();
}


//============================================================================
void CDockManager::loadPerspectives(QSettings& Settings)
{
	d->Perspectives.clear();
	int Size = Settings.beginReadArray("Perspectives");
	if (!Size)
	{
		Settings.endArray();
		return;
	}

	for (int i = 0; i < Size; ++i)
	{
		Settings.setArrayIndex(i);
		QString Name = Settings.value("Name").toString();
		QByteArray Data = Settings.value("State").toByteArray();
		if (Name.isEmpty() || Data.isEmpty())
		{
			continue;
		}

		d->Perspectives.insert(Name, Data);
	}

	Settings.endArray();
}

//============================================================================
QAction* CDockManager::addToggleViewActionToMenu(QAction* ToggleViewAction,
	const QString& Group, const QIcon& GroupIcon)
{
	bool AlphabeticallySorted = (MenuAlphabeticallySorted == d->MenuInsertionOrder);
	if (!Group.isEmpty())
	{
		QMenu* GroupMenu = d->ViewMenuGroups.value(Group, 0);
		if (!GroupMenu)
		{
			GroupMenu = new QMenu(Group, this);
			GroupMenu->setIcon(GroupIcon);
			d->addActionToMenu(GroupMenu->menuAction(), d->ViewMenu, AlphabeticallySorted);
			d->ViewMenuGroups.insert(Group, GroupMenu);
		}
		else if (GroupMenu->icon().isNull() && !GroupIcon.isNull())
		{
			GroupMenu->setIcon(GroupIcon);
		}

		d->addActionToMenu(ToggleViewAction, GroupMenu, AlphabeticallySorted);
		return GroupMenu->menuAction();
	}
	else
	{
		d->addActionToMenu(ToggleViewAction, d->ViewMenu, AlphabeticallySorted);
		return ToggleViewAction;
	}
}


//============================================================================
QMenu* CDockManager::viewMenu() const
{
	return d->ViewMenu;
}


//============================================================================
void CDockManager::setViewMenuInsertionOrder(eViewMenuInsertionOrder Order)
{
	d->MenuInsertionOrder = Order;
}


//===========================================================================
bool CDockManager::isRestoringState() const
{
	return d->RestoringState;
}


//===========================================================================
int CDockManager::startDragDistance()
{
	return QApplication::startDragDistance() * 1.5;
}


//===========================================================================
CDockManager::ConfigFlags CDockManager::configFlags()
{
	return StaticConfigFlags;
}


//===========================================================================
void CDockManager::setConfigFlags(const ConfigFlags Flags)
{
	StaticConfigFlags = Flags;
}


//===========================================================================
void CDockManager::setConfigFlag(eConfigFlag Flag, bool On)
{
	internal::setFlag(StaticConfigFlags, Flag, On);
}

//===========================================================================
bool CDockManager::testConfigFlag(eConfigFlag Flag)
{
    return configFlags().testFlag(Flag);
}


//===========================================================================
CIconProvider& CDockManager::iconProvider()
{
	static CIconProvider Instance;
	return Instance;
}


//===========================================================================
void CDockManager::onFocusChanged(QWidget* focusedOld, QWidget* focusedNow)
{
	std::cout << "CDockManager::onFocusChanged" << std::endl;
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
	else
	{
		DockWidget = internal::findParent<CDockWidget*>(focusedNow);
	}

#ifdef Q_OS_LINUX
    if (!DockWidget)
	{
		return;
	}
#else
    if (!DockWidget || !DockWidget->tabWidget()->isVisible())
	{
		return;
	}
#endif

	std::cout << "CDockManager::onFocusChanged " << DockWidget->tabWidget()->text().toStdString() << std::endl;
	d->updateDockWidgetFocus(DockWidget);
}


//===========================================================================
void CDockManager::onFocusedDockAreaViewToggled(bool Open)
{
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
void CDockManager::emitWidgetDroppedSignals(QWidget* DroppedWidget)
{
	CDockWidget* DockWidget = qobject_cast<CDockWidget*>(DroppedWidget);
	if (DockWidget)
	{
		CDockManager::setWidgetFocus(DockWidget->tabWidget());
		emit dockWidgetDropped(DockWidget);
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
void CDockManager::endFloatingWidgetDrop(CFloatingDockContainer* FloatingWidget)
{
	if (!FloatingWidget)
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
		std::cout << "Dropped focus dock widget " << DockWidget->objectName().toStdString() << std::endl;
		CDockManager::setWidgetFocus(DockWidget->tabWidget());
	}
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
