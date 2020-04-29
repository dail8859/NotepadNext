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
#include "DockOverlay.h"

#include <QPointer>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QGridLayout>
#include <QCursor>
#include <QIcon>
#include <QLabel>
#include <QtGlobal>
#include <QDebug>
#include <QMap>
#include <QWindow>

#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"

#include <iostream>

namespace ads
{

/**
 * Private data class of CDockOverlay
 */
struct DockOverlayPrivate
{
	CDockOverlay* _this;
	DockWidgetAreas AllowedAreas = InvalidDockWidgetArea;
	CDockOverlayCross* Cross;
	QPointer<QWidget> TargetWidget;
	DockWidgetArea LastLocation = InvalidDockWidgetArea;
	bool DropPreviewEnabled = true;
	CDockOverlay::eMode Mode = CDockOverlay::ModeDockAreaOverlay;
	QRect DropAreaRect;

	/**
	 * Private data constructor
	 */
	DockOverlayPrivate(CDockOverlay* _public) : _this(_public) {}
};

/**
 * Private data of CDockOverlayCross class
 */
struct DockOverlayCrossPrivate
{
	CDockOverlayCross* _this;
	CDockOverlay::eMode Mode = CDockOverlay::ModeDockAreaOverlay;
	CDockOverlay* DockOverlay;
	QHash<DockWidgetArea, QWidget*> DropIndicatorWidgets;
	QGridLayout* GridLayout;
	QColor IconColors[5];
	bool UpdateRequired = false;
	double LastDevicePixelRatio = 0.1;

	/**
	 * Private data constructor
	 */
	DockOverlayCrossPrivate(CDockOverlayCross* _public) : _this(_public) {}

	/**
	 *
	 * @param area
	 * @return
	 */
	QPoint areaGridPosition(const DockWidgetArea area);


	/**
	 * Palette based default icon colors
	 */
	QColor defaultIconColor(CDockOverlayCross::eIconColor ColorIndex)
	{
		QPalette pal = _this->palette();
		switch (ColorIndex)
		{
		case CDockOverlayCross::FrameColor: return pal.color(QPalette::Active, QPalette::Highlight);
		case CDockOverlayCross::WindowBackgroundColor: return pal.color(QPalette::Active, QPalette::Base);
		case CDockOverlayCross::OverlayColor:
			 {
				 QColor Color = pal.color(QPalette::Active, QPalette::Highlight);
				 Color.setAlpha(64);
				 return Color;
			 }
			 break;

		case CDockOverlayCross::ArrowColor: return pal.color(QPalette::Active, QPalette::Base);
		case CDockOverlayCross::ShadowColor: return QColor(0, 0, 0, 64);
		default:
			return QColor();
		}

		return QColor();
	}

	/**
	 * Stylehseet based icon colors
	 */
	QColor iconColor(CDockOverlayCross::eIconColor ColorIndex)
	{
		QColor Color = IconColors[ColorIndex];
		if (!Color.isValid())
		{
			Color = defaultIconColor(ColorIndex);
			IconColors[ColorIndex] = Color;
		}
		return Color;
	}

    //============================================================================
    /**
     * Helper function that returns the drop indicator width depending on the
     * operating system
     */
    qreal dropIndicatiorWidth(QLabel* l) const
    {
    #ifdef Q_OS_LINUX
        Q_UNUSED(l)
        return 40;
    #else
        return static_cast<qreal>(l->fontMetrics().height()) * 3.f;
    #endif
    }


	//============================================================================
	QWidget* createDropIndicatorWidget(DockWidgetArea DockWidgetArea,
		CDockOverlay::eMode Mode)
	{
		QLabel* l = new QLabel();
		l->setObjectName("DockWidgetAreaLabel");

        const qreal metric = dropIndicatiorWidth(l);
		const QSizeF size(metric, metric);

		l->setPixmap(createHighDpiDropIndicatorPixmap(size, DockWidgetArea, Mode));
		l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		l->setAttribute(Qt::WA_TranslucentBackground);
		l->setProperty("dockWidgetArea", DockWidgetArea);
		return l;
	}

