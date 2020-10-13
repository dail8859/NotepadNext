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

#ifdef Q_OS_LINUX
#include <QX11Info>
#include <QSettings>
#include <QFile>
#endif


#include <QApplication>

namespace ads
{

namespace internal
{
#ifdef Q_OS_LINUX
static QString _window_manager;
static QHash<QString, xcb_atom_t> _xcb_atom_cache;


//============================================================================
xcb_atom_t xcb_get_atom(const char *name)
{
	if (!QX11Info::isPlatformX11())
	{
		return XCB_ATOM_NONE;
	}
	auto key = QString(name);
	if(_xcb_atom_cache.contains(key))
	{
		return _xcb_atom_cache[key];
	}
	xcb_connection_t *connection = QX11Info::connection();
	xcb_intern_atom_cookie_t request = xcb_intern_atom(connection, 1, strlen(name), name);
	xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, request, NULL);
	if (!reply)
	{
		return XCB_ATOM_NONE;
	}
	xcb_atom_t atom = reply->atom;
	if(atom == XCB_ATOM_NONE)
	{
		ADS_PRINT("Unknown Atom response from XServer: " << name);
	}
	else
	{
		_xcb_atom_cache.insert(key, atom);
	}
	free(reply);
	return atom;
}


//============================================================================
void xcb_update_prop(bool set, WId window, const char *type, const char *prop, const char *prop2)
{
	auto connection = QX11Info::connection();
	xcb_atom_t type_atom = xcb_get_atom(type);
	xcb_atom_t prop_atom = xcb_get_atom(prop);
	xcb_client_message_event_t event;
	event.response_type = XCB_CLIENT_MESSAGE;
	event.format = 32;
	event.sequence = 0;
	event.window = window;
	event.type = type_atom;
	event.data.data32[0] = set ? 1 : 0;
	event.data.data32[1] = prop_atom;
	event.data.data32[2] = prop2 ? xcb_get_atom(prop2) : 0;
	event.data.data32[3] = 0;
	event.data.data32[4] = 0;

	xcb_send_event(connection, 0, window,
				   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_PROPERTY_CHANGE,
				   (const char *)&event);
	xcb_flush(connection);
}


//============================================================================
xcb_get_property_reply_t* _xcb_get_props(WId window, const char *type, unsigned int atom_type)
{
	if (!QX11Info::isPlatformX11())
	{
		return nullptr;
	}
	xcb_connection_t *connection = QX11Info::connection();
	xcb_atom_t type_atom = xcb_get_atom(type);
	if (type_atom == XCB_ATOM_NONE)
	{
		return nullptr;
	}
	xcb_get_property_cookie_t request = xcb_get_property_unchecked(connection, 0, window, type_atom, atom_type, 0, 1024);
	xcb_get_property_reply_t *reply = xcb_get_property_reply(connection, request, nullptr);
	if(reply && reply->type != atom_type)
	{
		ADS_PRINT("ATOM TYPE MISMATCH (" << type <<"). Expected: " << atom_type << "  but got " << reply->type);
		free(reply);
		return nullptr;
	}
	return reply;
}


//============================================================================
template <typename T>
void xcb_get_prop_list(WId window, const char *type, QVector<T> &ret, unsigned int atom_type)
{
	xcb_get_property_reply_t *reply = _xcb_get_props(window, type, atom_type);
	if (reply && reply->format == 32 && reply->type == atom_type && reply->value_len > 0)
	{
		const xcb_atom_t *data = static_cast<const T *>(xcb_get_property_value(reply));
		ret.resize(reply->value_len);
		memcpy((void *)&ret.first(), (void *)data, reply->value_len * sizeof(T));
	}
	free(reply);
}


//============================================================================
QString xcb_get_prop_string(WId window, const char *type)
{
	QString ret;
	// try utf8 first
	xcb_atom_t utf_atom = xcb_get_atom("UTF8_STRING");
	if(utf_atom != XCB_ATOM_NONE)
	{
		xcb_get_property_reply_t *reply = _xcb_get_props(window, type, utf_atom);
		if (reply && reply->format == 8 && reply->type == utf_atom)
		{
			const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
			ret = QString::fromUtf8(value, xcb_get_property_value_length(reply));
			free(reply);
			return ret;
		}
		free(reply);
	}
	// Fall back to XCB_ATOM_STRING
	xcb_get_property_reply_t *reply = _xcb_get_props(window, type, XCB_ATOM_STRING);
	if (reply && reply->format == 8 && reply->type == XCB_ATOM_STRING)
	{
		const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
		ret = QString::fromLatin1(value, xcb_get_property_value_length(reply));
	}
	free(reply);
	return ret;
}


//============================================================================
bool xcb_dump_props(WId window, const char *type)
{
	QVector<xcb_atom_t> atoms;
	xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
	qDebug() << "\n\n!!!" << type << "  -  " << atoms.length();
	xcb_connection_t *connection = QX11Info::connection();
	for (auto atom : atoms)
	{
		auto foo = xcb_get_atom_name(connection, atom);
		auto bar = xcb_get_atom_name_reply(connection, foo, nullptr);
		qDebug() << "\t" << xcb_get_atom_name_name(bar);
		free(bar);
	}
	return true;
}


//============================================================================
void xcb_add_prop(bool state, WId window, const char *type, const char *prop)
{
	if (!QX11Info::isPlatformX11())
	{
		return;
	}
	xcb_atom_t prop_atom = xcb_get_atom(prop);
	xcb_atom_t type_atom = xcb_get_atom(type);
	if (prop_atom == XCB_ATOM_NONE || type_atom == XCB_ATOM_NONE)
	{
		return;
	}
	QVector<xcb_atom_t> atoms;
	xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
	int index = atoms.indexOf(prop_atom);
	if (state && index == -1)
	{
		atoms.push_back(prop_atom);
	}
	else if (!state && index >= 0)
	{
		atoms.remove(index);
	}
	xcb_connection_t *connection = QX11Info::connection();
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, type_atom, XCB_ATOM_ATOM, 32, atoms.count(), atoms.constData());
	xcb_flush(connection);
}


