#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_TaskPlatMatch
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/3D ../../3rdParty/include
INCLUDEPATH += . ../../include ../../include/lyUI

DEFINES += LY_TASKPLATMATCH_LIBRARY

SOURCES += 	lY_TaskPlatMatch.cpp     lY_TaskPlatMatchInterface.cpp \
			CDlgTaskPlatMatch.cpp	CDlgSetPlat.cpp	\
			ITaskPlatMatchActivator.cpp \
			ThreadSavetoSql.cpp 

HEADERS += 	lY_TaskPlatMatch.h    lY_TaskPlatMatch_global.h     lY_TaskPlatMatchInterface.h \
			CDlgTaskPlatMatch.h		CDlgSetPlat.h \
			ITaskPlatMatchActivator.h	\
			MissionPlatType.h \
			ThreadSavetoSql.h 

FORMS +=    CDlgTaskPlatMatch.ui \
			CDlgSetPlat.ui 
			
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
LIBS += -llyCoreD -lmap3dD -lGeometryD -llyUID -luser32 -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI -luser32 -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}

FORMS +=  CDlgTaskPlatMatch.ui      

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}