	//============================================================================
	void updateDropIndicatorIcon(QWidget* DropIndicatorWidget)
	{
		QLabel* l = qobject_cast<QLabel*>(DropIndicatorWidget);
        const qreal metric = dropIndicatiorWidth(l);
		const QSizeF size(metric, metric);

		int Area = l->property("dockWidgetArea").toInt();
		l->setPixmap(createHighDpiDropIndicatorPixmap(size, (DockWidgetArea)Area, Mode));
	}

	//============================================================================
	QPixmap createHighDpiDropIndicatorPixmap(const QSizeF& size, DockWidgetArea DockWidgetArea,
		CDockOverlay::eMode Mode)
	{
		QColor borderColor = iconColor(CDockOverlayCross::FrameColor);
		QColor backgroundColor = iconColor(CDockOverlayCross::WindowBackgroundColor);

#if QT_VERSION >= 0x050600
		double DevicePixelRatio = _this->window()->devicePixelRatioF();
#else
        double DevicePixelRatio = _this->window()->devicePixelRatio();
#endif
		QSizeF PixmapSize = size * DevicePixelRatio;
		QPixmap pm(PixmapSize.toSize());
		pm.fill(QColor(0, 0, 0, 0));

		QPainter p(&pm);
		QPen pen = p.pen();
		QRectF ShadowRect(pm.rect());
		QRectF baseRect;
		baseRect.setSize(ShadowRect.size() * 0.7);
		baseRect.moveCenter(ShadowRect.center());

		// Fill
		QColor ShadowColor = iconColor(CDockOverlayCross::ShadowColor);
		if (ShadowColor.alpha() == 255)
		{
			ShadowColor.setAlpha(64);
		}
		p.fillRect(ShadowRect, ShadowColor);

		// Drop area rect.
		p.save();
		QRectF areaRect;
		QLineF areaLine;
		QRectF nonAreaRect;
		switch (DockWidgetArea)
		{
			case TopDockWidgetArea:
				areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
				nonAreaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
				areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
				break;
			case RightDockWidgetArea:
				areaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
				nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
				areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
				break;
			case BottomDockWidgetArea:
				areaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
				nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
				areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
				break;
			case LeftDockWidgetArea:
				areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
				nonAreaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
				areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
				break;
			default:
				break;
		}

		QSizeF baseSize = baseRect.size();
		if (CDockOverlay::ModeContainerOverlay == Mode && DockWidgetArea != CenterDockWidgetArea)
		{
			baseRect = areaRect;
		}

		p.fillRect(baseRect, backgroundColor);
		if (areaRect.isValid())
		{
			pen = p.pen();
			pen.setColor(borderColor);
			QColor Color = iconColor(CDockOverlayCross::OverlayColor);
			if (Color.alpha() == 255)
			{
				Color.setAlpha(64);
			}
			p.setBrush(Color);
			p.setPen(Qt::NoPen);
			p.drawRect(areaRect);

			pen = p.pen();
			pen.setWidth(1);
			pen.setColor(borderColor);
			pen.setStyle(Qt::DashLine);
			p.setPen(pen);
			p.drawLine(areaLine);
		}
		p.restore();

		p.save();
		// Draw outer border
		pen = p.pen();
		pen.setColor(borderColor);
		pen.setWidth(1);
		p.setBrush(Qt::NoBrush);
		p.setPen(pen);
		p.drawRect(baseRect);

		// draw window title bar
		p.setBrush(borderColor);
		QRectF FrameRect(baseRect.topLeft(), QSizeF(baseRect.width(), baseSize.height() / 10));
		p.drawRect(FrameRect);
		p.restore();

		// Draw arrow for outer container drop indicators
		if (CDockOverlay::ModeContainerOverlay == Mode && DockWidgetArea != CenterDockWidgetArea)
		{
			QRectF ArrowRect;
			ArrowRect.setSize(baseSize);
			ArrowRect.setWidth(ArrowRect.width() / 4.6);
			ArrowRect.setHeight(ArrowRect.height() / 2);
			ArrowRect.moveCenter(QPointF(0, 0));
			QPolygonF Arrow;
			Arrow << ArrowRect.topLeft()
				<< QPointF( ArrowRect.right(),  ArrowRect.center().y())
				<< ArrowRect.bottomLeft();
			p.setPen(Qt::NoPen);
			p.setBrush(iconColor(CDockOverlayCross::ArrowColor));
			p.setRenderHint(QPainter::Antialiasing, true);
			p.translate(nonAreaRect.center().x(), nonAreaRect.center().y());

			switch (DockWidgetArea)
			{
			case TopDockWidgetArea:
				 p.rotate(-90);
				 break;
			case RightDockWidgetArea:
				 break;
			case BottomDockWidgetArea:
				 p.rotate(90);
				 break;
			case LeftDockWidgetArea:
				p.rotate(180);
				 break;
			default:
				 break;
			}

			p.drawPolygon(Arrow);
		}

		pm.setDevicePixelRatio(DevicePixelRatio);
		return pm;
	}

};


//============================================================================
CDockOverlay::CDockOverlay(QWidget* parent, eMode Mode) :
	QFrame(parent),
	d(new DockOverlayPrivate(this))
{
	d->Mode = Mode;
	d->Cross = new CDockOverlayCross(this);
#ifdef Q_OS_LINUX
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
	setWindowOpacity(1);
	setWindowTitle("DockOverlay");
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	d->Cross->setVisible(false);
	setVisible(false);
}


//============================================================================
CDockOverlay::~CDockOverlay()
{
	delete d;
}


//============================================================================
void CDockOverlay::setAllowedAreas(DockWidgetAreas areas)
{
	if (areas == d->AllowedAreas)
		return;
	d->AllowedAreas = areas;
	d->Cross->reset();
}


//============================================================================
DockWidgetAreas CDockOverlay::allowedAreas() const
{
	return d->AllowedAreas;
}


//============================================================================
DockWidgetArea CDockOverlay::dropAreaUnderCursor() const
{
	DockWidgetArea Result = d->Cross->cursorLocation();
	if (Result != InvalidDockWidgetArea)
	{
		return Result;
	}

	CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(d->TargetWidget.data());
	if (!DockArea)
	{
		return Result;
	}

	if (DockArea->allowedAreas().testFlag(CenterDockWidgetArea)
	 && !DockArea->titleBar()->isHidden()
	 && DockArea->titleBarGeometry().contains(DockArea->mapFromGlobal(QCursor::pos())))
	{
		return CenterDockWidgetArea;
	}

	return Result;
}


//============================================================================
DockWidgetArea CDockOverlay::visibleDropAreaUnderCursor() const
{
	if (isHidden() || !d->DropPreviewEnabled)
	{
		return InvalidDockWidgetArea;
	}
	else
	{
		return dropAreaUnderCursor();
	}
}


//============================================================================
DockWidgetArea CDockOverlay::showOverlay(QWidget* target)
{
	if (d->TargetWidget == target)
	{
		// Hint: We could update geometry of overlay here.
		DockWidgetArea da = dropAreaUnderCursor();
		if (da != d->LastLocation)
		{
			repaint();
			d->LastLocation = da;
		}
		return da;
	}

	d->TargetWidget = target;
	d->LastLocation = InvalidDockWidgetArea;

	// Move it over the target.
	resize(target->size());
	QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
	move(TopLeft);
	show();
	d->Cross->updatePosition();
	d->Cross->updateOverlayIcons();
	return dropAreaUnderCursor();
}


//============================================================================
void CDockOverlay::hideOverlay()
{
	hide();
	d->TargetWidget.clear();
	d->LastLocation = InvalidDockWidgetArea;
	d->DropAreaRect = QRect();
}


//============================================================================
void CDockOverlay::enableDropPreview(bool Enable)
{
	d->DropPreviewEnabled = Enable;
	update();
}


//============================================================================
bool CDockOverlay::dropPreviewEnabled() const
{
	return d->DropPreviewEnabled;
}


//============================================================================
void CDockOverlay::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	// Draw rect based on location
	if (!d->DropPreviewEnabled)
	{
		d->DropAreaRect = QRect();
		return;
	}

