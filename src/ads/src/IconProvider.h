#ifndef IconProviderH
#define IconProviderH
//============================================================================
/// \file   IconProvider.h
/// \author Uwe Kindler
/// \date   18.10.2019
/// \brief  Declaration of CIconProvider
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QIcon>

#include "ads_globals.h"

namespace ads
{

struct IconProviderPrivate;

/**
 * This object provides all icons that are required by the advanced docking
 * system.
 * The IconProvider enables the user to register custom icons in case using
 * stylesheets is not an option.
 */
class ADS_EXPORT CIconProvider
{
private:
	IconProviderPrivate* d; ///< private data (pimpl)
	friend struct IconProviderPrivate;

public:
	/**
	 * Default Constructor
	 */
	CIconProvider();

	/**
	 * Virtual Destructor
	 */
	virtual ~CIconProvider();

	/**
	 * The function returns a custom icon if one is registered and a null Icon
	 * if no custom icon is registered
	 */
	QIcon customIcon(eIcon IconId) const;

	/**
	 * Registers a custom icon for the given IconId
	 */
	void registerCustomIcon(eIcon IconId, const QIcon &icon);
}; // class IconProvider

} // namespace ads


//---------------------------------------------------------------------------
#endif // IconProviderH
