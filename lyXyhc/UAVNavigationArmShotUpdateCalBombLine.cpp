#include "UAVNavigationArmShotUpdateCalBombLine.h"

#include "lyXyhc.h"
#include "TrackMgr.h"
#include "IXyhcActivator.h"

#include "Protocol/DY_ZC_Control/DY_ZC_HUD_Control.h"

#include "UAVCurrentShotAiming.h"

#include <QMatrix4x3>

UAVNavigationArmShotUpdateCalBombLine::UAVNavigationArmShotUpdateCalBombLine(QObject *parent)
    : QObject(parent)
    , m_TimerUpdateMark_80mm(this)
{

    connect(&m_TimerUpdateMark_80mm, &QTimer::timeout, this, &UAVNavigationArmShotUpdateCalBombLine::UpdateMark_80mm);
    m_TimerUpdateMark_80mm.start(100);
}

double UAVNavigationArmShotUpdateCalBombLine::HeadAngle(const double angle) const
{

    double headingAngle = angle;
    if (qFuzzyIsNull(std::fmod(std::abs(angle), 360)))
    {
        headingAngle = 0;
    }
    else if (angle >= 360)
    {
        headingAngle = std::fmod(std::abs(angle), 360);
    }
    else if (angle < 0)
    {
        headingAngle = 360 - std::fmod(std::abs(angle), 360);
    }
    return headingAngle;
}

