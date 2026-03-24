#include "IUAVNavigationArmShotAlone54CruiseControl.h"

#include <QMessageBox>

#include "LyMessageBox.h"

#include "IUAVNavigationArmShotAlone54Activator.h"

#define __DEBUG__LOGINFO__

#if defined(__DEBUG__LOGINFO__)
    #include <QDebug>
    #define logInfoFormat(format, ...)  qDebug(format, ##__VA_ARGS__)
    #define logInfo                     qDebug
#else
    #define logInfoFormat(format, ...)
    #define logInfo() /##/
#endif

IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl(QObject *parent)
    : QObject(parent)
    , m_CXYWeaponManage()
    , m_MultiWeaponComponentMap()
    , m_DY54_BombType_Cruise_IO1_Enable_Value(0x63)
    , m_DY54_BombType_Cruise_IO1_Disable_Value(0x65)
    , m_DY54_BombType_Cruise_IO2_Enable_Value(0x63)
    , m_DY54_BombType_Cruise_IO2_Disable_Value(0x65)
    , m_DY54_BombType_Cruise_Command_UnLock_Value(3584)
    , m_DY54_BombType_Cruise_Command_Lock_Value(0)
    , m_DY54_BombType_Cruise_Pill_Command_Reset_Value(2204)
    , m_DY54_BombType_Cruise_Pill_1_Command_Shot_Value(2816)
    , m_DY54_BombType_Cruise_Pill_2_Command_Shot_Value(3408)
    , m_DY54_BombType_Cruise_Pill_3_Command_Shot_Value(3755)
    , m_DY54_BombType_Cruise_Pill_4_Command_Shot_Value(4096)
{
}

IUAVNavigationArmShotAlone54CruiseControl::~IUAVNavigationArmShotAlone54CruiseControl()
{

}

