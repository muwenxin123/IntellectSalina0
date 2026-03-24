#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_SystemMaintenanceService
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D

DEFINES += LY_SYSTEMMAINTENANCESERVICE_LIBRARY

SOURCES += lY_SystemMaintenanceService.cpp     lY_SystemMaintenanceServiceInterface.cpp \
    ISystemMaintenanceImpl.cpp \
    ISystemMaintenanceActivator.cpp

HEADERS += lY_SystemMaintenanceService.h    lY_SystemMaintenanceService_global.h     lY_SystemMaintenanceServiceInterface.h \
    ISystemMaintenanceImpl.h \
    ISystemMaintenanceActivator.h

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
LIBS += -llyCoreD -lmap3dD -lGeometryD -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lmap3d -lGeometry -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS += 

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
