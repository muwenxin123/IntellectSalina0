QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LY_AgricultureHomepage
TEMPLATE = lib
INCLUDEPATH += ../../include ../../include/lyUI


DEFINES += LY_AGRICULTUREHOMEPAGE_LIBRARY

SOURCES += LY_AgricultureHomepage.cpp \
    LY_AgricultureHomepageActivator.cpp \
    LY_AgricultureHomepageInterface.cpp \
    ly_headerwidget.cpp \
    ly_tagwidget.cpp \
    ly_moudlemanagewidget.cpp \
    ly_monitortreewidget.cpp \
    curlutil.cpp \
    ly_mointorviewmodel.cpp \
    camerawidget.cpp \
    ffmpeguntil.cpp \
    ly_customtreewidget.cpp \
    ly_cameralayoutwidget.cpp \
    ly_cameralayoutwidget.cpp \
    ly_mointeraddwidget.cpp \
    ly_agriculhomepagewidget.cpp \
    ly_harmmanagerwidget.cpp \
    ly_photolayoutwidget.cpp \
    ly_photolayoutwidget.cpp \
    ly_photolayoutwidget.cpp \
    ly_customcombobox.cpp \
    ly_agrisprayprescommandwidget.cpp \
    ly_addmointtypewidget.cpp \
    ly_restoresensorwidget.cpp \
    videodecoderthread.cpp \
    ly_pestmointorwidget.cpp \
    ly_photoanalysiswidget.cpp \
    ly_photoanalysisdetailwidget.cpp

HEADERS += LY_AgricultureHomepage.h \
  LY_AgricultureHomepage_global.h \
  LY_AgricultureHomepageActivator.h \
    LY_AgricultureHomepageInterface.h \
    ly_headerwidget.h \
    ly_tagwidget.h \
    ly_moudlemanagewidget.h \
    ly_monitortreewidget.h \
    curlutil.h \
    ly_mointorviewmodel.h \
    camerawidget.h \
    ffmpeguntil.h \
    ly_customtreewidget.h \
    ly_sencorlistwidget.h \
    ly_cameralayoutwidget.h \
    ly_cameralayoutwidget.h \
    ly_mointeraddwidget.h \
    ly_agriculhomepagewidget.h \
    ly_harmmanagerwidget.h \
    ly_photolayoutwidget.h \
    ly_photolayoutwidget.h \
    ly_photolayoutwidget.h \
    ly_customcombobox.h \
    ly_agrisprayprescommandwidget.h \
    ly_addmointtypewidget.h \
    ly_restoresensorwidget.h \
    videodecoderthread.h \
    ly_pestmointwidget.h \
    ly_pestmointorwidget.h \
    ly_photoanalysiswidget.h \
    ly_photoanalysisdetailwidget.h


win32 {
  LIBS += -L../../lib
  DESTDIR = ../../lib
# header.path=../../include
} else {
  LIBS += -L/usr/local/lib
  DESTDIR = /usr/local/lib
# header.path=/usr/local/include
}
DLLDESTDIR = ../../bin

CONFIG += debug_and_release
CONFIG(debug, debug|release) {
  LIBS += \
    -llyCoreD \
    -llyUID \
    -lmap3dD \
    -lGeometryD
} else {
  LIBS += \
    -llyCore \
    -llyUI \
    -lmap3d \
    -lGeometry
}

build_pass:CONFIG(debug, debug|release) {
  unix: TARGET = $$join(TARGET,,,D)#_debug
  else: TARGET = $$join(TARGET,,,D)
}

#win32 {
#	DEF_FILE = $$join(TARGET,,,.def)
#}

FORMS += \
    ly_headerwidget.ui \
    ly_tagwidget.ui \
    ly_moudlemanagewidget.ui \
    ly_cameralayoutwidget.ui \
    ly_mointeraddwidget.ui \
    ly_agriculhomepagewidget.ui \
    ly_harmmanagerwidget.ui \
    ly_photolayoutwidget.ui \
    ly_agrisprayprescommandwidget.ui \
    ly_addmointtypewidget.ui \
    ly_restoresensorwidget.ui \
    ly_pestmointorwidget.ui \
    ly_photoanalysiswidget.ui \
    ly_photoanalysisdetailwidget.ui
