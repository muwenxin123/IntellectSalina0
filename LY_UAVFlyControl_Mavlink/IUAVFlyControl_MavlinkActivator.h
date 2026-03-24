#ifndef IUAVFLYCONTROLACTIVATOR_MAVLINK_H
#define IUAVFLYCONTROLACTIVATOR_MAVLINK_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/JoystickManage/IJoystickManageService.h"
#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "IZoneManageService.h"

class IUAVFlyControl_MavlinkActivator
        : public qnzkna::framework::IBundleActivator
        , public qnzkna::framework::IMessageInterfaceTracker
        , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IUAVFlyControl_MavlinkActivator();
    virtual ~IUAVFlyControl_MavlinkActivator();

    static IUAVFlyControl_MavlinkActivator &Get();

    bool sendMessage(const char * const sendMsg, const unsigned int msgLength, const unsigned long long weaponId);

    qnzkna::framework::IRegistry                                   *getIRegistryImpl()                  const;
    qnzkna::PositionManage::IPositionManageService                 *getPositionManageService()          const;
    qnzkna::SensorManage::ISensorManageService                     *getSensorManageService()            const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getIUAVRealTimeStateManageService() const;
    qnzkna::JoystickManage::IJoystickManageService                 *getJoystickManageService()          const;
    qnzkna::SystemMaintenance::ISystemMaintenanceService           *getSystemMaintenanceService()       const;
	qnzkna::TaskManage::INebulaTaskManageService				   *getPTaskManageService()				const;
	qnzkna::ZoneManage::IZoneManageService						   *getZoneManageService()				const;

public:
    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );

    virtual bool addingService( const qnzkna::framework::ServiceReference& service );
    virtual bool removedService( const qnzkna::framework::ServiceReference& service );
    virtual bool dealingMessage( const qnzkna::framework::MessageReference& message );

private:
    qnzkna::framework::IRegistry*                                             m_IRegistryImpl;

    qnzkna::SystemConfig::IConfigProjectControlService*                       m_pIConfigProjectControlService;
    qnzkna::WeaponManage::IWeaponManageService*                               m_pIWeaponManageService;
    qnzkna::SystemMaintenance::ISystemMaintenanceService*                     m_pSystemMaintenanceService;
    qnzkna::PositionManage::IPositionManageService*                           m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService*                               m_pISensorManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*           m_pIUAVRealTimeStateManageService;
    qnzkna::JoystickManage::IJoystickManageService*                           m_pIJoystickManageService;
	qnzkna::TaskManage::INebulaTaskManageService*			             m_pITaskManageService;
	qnzkna::ZoneManage::IZoneManageService*									m_pIZoneManageService;
};

#endif
