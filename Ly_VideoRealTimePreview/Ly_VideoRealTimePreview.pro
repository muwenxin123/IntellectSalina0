QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ly_VideoRealTimePreview
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI

DEFINES += LY_VIDEOREALTIMEPREVIEW_LIBRARY

SOURCES += Ly_VideoRealTimePreview.cpp \
  Ly_VideoRealTimePreviewInterface.cpp \
    Ly_VideoRealTimePreviewWidget.cpp \
	IVideoRealTimePreviewActivator.CPP \
    IVideoRealTimePreviewActivator.cpp
	

HEADERS += Ly_VideoRealTimePreview.h \
  Ly_VideoRealTimePreview_global.h \
  Ly_VideoRealTimePreviewInterface.h \
    Ly_VideoRealTimePreviewWidget.h \
	IVideoRealTimePreviewActivator.h \
    IVideoRealTimePreviewActivator.h

win32 {
  LIBS += -L../../lib
  DESTDIR = ../../lib
# header.path=../../include
} else {
  LIBS += -L/usr/local/lib
  DESTDIR = /usr/local/lib
# header.path=/usr/local/include
}
DLLDESTDIR = ../../bin

CONFIG += debug_and_release
CONFIG(debug, debug|release) {
  LIBS += \
    -llyCoreD \
    -llyUID \
    -lmap3dD \
    -lGeometryD
} else {
  LIBS += \
    -llyCore \
    -llyUI \
    -lmap3d \
    -lGeometry
}

build_pass:CONFIG(debug, debug|release) {
  unix: TARGET = $$join(TARGET,,,D)#_debug
  else: TARGET = $$join(TARGET,,,D)
}

FORMS += \
    Ly_VideoRealTimePreviewWidget.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
