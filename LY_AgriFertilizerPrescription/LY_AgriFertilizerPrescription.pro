QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_AgriFertilizerPrescription
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI


DEFINES += LY_AgriFertilizerPrescription_LIBRARY

SOURCES += \
    LY_AgriFertilizerPrescription.cpp \
    LY_AgriFertilizerPrescriptionActivator.cpp \
    LY_AgriFertilizerPrescriptionInterface.cpp \
    ly_agrifertilizerpreswidget.cpp \
    ly_agrifertilizepescviewmodel.cpp \
    ly_agrifertilizerpresviewmodel.cpp


HEADERS += \
    LY_AgriFertilizerPrescription.h \
    LY_AgriFertilizerPrescription_global.h \
    LY_AgriFertilizerPrescriptionActivator.h \
    LY_AgriFertilizerPrescriptionInterface.h \
    ly_agrifertilizerpreswidget.h \
    ly_agrifertilizepescviewmodel.h \
    ly_agrifertilizerpresviewmodel.h



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

#FORMS += \
#    windwidget.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}

FORMS += \
    ly_ploughheaderwidget.ui \
    ly_agrifertilizerpreswidget.ui
