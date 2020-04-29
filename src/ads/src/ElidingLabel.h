#ifndef ElidingLabelH
#define ElidingLabelH
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
/// \file   ElidingLabel.h
/// \author Uwe Kindler
/// \date   05.11.2018
/// \brief  Declaration of CElidingLabel
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QLabel>

#include "ads_globals.h"

namespace ads
{
struct ElidingLabelPrivate;

/**
 * A QLabel that supports eliding text.
 * Because the functions setText() and text() are no virtual functions setting
 * and reading the text via a pointer to the base class QLabel does not work
 * properly
 */
class ADS_EXPORT CElidingLabel : public QLabel
{
	Q_OBJECT
private:
	ElidingLabelPrivate* d;
	friend struct ElidingLabelPrivate;

protected:
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void resizeEvent( QResizeEvent *event ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent *ev ) override;

public:
    using Super = QLabel;

	CElidingLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
	CElidingLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~CElidingLabel();

	/**
	 * Returns the text elide mode.
	 * The default mode is ElideNone
	 */
	Qt::TextElideMode elideMode() const;

	/**
	 * Sets the text elide mode
	 */
	void setElideMode(Qt::TextElideMode mode);

	/**
	 * This function indicates whether the text on this label is currently elided
	 */
	bool isElided() const;

public: // reimplements QLabel ----------------------------------------------
	virtual QSize minimumSizeHint() const override;
	virtual QSize sizeHint() const override;
	void setText(const QString &text);
	QString text() const;

signals:
	/**
	 * This signal is emitted if the user clicks on the label (i.e. pressed
	 * down then released while the mouse cursor is inside the label)
	 */
	void clicked();

	/**
	 * This signal is emitted if the user does a double click on the label
	 */
	void doubleClicked();
    
    /**
	 * This signal is emitted when isElided() state of this label is changed
	 */
	void elidedChanged(bool elided);
}; //class CElidingLabel

} // namespace QtLabb

//---------------------------------------------------------------------------
#endif // ElidingLabelH