	QRect r = rect();
	const DockWidgetArea da = dropAreaUnderCursor();
	double Factor = (CDockOverlay::ModeContainerOverlay == d->Mode) ?
		3 : 2;

	switch (da)
	{
    case TopDockWidgetArea: r.setHeight(r.height() / Factor); break;
	case RightDockWidgetArea: r.setX(r.width() * (1 - 1 / Factor)); break;
	case BottomDockWidgetArea: r.setY(r.height() * (1 - 1 / Factor)); break;
	case LeftDockWidgetArea: r.setWidth(r.width() / Factor); break;
	case CenterDockWidgetArea: r = rect();break;
	default: return;
	}
	QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
    QPen Pen = painter.pen();
    Pen.setColor(Color.darker(120));
    Pen.setStyle(Qt::SolidLine);
    Pen.setWidth(1);
    Pen.setCosmetic(true);
    painter.setPen(Pen);
    Color = Color.lighter(130);
    Color.setAlpha(64);
    painter.setBrush(Color);
	painter.drawRect(r.adjusted(0, 0, -1, -1));
	d->DropAreaRect = r;
}


//============================================================================
QRect CDockOverlay::dropOverlayRect() const
{
	return d->DropAreaRect;
}


//============================================================================
void CDockOverlay::showEvent(QShowEvent* e)
{
	d->Cross->show();
	QFrame::showEvent(e);
}


