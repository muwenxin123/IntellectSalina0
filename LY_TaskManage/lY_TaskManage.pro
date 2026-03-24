#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core gui charts
QT += positioning
QT += network
QT += svg

CONFIG += c++11

#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_TaskManage
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../3rdParty/include
INCLUDEPATH += . ../../include ../../include/lyUI

DEFINES += LY_TASKMANAGE_LIBRARY

SOURCES += lY_TaskManage.cpp     lY_TaskManageInterface.cpp \ 
    ITaskManagerActivator.cpp \
    CDlgEditLine.cpp \
    DrawVehicleIcon.cpp \
    CoordinateTranfer.cpp \
    CDlgPlay.cpp \
    UAVProtocol.cpp \
    CDlgMissionList.cpp \
    CDlgMissionEdit.cpp \
    CDlgMissionPlatformList.cpp \
    CDlgMissionPlatformEdit.cpp \
    CDlgMissionPlatformLoadList.cpp \
    CDlgMissionPlatformLoadEdit.cpp \
    CDlgAutoPoint.cpp \
    CDlgMissionTree.cpp \
    CDlgActionList.cpp \
    CDlgActionEdit.cpp \
    CDlgCodeSet_Load_Tree.cpp \
    CDlgCodeSet_Load_Edit.cpp \
    CDlgCodeSet_Load_Edit_Child.cpp \
    CDlgSensorManage.cpp \
    CDlgFireManage.cpp \
    CDlgCommunicateManage.cpp \
    CDlgMissionLineList.cpp \
    CDlgSensorManageEdit.cpp \
    CDlgFireManageEdit.cpp \
    CDlgMissionLinePointEdit.cpp \
    CDlgSetPlat.cpp \
    CButton_setPlat.cpp \
    delegate.cpp \
    CDlgTaskLineEdit.cpp \
    WidgetLineEdit.cpp \
    CDlgAddMark.cpp \
    CDlgMissionPlatformInfo.cpp \
    CDlgMarkSchemeList.cpp \
    CDlgMessageBox.cpp \ 
    CDlgMissionPlanningOverview.cpp \
    CDlgMissionAllocation.cpp \
    mutiComboBox.cpp \
    ThreadSavetoSql.cpp \
    CDlgMarkMaintain.cpp \
    CDlgAddMaintainMark.cpp \
    IResourceCarryingPlanDialog.cpp \
    ISAddCarryingPlanResDialog.cpp \
    XComboBox.cpp \
    CDlgPlaySpeedControl.cpp \
    CDlgForecastTendency.cpp \
    geometric.cpp

HEADERS += lY_TaskManage.h    lY_TaskManage_global.h     lY_TaskManageInterface.h \
    ITaskManagerActivator.h \
    CDlgEditLine.h \
    DrawVehicleIcon.h \
    LineEditHead.h \
    CommonHead.h \
    CommonStruct.h \
    CoordinateTranfer.h \
    CDlgPlay.h \
    UAVProtocol.h \
    CDlgMissionList.h \
    CDlgMissionEdit.h \
    CDlgMissionPlatformList.h \
    CDlgMissionPlatformEdit.h \
    CDlgMissionPlatformLoadList.h \
    CDlgMissionPlatformLoadEdit.h \
    CDlgAutoPoint.h \
    CDlgMissionTree.h \
    CDlgActionList.h \
    CDlgActionEdit.h \
    CDlgCodeSet_Load_Tree.h \
    CDlgCodeSet_Load_Edit.h \
    CDlgCodeSet_Load_Edit_Child.h \
    CDlgSensorManage.h \
    CDlgFireManage.h \
    CDlgCommunicateManage.h \
    CDlgMissionLineList.h \
    CDlgSensorManageEdit.h \
    CDlgFireManageEdit.h \
    CDlgMissionLinePointEdit.h \
    CDlgSetPlat.h \
    CButton_setPlat.h \
    delegate.h \
    CDlgTaskLineEdit.h \
    WidgetLineEdit.h \
    CDlgAddMark.h \
    CDlgMissionPlatformInfo.h \
    CDlgMarkSchemeList.h \
    CDlgMessageBox.h \ 
    CDlgMissionPlanningOverview.h \
    CDlgMissionAllocation.h \
    mutiComboBox.h \
    ThreadSavetoSql.h \
    MissionPlatType.h \
    CDlgMarkMaintain.h \
    CDlgAddMaintainMark.h \
    IResourceCarryingPlanDialog.h \
    ISAddCarryingPlanResDialog.h \
    XComboBox.h \
    CDlgPlaySpeedControl.h \
    CDlgForecastTendency.h \ 
    geometric.h

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
LIBS += -llyCoreD -lmap3dD -lGeometryD -llyUID -luser32 -lLyUcisPluginFrameWorkD -lLY_TinyXmlD
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI -luser32 -lLyUcisPluginFrameWork -lLY_TinyXml
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS +=    CDlgEditLine.ui \
    DrawVehicleIcon.ui \
    CDlgPlay.ui \
    CDlgMissionList.ui \
    CDlgMissionEdit.ui \
    CDlgMissionPlatformList.ui \
    CDlgMissionPlatformEdit.ui \
    CDlgMissionPlatformLoadList.ui \
    CDlgMissionPlatformLoadEdit.ui \
    CDlgAutoPoint.ui \
    CDlgMissionTree.ui \
    CDlgActionList.ui \
    CDlgActionEdit.ui \
    CDlgCodeSet_Load_Tree.ui \
    CDlgCodeSet_Load_Edit.ui \
    CDlgCodeSet_Load_Edit_Child.ui \
    CDlgSensorManage.ui \
    CDlgFireManage.ui \
    CDlgCommunicateManage.ui \
    CDlgMissionLineList.ui \
    CDlgSensorManageEdit.ui \
    CDlgFireManageEdit.ui \
    CDlgMissionLinePointEdit.ui \
    CDlgSetPlat.ui \
    CDlgTaskLineEdit.ui \
    WidgetLineEdit.ui  \
    CDlgAddMark.ui \
    CDlgMissionPlatformInfo.ui \
    CDlgMarkSchemeList.ui \
    CDlgMessageBox.ui \
    CDlgMissionPlanningOverview.ui \
    CDlgMissionAllocation.ui \
    CDlgMarkMaintain.ui \
    CDlgAddMaintainMark.ui \
    IResourceCarryingPlanDialog.ui \
    ISAddCarryingPlanResDialog.ui \
    CDlgPlaySpeedControl.ui \
    CDlgForecastTendency.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
include(lib_ly_ccip.pri)
