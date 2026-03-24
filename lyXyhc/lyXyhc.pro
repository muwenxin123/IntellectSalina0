#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += positioning
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lyXyhc
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../include/lyUI

DEFINES += LYXYHC_LIBRARY

SOURCES += lyXyhc.cpp     lyXyhcInterface.cpp TrackMgr.cpp \
    IXyhcActivator.cpp \
    UAVCurrentShotAiming.cpp \
    UAVNavigationArmShotAutoAiming.cpp \
    UAVNavigationArmShotUpdateCalBombLine.cpp \
    XygsZC_02_Process.cpp \
    cmdTrackCircleNum.cpp \
    cmd.cpp \
    cmdTrack.cpp \
    cmdFlight.cpp \
    cmdFlightTurn.cpp \
    cmdPlane.cpp \
    QColorMenu.cpp \
    QTrack.cpp \
    trackList.cpp \
    Synergy/CDlgPlannedSynergy.cpp \
    Synergy/CDlgZoneSynergy.cpp \
    Synergy/CDlgTimeSynergy.cpp \
    Synergy/CDlgInfoSynergy.cpp \
    Synergy/CDlgScanSynergy01.cpp \
    Synergy/CDlgScanSynergy02.cpp \
    Synergy/CDlgFireSynergy01.cpp \
    Synergy/CDlgFireSynergy02.cpp \
    CmdTakeBackControl.cpp \
    CDlgSystemMessageDistribute.cpp \
    SelectMarkDialog.cpp \
    SystemPlane_Timer.cpp 


HEADERS += lyXyhc.h    lyXyhc_global.h     lyXyhcInterface.h TrackMgr.h \
    IXyhcActivator.h \
    UAVCurrentShotAiming.h \
    UAVNavigationArmShotAutoAiming.h \
    UAVNavigationArmShotUpdateCalBombLine.h \
    XygsZC_02_Process.h \
    cmdTrackCircleNum.h \
    cmd.h \
    cmdTrack.h \
    cmdFlight.h \
    cmdFlightTurn.h \
    cmdPlane.h \
    QColorMenu.h \
    QTrack.h \
    trackList.h \
    Synergy/CDlgPlannedSynergy.h \
    Synergy/CDlgZoneSynergy.h \
    Synergy/CDlgTimeSynergy.h \
    Synergy/CDlgInfoSynergy.h \
    Synergy/CDlgScanSynergy01.h \
    Synergy/CDlgScanSynergy02.h \
    Synergy/CDlgFireSynergy01.h \
    Synergy/CDlgFireSynergy02.h \
    CmdTakeBackControl.h \
    CDlgSystemMessageDistribute.h \
    SelectMarkDialog.h \
    SystemPlane_Timer.h 

win32 {
    LIBS += -L../../lib
    DESTDIR = ../../lib
    DLLDESTDIR = ../../bin
#   CONFIG -= flat
} else {
    LIBS += -L/usr/local/lib
    DESTDIR = /usr/local/lib
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

FORMS += \ 
    cmdTrackCircleNum.ui \
    cmd.ui \
    cmdTrack.ui \
    cmdFlight.ui \
    cmdFlightTurn.ui \
    cmdPlane.ui \
    Synergy/CDlgPlannedSynergy.ui \
    Synergy/CDlgZoneSynergy.ui \
    Synergy/CDlgTimeSynergy.ui \
    Synergy/CDlgInfoSynergy.ui \
    Synergy/CDlgScanSynergy01.ui \
    Synergy/CDlgScanSynergy02.ui \
    Synergy/CDlgFireSynergy01.ui \
    Synergy/CDlgFireSynergy02.ui \
    CmdTakeBackControl.ui \
    CDlgSystemMessageDistribute.ui \
    SelectMarkDialog.ui 

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}



include(lib_ly_ccip.pri)


# MessageObjectManage
# include(MessageObjectManage/MessageObjectManage.pri)
# include(../../include/DataManage/MessageObjectManage/MessageObjectManageService.pri)
