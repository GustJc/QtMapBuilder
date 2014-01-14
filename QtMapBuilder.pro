#-------------------------------------------------
#
# Project created by QtCreator 2014-01-05T18:02:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtMapBuilder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tileseteditor.cpp \
    mapview.cpp

HEADERS  += mainwindow.h \
    tileseteditor.h \
    mapview.h \
    defines.h

FORMS    += mainwindow.ui \
    tileseteditor.ui

RESOURCES += \
    QtMapBuilder.qrc
