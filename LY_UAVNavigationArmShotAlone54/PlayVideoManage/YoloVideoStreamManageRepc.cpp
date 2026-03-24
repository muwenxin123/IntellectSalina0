#include "YoloVideoStreamManageRepc.h"

YoloVideoStreamManageRepc::YoloVideoStreamManageRepc(QObject *parent)
    : QObject(parent)
    , m_CreateHost(false)
    , m_RemoteObjectHost(this)
    , m_YoloVideoStreamManage(this)
{
}

void YoloVideoStreamManageRepc::CreateHost(const QString &hostAddress)
{
    if (m_CreateHost)
    {
        return ;
    }
    m_RemoteObjectHost.setHostUrl(QUrl(hostAddress));

    m_CreateHost = m_RemoteObjectHost.enableRemoting(&m_YoloVideoStreamManage);
}
