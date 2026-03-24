#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_IntellectImageShangBo

TEMPLATE = lib
INCLUDEPATH += ../include ../../include ../../include/lyUI ../../include/lyDock

DEFINES += LY_INTELLECTIMAGESHANGBO_LIBRARY

SOURCES += \
    LY_IntellectImageShangBo.cpp \
    LY_IntellectImageShangBoInterface.cpp \
    IIntellectImageShangBoActivator.cpp \
    IntellectImageShangBoDialog.cpp

HEADERS += \
    LY_IntellectImageShangBoInterface.h \
    LY_IntellectImageShangBo_global.h \
    IIntellectImageShangBoActivator.h \
    LY_IntellectImageShangBo.h \
    IntellectImageShangBoDialog.h

FORMS += \
    IntellectImageShangBoDialog.ui


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
LIBS += -llyCoreD -lGeometryD -llyUID -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lGeometry -llyUI -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

