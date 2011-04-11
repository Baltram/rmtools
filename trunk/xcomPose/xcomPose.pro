#-------------------------------------------------
#
# Project created by QtCreator 2011-02-13T22:42:55
#
#-------------------------------------------------

QT += core gui

TARGET = xcomPose
TEMPLATE = app

TRANSLATIONS = xcomPose_de.ts

DEFINES += RM_PHYSX=1

include("..\rmStuff\rmStuff.pri")

#
# You can download PhysX SDK v2.8.1 from
# http://developer.download.nvidia.com/PhysX/2.8.1/PhysX_SDK_2.8.1_build_13_for_PC.msi
#

INCLUDEPATH += "..\NVIDIA PhysX SDK\v2.8.1\SDKs\Foundation\include" \
	"..\NVIDIA PhysX SDK\v2.8.1\SDKs\Physics\include" \
	"..\NVIDIA PhysX SDK\v2.8.1\SDKs\PhysXLoader\include" \
	"..\NVIDIA PhysX SDK\v2.8.1\SDKs\Cooking\include"

LIBS += "..\NVIDIA PhysX SDK\v2.8.1\SDKs\lib\Win32\PhysXLoader.lib" \
	"..\NVIDIA PhysX SDK\v2.8.1\SDKs\lib\Win32\NxCooking.lib"

SOURCES += main.cpp\
	mainwindow.cpp \
    xcompose.cpp \
    trianglemeshwidget.cpp \
    shapematcombobox.cpp \
    trianglemeshtabwidget.cpp \
    shortinstruction.cpp

HEADERS += mainwindow.h \
    global.h \
    xcompose.h \
    trianglemeshwidget.h \
    shapematcombobox.h \
    trianglemeshtabwidget.h \
    shortinstruction.h

FORMS += mainwindow.ui \
    shortinstruction.ui

RESOURCES += \
    translations.qrc \
    flags.qrc
