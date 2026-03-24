#include "QTrack.h"
#include <float.h>

QTrack::QTrack()
{
    Reset();
}

QTrack::~QTrack()
{
}

void QTrack::Reset()
{
    memset((void *)this, 0, sizeof(QTrack) - sizeof(m_lsPoints));
    m_status.Flag       = 1;
    m_status.XianHangJi = 1;
    m_nDisplayCount = 5;
    m_nOffsetX = 3;
    m_nOffsetY = -3;
    m_fMinX = (float)1E30;
    m_fMinY = (float)1E30;
    m_fMaxX = (float) -1E30;
    m_fMaxY = (float) -1E30;

    m_nMarkAssign = 0;

    memset(m_strMarkID, 0, sizeof(m_strMarkID));

    m_AutoAimingState            = 0;
    m_UavMarkTrackShowTimes    = 0;
    m_dMarkX                   = 0;
    m_dMarkY                   = 0;
    m_dMarkH                   = 0;
    m_MarkVehicleCalShow       = 1;
    m_MarkVehicleCalLabelShow  = 0;
    m_MarkVehicleCalShowChange = 1;
    m_MarkVehicleCalUnuse2     = 0;
    m_nMarkVehicleDistShow     = 0;
    m_dMarkVehicleDist         = DBL_MAX;
    m_dMarkVehicleHeight       = DBL_MAX;
    m_dMarkVehicleYaw          = DBL_MAX;
    m_dMarkVehicleYawSame      = DBL_MAX;
    m_dMarkVehicleMinX         = DBL_MAX;
    m_dMarkVehicleMinY         = DBL_MAX;

    SensorViewShow              = 1;
    SensorViewShowChange        = 1;
    SensorUnuse2                = 0;
    SensorUnuse3                = 0;

    m_Longitude                 = 0;
    m_Latitude                  = 0;

    unNodeType                  = 0;
    unNodeID                    = 0;
    unNodeNameSimple[0]         = 0;
}