//============================================================================
QString detectWindowManagerX11()
{
	// Tries to detect the windowmanager via X11.
	// See: https://specifications.freedesktop.org/wm-spec/1.3/ar01s03.html#idm46018259946000
	if (!QX11Info::isPlatformX11())
	{
		return "UNKNOWN";
	}
	xcb_connection_t *connection = QX11Info::connection();
	xcb_screen_t *first_screen = xcb_setup_roots_iterator (xcb_get_setup (connection)).data;
	if(!first_screen)
	{
		ADS_PRINT("No screen found via XCB.");
		return "UNKNOWN";
	}
	// Get supporting window ()
	xcb_window_t root = first_screen->root;
	xcb_window_t support_win = 0;
	QVector<xcb_window_t> sup_windows;
	xcb_get_prop_list(root, "_NET_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_WINDOW);
	if(sup_windows.length() == 0)
	{
		// This doesn't seem to be in use anymore, but wmctrl does the same so lets play safe.
		// Both XCB_ATOM_CARDINAL and XCB_ATOM_WINDOW break down to a uint32_t, so reusing sup_windows should be fine.
		xcb_get_prop_list(root, "_WIN_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_CARDINAL);
	}
	if(sup_windows.length() == 0)
	{
		ADS_PRINT("Failed to get the supporting window on non EWMH comform WM.");
		return "UNKNOWN";
	}
	support_win = sup_windows[0];
	QString ret = xcb_get_prop_string(support_win, "_NET_WM_NAME");
	if(ret.length() == 0)
	{
		ADS_PRINT("Empty WM name occured.");
		return "UNKNOWN";
	}
	return ret;
}

//============================================================================
QString windowManager()
{
	if(_window_manager.length() == 0)
	{
		_window_manager = detectWindowManagerX11();
	}
	return _window_manager;
}
#endif


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
