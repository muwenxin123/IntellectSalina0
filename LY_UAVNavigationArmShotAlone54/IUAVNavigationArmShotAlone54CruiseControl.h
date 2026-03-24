#ifndef IUAVNAVIGATIONARMSHOTALONE54CRUISECONTROL_H
#define IUAVNAVIGATIONARMSHOTALONE54CRUISECONTROL_H

#include <QObject>

#include "DataManage/Common/TypeDefine_Node.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include "DataManage/XYWeaponManage/XYWeaponManage.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

class IUAVNavigationArmShotAlone54CruiseControl : public QObject
{
    Q_OBJECT

public:
    explicit IUAVNavigationArmShotAlone54CruiseControl(QObject *parent = nullptr);
    virtual ~IUAVNavigationArmShotAlone54CruiseControl();

    void InitWeaponComponent();

    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Enable_Value(const long long int DY54_BombType_Cruise_IO1_Enable_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Disable_Value(const long long int DY54_BombType_Cruise_IO1_Disable_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Enable_Value(const long long int DY54_BombType_Cruise_IO2_Enable_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Disable_Value(const long long int DY54_BombType_Cruise_IO2_Disable_Value);

    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_UnLock_Value(const long long int DY54_BombType_Cruise_Command_UnLock_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_Lock_Value(const long long int DY54_BombType_Cruise_Command_Lock_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_Command_Reset_Value(const long long int DY54_BombType_Cruise_Pill_Command_Reset_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_1_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_1_Command_Shot_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_2_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_2_Command_Shot_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_3_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_3_Command_Shot_Value);
    void IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_4_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_4_Command_Shot_Value);

    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO1_Enable(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO1_Disable(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO2_Enable(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO2_Disable(const QString &strComponentID);

    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_UnLock(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_Lock(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_Command_Reset(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_1_Command_Shot(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_2_Command_Shot(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_3_Command_Shot(const QString &strComponentID);
    void IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_4_Command_Shot(const QString &strComponentID);

private:
    void SendFlyCmd_XygsJL_16(const xygsJL_16 &bag, const std::string &strComponentID) const;
    void SendFlyCmd_XygsJL_18(const xygsJL_18 &bag, const std::string &strComponentID) const;
    void SendFlyCmdMsg_XygsJL(const char *const message, const unsigned int msglength, const qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const;

private:
    const CXYWeaponManage                         m_CXYWeaponManage;
    qnzkna::WeaponManage::MultiWeaponComponentMap m_MultiWeaponComponentMap;
    long long int                                 m_DY54_BombType_Cruise_IO1_Enable_Value;
    long long int                                 m_DY54_BombType_Cruise_IO1_Disable_Value;
    long long int                                 m_DY54_BombType_Cruise_IO2_Enable_Value;
    long long int                                 m_DY54_BombType_Cruise_IO2_Disable_Value;
    long long int                                 m_DY54_BombType_Cruise_Command_UnLock_Value;
    long long int                                 m_DY54_BombType_Cruise_Command_Lock_Value;
    long long int                                 m_DY54_BombType_Cruise_Pill_Command_Reset_Value;
    long long int                                 m_DY54_BombType_Cruise_Pill_1_Command_Shot_Value;
    long long int                                 m_DY54_BombType_Cruise_Pill_2_Command_Shot_Value;
    long long int                                 m_DY54_BombType_Cruise_Pill_3_Command_Shot_Value;
    long long int                                 m_DY54_BombType_Cruise_Pill_4_Command_Shot_Value;
};

#endif
