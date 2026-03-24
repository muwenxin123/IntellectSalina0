#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui
QT 		 += sql
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_ScenariosReplay
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../include/lyUI ../../include/lyDock

DEFINES += LY_SCENARIOSREPLAY_LIBRARY

LIBS += -L../../lib

SOURCES += LY_ScenariosReplay.cpp     LY_ScenariosReplayInterface.cpp \
    ScenarioReplayDialog.cpp \
    IScenariosReplayActivator.cpp \
    IScenariosReplayThread.cpp \
    ISReplayViewModel.cpp \
    RangeSlider.cpp \
    H264File.cpp \
    ISSendVideoThread.cpp

HEADERS += LY_ScenariosReplay.h    LY_ScenariosReplay_global.h     LY_ScenariosReplayInterface.h \
    ScenarioReplayDialog.h \
    IScenariosReplayActivator.h \
    IScenariosReplayThread.h \
    ISReplayViewModel.h \
    RangeSlider.h \
    H264File.h \
    ISSendVideoThread.h

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
LIBS += -llyCoreD -lmap3dD -lGeometryD -llyUID -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS += \         
    ScenarioReplayDialog.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