void IUAVNavigationArmShotAlone54CruiseControl::InitWeaponComponent()
{
    m_MultiWeaponComponentMap.clear();

    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
    if (!pSystemMaintenanceService)
    {
        return ;
    }

    pSystemMaintenanceService->GetWeaponComponents(&m_MultiWeaponComponentMap);
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Enable_Value(const long long int DY54_BombType_Cruise_IO1_Enable_Value)
{
    m_DY54_BombType_Cruise_IO1_Enable_Value = DY54_BombType_Cruise_IO1_Enable_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Disable_Value(const long long int DY54_BombType_Cruise_IO1_Disable_Value)
{
    m_DY54_BombType_Cruise_IO1_Disable_Value = DY54_BombType_Cruise_IO1_Disable_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Enable_Value(const long long int DY54_BombType_Cruise_IO2_Enable_Value)
{
    m_DY54_BombType_Cruise_IO2_Enable_Value = DY54_BombType_Cruise_IO2_Enable_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Disable_Value(const long long int DY54_BombType_Cruise_IO2_Disable_Value)
{
    m_DY54_BombType_Cruise_IO2_Disable_Value = DY54_BombType_Cruise_IO2_Disable_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_UnLock_Value(const long long int DY54_BombType_Cruise_Command_UnLock_Value)
{
    m_DY54_BombType_Cruise_Command_UnLock_Value = DY54_BombType_Cruise_Command_UnLock_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_Lock_Value(const long long int DY54_BombType_Cruise_Command_Lock_Value)
{
    m_DY54_BombType_Cruise_Command_Lock_Value = DY54_BombType_Cruise_Command_Lock_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_Command_Reset_Value(const long long int DY54_BombType_Cruise_Pill_Command_Reset_Value)
{
    m_DY54_BombType_Cruise_Pill_Command_Reset_Value = DY54_BombType_Cruise_Pill_Command_Reset_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_1_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_1_Command_Shot_Value)
{
    m_DY54_BombType_Cruise_Pill_1_Command_Shot_Value = DY54_BombType_Cruise_Pill_1_Command_Shot_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_2_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_2_Command_Shot_Value)
{
    m_DY54_BombType_Cruise_Pill_2_Command_Shot_Value = DY54_BombType_Cruise_Pill_2_Command_Shot_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_3_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_3_Command_Shot_Value)
{
    m_DY54_BombType_Cruise_Pill_3_Command_Shot_Value = DY54_BombType_Cruise_Pill_3_Command_Shot_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_4_Command_Shot_Value(const long long int DY54_BombType_Cruise_Pill_4_Command_Shot_Value)
{
    m_DY54_BombType_Cruise_Pill_4_Command_Shot_Value = DY54_BombType_Cruise_Pill_4_Command_Shot_Value;
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO1_Enable(const QString &strComponentID)
{
    xygsJL_18 bag18;
    memset(&bag18, 0, sizeof(bag18));

    bag18.flags            = 18;
    bag18.groupIndex       = 0;
    bag18.planeIndex       = 0;
    bag18.param.comandMain = 0xF5;
    bag18.param.comandSub  = static_cast<unsigned char>(m_DY54_BombType_Cruise_IO1_Enable_Value);
    bag18.param.comandExt  = 0x01;

    SendFlyCmd_XygsJL_18(bag18, strComponentID.toStdString());
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO1_Disable(const QString &strComponentID)
{
    xygsJL_18 bag18;
    memset(&bag18, 0, sizeof(bag18));

    bag18.flags            = 18;
    bag18.groupIndex       = 0;
    bag18.planeIndex       = 0;
    bag18.param.comandMain = 0xF5;
    bag18.param.comandSub  = static_cast<unsigned char>(m_DY54_BombType_Cruise_IO1_Disable_Value);
    bag18.param.comandExt  = 0x01;

    SendFlyCmd_XygsJL_18(bag18, strComponentID.toStdString());
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO2_Enable(const QString &strComponentID)
{
    xygsJL_18 bag18;
    memset(&bag18, 0, sizeof(bag18));

    bag18.flags            = 18;
    bag18.groupIndex       = 0;
    bag18.planeIndex       = 0;
    bag18.param.comandMain = 0xF5;
    bag18.param.comandSub  = static_cast<unsigned char>(m_DY54_BombType_Cruise_IO2_Enable_Value);
    bag18.param.comandExt  = 0x03;

    SendFlyCmd_XygsJL_18(bag18, strComponentID.toStdString());
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_IO2_Disable(const QString &strComponentID)
{
    xygsJL_18 bag18;
    memset(&bag18, 0, sizeof(bag18));

    bag18.flags            = 18;
    bag18.groupIndex       = 0;
    bag18.planeIndex       = 0;
    bag18.param.comandMain = 0xF5;
    bag18.param.comandSub  = static_cast<unsigned char>(m_DY54_BombType_Cruise_IO2_Disable_Value);
    bag18.param.comandExt  = 0x03;

    SendFlyCmd_XygsJL_18(bag18, strComponentID.toStdString());
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_UnLock(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_Command_Reset_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" º½¿ÕÕ¨µ¯£º½âËø£¡È·¶¨½âËøÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_Lock(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    qDebug() << "IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command : "
             << " ,IO1_Enable : "     << m_DY54_BombType_Cruise_IO1_Enable_Value
             << " ,IO1_Disable : "    << m_DY54_BombType_Cruise_IO1_Disable_Value
             << " ,IO2_Enable : "     << m_DY54_BombType_Cruise_IO2_Enable_Value
             << " ,IO2_Disable : "    << m_DY54_BombType_Cruise_IO2_Disable_Value
             << " ,Command_UnLock : " << m_DY54_BombType_Cruise_Command_UnLock_Value
             << " ,Command_Lock : "   << m_DY54_BombType_Cruise_Command_Lock_Value
             << " ,Command_Reset : "  << m_DY54_BombType_Cruise_Pill_Command_Reset_Value
             << " ,1_Command_Shot : " << m_DY54_BombType_Cruise_Pill_1_Command_Shot_Value
             << " ,2_Command_Shot : " << m_DY54_BombType_Cruise_Pill_2_Command_Shot_Value
             << " ,3_Command_Shot : " << m_DY54_BombType_Cruise_Pill_3_Command_Shot_Value
             << " ,4_Command_Shot : " << m_DY54_BombType_Cruise_Pill_4_Command_Shot_Value
             ;

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_Lock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_Command_Reset_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" º½¿ÕÕ¨µ¯£º¼ÓËø£¡È·¶¨¼ÓËøÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_Command_Reset(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_Command_Reset_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" º½¿ÕÕ¨µ¯£º¸´Î»£¡È·¶¨¸´Î»Âð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_1_Command_Shot(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_1_Command_Shot_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 1ºÅº½¿ÕÕ¨µ¯£º·¢Éä£¡È·¶¨·¢ÉäÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_2_Command_Shot(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_2_Command_Shot_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 2ºÅº½¿ÕÕ¨µ¯£º·¢Éä£¡È·¶¨·¢ÉäÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_3_Command_Shot(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_3_Command_Shot_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 3ºÅº½¿ÕÕ¨µ¯£º·¢Éä£¡È·¶¨·¢ÉäÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_4_Command_Shot(const QString &strComponentID)
{
    if (strComponentID.isEmpty())
    {
        return ;
    }

    {

    }

    {
        xygsJL_16 bag16;
        memset(&bag16, 0, sizeof(bag16));

        bag16.flags                  = 16;
        bag16.groupIndex             = 0;
        bag16.planeIndex             = 0;
        bag16.param.channal_1        = static_cast<short>(m_DY54_BombType_Cruise_Command_UnLock_Value);
        bag16.param.channal_2        = static_cast<short>(m_DY54_BombType_Cruise_Pill_4_Command_Shot_Value);
        bag16.param.channal_3        = 0;
        bag16.param.channal_4        = 0;

        SendFlyCmd_XygsJL_16(bag16, strComponentID.toStdString());
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 4ºÅº½¿ÕÕ¨µ¯£º·¢Éä£¡È·¶¨·¢ÉäÂð£¿";
}

void IUAVNavigationArmShotAlone54CruiseControl::SendFlyCmd_XygsJL_16(const xygsJL_16 &bag, const std::string &strComponentID) const
{
    qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
    qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IUAVNavigationArmShotAlone54Activator::Get().getPIPositionManageService();
    qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService();
    qnzkna::framework::IRegistry                           *pIRegistry                     = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();

    if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
    {
        QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
        waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
        waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
        waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
        waringText += tr("Instruction not send");
        LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT      nNodeType = NODE_TYPE_CMDUNIT;
    std::string                             strNodeId;
    if (!pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetNodeInfo is false"), QMessageBox::Yes);
        return ;
    }

    XYWeaponIDSetting xyWeaponIDSetting2;
    if (!m_CXYWeaponManage.GetXYWeaponIDSetting(strComponentID, xyWeaponIDSetting2))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("XYWeaponManage GetXYWeaponIDSetting is false"), QMessageBox::Yes);
        return ;
    }

    qnzkna::WeaponManage::MultiWeaponComponentMap::const_iterator iter = m_MultiWeaponComponentMap.find(strComponentID);
    if (iter == m_MultiWeaponComponentMap.end())
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetWeaponComponent is nullptr"), QMessageBox::Yes);
        return ;
    }

    if (strComponentID != iter->second.getComponentID())
    {
        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(iter->second.getComponentID()) + tr(", SystemMaintenanceService UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }
    const std::string &strComponentName = iter->second.getComponentName();
    const std::string &strProtocolType  = iter->second.getProtocolType();

    const XYWeaponIDSetting &xyWeaponIDSetting     = xyWeaponIDSetting2;
    if (strComponentID != xyWeaponIDSetting.strWeaponID)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(xyWeaponIDSetting.strWeaponID) + tr(", XYWeaponIDSetting UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }

    xygsJL_16 bag16;
    memset(&bag16, 0, sizeof(bag16));
    bag16       = bag;
    bag16.flags = 16;

    const int nXYUAVGroupID = strtol(xyWeaponIDSetting.strXYWeaponGroupID.c_str(), nullptr, 16) ;
    const int nXYUAVID      = strtol(xyWeaponIDSetting.strXYWeaponID.c_str(), nullptr, 16);

    bag16.groupIndex = nXYUAVGroupID;
    bag16.planeIndex = nXYUAVID;
    SendFlyCmdMsg_XygsJL((const char *const)&bag16, sizeof(bag16), nNodeType, strComponentName, strNodeId, xyWeaponIDSetting.strChannelID, strComponentID, strProtocolType);
}

void IUAVNavigationArmShotAlone54CruiseControl::SendFlyCmd_XygsJL_18(const xygsJL_18 &bag, const std::string &strComponentID) const
{
    qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
    qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IUAVNavigationArmShotAlone54Activator::Get().getPIPositionManageService();
    qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService();
    qnzkna::framework::IRegistry                           *pIRegistry                     = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();

    if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
    {
        QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
        waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
        waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
        waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
        waringText += tr("Instruction not send");
        LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT      nNodeType = NODE_TYPE_CMDUNIT;
    std::string                             strNodeId;
    if (!pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetNodeInfo is false"), QMessageBox::Yes);
        return ;
    }

    XYWeaponIDSetting xyWeaponIDSetting2;
    if (!m_CXYWeaponManage.GetXYWeaponIDSetting(strComponentID, xyWeaponIDSetting2))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("XYWeaponManage GetXYWeaponIDSetting is false"), QMessageBox::Yes);
        return ;
    }

    qnzkna::WeaponManage::MultiWeaponComponentMap::const_iterator iter = m_MultiWeaponComponentMap.find(strComponentID);
    if (iter == m_MultiWeaponComponentMap.end())
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetWeaponComponent is nullptr"), QMessageBox::Yes);
        return ;
    }

    if (strComponentID != iter->second.getComponentID())
    {
        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(iter->second.getComponentID()) + tr(", SystemMaintenanceService UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }
    const std::string &strComponentName = iter->second.getComponentName();
    const std::string &strProtocolType  = iter->second.getProtocolType();

    const XYWeaponIDSetting &xyWeaponIDSetting     = xyWeaponIDSetting2;
    if (strComponentID != xyWeaponIDSetting.strWeaponID)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(xyWeaponIDSetting.strWeaponID) + tr(", XYWeaponIDSetting UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }

    xygsJL_18 bag18;
    memset(&bag18, 0, sizeof(bag18));
    bag18       = bag;
    bag18.flags = 18;

    const int nXYUAVGroupID = strtol(xyWeaponIDSetting.strXYWeaponGroupID.c_str(), nullptr, 16) ;
    const int nXYUAVID      = strtol(xyWeaponIDSetting.strXYWeaponID.c_str(), nullptr, 16);

    bag18.groupIndex = nXYUAVGroupID;
    bag18.planeIndex = nXYUAVID;
    SendFlyCmdMsg_XygsJL((const char *const)&bag18, sizeof(bag18), nNodeType, strComponentName, strNodeId, xyWeaponIDSetting.strChannelID, strComponentID, strProtocolType);
}

void IUAVNavigationArmShotAlone54CruiseControl::SendFlyCmdMsg_XygsJL(const char *const message, const unsigned int msglength, const NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const
{
    Q_UNUSED(strName)

    if (strComponentID == "" || nNodeType <= 0 || message == nullptr || msglength <= 0)
    {
        return ;
    }

    if (IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl() == nullptr)
    {
        return ;
    }

    if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY) == strProtocolType || std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54) == strProtocolType)
    {

#ifdef LY_UAVFLYCONTROL_SERVER_CLINET_RECV_SEND
        if (LY_UAVFlyControl::Get().getNetServerClient())
        {
            LY_UAVFlyControl::Get().NetServerClientSend(message, msglength, QString::fromStdString(strComponentID));
        }
        else
#endif
        {
            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType",   std::to_string(nNodeType));
            props.put("SourceID",     strNodeId);
            props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID",     strChId);
            props.put("WeaponID",     strComponentID);
            props.put("ProtocolType", strProtocolType);
            IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", message, msglength, props));
        }
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("PX4 MavLink Temporarily unsupported, Instruction not send"), QMessageBox::Yes);
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("DJi Temporarily unsupported, Instruction not send!"), QMessageBox::Yes);
    }
    else
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Dronoe The protocol type is not set or Temporarily unsupported, Instruction not send?"), QMessageBox::Yes);
    }
}

