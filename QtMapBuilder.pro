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
    mapview.cpp \
    entityadddialog.cpp \
    entity.cpp \
    globals.cpp

HEADERS  += mainwindow.h \
    tileseteditor.h \
    mapview.h \
    defines.h \
    entityadddialog.h \
    entity.h \
    globals.h

FORMS    += mainwindow.ui \
    tileseteditor.ui \
    entityadddialog.ui

RESOURCES += \
    QtMapBuilder.qrc
