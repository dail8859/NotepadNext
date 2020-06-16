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
/// \file   ads_globals.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QVariant>
#include <QPainter>
#include <QAbstractButton>
#include <QStyle>

#include "DockSplitter.h"
#include "DockManager.h"
#include "IconProvider.h"
#include "ads_globals.h"


namespace ads
{

namespace internal
{
//============================================================================
void replaceSplitterWidget(QSplitter* Splitter, QWidget* From, QWidget* To)
{
	int index = Splitter->indexOf(From);
	From->setParent(nullptr);
	Splitter->insertWidget(index, To);
}

//============================================================================
CDockInsertParam dockAreaInsertParameters(DockWidgetArea Area)
{
	switch (Area)
    {
	case TopDockWidgetArea: return CDockInsertParam(Qt::Vertical, false);
	case RightDockWidgetArea: return CDockInsertParam(Qt::Horizontal, true);
	case CenterDockWidgetArea:
	case BottomDockWidgetArea: return CDockInsertParam(Qt::Vertical, true);
	case LeftDockWidgetArea: return CDockInsertParam(Qt::Horizontal, false);
	default: CDockInsertParam(Qt::Vertical, false);
    } // switch (Area)

	return CDockInsertParam(Qt::Vertical, false);
}


//============================================================================
QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity)
{
	QPixmap TransparentPixmap(Source.size());
	TransparentPixmap.fill(Qt::transparent);
	QPainter p(&TransparentPixmap);
	p.setOpacity(Opacity);
	p.drawPixmap(0, 0, Source);
	return TransparentPixmap;
}


//============================================================================
void hideEmptyParentSplitters(CDockSplitter* Splitter)
{
	while (Splitter && Splitter->isVisible())
	{
		if (!Splitter->hasVisibleContent())
		{
			Splitter->hide();
		}
		Splitter = internal::findParent<CDockSplitter*>(Splitter);
	}
}


//============================================================================
void setButtonIcon(QAbstractButton* Button, QStyle::StandardPixmap StandarPixmap,
	ads::eIcon CustomIconId)
{
	// First we try to use custom icons if available
	QIcon Icon = CDockManager::iconProvider().customIcon(CustomIconId);
	if (!Icon.isNull())
	{
		Button->setIcon(Icon);
		return;
	}

#ifdef Q_OS_LINUX
	Button->setIcon(Button->style()->standardIcon(StandarPixmap));
#else
	// The standard icons does not look good on high DPI screens so we create
	// our own "standard" icon here.
	QPixmap normalPixmap = Button->style()->standardPixmap(StandarPixmap, 0, Button);
	Icon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
	Icon.addPixmap(normalPixmap, QIcon::Normal);
	Button->setIcon(Icon);
#endif
}


//============================================================================
void repolishStyle(QWidget* w, eRepolishChildOptions Options)
{
	if (!w)
	{
		return;
	}
	w->style()->unpolish(w);
	w->style()->polish(w);

	if (RepolishIgnoreChildren == Options)
	{
		return;
	}

	QList<QWidget*> Children = w->findChildren<QWidget*>(QString(),
		(RepolishDirectChildren == Options) ? Qt::FindDirectChildrenOnly: Qt::FindChildrenRecursively);
	for (auto Widget : Children)
	{
		Widget->style()->unpolish(Widget);
		Widget->style()->polish(Widget);
	}
}

} // namespace internal
} // namespace ads



//---------------------------------------------------------------------------
// EOF ads_globals.cpp
