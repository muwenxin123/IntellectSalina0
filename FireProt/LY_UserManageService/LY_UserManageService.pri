#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT       += core gui
#QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


INCLUDEPATH += ../../include ../../include/3D ../include

#DEFINES += LY_USERMANAGESERVICE_LIBRARY

SOURCES += \
    $$PWD/QDlgUserManage.cpp \
    $$PWD/QDlgUserCreateModify.cpp \
    $$PWD/QDlgUserLogin.cpp \
    $$PWD/UserManageServiceSql.cpp \
    $$PWD/UserManageService.cpp

HEADERS += \
    $$PWD/QDlgUserManage.h \
    $$PWD/QDlgUserCreateModify.h \
    $$PWD/QDlgUserLogin.h \
    $$PWD/UserManageServiceSql.h \
    $$PWD/UserManageService.h


FORMS += \
    $$PWD/QDlgUserLogin.ui \
    $$PWD/QDlgUserManage.ui \
    $$PWD/QDlgUserCreateModify.ui


CONFIG(debug, debug|release) {
LIBS += -luser32
} else {
LIBS += -luser32
}