void UAVNavigationArmShotUpdateCalBombLine::UpdateMark_80mm()
{
#ifndef USING_LY_CCIP_80MM_DEFINE

#elif defined USING_LY_CCIP_80MM_DEFINE

    static bool findTitleAndYawAndDist = false;
    bool   findAutoAimingState = false;
    double yawAutoAimingState  = 0, distAutoAimingState = 0;
    std::string strNodeNameSimple("");

    IProj *pProj = LyXyhc::Get().GetProj();
    for (auto &trackMgr : LyXyhc::Get().getTrackMgr().m_Track)
    {
        QTrack &track = trackMgr.second;
        if ((track.m_nMarkAssign == 1 || track.m_nMarkAssign == 2) && pProj != nullptr)
        {
            const unsigned long long unNodeID  = track.unNodeID;
            const double &dVehicleLongitude    = track.m_Longitude;
            const double &dVehicleLatitude     = track.m_Latitude;
            const double &dVehicleAltitudeTemp = track.m_nGaoDu;
            const double &dVehicleSpeedTemp    = track.m_nSuDu;
            const double &dVehicleYawTemp      = track.m_nHangXiang;
            const double &dVehicleRollTemp     = track.Roll;
            const double &dVehiclePitchAngle   = track.Pitch;
            const double &dMarkXTemp         = track.m_dMarkX;
            const double &dMarkYTemp         = track.m_dMarkY;
            const double &dMarkHTemp         = track.m_dMarkH;

            double dVehicleMarkDistance = DBL_MAX, dVehicleMarkAngle = DBL_MAX;
            pProj->L_A(dVehicleLongitude, dVehicleLatitude, dMarkXTemp, dMarkYTemp, dVehicleMarkDistance, dVehicleMarkAngle);

            double dVehicleProjX = DBL_MAX, dVehicleProjY = DBL_MAX;
            pProj->jwxy(dVehicleLongitude, dVehicleLatitude, &dVehicleProjX, &dVehicleProjY);

            double dMarkProjX = DBL_MAX, dMarkProjY = DBL_MAX;
            pProj->jwxy(dMarkXTemp, dMarkYTemp, &dMarkProjX, &dMarkProjY);

            double dAltitude     = dVehicleAltitudeTemp;
            if (dAltitude <= 0.000001 && dAltitude >= -0.000001)
            {
                dAltitude = dAltitude + 0.0005;
            }

            const double dRelAltitude  = (dAltitude - dMarkHTemp) + 1;

            char dMarkVehicleCalTemp      = 0;
            char dMarkVehicleDistShowTemp = 0;

            std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
            qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dVehicleLongitude, dVehicleLatitude, dAltitude, dVehicleSpeedTemp, dRelAltitude, dVehiclePitchAngle, ccipdata_80mmlist, 2000, 0.5);

            if (ccipdata_80mmlist.size() > 0)
            {
                bool  yawAngleSame  = false;
                double dDistance = DBL_MAX, dAngle = DBL_MAX;
                double dMinDistance3D = DBL_MAX, dMinDistance = DBL_MAX, dMarkDistance = DBL_MAX;
                double dHeight   = DBL_MAX, dMinHeight = DBL_MAX, dMinYaw = DBL_MAX;
                double dJD = 0, dWD = 0;

                double dMarkProjXTemp = DBL_MAX, dMarkProjYTemp = DBL_MAX;
                for (const auto ccipdata : ccipdata_80mmlist)
                {

                    pProj->A_L(dVehicleLongitude, dVehicleLatitude, ccipdata.x, dVehicleYawTemp, dJD, dWD);

                    pProj->L_A(dJD, dWD, dMarkXTemp, dMarkYTemp, dDistance, dAngle);

                    pProj->jwxy(dJD, dWD, &dMarkProjXTemp, &dMarkProjYTemp);

                    dHeight = (dAltitude + ccipdata.zd) - dMarkHTemp;

                    dMarkDistance = ((dHeight * dHeight) + (dDistance * dDistance));
                    if (dMarkDistance < dMinDistance3D && std::abs(dHeight) <= 0.5)
                    {

                        dMarkVehicleCalTemp      = 1;
                        dMarkVehicleDistShowTemp = 1;

                        dMinDistance3D = dMarkDistance;

                        const double dVehicleMarkDifferenceYawTemp = dVehicleMarkAngle - dVehicleYawTemp;
                        yawAngleSame = (dVehicleMarkDifferenceYawTemp >= -90 && dVehicleMarkDifferenceYawTemp <= 90);
                        if (yawAngleSame && (dVehicleMarkDistance > dDistance))
                        {
                            dMinDistance = dDistance;
                        }
                        else
                        {
                            dMinDistance = -dDistance;
                        }

                        if (dVehicleMarkDifferenceYawTemp >= -180 && dVehicleMarkDifferenceYawTemp <= 180)
                        {
                            dMinYaw = dVehicleMarkDifferenceYawTemp;
                        }
                        else
                        {
                            dMinYaw = HeadAngle(dVehicleMarkDifferenceYawTemp);
                        }

                        dMinHeight     = dHeight;
                        track.m_dMarkVehicleMinX    = dMarkProjXTemp;
                        track.m_dMarkVehicleMinY    = dMarkProjYTemp;
                        track.m_dMarkVehicleYawSame = yawAngleSame;
                    }

                }

                track.m_nMarkVehicleDistShow = 0;

                {
                    track.m_nMarkVehicleCal      = dMarkVehicleCalTemp;
                    track.m_nMarkVehicleDistShow = dMarkVehicleDistShowTemp;
                    track.m_dMarkVehicleDist3D   = std::sqrt(dMinDistance3D);
                    track.m_dMarkVehicleDist     = dMinDistance;
                    track.m_dMarkVehicleHeight   = dMinHeight;
                    track.m_dMarkVehicleYaw      = dMinYaw;
                }

                if (IXyhcActivator::Get().getIRegistryImpl() != nullptr)
                {
                    qnzkna::framework::MessageReferenceProps  props;
                    props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
                    props.put("TargetID",     std::to_string(unNodeID));

                    qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo controlInfo;
                    controlInfo.componentID                   = unNodeID;
                    strncpy(controlInfo.targetID, track.m_strMarkID, sizeof(track.m_strMarkID));
                    controlInfo.targetID[sizeof(controlInfo.targetID) - 1] = '\0';

                    controlInfo.autoAimingState               = track.m_AutoAimingState;

                    controlInfo.m_dVehiclePitch               = dVehiclePitchAngle;
                    controlInfo.m_dVehicleRoll                = dVehicleRollTemp;
                    controlInfo.m_dVehicleYaw                 = dVehicleYawTemp;
                    controlInfo.m_dVehicleSpeed               = dVehicleSpeedTemp;

                    controlInfo.m_dVehicleLongitude           = dVehicleLongitude;
                    controlInfo.m_dVehicleLatitude            = dVehicleLatitude;
                    controlInfo.m_dVehicleAltitude            = dVehicleAltitudeTemp;

                    controlInfo.m_dVehicleX                   = dVehicleProjX;
                    controlInfo.m_dVehicleY                   = dVehicleProjY;
                    controlInfo.m_dVehicleH                   = dVehicleAltitudeTemp;

                    controlInfo.m_dMarklePitch              = 0;
                    controlInfo.m_dMarkleRoll               = 0;
                    controlInfo.m_dMarkleYaw                = 0;
                    controlInfo.m_dMarkleSpeed              = 0;

                    controlInfo.m_dMarkLongitude            = dMarkXTemp;
                    controlInfo.m_dMarkLatitude             = dMarkYTemp;
                    controlInfo.m_dMarkAltitude             = dMarkHTemp;

                    controlInfo.m_dMarkX                    = dMarkProjX;
                    controlInfo.m_dMarkY                    = dMarkProjY;
                    controlInfo.m_dMarkH                    = dMarkHTemp;

                    controlInfo.nVehicleMarkCal             = dMarkVehicleCalTemp;
                    controlInfo.nVehicleMarkCalLineDraw     = 1;
                    controlInfo.nVehicleMarkDistShow        = dMarkVehicleDistShowTemp;
                    controlInfo.nMarkAssign                 = 1;
                    controlInfo.dVehicleMarkYawSame         = track.m_dMarkVehicleYawSame;
                    controlInfo.dVehicleMarkYaw             = dVehicleMarkAngle;
                    controlInfo.dVehicleMarkDist            = dVehicleMarkDistance;
                    controlInfo.dMarkVehicleMinDist3D       = track.m_dMarkVehicleDist3D;
                    controlInfo.dMarkVehicleMinDist         = track.m_dMarkVehicleDist;
                    controlInfo.dMarkVehicleMinHeightDiff   = track.m_dMarkVehicleHeight;
                    controlInfo.dMarkVehicleMinYawDiff      = track.m_dMarkVehicleYaw;
                    controlInfo.dMarkVehicleMinX            = track.m_dMarkVehicleMinX;
                    controlInfo.dMarkVehicleMinY            = track.m_dMarkVehicleMinY;
                    controlInfo.dMarkVehicleMinZ            = track.m_dMarkVehicleHeight + dMarkHTemp;

                    IXyhcActivator::Get().getIRegistryImpl()->messageNotify(
                        new qnzkna::framework::MessageReference(
                            "MsgDYZCHUDControl",
                            (const char *)&controlInfo,
                            sizeof(controlInfo),
                            props));

                    if (controlInfo.autoAimingState == 1)
                    {
                        findAutoAimingState = true;
                        strNodeNameSimple = std::string(track.unNodeNameSimple);
                        pProj->X_A(dMarkProjX, dMarkProjY, track.m_dMarkVehicleMinX, track.m_dMarkVehicleMinY, distAutoAimingState, yawAutoAimingState);
                    }
                }
            }
        }
    }

    if (findAutoAimingState && LyXyhc::Get().getPUAVCurrentShotAiming() != nullptr)
    {
        findTitleAndYawAndDist = true;
        LyXyhc::Get().getPUAVCurrentShotAiming()->updateTitleAndYawAndDist(strNodeNameSimple, yawAutoAimingState, distAutoAimingState);
    }
    else
    {
        if (findTitleAndYawAndDist)
        {
            findTitleAndYawAndDist = false;
            LyXyhc::Get().getPUAVCurrentShotAiming()->updateTitleAndYawAndDist(strNodeNameSimple, yawAutoAimingState, distAutoAimingState, true);
        }
    }
#endif
}
