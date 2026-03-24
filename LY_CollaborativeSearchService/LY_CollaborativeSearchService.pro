#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui network
QT       += sql
QT       += positioning
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_CollaborativeSearchService
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/map3d  ../../include/lyUI ../../3rdParty/include

DEFINES += LY_COLLABORATIVESEARCHSERVICE_LIBRARY

SOURCES += LY_CollaborativeSearchService.cpp     LY_CollaborativeSearchServiceInterface.cpp \
    RoutePlanningDialog.cpp \
    AreaSearchAlgorithm.cpp \
	aStar.cpp \
	kmeans.cpp \
	TinyXML/tinyxml2.cpp \
	UAVProtocol.cpp \
	RoutePlanningDisplayDialog.cpp \
    ICollaborativeSearchActivator.cpp \
    uav_dispose_route_dialog.cpp \
    SystemEventRecording.cpp \
    UAV_RouteLine/Dispose_targetAndUAVS.cpp \
    UAV_RouteLine/Dispose_UAVS_Match_Target.cpp \
    UAV_RouteLine/Dispose_UAVS_Route.cpp \
    UAV_RouteLine/UAV_Dispose_Route_Base.cpp \
    UAV_RouteLine/dispose_targetanduavsroute_dialog.cpp \
    RoutePlaningReportsDialog.cpp
    

HEADERS += LY_CollaborativeSearchService.h    LY_CollaborativeSearchService_global.h     LY_CollaborativeSearchServiceInterface.h \
    RoutePlanningDialog.h \
	AreaSearchAlgorithm.h \
	aStar.h \
    Dijkstra.h \
	FloodFill.hpp \
	kmeans.h \
	UAVProtocol.h \
	TinyXML/tinyxml2.h \
	RoutePlanningDisplayDialog.h \
	readAndWriteXML.h \
    ICollaborativeSearchActivator.h \
    uav_dispose_route_dialog.h \
    SystemEventRecording.h \
    UAV_RouteLine/Dispose_targetAndUAVS.h \
    UAV_RouteLine/Dispose_UAVS_Match_Target.h \
    UAV_RouteLine/Dispose_UAVS_Route.h \
    UAV_RouteLine/UAV_Dispose_Route_Base.h \
    UAV_RouteLine/target_UAV_info.h \
    UAV_RouteLine/dispose_targetanduavsroute_dialog.h \
    RoutePlaningReportsDialog.h

win32 {
    DEFINES += CORE_EXPORT=__declspec(dllimport) GUI_EXPORT=__declspec(dllimport)
    LIBS += -L../../lib 
	LIBS += -L../../3rdParty/lib
    DESTDIR = ../../bin
#   header.path=../../include
} else {
    LIBS += -L/usr/local/lib
    DESTDIR = /usr/local/lib
#   header.path=/usr/local/include
}

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
LIBS += -llyCoreD -lmap3dD -lGeometryD -llyUID -lLyUcisPluginFrameWorkD -lOsgExD -losgd
} else {
LIBS += -llyCore -lmap3d -lGeometry -llyUI -lLyUcisPluginFrameWork -lOsgEx -losg
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}


#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}

FORMS += \
    RoutePlanningDialog.ui \
	RoutePlanningDisplayDialog.ui \
    uav_dispose_route_dialog.ui \
    UAV_RouteLine/dispose_targetanduavsroute_dialog.ui \
    RoutePlaningReportsDialog.ui


include(lib_ly_ccip.pri)
