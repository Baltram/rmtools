TARGET  = ddsplugin
TEMPLATE = lib
CONFIG = qt plugin
VERSION = 0.0.1

QT += opengl \
      core

CONFIG += STATIC

HEADERS += ddsplugin/ddshandler.h

SOURCES += ddsplugin/ddsplugin.cpp \
           ddsplugin/ddshandler.cpp
