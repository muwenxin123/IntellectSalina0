#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT      += core gui sql
QT      += widgets
QT      += multimedia
QT      += multimediawidgets
QT      += positioning
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = LY_MarkManageService
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../include/jb

DEFINES += LY_MARKMANAGESERVICE_LIBRARY

SOURCES += LY_MarkManageService.cpp LY_MarkManageServiceInterface.cpp \
    Bag_XygsZCNEO_Markp.cpp \
    CDlgMarkList.cpp \
    CDlgMarkEdit.cpp \
    CombatZoneEditDialog.cpp \
    IMarkManageImpl.cpp \
    IMarkManageServiceActivator.cpp \
    geometric.cpp \
    CDlgMarkAssign.cpp \
    CDlgFiringData.cpp \
    CDlgMarkImage.cpp \
    CDlgMarkVideo.cpp \
    CDlgMarkPicture.cpp \
    CDlgMarkMerge.cpp \
    CDlgZoneList.cpp

HEADERS += LY_MarkManageService.h    LY_MarkManageService_global.h     LY_MarkManageServiceInterface.h \
    Bag_XygsZCNEO_Markp.h \
    CDlgMarkList.h \
    CDlgMarkEdit.h \
    CombatZoneEditDialog.h \
    IMarkManageImpl.h \
    IMarkManageServiceActivator.h \
    geometric.h \
    CDlgMarkAssign.h \
    CDlgFiringData.h \
    CDlgMarkImage.h \
    CDlgMarkVideo.h \
    CDlgMarkPicture.h \
    CDlgMarkMerge.h \
    CDlgZoneList.h

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
LIBS += -lLYCoreD -lmap3dD -lGeometryD -llyUID  -ljbD  -luser32 -lLyUcisPluginFrameWorkD  -lLY_TinyXmlD
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI  -ljb  -luser32 -lLyUcisPluginFrameWork  -lLY_TinyXml
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS +=  CDlgMarkList.ui CDlgMarkEdit.ui \
    CombatZoneEditDialog.ui \
    CDlgMarkAssign.ui \
    CDlgFiringData.ui \
    CDlgMarkImage.ui \
    CDlgMarkVideo.ui \
    CDlgMarkPicture.ui \
    CDlgMarkMerge.ui \
    CDlgZoneList.ui

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
