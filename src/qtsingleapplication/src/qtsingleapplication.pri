TEMPLATE = lib
CONFIG += lib_bundle
CONFIG += staticlib
CONFIG -= debug_and_release debug_and_release_target

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += network
QT += widgets

SOURCES += $$PWD/qtsingleapplication.cpp $$PWD/qtlocalpeer.cpp
HEADERS += $$PWD/qtsingleapplication.h $$PWD/qtlocalpeer.h

