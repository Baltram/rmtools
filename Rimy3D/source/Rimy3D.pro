QT += core \
      gui

TARGET = Rimy3D
TEMPLATE = app

include("../../mimicry/source/Mimicry.pri")
include("GLC_lib.pri")

SOURCES += main.cpp \
           mainwindow.cpp \
           rimy3d.cpp \
           glwidget.cpp \
           sceneinfo.cpp \
           texturefinder.cpp

HEADERS += mainwindow.h \
           rimy3d.h \
           glwidget.h \
           sceneinfo.h \
           texturefinder.h

FORMS += mainwindow.ui

TRANSLATIONS = resources/translations/Rimy3D_de.ts

RESOURCES += resources/flags.qrc \
             resources/translations.qrc

RC_FILE = resources/icon.rc
