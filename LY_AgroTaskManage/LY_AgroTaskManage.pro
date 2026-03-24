#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core gui
QT += positioning
QT += network
QT += svg

CONFIG += c++11

#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_AgroTaskManage
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../3rdParty/include
INCLUDEPATH += . ../../include ../../include/lyUI

DEFINES += LY_AGROTASKMANAGE_LIBRARY

SOURCES += LY_AgroTaskManage.cpp \
    LY_AgroTaskManageInterface.cpp \
    IAgroTaskManagerActivator.cpp \
    CDlgTaskSchedule.cpp \
    CDlgTaskEventRecord.cpp \
    CDlgAgroTaskManage.cpp \
    CDlgDebug.cpp \
    CDlgAgroTaskManageEdit.cpp \
    CDrawAxis.cpp \
    CDlgAgroParameterShow.cpp \
    CDlgAgroSampleParaShow.cpp \
    CDlgAgroManureParaShow.cpp \
    CDlgScanParaShow.cpp \
    gridbasesweepplanner.cpp \
    gridmap.cpp
	
HEADERS += LY_AgroTaskManage.h \
        LY_AgroTaskManage_global.h \
        LY_AgroTaskManageInterface.h \
    IAgroTaskManagerActivator.h \
    CDlgTaskSchedule.h \
    CDlgTaskEventRecord.h \
    CDlgAgroTaskManage.h \
    CDlgDebug.h \
    CDlgAgroTaskManageEdit.h \
    CDrawAxis.h \
    CDlgAgroParameterShow.h \
    CDlgAgroSampleParaShow.h \
    CDlgAgroManureParaShow.h \
    CDlgScanParaShow.h \
    gridbasesweepplanner.h \
    gridmap.h

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

FORMS += \    
    CDlgTaskSchedule.ui \
    CDlgTaskEventRecord.ui \
    CDlgAgroTaskManage.ui \
    CDlgDebug.ui \
    CDlgAgroTaskManageEdit.ui \
    CDlgAgroParameterShow.ui \
    CDlgAgroSampleParaShow.ui \
    CDlgAgroManureParaShow.ui \
    CDlgScanParaShow.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}

