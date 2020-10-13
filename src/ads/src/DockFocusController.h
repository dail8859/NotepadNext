#ifndef DockFocusControllerH
#define DockFocusControllerH
//============================================================================
/// \file   DockFocusController.h
/// \author Uwe Kindler
/// \date   05.06.2020
/// \brief  Declaration of CDockFocusController class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QObject>
#include "ads_globals.h"
#include "DockManager.h"

namespace ads
{
struct DockFocusControllerPrivate;
class CDockManager;
class CFloatingDockContainer;

/**
 * Manages focus styling of dock widgets and handling of focus changes
 */
class ADS_EXPORT CDockFocusController : public QObject
{
	Q_OBJECT
private:
	DockFocusControllerPrivate* d; ///< private data (pimpl)
    friend struct DockFocusControllerPrivate;

private slots:
	void onApplicationFocusChanged(QWidget *old, QWidget *now);
	void onFocusedDockAreaViewToggled(bool Open);
	void onStateRestored();
	void onDockWidgetVisibilityChanged(bool Visible);

public:
	using Super = QObject;
	/**
	 * Default Constructor
	 */
	CDockFocusController(CDockManager* DockManager);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockFocusController();

	/**
	 * Helper function to set focus depending on the configuration of the
	 * FocusStyling flag
	 */
	template <class QWidgetPtr>
	static void setWidgetFocus(QWidgetPtr widget)
	{
		if (!CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
		{
			return;
		}

		widget->setFocus(Qt::OtherFocusReason);
	}

	/**
	 * A container needs to call this function if a widget has been dropped
	 * into it
	 */
	void notifyWidgetOrAreaRelocation(QWidget* RelocatedWidget);

	/**
	 * This function is called, if a floating widget has been dropped into
	 * an new position.
	 * When this function is called, all dock widgets of the FloatingWidget
	 * are already inserted into its new position
	 */
	void notifyFloatingWidgetDrop(CFloatingDockContainer* FloatingWidget);

	/**
	 * Returns the dock widget that has focus style in the ui or a nullptr if
	 * not dock widget is painted focused.
	 */
	CDockWidget* focusedDockWidget() const;

public slots:
	/**
	 * Request a focus change to the given dock widget
	 */
	void setDockWidgetFocused(CDockWidget* focusedNow);
}; // class DockFocusController
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockFocusControllerH

