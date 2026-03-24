#ifndef YOLOVIDEOSTREAMMANAGEREPC_H
#define YOLOVIDEOSTREAMMANAGEREPC_H

#include <QObject>

#include "YoloVideoStreamManageCommonInterface.h"

namespace qnzkna
{
namespace PlayVideoManage
{

struct VideoStreamTargetInfo
{
    int              componentId;
    int              targetId;
    long long int    targetTime;
    int              discernType;
    int              targetType;
    int              videoWidth;
    int              videoHeight;
    int              targetRectSelected;
    int              targetRectX;
    int              targetRectY;
    int              targetRectWidth;
    int              targetRectHeight;

    VideoStreamTargetInfo()
    {
        componentId        = 0;
        targetId           = 0;
        targetTime         = 0;
        discernType        = 0;
        targetType         = 0;
        videoWidth         = 0;
        videoHeight        = 0;
        targetRectSelected = 0;
        targetRectX        = 0;
        targetRectY        = 0;
        targetRectWidth    = 0;
        targetRectHeight   = 0;
    }
};

}
}

class YoloVideoStreamManageRepc : public QObject
{
    Q_OBJECT
public:
    explicit YoloVideoStreamManageRepc(QObject *parent = nullptr);
    virtual ~YoloVideoStreamManageRepc() {}

    void CreateHost(const QString &hostAddress);

    bool GetCreateHost() const
    {
        return m_CreateHost;
    }

    YoloVideoStreamManageCommonInterface &getYoloVideoStreamManage()
    {
        return m_YoloVideoStreamManage;
    }

signals:

public slots:

private:
    bool                                 m_CreateHost;
    QRemoteObjectHost                    m_RemoteObjectHost;
    YoloVideoStreamManageCommonInterface m_YoloVideoStreamManage;
};

#endif
