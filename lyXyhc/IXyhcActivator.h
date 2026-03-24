#ifndef IXYHCACTIVATOR_H
#define IXYHCACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/JoystickManage/IJoystickManageService.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/INebulaTaskManageService.h"

class IXyhcActivator
    : public qnzkna::framework::IBundleActivator
    , public qnzkna::framework::IMessageInterfaceTracker
    , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IXyhcActivator();
    virtual ~IXyhcActivator();

    static IXyhcActivator &Get();

    qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;

    qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
    qnzkna::SystemMaintenance::ISystemMaintenanceService           *getISystemMaintenanceService() const;
    qnzkna::PositionManage::IPositionManageService                 *getPIPositionManageService() const;
    qnzkna::SensorManage::ISensorManageService                     *getPISensorManageService() const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getPIUAVRealTimeStateManageService() const;
    qnzkna::JoystickManage::IJoystickManageService                 *getPIJoystickManageService() const;
    qnzkna::MarkManage::IMarkManageService                 *getPIMarkManageService() const;
    qnzkna::WeaponManage::IWeaponManageService						*getPWeaponManageService() const;
    qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;

    bool sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID);

public:
    virtual void start(qnzkna::framework::IRegistry *const context);
    virtual void stop(qnzkna::framework::IRegistry *const context);

    virtual bool addingService(const qnzkna::framework::ServiceReference &service);
    virtual bool removedService(const qnzkna::framework::ServiceReference &service);
    virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
    qnzkna::framework::IRegistry                                        *m_IRegistryImpl;

    qnzkna::SystemConfig::IConfigProjectControlService                  *m_pIConfigProjectControlService;
    qnzkna::SystemMaintenance::ISystemMaintenanceService                *m_pISystemMaintenanceService;
    qnzkna::PositionManage::IPositionManageService                      *m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService                          *m_pISensorManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService      *m_pIUAVRealTimeStateManageService;
    qnzkna::JoystickManage::IJoystickManageService                      *m_pIJoystickManageService;
    qnzkna::MarkManage::IMarkManageService                      *m_pIMarkManageService;
    qnzkna::SensorManage::SensorInfoMap                                  m_SensorInfoMap;
    qnzkna::WeaponManage::IWeaponManageService                          *m_pIWeaponManageService;
    qnzkna::TaskManage::INebulaTaskManageService			             *m_pITaskManageService;
};

#endif
