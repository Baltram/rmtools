#-------------------------------------------------
#
# Project created by QtCreator 2011-01-27T16:38:27
#
#-------------------------------------------------

QT += core gui

TARGET = obj2xmac
TEMPLATE = app

TRANSLATIONS = obj2xmac_de.ts

include("..\rmStuff\rmStuff.pri")

SOURCES += main.cpp\
	mainwindow.cpp \
    obj2xmac.cpp \
    shortinstruction.cpp

HEADERS += mainwindow.hd \
    global.h \
    mainwindow.h \
    obj2xmac.h \
    shortinstruction.h

FORMS += mainwindow.ui \
    shortinstruction.ui

RESOURCES += flags.qrc \
    translations.qrc
