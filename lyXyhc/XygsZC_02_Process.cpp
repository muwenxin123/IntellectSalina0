#include "XygsZC_02_Process.h"

#include "lyXyhc.h"
#include "TrackMgr.h"

Q_DECLARE_METATYPE(XygsZC_02)

XygsZC_02_Process::XygsZC_02_Process(QObject *parent)
    : QObject(parent)
    , m_MutexXygsZC_02Map()
{
    QMutexLocker locker(&m_MutexXygsZC_02Map);
    m_XygsZC_02Map.clear();
    locker.unlock();

    connect(this, &XygsZC_02_Process::Process_XygsZC_02_Sig, this, &XygsZC_02_Process::Process_XygsZC_02_Slot, Qt::QueuedConnection);
}

void XygsZC_02_Process::Process_XygsZC_02_Slot(const unsigned long long unNodeID, const XygsZC_02 xygsZC_02)
{
    if (unNodeID == 0 || xygsZC_02.unNodeID == 0 || xygsZC_02.unNodeType != NODE_TYPE_SENSOR)
    {
        return ;
    }

    for (TrackMap::iterator it = LyXyhc::Get().getTrackMgr().m_Track.begin(); it != LyXyhc::Get().getTrackMgr().m_Track.end(); it++)
    {
        QTrack &track = it->second;
        if (track.unNodeID == unNodeID)
        {
            track.SensorPitch  = xygsZC_02.pitch;
            track.SensorRoll   = xygsZC_02.roll;
            track.SensorYaw    = xygsZC_02.yaw;
            break;
        }
    }
}
