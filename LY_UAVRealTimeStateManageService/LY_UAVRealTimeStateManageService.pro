#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core
QT += widgets

TEMPLATE = lib
DEFINES += LY_UAVREALTIMESTATEMANAGESERVICE_LIBRARY

TARGET = LY_UAVRealTimeStateManageService

CONFIG += plugin

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += . ../../include ../../include/lyUI

LIBS += -L../../lib

SOURCES += \
    IUAVRealTimeStateManageActivator.cpp \
    IUAVRealTimeStateManageImpl.cpp \
    LY_UAVRealTimeStateManageServiceInterface.cpp \
    LY_UAVRealTimeStateManageService.cpp \


HEADERS += \
    IUAVRealTimeStateManageActivator.h \
    IUAVRealTimeStateManageImpl.h \
    LY_UAVRealTimeStateManageService.h \
    LY_UAVRealTimeStateManageServiceInterface.h \
    LY_UAVRealTimeStateManageService_global.h \
    LY_UAVRealTimeStateManageService.h \



win32 {
    DEFINES += CORE_EXPORT=__declspec(dllimport) GUI_EXPORT=__declspec(dllimport)
    LIBS += -L../../lib
    DESTDIR = ../../bin
#   header.path=../../include
} else {
    LIBS += -L/usr/local/lib
    DESTDIR = /usr/local/lib
#   header.path=/usr/local/include
}

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
LIBS += -llyCoreD -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

# Default rules for deployment.
#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target

