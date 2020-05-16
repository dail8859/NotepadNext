ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = AdvancedDockingSystemDemo
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

win32 {
QT += axcontainer
}

CONFIG += c++14
CONFIG += debug_and_release
DEFINES += QT_DEPRECATED_WARNINGS

adsBuildStatic {
    DEFINES += ADS_STATIC
}

SOURCES += \
	main.cpp \
	MainWindow.cpp \
	StatusDialog.cpp

HEADERS += \
	MainWindow.h \
	StatusDialog.h

FORMS += \
	mainwindow.ui \
	StatusDialog.ui
	
RESOURCES += demo.qrc


LIBS += -L$${ADS_OUT_ROOT}/lib

# Dependency: AdvancedDockingSystem (shared)
CONFIG(debug, debug|release){
    win32 {
        LIBS += -lqtadvanceddockingd
    }
    else:mac {
        LIBS += -lqtadvanceddocking_debug
    }
    else {
        LIBS += -lqtadvanceddocking
    }
}
else{
    LIBS += -lqtadvanceddocking
}

INCLUDEPATH += ../src
DEPENDPATH += ../src
