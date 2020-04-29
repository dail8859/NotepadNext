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
/// \file   ElidingLabel.cpp
/// \author Uwe Kindler
/// \date   05.11.2018
/// \brief  Implementation of CElidingLabel
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ElidingLabel.h"
#include <QMouseEvent>


namespace ads
{
/**
 * Private data of public CClickableLabel
 */
struct ElidingLabelPrivate
{
	CElidingLabel* _this;
	Qt::TextElideMode ElideMode = Qt::ElideNone;
	QString Text;
	bool IsElided = false;

	ElidingLabelPrivate(CElidingLabel* _public) : _this(_public) {}

	void elideText(int Width);

	/**
	 * Convenience function to check if the
	 */
	bool isModeElideNone() const
	{
		return Qt::ElideNone == ElideMode;
	}
};


//============================================================================
void ElidingLabelPrivate::elideText(int Width)
{
	if (isModeElideNone())
	{
		return;
	}
    QFontMetrics fm = _this->fontMetrics();
    QString  str = fm.elidedText(Text, ElideMode, Width - _this->margin() * 2 - _this->indent());
    if (str == "…")
    {
    	str = Text.at(0);
    }
    bool WasElided = IsElided;
    IsElided = str != Text;
    if(IsElided != WasElided)
    {
        emit _this->elidedChanged(IsElided);
    }
    _this->QLabel::setText(str);
}


//============================================================================
CElidingLabel::CElidingLabel(QWidget* parent, Qt::WindowFlags f)
	: QLabel(parent, f),
	  d(new ElidingLabelPrivate(this))
{

}


//============================================================================
CElidingLabel::CElidingLabel(const QString& text, QWidget* parent, Qt::WindowFlags f)
	: QLabel(text, parent,f),
	  d(new ElidingLabelPrivate(this))
{
	d->Text = text;
	internal::setToolTip(this, text);
}


//============================================================================
CElidingLabel::~CElidingLabel()
{
	delete d;
}


//============================================================================
Qt::TextElideMode CElidingLabel::elideMode() const
{
	return d->ElideMode;
}


//============================================================================
void CElidingLabel::setElideMode(Qt::TextElideMode mode)
{
	d->ElideMode = mode;
	d->elideText(size().width());
}

//============================================================================
bool CElidingLabel::isElided() const
{
	return d->IsElided;
}


//============================================================================
void CElidingLabel::mouseReleaseEvent(QMouseEvent* event)
{
	Super::mouseReleaseEvent(event);
	if (event->button() != Qt::LeftButton)
	{
		return;
	}

	emit clicked();
}


//============================================================================
void CElidingLabel::mouseDoubleClickEvent( QMouseEvent *ev )
{
	Q_UNUSED(ev)
	emit doubleClicked();
	Super::mouseDoubleClickEvent(ev);
}


//============================================================================
void CElidingLabel::resizeEvent(QResizeEvent *event)
{
	if (!d->isModeElideNone())
	{
		d->elideText(event->size().width());
	}
    Super::resizeEvent(event);
}


//============================================================================
QSize CElidingLabel::minimumSizeHint() const
{
    if (pixmap() != nullptr || d->isModeElideNone())
    {
        return QLabel::minimumSizeHint();
    }
    const QFontMetrics  &fm = fontMetrics();
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        QSize size(fm.horizontalAdvance(d->Text.left(2) + "…"), fm.height());
    #else
        QSize size(fm.width(d->Text.left(2) + "…"), fm.height());
    #endif
    return size;
}


//============================================================================
QSize CElidingLabel::sizeHint() const
{
    if (pixmap() != nullptr || d->isModeElideNone())
    {
        return QLabel::sizeHint();
    }
    const QFontMetrics& fm = fontMetrics();
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        QSize size(fm.horizontalAdvance(d->Text), QLabel::sizeHint().height());
    #else
        QSize size(fm.width(d->Text), QLabel::sizeHint().height());
    #endif
	return size;
}


//============================================================================
void CElidingLabel::setText(const QString &text)
{
	d->Text = text;
	if (d->isModeElideNone())
	{
		Super::setText(text);
	}
	else
	{
		internal::setToolTip(this, text);
		d->elideText(this->size().width());
	}
}


//============================================================================
QString CElidingLabel::text() const
{
	return d->Text;
}
} // namespace QtLabb

//---------------------------------------------------------------------------
// EOF ClickableLabel.cpp
