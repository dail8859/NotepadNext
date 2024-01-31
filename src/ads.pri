# This file is part of Notepad Next.
# Copyright 2020 Justin Dailey
#
# Notepad Next is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Notepad Next is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.

QT += core gui widgets

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
		QMAKE_CXXFLAGS += /utf-8
	}
}

RESOURCES += $$PWD/ads/src/ads.qrc

HEADERS += \
    $$PWD/ads/src/ads_globals.h \
    $$PWD/ads/src/DockAreaWidget.h \
    $$PWD/ads/src/DockAreaTabBar.h \
    $$PWD/ads/src/DockContainerWidget.h \
    $$PWD/ads/src/DockManager.h \
    $$PWD/ads/src/DockWidget.h \
    $$PWD/ads/src/DockWidgetTab.h \
    $$PWD/ads/src/DockingStateReader.h \
    $$PWD/ads/src/FloatingDockContainer.h \
    $$PWD/ads/src/FloatingDragPreview.h \
    $$PWD/ads/src/DockOverlay.h \
    $$PWD/ads/src/DockSplitter.h \
    $$PWD/ads/src/DockAreaTitleBar_p.h \
    $$PWD/ads/src/DockAreaTitleBar.h \
    $$PWD/ads/src/ElidingLabel.h \
    $$PWD/ads/src/IconProvider.h \
    $$PWD/ads/src/DockComponentsFactory.h  \
    $$PWD/ads/src/DockFocusController.h \
    $$PWD/ads/src/AutoHideDockContainer.h \
    $$PWD/ads/src/AutoHideSideBar.h \
    $$PWD/ads/src/AutoHideTab.h \
    $$PWD/ads/src/PushButton.h \
    $$PWD/ads/src/ResizeHandle.h


SOURCES += \
    $$PWD/ads/src/ads_globals.cpp \
    $$PWD/ads/src/DockAreaWidget.cpp \
    $$PWD/ads/src/DockAreaTabBar.cpp \
    $$PWD/ads/src/DockContainerWidget.cpp \
    $$PWD/ads/src/DockManager.cpp \
    $$PWD/ads/src/DockWidget.cpp \
    $$PWD/ads/src/DockingStateReader.cpp \
    $$PWD/ads/src/DockWidgetTab.cpp \
    $$PWD/ads/src/FloatingDockContainer.cpp \
    $$PWD/ads/src/FloatingDragPreview.cpp \
    $$PWD/ads/src/DockOverlay.cpp \
    $$PWD/ads/src/DockSplitter.cpp \
    $$PWD/ads/src/DockAreaTitleBar.cpp \
    $$PWD/ads/src/ElidingLabel.cpp \
    $$PWD/ads/src/IconProvider.cpp \
    $$PWD/ads/src/DockComponentsFactory.cpp \
    $$PWD/ads/src/DockFocusController.cpp \
    $$PWD/ads/src/AutoHideDockContainer.cpp \
    $$PWD/ads/src/AutoHideSideBar.cpp \
    $$PWD/ads/src/AutoHideTab.cpp \
    $$PWD/ads/src/PushButton.cpp \
    $$PWD/ads/src/ResizeHandle.cpp


unix:!macx {
	HEADERS += $$PWD/ads/src/linux/FloatingWidgetTitleBar.h
	SOURCES += $$PWD/ads/src/linux/FloatingWidgetTitleBar.cpp
	LIBS += -lxcb
	QT += gui-private
}


INCLUDEPATH += $$PWD/ads/src/
