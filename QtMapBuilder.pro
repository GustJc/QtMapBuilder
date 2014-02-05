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
    globals.cpp \
    scriptselectorwidget.cpp \
    luamanager.cpp \
    resourcemanager.cpp \
    scriptmainwindow.cpp

HEADERS  += mainwindow.h \
    tileseteditor.h \
    mapview.h \
    defines.h \
    entityadddialog.h \
    entity.h \
    globals.h \
    scriptselectorwidget.h \
    luamanager.h \
    resourcemanager.h \
    scriptmainwindow.h

FORMS    += mainwindow.ui \
    tileseteditor.ui \
    entityadddialog.ui

RESOURCES += \
    QtMapBuilder.qrc

unix|win32: LIBS += -LC:\Library\Lua\lua-5.1.4\lib -llua514

INCLUDEPATH += C:\Library\Lua\lua-5.1.4\include
DEPENDPATH += C:\Library\Lua\lua-5.1.4\include

QMAKE_CXXFLAGS += -isystem C:\Library\Lua\boost_1_45_0
INCLUDEPATH += -isystem C:\Library\Lua\boost_1_45_0
DEPENDPATH += C:\Library\Lua\boost_1_45_0

win32: LIBS += -LC:\Library\Lua\luabind-0.9.1\stage -lLuabind-Static

QMAKE_CXXFLAGS += -isystem C:\Library\Lua\luabind-0.9.1
INCLUDEPATH += -isystem C:\Library\Lua\luabind-0.9.1
DEPENDPATH += C:\Library\Lua\luabind-0.9.1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../Library/QScintilla-gpl-2.8/dist/lib/ -lqscintilla2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../Library/QScintilla-gpl-2.8/dist/lib/ -lqscintilla2d

INCLUDEPATH += $$PWD/../../../../../../../Library/QScintilla-gpl-2.8/dist/include
DEPENDPATH += $$PWD/../../../../../../../Library/QScintilla-gpl-2.8/dist/include
