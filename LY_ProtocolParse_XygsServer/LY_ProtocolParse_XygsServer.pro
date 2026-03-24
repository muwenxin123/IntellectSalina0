#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core
QT += network


TEMPLATE = lib
DEFINES += LY_PROTOCOLPARSE_XYGSSERVER_LIBRARY

TARGET = LY_ProtocolParse_XygsServer

CONFIG += plugin

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += . ../../include

LIBS += -L../../lib

SOURCES += \
    IProtocolParseXygsServerActivator.cpp \
    IProtocolParseXygsServerImpl.cpp \
    IProtocolParseXygsServerImpl_Control.cpp \
    IProtocolParseXygsServerImpl_Control_Handle.cpp \
    LY_ProtocolParse_XygsServer.cpp \
    LY_ProtocolParse_XygsServerInterface.cpp \
    LogRecord.cpp


HEADERS += \
    IProtocolParseXygsServerActivator.h \
    IProtocolParseXygsServerImpl.h \
    IProtocolParseXygsServerImpl_Control.h \
    IProtocolParseXygsServerImpl_Control_Handle.h \
    LY_ProtocolParse_XygsServer.h \
    LY_ProtocolParse_XygsServerInterface.h \
    LY_ProtocolParse_XygsServer_global.h \
    LogRecord.h



win32 {
    DEFINES += CORE_EXPORT=__declspec(dllimport) GUI_EXPORT=__declspec(dllimport)
    LIBS += -L../../lib -L../../lib/Protocol
    DESTDIR = ../../bin
#   header.path=../../include
} else {
    LIBS += -L/usr/local/lib
    DESTDIR = /usr/local/lib
#   header.path=/usr/local/include
}

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -llyCoreD -lprotocolDll -lLyUcisPluginFrameWorkD
} else {
    LIBS += -llyCore -lprotocolDll -lLyUcisPluginFrameWork
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


exists(Xyhc_Control/Xyhc_Control.pri){
    include(Xyhc_Control/Xyhc_Control.pri)
}


#exists(SbgECom_Control/SbgECom_Control.pri){
#    include(SbgECom_Control/SbgECom_Control.pri)
#}


#exists(TransceiverHaiGe/TransceiverHaiGe.pri){
#    include(TransceiverHaiGe/TransceiverHaiGe.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_DY40mm_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_DY40mm_Control.h

#exists(DY40mm_Control/DY40mm_Control.pri){
#    include(DY40mm_Control/DY40mm_Control.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_DY54_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_DY54_Control.h

#exists(DY54_Control/DY54_Control.pri){
#    include(DY54_Control/DY54_Control.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_DYLY_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_DYLY_Control.h

#exists(DYLY_Control/DYLY_Control.pri){
#    include(DYLY_Control/DYLY_Control.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_DYXiaoFang_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_DYXiaoFang_Control.h

#exists(DYXiaoFang_Control/DYXiaoFang_Control.pri){
#    include(DYXiaoFang_Control/DYXiaoFang_Control.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_ZC54_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_ZC54_Control.h

#exists(ZC54_Control/ZC54_Control.pri){
#    include(ZC54_Control/ZC54_Control.pri)
#}


#SOURCES += \
#    IProtocolParseXygsServerImpl_ZC_Control.cpp

#HEADERS += \
#    IProtocolParseXygsServerImpl_ZC_Control.h

#exists(ZC_Control/ZC_Control.pri){
#    include(ZC_Control/ZC_Control.pri)
#}


#exists(ZCInfraredAiming54/ZCInfraredAiming54.pri){
#    include(ZCInfraredAiming54/ZCInfraredAiming54.pri)
#}


#exists(ZCNeo_Control/ZCNeo_Control.pri){
#    include(ZCNeo_Control/ZCNeo_Control.pri)
#}


#exists(ZCPinLing_Control/ZCPinLing_Control.pri){
#    include(ZCPinLing_Control/ZCPinLing_Control.pri)
#}


#exists(ZCXK400_Control/ZCXK400_Control.pri){
#    include(ZCXK400_Control/ZCXK400_Control.pri)
#}


#exists(SystemState_Control/SystemState_Control.pri){
#    include(SystemState_Control/SystemState_Control.pri)
#}


exists(Mavlink_Control/Mavlink_Control.pri){
   include(Mavlink_Control/Mavlink_Control.pri)
}

#exists(LY_ProtocolParse_Radar_IfrS20G/LY_ProtocolParse_Radar_IfrS20G.pri){
#    include(LY_ProtocolParse_Radar_IfrS20G/LY_ProtocolParse_Radar_IfrS20G.pri)
#}


#exists(DYAngFei_Control/DYAngFei_Control.pri){
#    include(DYAngFei_Control/DYAngFei_Control.pri)
#}

#exists(DY60_Control/DY60_Control.pri){
#    include(DY60_Control/DY60_Control.pri)
#}

