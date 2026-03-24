QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_ZoneManager
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI

DEFINES += LY_ZONEMANAGER_LIBRARY

SOURCES += LY_ZoneManager.cpp \
  LY_ZoneManagerInterface.cpp \
    ZoneAdditionDrawWidget.cpp \
    zoneadditiondrawwidget.cpp \
    CDlgZoneParameterEdit.cpp \
    IZoneManagerActivator.cpp \
    CDlgZoneRuleManage.cpp \
    CDlgZoneFileManage.cpp \
    IZoneTypeMatchDialog.cpp \
    ICDlgSet3DLableColor.cpp \
    CDlgZoneScale.cpp \
    DataResult.cpp

HEADERS += LY_ZoneManager.h \
  LY_ZoneManager_global.h \
  LY_ZoneManagerInterface.h \
    ZoneManageWidget.h \
    ZoneAdditionDrawWidget.h \
    CDlgZoneParameterEdit.h \
    IZoneManagerActivator.h \
    CDlgZoneRuleManage.h \
    CDlgZoneFileManage.h \
    IZoneTypeMatchDialog.h \
    ICDlgSet3DLableColor.h \
    CDlgZoneScale.h \
    DataResult.h

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
    ZoneManageWidget.ui \
    ZoneAdditionDrawWidget.ui \
    CDlgZoneParameterEdit.ui \
    CDlgZoneRuleManage.ui \
    CDlgZoneFileManage.ui \
    IZoneTypeMatchDialog.ui \
    ICDlgSet3DLableColor.ui \
    CDlgZoneScale.ui \
    DataResult.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