//============================================================================
void CDockOverlay::hideEvent(QHideEvent* e)
{
	d->Cross->hide();
	QFrame::hideEvent(e);
}


//============================================================================
bool CDockOverlay::event(QEvent *e)
{
	bool Result = Super::event(e);
	if (e->type() == QEvent::Polish)
	{
		d->Cross->setupOverlayCross(d->Mode);
	}
	return Result;
}


//============================================================================
static int areaAlignment(const DockWidgetArea area)
{
	switch (area)
	{
		case TopDockWidgetArea: return (int) Qt::AlignHCenter | Qt::AlignBottom;
		case RightDockWidgetArea: return (int) Qt::AlignLeft | Qt::AlignVCenter;
		case BottomDockWidgetArea: return (int) Qt::AlignHCenter | Qt::AlignTop;
		case LeftDockWidgetArea: return (int) Qt::AlignRight | Qt::AlignVCenter;
		case CenterDockWidgetArea:  return (int) Qt::AlignCenter;
		default: return Qt::AlignCenter;
	}
}

//============================================================================
// DockOverlayCrossPrivate
//============================================================================
QPoint DockOverlayCrossPrivate::areaGridPosition(const DockWidgetArea area)
{
	if (CDockOverlay::ModeDockAreaOverlay == Mode)
	{
		switch (area)
		{
			case TopDockWidgetArea: return QPoint(1, 2);
			case RightDockWidgetArea: return QPoint(2, 3);
			case BottomDockWidgetArea: return QPoint(3, 2);
			case LeftDockWidgetArea: return QPoint(2, 1);
			case CenterDockWidgetArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
	else
	{
		switch (area)
		{
			case TopDockWidgetArea: return QPoint(0, 2);
			case RightDockWidgetArea: return QPoint(2, 4);
			case BottomDockWidgetArea: return QPoint(4, 2);
			case LeftDockWidgetArea: return QPoint(2, 0);
			case CenterDockWidgetArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
}


//============================================================================
CDockOverlayCross::CDockOverlayCross(CDockOverlay* overlay) :
	QWidget(overlay->parentWidget()),
	d(new DockOverlayCrossPrivate(this))
{
	d->DockOverlay = overlay;
#ifdef Q_OS_LINUX
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
	setWindowTitle("DockOverlayCross");
	setAttribute(Qt::WA_TranslucentBackground);

	d->GridLayout = new QGridLayout();
	d->GridLayout->setSpacing(0);
	setLayout(d->GridLayout);
}


//============================================================================
CDockOverlayCross::~CDockOverlayCross()
{
	delete d;
}


//============================================================================
void CDockOverlayCross::setupOverlayCross(CDockOverlay::eMode Mode)
{
	d->Mode = Mode;

	QHash<DockWidgetArea, QWidget*> areaWidgets;
	areaWidgets.insert(TopDockWidgetArea, d->createDropIndicatorWidget(TopDockWidgetArea, Mode));
	areaWidgets.insert(RightDockWidgetArea, d->createDropIndicatorWidget(RightDockWidgetArea, Mode));
	areaWidgets.insert(BottomDockWidgetArea, d->createDropIndicatorWidget(BottomDockWidgetArea, Mode));
	areaWidgets.insert(LeftDockWidgetArea, d->createDropIndicatorWidget(LeftDockWidgetArea, Mode));
	areaWidgets.insert(CenterDockWidgetArea, d->createDropIndicatorWidget(CenterDockWidgetArea, Mode));
#if QT_VERSION >= 0x050600
	d->LastDevicePixelRatio = devicePixelRatioF();
#else
    d->LastDevicePixelRatio = devicePixelRatio();
#endif
	setAreaWidgets(areaWidgets);
	d->UpdateRequired = false;
}


//============================================================================
void CDockOverlayCross::updateOverlayIcons()
{
	if (windowHandle()->devicePixelRatio() == d->LastDevicePixelRatio)
	{
		return;
	}

	for (auto Widget : d->DropIndicatorWidgets)
	{
		d->updateDropIndicatorIcon(Widget);
	}
#if QT_VERSION >= 0x050600
	d->LastDevicePixelRatio = devicePixelRatioF();
#else
    d->LastDevicePixelRatio = devicePixelRatio();
#endif
}


//============================================================================
void CDockOverlayCross::setIconColor(eIconColor ColorIndex, const QColor& Color)
{
	d->IconColors[ColorIndex] = Color;
	d->UpdateRequired = true;
}


//============================================================================
QColor CDockOverlayCross::iconColor(eIconColor ColorIndex) const
{
	return d->IconColors[ColorIndex];
}


//============================================================================
void CDockOverlayCross::setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets)
{
	// Delete old widgets.
	QMutableHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		QWidget* widget = i.value();
		d->GridLayout->removeWidget(widget);
		delete widget;
		i.remove();
	}

	// Insert new widgets into grid.
	d->DropIndicatorWidgets = widgets;
	QHashIterator<DockWidgetArea, QWidget*> i2(d->DropIndicatorWidgets);
	while (i2.hasNext())
	{
		i2.next();
		const DockWidgetArea area = i2.key();
		QWidget* widget = i2.value();
		QPoint p = d->areaGridPosition(area);
		d->GridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment) areaAlignment(area));
	}

	if (CDockOverlay::ModeDockAreaOverlay == d->Mode)
	{
		d->GridLayout->setContentsMargins(0, 0, 0, 0);
		d->GridLayout->setRowStretch(0, 1);
		d->GridLayout->setRowStretch(1, 0);
		d->GridLayout->setRowStretch(2, 0);
		d->GridLayout->setRowStretch(3, 0);
		d->GridLayout->setRowStretch(4, 1);

		d->GridLayout->setColumnStretch(0, 1);
		d->GridLayout->setColumnStretch(1, 0);
		d->GridLayout->setColumnStretch(2, 0);
		d->GridLayout->setColumnStretch(3, 0);
		d->GridLayout->setColumnStretch(4, 1);
	}
	else
	{
		d->GridLayout->setContentsMargins(4, 4, 4, 4);
		d->GridLayout->setRowStretch(0, 0);
		d->GridLayout->setRowStretch(1, 1);
		d->GridLayout->setRowStretch(2, 1);
		d->GridLayout->setRowStretch(3, 1);
		d->GridLayout->setRowStretch(4, 0);

		d->GridLayout->setColumnStretch(0, 0);
		d->GridLayout->setColumnStretch(1, 1);
		d->GridLayout->setColumnStretch(2, 1);
		d->GridLayout->setColumnStretch(3, 1);
		d->GridLayout->setColumnStretch(4, 0);
	}
	reset();
}


