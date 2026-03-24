#include "WeaponMarkAssignAimingStrike.h"
#include "Protocol/XygsSystemControl/SystemControl.h"

#include <float.h>
#include "lyXyhcInterface.h"
#include "lyXyhc.h"

#include "MessageObjectManage/IMessageObjectManageServiceImpl.h"
#include "DataManage/MessageObjectManage/WeaponMarkAssignAimingStrikeObject.h"

#include <QDebug>

WeaponMarkAssignAimingStrike::WeaponMarkAssignAimingStrike(QObject *parent)
    : QObject(parent)
    , m_DY54CommandOperObject(nullptr)
{
    m_DY54CommandOperObject = new qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject(this);
    if (m_DY54CommandOperObject)
    {
        qnzkna::MessageObjectManage::IMessageObjectManageServiceImpl::Get().AddMessageObject(m_DY54CommandOperObject->MessageObjectID(), m_DY54CommandOperObject);

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_1_Command_Insure, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_1_Command_Insure" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 1;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_1_Command_Percussion, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_1_Command_Percussion" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_1_Command_Reset, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_1_Command_Reset" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 4;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_2_Command_Insure, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_2_Command_Insure" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 1;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_2_Command_Percussion, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_2_Command_Percussion" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_Pill_2_Command_Reset, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_Pill_2_Command_Reset" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 1;
                const unsigned char byAmmunitionState = 4;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Command_UnLock, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Command_UnLock" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 1;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Command_Lock, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Command_Lock" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 3;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Pill_Command_Reset, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Pill_Command_Reset" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 4;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Pill_1_Command_Shot, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Pill_1_Command_Shot" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Pill_2_Command_Shot, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Pill_2_Command_Shot" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Pill_3_Command_Shot, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Pill_3_Command_Shot" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_BombType_Cruise_Pill_4_Command_Shot, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_BombType_Cruise_Pill_4_Command_Shot" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(nWeaponId);
            if (track != nullptr)
            {

                const unsigned char byAmmunitionType  = 2;
                const unsigned char byAmmunitionState = 2;

                float fAzimuth  = 0;
                float fDistance = -1;
                if (LyXyhc::Get().GetProj() != nullptr && track->m_nMarkID != 0 && track->m_strMarkID[0] != '\0' && (track->m_nMarkAssign == 1 || track->m_nMarkAssign == 2))
                {
                    const double dMarkX             = track->m_dMarkX;
                    const double dMarkY             = track->m_dMarkY;

                    const double dLongitudeVehicle    = track->m_Longitude;
                    const double dLatitudeVehicle     = track->m_Latitude;

                    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
                    LyXyhc::Get().GetProj()->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
                    fAzimuth  = dAngleVehicleMark;
                    fDistance = dDistanceVehicleMark;
                }
                SendWeaponMarkStrike(nWeaponId, track->m_strMarkID, byAmmunitionType, byAmmunitionState, fAzimuth, fDistance);
            }
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_StartAutoAiming, [this](const QString & strWeaponId, const QString & byMarkId)
        {
            qDebug() << "DY54_StartAutoAiming" << strWeaponId << byMarkId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();

            SendWeaponMarkAiming(nWeaponId, byMarkId.toStdString().c_str(), true);
        });

        connect(m_DY54CommandOperObject, &qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject::DY54_StopAutoAiming, [this](const QString & strWeaponId)
        {
            qDebug() << "DY54_StopAutoAiming" << strWeaponId;

            const unsigned long long int nWeaponId  = strWeaponId.toULongLong();
            SendWeaponMarkAiming(nWeaponId, "", false);
        });
    }
}

void WeaponMarkAssignAimingStrike::DY54_StartAutoAiming(const QString &strWeaponId, const QString &byMarkId)
{
    if (m_DY54CommandOperObject != nullptr)
    {
        emit m_DY54CommandOperObject->DY54_StartAutoAiming(strWeaponId, byMarkId);
    }
}

