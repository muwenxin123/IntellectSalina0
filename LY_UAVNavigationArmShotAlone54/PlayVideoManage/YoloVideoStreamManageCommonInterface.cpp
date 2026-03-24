#include "YoloVideoStreamManageCommonInterface.h"

#include <QTimer>

YoloVideoStreamManageCommonInterface::YoloVideoStreamManageCommonInterface(QObject *parent)
    : YoloVideoStreamManageCommonInterfaceSimpleSource(parent)
    , m_StateChangeTimer(this)
{
    m_StateChangeTimer.start(5000);

    connect(&m_StateChangeTimer, &QTimer::timeout, this, [this]()
    {
        if (currState())
        {
            setCurrState(false);
        }
        else
        {
            setCurrState(true);
        }
    });

    qDebug() << "Source State is " << currState();
    m_StateChangeTimer.start();
}

void YoloVideoStreamManageCommonInterface::video_widow_show(const bool show)
{
    qDebug() << __FILE__ << __LINE__ << "video_widow_show " << show;
    emit video_widow_show_recv(show);
}

void YoloVideoStreamManageCommonInterface::video_selectrect(const QByteArray videostreamtargetinfo)
{
    emit video_selectrect_recv(videostreamtargetinfo);
}

void YoloVideoStreamManageCommonInterface::server_slot(bool clientState)
{
    qDebug() << __FILE__ << __LINE__;

}
