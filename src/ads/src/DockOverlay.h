#ifndef DockOverlayH
#define DockOverlayH
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
//                                   INCLUDES
//============================================================================
#include <QPointer>
#include <QHash>
#include <QRect>
#include <QFrame>

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QGridLayout)

namespace ads
{
struct DockOverlayPrivate;
class CDockOverlayCross;

/*!
 * DockOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class ADS_EXPORT CDockOverlay : public QFrame
{
	Q_OBJECT
private:
	DockOverlayPrivate* d; //< private data class
	friend struct DockOverlayPrivate;
	friend class DockOverlayCross;

public:
	using Super = QFrame;

	enum eMode
	{
		ModeDockAreaOverlay,
		ModeContainerOverlay
	};

	/**
	 * Creates a dock overlay
	 */
	CDockOverlay(QWidget* parent, eMode Mode = ModeDockAreaOverlay);

	/**
	 * Virtual destructor
	 */
	virtual ~CDockOverlay();

	/**
	 * Configures the areas that are allowed for docking
	 */
	void setAllowedAreas(DockWidgetAreas areas);

	/**
	 * Returns flags with all allowed drop areas
	 */
	DockWidgetAreas allowedAreas() const;

	/**
	 * Returns the drop area under the current cursor location
	 */
	DockWidgetArea dropAreaUnderCursor() const;

	/**
	 * This function returns the same like dropAreaUnderCursor() if this
	 * overlay is not hidden and if drop preview is enabled and returns
	 * InvalidDockWidgetArea if it is hidden or drop preview is disabled.
	 */
	DockWidgetArea visibleDropAreaUnderCursor() const;

	/**
	 * Show the drop overly for the given target widget
	 */
	DockWidgetArea showOverlay(QWidget* target);

	/**
	 * Hides the overlay
	 */
	void hideOverlay();

	/**
	 * Enables / disables the semi transparent overlay rectangle that represents
	 * the future area of the dropped widget
	 */
	void enableDropPreview(bool Enable);

	/**
	 * Returns true if drop preview is enabled
	 */
	bool dropPreviewEnabled() const;

	/**
	 * The drop overlay rectangle for the target area
	 */
	QRect dropOverlayRect() const;

	/**
	 * Handle polish events
	 */
	virtual bool event(QEvent *e) override;

protected:
	virtual void paintEvent(QPaintEvent *e) override;
	virtual void showEvent(QShowEvent* e) override;
	virtual void hideEvent(QHideEvent* e) override;
};


struct DockOverlayCrossPrivate;
/*!
 * DockOverlayCross shows a cross with 5 different drop area possibilities.
 * I could have handled everything inside DockOverlay, but because of some
 * styling issues it's better to have a separate class for the cross.
 * You can style the cross icon using the property system.
 * \code
 * ads--CDockOverlayCross
  {
		qproperty-iconFrameColor: palette(highlight);
		qproperty-iconBackgroundColor: palette(base);
		qproperty-iconOverlayColor: palette(highlight);
		qproperty-iconArrowColor: rgb(227, 227, 227);
		qproperty-iconShadowColor: rgb(0, 0, 0);
	}
 * \endcode
 * Or you can use the iconColors property to pass in AARRGGBB values as
 * hex string like shown in the example below.
 * \code
 * ads--CDockOverlayCross
 * {
 *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
 * }
 * \endcode
 */
class CDockOverlayCross : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString iconColors READ iconColors WRITE setIconColors)
	Q_PROPERTY(QColor iconFrameColor READ iconColor WRITE setIconFrameColor)
	Q_PROPERTY(QColor iconBackgroundColor READ iconColor WRITE setIconBackgroundColor)
	Q_PROPERTY(QColor iconOverlayColor READ iconColor WRITE setIconOverlayColor)
	Q_PROPERTY(QColor iconArrowColor READ iconColor WRITE setIconArrowColor)
	Q_PROPERTY(QColor iconShadowColor READ iconColor WRITE setIconShadowColor)

public:
	enum eIconColor
	{
		FrameColor,///< the color of the frame of the small window icon
		WindowBackgroundColor,///< the background color of the small window in the icon
		OverlayColor,///< the color that shows the overlay (the dock side) in the icon
		ArrowColor,///< the arrow that points into the direction
		ShadowColor///< the color of the shadow rectangle that is painted below the icons
	};

private:
	DockOverlayCrossPrivate* d;
	friend struct DockOverlayCrossPrivate;
	friend class CDockOverlay;

protected:
	/**
     * This function returns an empty string and is only here to silence
     * moc
     */
    QString iconColors() const;

    /**
     * This is a dummy function for the property system
     */
    QColor iconColor() const {return QColor();}
    void setIconFrameColor(const QColor& Color) {setIconColor(FrameColor, Color);}
    void setIconBackgroundColor(const QColor& Color) {setIconColor(WindowBackgroundColor, Color);}
    void setIconOverlayColor(const QColor& Color) {setIconColor(OverlayColor, Color);}
    void setIconArrowColor(const QColor& Color) {setIconColor(ArrowColor, Color);}
    void setIconShadowColor(const QColor& Color) {setIconColor(ShadowColor, Color);}

public:
	/**
	 * Creates an overlay cross for the given overlay
	 */
	CDockOverlayCross(CDockOverlay* overlay);

	/**
	 * Virtual destructor
	 */
	virtual ~CDockOverlayCross();

	/**
	 * Sets a certain icon color
	 */
	void setIconColor(eIconColor ColorIndex, const QColor& Color);

	/**
	 * Returns the icon color given by ColorIndex
	 */
	QColor iconColor(eIconColor ColorIndex) const;

	/**
	 * Returns the dock widget area depending on the current cursor location.
	 * The function checks, if the mouse cursor is inside of any drop indicator
	 * widget and returns the corresponding DockWidgetArea.
	 */
	DockWidgetArea cursorLocation() const;

	/**
	 * Sets up the overlay cross for the given overlay mode
	 */
	void setupOverlayCross(CDockOverlay::eMode Mode);

	/**
	 * Recreates the overlay icons.
	 */
	void updateOverlayIcons();

	/**
	 * Resets and updates the
	 */
	void reset();

	/**
	 * Updates the current position
	 */
	void updatePosition();

	/**
	 * A string with all icon colors to set.
	 * You can use this property to style the overly icon via CSS stylesheet
	 * file. The colors are set via a color identifier and a hex AARRGGBB value like
	 * in the example below.
	 * \code
	 * ads--CDockOverlayCross
	 * {
	 *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
	 * }
	 */
    void setIconColors(const QString& Colors);

protected:
	virtual void showEvent(QShowEvent* e) override;
	void setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets);
}; // CDockOverlayCross

} // namespace ads
#endif // DockOverlayH
