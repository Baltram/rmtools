TARGET  = tgaplugin
TEMPLATE = lib
CONFIG = qt plugin
VERSION = 0.0.1

QT += opengl \
      core

CONFIG += STATIC

HEADERS += tgaplugin/tgahandler.h

SOURCES += tgaplugin/tgaplugin.cpp \
           tgaplugin/tgahandler.cpp
