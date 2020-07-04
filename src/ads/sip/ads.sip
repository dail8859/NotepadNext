%Module(name=PyQtAds.QtAds.ads, call_super_init=True, keyword_arguments="Optional", use_limited_api=True)
%Import QtCore/QtCoremod.sip
%DefaultSupertype sip.simplewrapper

%Include ads_globals.sip
%Include DockWidget.sip
%Include DockAreaTabBar.sip
%Include DockAreaTitleBar_p.sip
%Include DockAreaTitleBar.sip
%Include DockAreaWidget.sip
%Include DockComponentsFactory.sip
%Include DockContainerWidget.sip
%Include DockingStateReader.sip
%Include DockFocusController.sip
%Include DockManager.sip
%Include DockOverlay.sip
%Include DockSplitter.sip
%Include DockWidgetTab.sip
%Include ElidingLabel.sip
%Include FloatingDockContainer.sip
%Include FloatingDragPreview.sip
%Include IconProvider.sip
%If (WS_X11)
    %Include linux/FloatingWidgetTitleBar.sip
%End
