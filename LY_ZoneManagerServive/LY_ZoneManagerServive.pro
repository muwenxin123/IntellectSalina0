QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_ZoneManagerServive
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI

DEFINES += LY_ZONEMANAGERSERVIVE_LIBRARY

SOURCES += LY_ZoneManagerServive.cpp \
  LY_ZoneManagerServiveInterface.cpp \
  IZoneManagerImpl.cpp

HEADERS += LY_ZoneManagerServive.h \
  LY_ZoneManagerServive_global.h \
  LY_ZoneManagerServiveInterface.h \
  IZoneManagerImpl.h
  

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
  rightForm.ui \
  popform.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
