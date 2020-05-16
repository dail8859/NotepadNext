#ifndef DockComponentsFactoryH
#define DockComponentsFactoryH
//============================================================================
/// \file   DockComponentsFactory.h
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Declaration of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ads_globals.h"

namespace ads
{
class CDockWidgetTab;
class CDockAreaTitleBar;
class CDockAreaTabBar;
class CDockAreaWidget;
class CDockWidget;



/**
 * Factory for creation of certain GUI elements for the docking framework.
 * A default unique instance provided by CDockComponentsFactory is used for
 * creation of all supported components. To inject your custom components,
 * you can create your own derived dock components factory and register
 * it via setDefaultFactory() function.
 * \code
 * CDockComponentsFactory::setDefaultFactory(new MyComponentsFactory()));
 * \endcode
 */
class ADS_EXPORT CDockComponentsFactory
{
public:
	/**
	 * Force virtual destructor
	 */
	virtual ~CDockComponentsFactory() {}

	/**
	 * This default implementation just creates a dock widget tab with
	 * new CDockWidgetTab(DockWIdget).
	 */
	virtual CDockWidgetTab* createDockWidgetTab(CDockWidget* DockWidget) const;

	/**
	 * This default implementation just creates a dock area tab bar with
	 * new CDockAreaTabBar(DockArea).
	 */
	virtual CDockAreaTabBar* createDockAreaTabBar(CDockAreaWidget* DockArea) const;

	/**
	 * This default implementation just creates a dock area title bar with
	 * new CDockAreaTitleBar(DockArea).
	 */
	virtual CDockAreaTitleBar* createDockAreaTitleBar(CDockAreaWidget* DockArea) const;

	/**
	 * Returns the default components factory
	 */
	static const CDockComponentsFactory* factory();

	/**
	 * Sets a new default factory for creation of GUI elements.
	 * This function takes ownership of the given Factory.
	 */
	static void setFactory(CDockComponentsFactory* Factory);

	/**
	 * Resets the current factory to the
	 */
	static void resetDefaultFactory();
};


/**
 * Convenience function to ease factory instance access
 */
inline const CDockComponentsFactory* componentsFactory()
{
	return CDockComponentsFactory::factory();
}

} // namespace ads

//---------------------------------------------------------------------------
#endif // DockComponentsFactoryH
