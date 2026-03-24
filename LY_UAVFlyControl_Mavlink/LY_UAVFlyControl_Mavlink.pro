#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core
QT += widgets
QT += axcontainer

QT += positioning

TEMPLATE = lib
DEFINES += LY_UAVFLYCONTROL_MAVLINK_LIBRARY

TARGET = LY_UAVFlyControl_Mavlink

CONFIG += plugin

CONFIG += force_debug_info

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QGC_APPLICATION_MAVLINK_VEHICLE_CONTROL_SEND_COMMAN_DMESSAGE

# DEFINES += QGC_APPLICATION_MAVLINK_VEHICLE_CONTROL_SEND_COMMAN_DMESSAGE

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += . ../../include ../../include/lyUI ../../include/lyDock

LIBS += -L../../lib

SOURCES += \
    IUAVFlyControl_MavlinkActivator.cpp \
    LY_UAVFlyControl_Mavlink.cpp \
    LY_UAVFlyControl_MavlinkInterface.cpp \
    ApplicationSetting/UAVFlyManage_MavlinkApplicationSetting.cpp \
    FlyControl/UAVSelectDialog.cpp \
    FlyControl/FlowLayout.cpp \
    FlyControl/UAVFlyManage_MavlinkDialog.cpp \
    FlyControl/UAVPushButton.cpp \
    TaskManage/NumberLineEditDelegate.cpp \
    TaskManage/MavlinkMissionManagerMissionCommandList.cpp \
    TaskManage/UAVFlyManage_MavlinkMissionManagerOpenFileDialog.cpp \
    TaskManage/UAVFlyManage_MavlinkTaskManageDialog.cpp \
    TaskManage/UAVFlyManage_MavlinkMissionManagerDialog.cpp \
    VehicleManage/Controls/UAVFlyManage_MavlinkVehicleControlsDialog.cpp \
    VehicleManage/StatusInfo/UAVFlyManage_MavlinkVehicleStatusInfoDialog.cpp \
    VehicleManage/UAVFlyManage_MavlinkVehicleDialog.cpp \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageEditDialog.cpp \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageDialog.cpp \
    VehicleManage/GuidedGoto/UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.cpp \
    VehicleManage/UAVFlyManage_MavlinkVehicleManageDialog.cpp \
    UAVFlyManage_MavlinkMainDialog.cpp \
    UAVFlyManage_MavlinkMessageIndicatorDialog.cpp


HEADERS += \
    IUAVFlyControl_MavlinkActivator.h \
    LY_UAVFlyControl_Mavlink.h \
    LY_UAVFlyControl_MavlinkInterface.h \
    LY_UAVFlyControl_Mavlink_global.h \
    ApplicationSetting/UAVFlyManage_MavlinkApplicationSetting.h \
    FlyControl/UAVSelectDialog.h \
    FlyControl/FlowLayout.h \
    FlyControl/UAVFlyManage_MavlinkDialog.h \
    FlyControl/UAVPushButton.h \
    TaskManage/NumberLineEditDelegate.h \
    TaskManage/MavlinkMissionManagerMissionCommandList.h \
    TaskManage/UAVFlyManage_MavlinkMissionManagerOpenFileDialog.h \
    TaskManage/UAVFlyManage_MavlinkTaskManageDialog.h \
    TaskManage/UAVFlyManage_MavlinkMissionManagerDialog.h \
    VehicleManage/Controls/UAVFlyManage_MavlinkVehicleControlsDialog.h \
    VehicleManage/StatusInfo/UAVFlyManage_MavlinkVehicleStatusInfoDialog.h \
    VehicleManage/UAVFlyManage_MavlinkVehicleDialog.h \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageEditDialog.h \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageDialog.h \
    VehicleManage/GuidedGoto/UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.h \
    VehicleManage/UAVFlyManage_MavlinkVehicleManageDialog.h \
    UAVFlyManage_MavlinkMainDialog.h \
    UAVFlyManage_MavlinkMessageIndicatorDialog.h


#SOURCES += \
#    SwarmControlManageWeaponControl/ISwarmControlManageWeaponControlImpl.cpp \

#HEADERS += \
#    SwarmControlManageWeaponControl/ISwarmControlManageWeaponControlImpl.h \

win32 {
    DEFINES += CORE_EXPORT=__declspec(dllimport) GUI_EXPORT=__declspec(dllimport)
    LIBS += -L../../lib  -ldinput8 -ldxguid
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


# Default rules for deployment.
#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target

FORMS += \
    FlyControl/UAVFlyManage_MavlinkDialog.ui \
    FlyControl/UAVSelectDialog.ui \
    ApplicationSetting/UAVFlyManage_MavlinkApplicationSetting.ui \
    TaskManage/UAVFlyManage_MavlinkMissionManagerOpenFileDialog.ui \
    TaskManage/UAVFlyManage_MavlinkTaskManageDialog.ui \
    TaskManage/UAVFlyManage_MavlinkMissionManagerDialog.ui \
    VehicleManage/Controls/UAVFlyManage_MavlinkVehicleControlsDialog.ui \
    VehicleManage/StatusInfo/UAVFlyManage_MavlinkVehicleStatusInfoDialog.ui \
    VehicleManage/UAVFlyManage_MavlinkVehicleDialog.ui \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageEditDialog.ui \
    VehicleManage/Parameter/UAVFlyManage_MavlinkParameterManageDialog.ui \
    VehicleManage/GuidedGoto/UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.ui \
    VehicleManage/UAVFlyManage_MavlinkVehicleManageDialog.ui \
    UAVFlyManage_MavlinkMainDialog.ui \
    UAVFlyManage_MavlinkMessageIndicatorDialog.ui

RESOURCES += \
    TaskManage/taskmanager.qrc

include(groundcontrol/groundcontrol.pri)
