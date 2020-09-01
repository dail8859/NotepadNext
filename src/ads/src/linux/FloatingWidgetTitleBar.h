#ifndef FLOATINGWIDGETTITLEBAR_H
#define FLOATINGWIDGETTITLEBAR_H
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
/// \file   FloatingWidgetTitleBar.h
/// \author Uwe Kindler
/// \date   13.05.2019
/// \brief  Declaration of CFloatingWidgetTitleBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>
#include <QIcon>

namespace ads
{
class CFloatingDockContainer;

struct FloatingWidgetTitleBarPrivate;


/**
 * Titlebar for floating widgets to capture non client are mouse events.
 * Linux does not support NonClieantArea mouse events like
 * QEvent::NonClientAreaMouseButtonPress. Because these events are required
 * for the docking system to work properly, we use our own titlebar here to
 * capture the required mouse events.
 */
class CFloatingWidgetTitleBar : public QFrame
{
	Q_OBJECT
    Q_PROPERTY(QIcon maximizeIcon READ maximizeIcon WRITE setMaximizeIcon)
    Q_PROPERTY(QIcon normalIcon READ normalIcon WRITE setNormalIcon)
private:
	FloatingWidgetTitleBarPrivate *d; ///< private data (pimpl)

protected:
	virtual void mousePressEvent(QMouseEvent *ev) override;
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;
	virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    void setMaximizeIcon(const QIcon& Icon);
    QIcon maximizeIcon() const;
    void setNormalIcon(const QIcon& Icon);
    QIcon normalIcon() const;

public:
	using Super = QWidget;
	explicit CFloatingWidgetTitleBar(CFloatingDockContainer *parent = nullptr);

	/**
	 * Virtual Destructor
	 */
	virtual ~CFloatingWidgetTitleBar();

	/**
	 * Enables / disables the window close button.
	 */
	void enableCloseButton(bool Enable);

	/**
	 * Sets the window title, that means, the text of the internal tile label.
	 */
	void setTitle(const QString &Text);

    /**
     * Update stylesheet style if a property changes
     */
    void updateStyle();

	/**
	 * Change the maximize button icon according to current windows state
	 */
    void setMaximizedIcon(bool maximized);

signals:
	/**
	 * This signal is emitted, if the close button is clicked.
	 */
	void closeRequested();

    /**
    * This signal is emitted, if the maximize button is clicked.
    */
    void maximizeRequested();
};
} // namespace ads
#endif // FLOATINGWIDGETTITLEBAR_H
