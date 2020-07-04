%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)
    
%MappedType QMap<QString, ads::CDockWidget*>
        /TypeHint="Dict[QString, CDockWidget*]", TypeHintValue="{}"/
{
%TypeHeaderCode
#include <qmap.h>
%End

%ConvertFromTypeCode
    PyObject *d = PyDict_New();

    if (!d)
        return 0;
    
    QMap<QString, ads::CDockWidget*>::const_iterator it = sipCpp->constBegin();
    QMap<QString, ads::CDockWidget*>::const_iterator end = sipCpp->constEnd();

    while (it != end)
    {
        QString *k = new QString(it.key());
        PyObject *kobj = sipConvertFromType(k, sipType_QString,
                sipTransferObj);

        if (!kobj)
        {
            delete k;
            Py_DECREF(d);

            return 0;
        }

        PyObject *vobj = sipConvertFromType(it.value(), sipType_ads_CDockWidget,
                sipTransferObj);

        if (!vobj)
        {
            Py_DECREF(kobj);
            Py_DECREF(d);

            return 0;
        }

        int rc = PyDict_SetItem(d, kobj, vobj);

        Py_DECREF(vobj);
        Py_DECREF(kobj);

        if (rc < 0)
        {
            Py_DECREF(d);

            return 0;
        }

        ++it;
    }
    
    return d;
%End

%ConvertToTypeCode
    if (!sipIsErr)
            return PyDict_Check(sipPy);

    QMap<QString, ads::CDockWidget*> *qm = new QMap<QString, ads::CDockWidget*>;
    
    Py_ssize_t pos = 0;
    PyObject *kobj, *vobj;
    while (PyDict_Next(sipPy, &pos, &kobj, &vobj))
    {
        int kstate;
        QString *k = reinterpret_cast<QString *>(
                sipForceConvertToType(kobj, sipType_QString, sipTransferObj,
                        SIP_NOT_NONE, &kstate, sipIsErr));

        if (*sipIsErr)
        {
            PyErr_Format(PyExc_TypeError,
                    "a dict key has type '%s' but '_TYPE1_' is expected",
                    sipPyTypeName(Py_TYPE(kobj)));

            delete qm;

            return 0;
        }

        int vstate;
        ads::CDockWidget *v = reinterpret_cast<ads::CDockWidget *>(
                sipForceConvertToType(vobj, sipType_ads_CDockWidget, sipTransferObj,
                        SIP_NOT_NONE, &vstate, sipIsErr));

        if (*sipIsErr)
        {
            PyErr_Format(PyExc_TypeError,
                    "a dict value has type '%s' but '_TYPE2_' is expected",
                    sipPyTypeName(Py_TYPE(vobj)));

            sipReleaseType(k, sipType_QString, kstate);
            delete qm;

            return 0;
        }

        qm->insert(*k, v);

        sipReleaseType(v, sipType_ads_CDockWidget, vstate);
        sipReleaseType(k, sipType_QString, kstate);
    }
 
    *sipCppPtr = qm;
 
    return sipGetState(sipTransferObj);
%End
};

namespace ads
{

class CDockManager : ads::CDockContainerWidget
{
    
    %TypeHeaderCode
    #include <DockManager.h>
    %End

protected:
	void registerFloatingWidget(ads::CFloatingDockContainer* FloatingWidget /Transfer/);
	void removeFloatingWidget(ads::CFloatingDockContainer* FloatingWidget) /TransferBack/;
	void registerDockContainer(ads::CDockContainerWidget* DockContainer /Transfer/);
	void removeDockContainer(ads::CDockContainerWidget* DockContainer /TransferBack/);
	ads::CDockOverlay* containerOverlay() const;
	ads::CDockOverlay* dockAreaOverlay() const;
    void notifyWidgetOrAreaRelocation(QWidget* RelocatedWidget);
    void notifyFloatingWidgetDrop(ads::CFloatingDockContainer* FloatingWidget);

    virtual void showEvent(QShowEvent *event);

public:
	enum eViewMenuInsertionOrder
	{
		MenuSortedByInsertion,
		MenuAlphabeticallySorted
	};

