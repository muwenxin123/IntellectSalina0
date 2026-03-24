#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core sql
QT += widgets
QT += network

QT += concurrent

TEMPLATE = lib
DEFINES += LY_UAVNAVIGATIONARMSHOTALONE54_LIBRARY

TARGET = LY_UAVNavigationArmShotAlone54

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

INCLUDEPATH += . ../../include ../../include/lyUI ../../include/lyDock


SOURCES += \
    BombStatusWidget.cpp \
    IUAVNavigationArmShotAlone54Activator.cpp \
    IUAVNavigationArmShotAlone54Dialog.cpp \
    IUAVNavigationArmShotAlone54CruiseControl.cpp \
    ImageStatusWidget.cpp \
    LY_UAVNavigationArmShotAlone54.cpp \
    LY_UAVNavigationArmShotAlone54Interface.cpp \
    VideoHUDDialog.cpp \
    IUAVTargetTree.cpp \
    PlayVideo_38mmlauncher_Control.cpp \
    UAVArmShotAloneListDialog.cpp


HEADERS += \
    BombStatusWidget.h \
    IUAVNavigationArmShotAlone54Activator.h \
    IUAVNavigationArmShotAlone54Dialog.h \
    IUAVNavigationArmShotAlone54CruiseControl.h \
    ImageStatusWidget.h \
    LY_UAVNavigationArmShotAlone54.h \
    LY_UAVNavigationArmShotAlone54Interface.h \
    LY_UAVNavigationArmShotAlone54_global.h \
    VideoHUDDialog.h \
    IUAVTargetTree.h \
    PlayVideo_38mmlauncher_Control.h \
    UAVArmShotAloneListDialog.h



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
LIBS += -llyCoreD -llyUID -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -llyUI -lLyUcisPluginFrameWork
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
    BombStatusWidget.ui \
    IUAVNavigationArmShotAlone54Dialog.ui \
    ImageStatusWidget.ui \
    VideoHUDDialog.ui \
    IUAVTargetTree.ui \
    PlayVideo_38mmlauncher_Control.ui \
    UAVArmShotAloneListDialog.ui

RESOURCES += \
    UAVNavigationArmShotAlone54.qrc


include(PlayVideoManage/PlayVideoManage.pri)
include(lib_ly_ccip.pri)

# MessageObjectManage
include(../../include/DataManage/MessageObjectManage/MessageObjectManageService.pri)

