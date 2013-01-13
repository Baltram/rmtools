TARGET  = ximgplugin
TEMPLATE = lib
CONFIG = qt plugin
VERSION = 0.0.1

QT += opengl \
      core

CONFIG += STATIC

HEADERS += ximgplugin/ximghandler.h \
           ddsplugin/ddshandler.h

SOURCES += ximgplugin/ximgplugin.cpp \
           ximgplugin/ximghandler.cpp \
           ddsplugin/ddshandler.cpp