	enum eConfigFlag
	{
		ActiveTabHasCloseButton,
		DockAreaHasCloseButton,
		DockAreaCloseButtonClosesTab,
		OpaqueSplitterResize,
		XmlAutoFormattingEnabled,
		XmlCompressionEnabled,
		TabCloseButtonIsToolButton,
		AllTabsHaveCloseButton,
		RetainTabSizeWhenCloseButtonHidden,
		OpaqueUndocking,
		DragPreviewIsDynamic,
		DragPreviewShowsContentPixmap,
		DragPreviewHasWindowFrame,
		AlwaysShowTabs,
		DockAreaHasUndockButton,
		DockAreaHasTabsMenuButton,
		DockAreaHideDisabledButtons,
		DockAreaDynamicTabsMenuButtonVisibility,
		FloatingContainerHasWidgetTitle,
		FloatingContainerHasWidgetIcon,
		HideSingleCentralWidgetTitleBar,
        FocusHighlighting,
        DefaultDockAreaButtons,
		DefaultBaseConfig,
        DefaultOpaqueConfig,
		DefaultNonOpaqueConfig,
		NonOpaqueWithWindowFrame,
	};
    typedef QFlags<ads::CDockManager::eConfigFlag> ConfigFlags;

	CDockManager(QWidget* parent /TransferThis/ = 0);
	virtual ~CDockManager();
	static ads::CDockManager::ConfigFlags configFlags();
	static void setConfigFlags(const ads::CDockManager::ConfigFlags Flags);
	static void setConfigFlag(ads::CDockManager::eConfigFlag Flag, bool On = true);
	static bool testConfigFlag(eConfigFlag Flag);
    static ads::CIconProvider& iconProvider();
	ads::CDockAreaWidget* addDockWidget(ads::DockWidgetArea area, ads::CDockWidget* Dockwidget /Transfer/,
		ads::CDockAreaWidget* DockAreaWidget /Transfer/ = 0);
	ads::CDockAreaWidget* addDockWidgetTab(ads::DockWidgetArea area,
		ads::CDockWidget* Dockwidget /Transfer/);
	ads::CDockAreaWidget* addDockWidgetTabToArea(ads::CDockWidget* Dockwidget /Transfer/,
		ads::CDockAreaWidget* DockAreaWidget /Transfer/);
    ads::CFloatingDockContainer* addDockWidgetFloating(ads::CDockWidget* DockWidget /Transfer/);
	ads::CDockWidget* findDockWidget(const QString& ObjectName) const;
	void removeDockWidget(ads::CDockWidget* Dockwidget) /TransferBack/;
	QMap<QString, ads::CDockWidget*> dockWidgetsMap() const;
	const QList<ads::CDockContainerWidget*> dockContainers() const;
	const QList<ads::CFloatingDockContainer*> floatingWidgets() const;
	unsigned int zOrderIndex() const;
	QByteArray saveState(int version = 0) const;
	bool restoreState(const QByteArray &state, int version = 0);
	void addPerspective(const QString& UniquePrespectiveName);
	void removePerspective(const QString& Name);
	void removePerspectives(const QStringList& Names);
	QStringList perspectiveNames() const;
	void savePerspectives(QSettings& Settings) const;
	void loadPerspectives(QSettings& Settings);
	QAction* addToggleViewActionToMenu(QAction* ToggleViewAction /Transfer/,
		const QString& Group = QString(), const QIcon& GroupIcon = QIcon());
	QMenu* viewMenu() const;
	void setViewMenuInsertionOrder(ads::CDockManager::eViewMenuInsertionOrder Order);
	bool isRestoringState() const;
	static int startDragDistance();

public slots:
	void openPerspective(const QString& PerspectiveName);
    void setDockWidgetFocused(ads::CDockWidget* DockWidget);

signals:
	void perspectiveListChanged();
	void perspectivesRemoved();
	void restoringState();
    void stateRestored();
    void openingPerspective(const QString& PerspectiveName);
    void perspectiveOpened(const QString& PerspectiveName);
	void floatingWidgetCreated(CFloatingDockContainer* FloatingWidget);
    void dockAreaCreated(ads::CDockAreaWidget* DockArea);
    void dockWidgetAboutToBeRemoved(ads::CDockWidget* DockWidget);
    void dockWidgetRemoved(ads::CDockWidget* DockWidget);
    void focusedDockWidgetChanged(ads::CDockWidget* old, ads::CDockWidget* now);
};

};

%End