void WeaponMarkAssignAimingStrike::DY54_StopAutoAiming(const QString &strWeaponId)
{
    if (m_DY54CommandOperObject != nullptr)
    {
        emit m_DY54CommandOperObject->DY54_StopAutoAiming(strWeaponId);
    }
}

void WeaponMarkAssignAimingStrike::SendWeaponMarkAssign(const unsigned int nWeaponId, const char *const byMarkId, const bool bAssign)
{
    if (byMarkId == nullptr)
    {
        return ;
    }

    char sendBuf[sizeof(NetHeader) + sizeof(WeaponMarkAssign)] = { 0 };

    NetHeader *pHeader  = (NetHeader *)(sendBuf);
    pHeader->ucType     = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;
    pHeader->ucSubType  = 0x01;
    pHeader->unLength   = sizeof(NetHeader) + sizeof(WeaponMarkAssign);
    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    WeaponMarkAssign *weaponMarkAssign = (WeaponMarkAssign *)(sendBuf + sizeof(NetHeader));
    weaponMarkAssign->unMainType         = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE;
    weaponMarkAssign->unSubType          = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE_UNSUBTYPE_WEAPONMARKASSIGN;
    weaponMarkAssign->nWeaponId          = nWeaponId;
    memcpy(weaponMarkAssign->byMarkId, byMarkId, sizeof(weaponMarkAssign->byMarkId));
    weaponMarkAssign->bAssign            = bAssign;

    LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));
}

void WeaponMarkAssignAimingStrike::SendWeaponMarkAiming(const unsigned int nWeaponId, const char *const byMarkId, const bool bAiming)
{
    if (byMarkId == nullptr)
    {
        return ;
    }

    char sendBuf[sizeof(NetHeader) + sizeof(WeaponMarkAiming)] = { 0 };

    NetHeader *pHeader  = (NetHeader *)sendBuf;
    pHeader->ucType     = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;
    pHeader->ucSubType  = 0x01;
    pHeader->unLength   = sizeof(NetHeader) + sizeof(WeaponMarkAiming);
    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    WeaponMarkAiming *weaponMarkAiming = (WeaponMarkAiming *)(sendBuf + sizeof(NetHeader));
    weaponMarkAiming->unMainType         = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE;
    weaponMarkAiming->unSubType          = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE_UNSUBTYPE_WEAPONMARKAIMING;
    weaponMarkAiming->nWeaponId          = nWeaponId;
    memcpy(weaponMarkAiming->byMarkId, byMarkId, sizeof(weaponMarkAiming->byMarkId));
    weaponMarkAiming->bAiming            = bAiming;

    LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));
}

void WeaponMarkAssignAimingStrike::SendWeaponMarkStrike(const unsigned int nWeaponId, const char *const byMarkId, const unsigned char byAmmunitionType, const unsigned char byAmmunitionState, const float fAzimuth, const float fDistance)
{
    if (byMarkId == nullptr)
    {
        return ;
    }

    char sendBuf[sizeof(NetHeader) + sizeof(WeaponMarkStrike)] = { 0 };

    NetHeader *pHeader  = (NetHeader *)sendBuf;
    pHeader->ucType     = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;
    pHeader->ucSubType  = 0x01;
    pHeader->unLength   = sizeof(NetHeader) + sizeof(WeaponMarkStrike);
    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    WeaponMarkStrike *weaponMarkStrike = (WeaponMarkStrike *)(sendBuf + sizeof(NetHeader));
    weaponMarkStrike->unMainType         = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE;
    weaponMarkStrike->unSubType          = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_WEAPONMARKASSIGNAIMINGSTRIKE_UNSUBTYPE_WEAPONMARKSTRIKE;
    weaponMarkStrike->nWeaponId          = nWeaponId;
    memcpy(weaponMarkStrike->byMarkId, byMarkId, sizeof(weaponMarkStrike->byMarkId));
    weaponMarkStrike->byAmmunitionType   = byAmmunitionType;
    weaponMarkStrike->byAmmunitionState  = byAmmunitionState;
    weaponMarkStrike->fAzimuth           = fAzimuth;
    weaponMarkStrike->fDistance          = fDistance;

    LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));
}
