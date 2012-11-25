QT += core \
      gui

TARGET = Rimy3D
TEMPLATE = app

include("../../mimicry/source/Mimicry.pri")
include("GLC_lib.pri")

SOURCES += glwidget.cpp \
           main.cpp \
           mainwindow.cpp \
           rimy3d.cpp \
           sceneinfo.cpp \
           searchpathsdialog.cpp \
           texturefinder.cpp \
           exportsettingsdialog.cpp

HEADERS += glwidget.h \
           mainwindow.h \
           rimy3d.h \
           sceneinfo.h \
           searchpathsdialog.h \
           texturefinder.h \
           exportsettingsdialog.h

FORMS += mainwindow.ui \
         searchpathsdialog.ui \
         exportsettingsdialog.ui

TRANSLATIONS = resources/translations/Rimy3D_de.ts

RESOURCES += resources/flags.qrc \
             resources/translations.qrc

RC_FILE = resources/icon.rc
