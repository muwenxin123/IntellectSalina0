# play video stream manage

CONFIG += play_video_stream_manage

play_video_stream_manage {

    QT     += core widgets

    CONFIG += c++11

    DEFINES += PLAY_VIDEO_STREAM_MANAGE_DEFINES

    INCLUDEPATH += ../../include/ffmpeg


    LIBS += -L../../lib/ffmpeg/lib  -L../../lib/HCNetSDKCom


    SOURCES += \
        $$PWD/PlayVideoManageWidget.cpp \
        $$PWD/RealPlay.cpp


    HEADERS += \
        $$PWD/GeneralDef.h \
        $$PWD/IPlayVideoManage.h \
        $$PWD/PlayVideoManageWidget.h \
        $$PWD/RealPlay.h


    FORMS += \
        $$PWD/PlayVideoManageWidget.ui


    #qtHaveModule(remoteobjects): CONFIG += play_yolo_video_stream_manage
	CONFIG += play_yolo_video_stream_manage
    play_yolo_video_stream_manage {

        DEFINES += PLAY_YOLO_VIDEO_STREAM_MANAGE_DEFINES

        QT   += remoteobjects

        LIBS +=  -luser32


        REPC_SOURCE = $$PWD/YoloVideoStreamManageCommonInterface.rep

        SOURCES += \
            $$PWD/YoloVideoStreamManageCommonInterface.cpp \
            $$PWD/YoloVideoStreamManageRepc.cpp \
            $$PWD/PlayYoloVideoStreamManageWidget.cpp

        HEADERS += \
            $$PWD/YoloVideoStreamManageCommonInterface.h \
            $$PWD/YoloVideoStreamManageRepc.h \
            $$PWD/PlayYoloVideoStreamManageWidget.h

        FORMS += \
            $$PWD/PlayYoloVideoStreamManageWidget.ui

        }

}


CONFIG += play_video_stream_manage_play_video_manage_54

play_video_stream_manage_play_video_manage_54 {

    DEFINES += PLAY_VIDEO_STREAM_MANAGE_PLAY_VIDEO_MANAGE_54_DEFINES

    FORMS += \
        $$PWD/PlayVideoManageWidget54.ui

    HEADERS += \
        $$PWD/PlayVideoManageWidget54.h

    SOURCES += \
        $$PWD/PlayVideoManageWidget54.cpp
}

