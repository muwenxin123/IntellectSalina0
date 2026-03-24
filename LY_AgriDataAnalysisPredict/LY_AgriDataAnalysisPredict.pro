#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core
QT += widgets


TEMPLATE = lib
DEFINES += LY_AGRIDATAANALYSISPREDICT_LIBRARY


TARGET = LY_AgriDataAnalysisPredict

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

INCLUDEPATH += . ../../include ../../include/lyUI ../../include/qwt/include

LIBS += -L../../lib

SOURCES += \
    IAgriDataAnalysisPredictActivator.cpp \
    LY_AgriDataAnalysisPredict.cpp \
    LY_AgriDataAnalysisPredictInterface.cpp \

HEADERS += \
    IAgriDataAnalysisPredictActivator.h \
    LY_AgriDataAnalysisPredict.h \
    LY_AgriDataAnalysisPredictInterface.h \
    LY_AgriDataAnalysisPredict_global.h \


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
LIBS += -llyCoreD -lGeometryD -llyUID -lLY_TinyXmlD -lLyUcisPluginFrameWorkD
} else {
LIBS += -llyCore -lGeometry -llyUI -lLY_TinyXml -lLyUcisPluginFrameWork
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,D)#_debug
    else: TARGET = $$join(TARGET,,,D)
}
