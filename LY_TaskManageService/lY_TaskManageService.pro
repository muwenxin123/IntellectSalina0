#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui sql
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_TaskManageService
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D

DEFINES += LY_TASKMANAGESERVICE_LIBRARY

SOURCES += lY_TaskManageService.cpp     lY_TaskManageServiceInterface.cpp \
    ITaskManageImpl.cpp

HEADERS += lY_TaskManageService.h    lY_TaskManageService_global.h     lY_TaskManageServiceInterface.h \
    ITaskManageImpl.h

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
LIBS += -llyCoreD -lmap3dD -lGeometryD -llyUID -lLyUcisPluginFrameWorkD -lLY_TinyXmlD
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI -lLyUcisPluginFrameWork -lLY_TinyXml
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS +=

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
