QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_AmmunitionManageService
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI

DEFINES += LY_AMMUNITIONMANAGESERVICE_LIBRARY

SOURCES += LY_AmmunitionManageService.cpp \
  LY_AmmunitionManageServiceInterface.cpp \
    IAmmunitionManageImpl.cpp

HEADERS += LY_AmmunitionManageService.h \
  LY_AmmunitionManageService_global.h \
  LY_AmmunitionManageServiceInterface.h \
    IAmmunitionManageImpl.h

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
    -lGeometryD \
    -lLyUcisPluginFrameWorkD
} else {
  LIBS += \
    -llyCore \
    -llyUI \
    -lmap3d \
    -lGeometry \
    -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
  unix: TARGET = $$join(TARGET,,,D)#_debug
  else: TARGET = $$join(TARGET,,,D)
}

FORMS +=

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
