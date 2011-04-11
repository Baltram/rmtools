#-------------------------------------------------
#
# Project created by QtCreator 2010-12-19T17:50:18
#
#-------------------------------------------------

TRANSLATIONS = xmatEdit_de.ts

QT += core gui

DEFINES += HAS_GUID_LINE_EDIT=1 \
 RM_QTPROPERTYBROWSER=1

TARGET = xmatEdit
TEMPLATE = app

include("..\rmStuff\rmStuff.pri")

#
# The original QtPropertyBrowser solution is available at http://qt.gitorious.org/qt-solutions.
#

include("..\qtpropertybrowser\qtpropertybrowser.pri")

SOURCES += main.cpp \
    mainwindow.cpp \
    xmatedit.cpp

HEADERS += mainwindow.h \
    global.h \
    guidlineedit.h \
    xmatedit.h

FORMS += mainwindow.ui

RESOURCES += flags.qrc \
	translations.qrc
