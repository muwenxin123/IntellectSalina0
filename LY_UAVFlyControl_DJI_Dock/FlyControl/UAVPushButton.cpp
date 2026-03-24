#include "UAVPushButton.h"

#define USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMWEAPONSELECTCONTROL_ENABLE
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMWEAPONSELECTCONTROL_ENABLE

#include <QDateTime>

#include "LY_UAVFlyControl_DJI_DockInterface.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"

void SystemWeaponSelectControl_SendMsgToAll(const unsigned long long int    weaponId,
                                            const unsigned long long int    vehicleID,
                                            const QString                   strComponentID,
                                            const QString                   strComponentName,
                                            const bool                      select
                                        )
{
    if(weaponId == 0){
        return ;
    }

    if(vehicleID == 0){
        return ;
    }

    if(strComponentName.isEmpty()){
        return ;
    }

    if(strComponentID.isEmpty()){
        return ;
    }

    qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemWeaponSelectControl systemWeaponSelectControl;
    systemWeaponSelectControl.head.id             = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMWEAPONSELECTCONTROL;
    systemWeaponSelectControl.head.time           = QDateTime::currentMSecsSinceEpoch();
    systemWeaponSelectControl.weaponType          = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemWeaponSelectControl_WeaponType_Enum::SystemWeaponSelectControl_WeaponType_UAV;
    systemWeaponSelectControl.weaponId            = weaponId;
    systemWeaponSelectControl.vehicleID           = vehicleID;
    systemWeaponSelectControl.strComponentID      = strComponentID;
    systemWeaponSelectControl.strComponentName    = strComponentName;
    systemWeaponSelectControl.select              = select;

    void *lParam = (void *)(&systemWeaponSelectControl);
    if(lParam == nullptr){
        return ;
    }

    LY_UAVFlyControl_DJI_DockInterface::Get().SendMsgToAll(IInterface::MSG_Command, QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID, lParam);
}
#endif

UAVPushButton::UAVPushButton(const QString& weaponId, const QString& vehicleID, const QObject* vehicle,
                             const QString& strComponentName, const QString& strComponentID,
                             const QString& strProtocolType,
                             QWidget *parent)
    : QPushButton(strComponentName, parent)
    , m_weaponId(weaponId)
    , m_vehicleID(vehicleID)
    , m_vehicle(vehicle)
    , m_strComponentName(strComponentName)
    , m_strComponentID(strComponentID)
    , m_strProtocolType(strProtocolType)
{
    setText(strComponentName);

    setStyleSheet(QString("UAVPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));

    connect(this, &QPushButton::clicked, this, &UAVPushButton::UAVPushButtonClicked);
}

UAVPushButton::~UAVPushButton()
{
}

void UAVPushButton::UAVPushButtonClicked()
{
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMWEAPONSELECTCONTROL_ENABLE
    // SystemWeaponSelectControl_SendMsgToAll(m_weaponId, m_vehicleID, m_strComponentID, m_strComponentName, this->isChecked());
#endif
}

const QString& UAVPushButton::getWeaponId() const
{
    return m_weaponId;
}

const QString& UAVPushButton::getVehicleID() const
{
    return m_vehicleID;
}

const QObject *UAVPushButton::getVehicle() const
{
    return m_vehicle;
}

const QString &UAVPushButton::getStrComponentID() const
{
    return m_strComponentID;
}

const QString &UAVPushButton::getStrProtocolType() const
{
    return m_strProtocolType;
}

const QString &UAVPushButton::getStrComponentName() const
{
    return m_strComponentName;
}
