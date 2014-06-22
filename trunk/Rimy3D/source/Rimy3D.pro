QT += core \
      gui \
      network

TARGET = Rimy3D
TEMPLATE = app

CONFIG += STATIC

LIBS += ../../QImageIOPlugin/build/tgaplugin.lib
LIBS += ../../QImageIOPlugin/build/ddsplugin.lib
LIBS += ../../QImageIOPlugin/build/ximgplugin.lib

INCLUDEPATH += "$$[QT_INSTALL_PREFIX]/src/3rdparty/zlib" \
               "$$[QT_INSTALL_PREFIX]/../$$[QT_VERSION]/src/3rdparty/zlib"

include("../../mimicry/source/Mimicry.pri")
include("GLC_lib.pri")

SOURCES += glwidget.cpp \
           main.cpp \
           mainwindow.cpp \
           rimy3d.cpp \
           sceneinfo.cpp \
           searchpathsdialog.cpp \
           texturefinder.cpp \
           exportsettingsdialog.cpp \
           pluginsdialog.cpp \
           preferencesdialog.cpp \
           networkmanager.cpp \
           genomematerialdialog.cpp

HEADERS += glwidget.h \
           mainwindow.h \
           rimy3d.h \
           sceneinfo.h \
           searchpathsdialog.h \
           texturefinder.h \
           exportsettingsdialog.h \
           pluginsdialog.h \
           preferencesdialog.h \
           networkmanager.h \
           genomematerialdialog.h

FORMS += mainwindow.ui \
         searchpathsdialog.ui \
         exportsettingsdialog.ui \
         pluginsdialog.ui \
         preferencesdialog.ui \
         genomematerialdialog.ui

TRANSLATIONS = resources/translations/Rimy3D_de.ts

RESOURCES += resources/flags.qrc \
             resources/translations.qrc \
             resources/plugins.qrc

RC_FILE = resources/icon.rc
