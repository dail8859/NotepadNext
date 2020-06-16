%If (Qt_5_0_0 -)

namespace ads
{
    
class CDockComponentsFactory
{

    %TypeHeaderCode
    #include <DockComponentsFactory.h>
    %End

public:
	virtual ~CDockComponentsFactory();
	virtual CDockWidgetTab* createDockWidgetTab(CDockWidget* DockWidget /Transfer/ ) const;
	virtual CDockAreaTabBar* createDockAreaTabBar(CDockAreaWidget* DockArea /Transfer/ ) const;
	virtual CDockAreaTitleBar* createDockAreaTitleBar(CDockAreaWidget* DockArea /Transfer/ ) const;
	static const CDockComponentsFactory* factory();
	static void setFactory(CDockComponentsFactory* Factory /KeepReference/);
	static void resetDefaultFactory();
    
};

};

%End
