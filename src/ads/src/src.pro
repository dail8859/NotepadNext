TARGET = ads
TEMPLATE = lib
CONFIG += lib_bundle
CONFIG += staticlib
QT += core gui widgets

include(../../Config.pri)

DEFINES += ADS_STATIC

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
	}
}

RESOURCES += ads.qrc

HEADERS += \
    ads_globals.h \
    DockAreaWidget.h \
    DockAreaTabBar.h \
    DockContainerWidget.h \
    DockManager.h \
    DockWidget.h \
    DockWidgetTab.h \ 
    DockingStateReader.h \
    FloatingDockContainer.h \
    FloatingDragPreview.h \
    DockOverlay.h \
    DockSplitter.h \
    DockAreaTitleBar.h \
    ElidingLabel.h \
    IconProvider.h \
    DockComponentsFactory.h 


SOURCES += \
    ads_globals.cpp \
    DockAreaWidget.cpp \
    DockAreaTabBar.cpp \
    DockContainerWidget.cpp \
    DockManager.cpp \
    DockWidget.cpp \
    DockingStateReader.cpp \
    DockWidgetTab.cpp \
    FloatingDockContainer.cpp \
    FloatingDragPreview.cpp \
    DockOverlay.cpp \
    DockSplitter.cpp \
    DockAreaTitleBar.cpp \
    ElidingLabel.cpp \
    IconProvider.cpp \
    DockComponentsFactory.cpp


unix {
    HEADERS += linux/FloatingWidgetTitleBar.h
    SOURCES += linux/FloatingWidgetTitleBar.cpp
}

DISTFILES +=