//============================================================================
DockWidgetArea CDockOverlayCross::cursorLocation() const
{
	const QPoint pos = mapFromGlobal(QCursor::pos());
	QHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		if (d->DockOverlay->allowedAreas().testFlag(i.key())
			&& i.value()
			&& i.value()->isVisible()
			&& i.value()->geometry().contains(pos))
		{
			return i.key();
		}
	}
	return InvalidDockWidgetArea;
}


//============================================================================
void CDockOverlayCross::showEvent(QShowEvent*)
{
	if (d->UpdateRequired)
	{
		setupOverlayCross(d->Mode);
	}
	this->updatePosition();
}


//============================================================================
void CDockOverlayCross::updatePosition()
{
	resize(d->DockOverlay->size());
	QPoint TopLeft = d->DockOverlay->pos();
	QPoint Offest((this->width() - d->DockOverlay->width()) / 2,
		(this->height() - d->DockOverlay->height()) / 2);
	QPoint CrossTopLeft = TopLeft - Offest;
	move(CrossTopLeft);
}


//============================================================================
void CDockOverlayCross::reset()
{
	QList<DockWidgetArea> allAreas;
	allAreas << TopDockWidgetArea << RightDockWidgetArea
		<< BottomDockWidgetArea << LeftDockWidgetArea << CenterDockWidgetArea;
	const DockWidgetAreas allowedAreas = d->DockOverlay->allowedAreas();

	// Update visibility of area widgets based on allowedAreas.
	for (int i = 0; i < allAreas.count(); ++i)
	{
		QPoint p = d->areaGridPosition(allAreas.at(i));
		QLayoutItem* item = d->GridLayout->itemAtPosition(p.x(), p.y());
		QWidget* w = nullptr;
		if (item && (w = item->widget()) != nullptr)
		{
			w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
		}
	}
}


//============================================================================
void CDockOverlayCross::setIconColors(const QString& Colors)
{
	static const QMap<QString, int> ColorCompenentStringMap{
		{"Frame", CDockOverlayCross::FrameColor},
		{"Background", CDockOverlayCross::WindowBackgroundColor},
		{"Overlay", CDockOverlayCross::OverlayColor},
		{"Arrow", CDockOverlayCross::ArrowColor},
		{"Shadow", CDockOverlayCross::ShadowColor}};

	auto ColorList = Colors.split(' ', QString::SkipEmptyParts);
	for (const auto& ColorListEntry : ColorList)
	{
		auto ComponentColor = ColorListEntry.split('=', QString::SkipEmptyParts);
		int Component = ColorCompenentStringMap.value(ComponentColor[0], -1);
		if (Component < 0)
		{
			continue;
		}
		d->IconColors[Component] = QColor(ComponentColor[1]);
	}

	d->UpdateRequired = true;
}

//============================================================================
QString CDockOverlayCross::iconColors() const
{
	return QString();
}



} // namespace ads
//----------------------------------------------------------------------------

