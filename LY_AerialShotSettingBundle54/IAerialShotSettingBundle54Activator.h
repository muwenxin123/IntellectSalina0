#ifndef IAERIALSHOTSETTINGBUNDLE54ACTIVATOR_H
#define IAERIALSHOTSETTINGBUNDLE54ACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/JoystickManage/IJoystickManageService.h"
#include "DataManage/IZKLYTargetManageService.h"

class IAerialShotSettingBundle54Activator
    : public qnzkna::framework::IBundleActivator
    , public qnzkna::framework::IMessageInterfaceTracker
    , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IAerialShotSettingBundle54Activator();
    virtual ~IAerialShotSettingBundle54Activator();

    static IAerialShotSettingBundle54Activator &Get();

    qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
    qnzkna::SystemMaintenance::ISystemMaintenanceService           *getSystemMaintenanceService() const;
    qnzkna::PositionManage::IPositionManageService                 *getPIPositionManageService() const;
    qnzkna::SensorManage::ISensorManageService                     *getPISensorManageService() const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getPIUAVRealTimeStateManageService() const;
    qnzkna::JoystickManage::IJoystickManageService                 *getPIJoystickManageService() const;
    qnzkna::TargetManage::IZKLYTargetManageService					*getPTIZKLYTargetManageService() const;

    bool sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID, const std::string &currentComponentID);

public:
    virtual void start(qnzkna::framework::IRegistry *const context);
    virtual void stop(qnzkna::framework::IRegistry *const context);

    virtual bool addingService(const qnzkna::framework::ServiceReference &service);
    virtual bool removedService(const qnzkna::framework::ServiceReference &service);
    virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
    qnzkna::framework::IRegistry                                        *m_IRegistryImpl;
    qnzkna::SystemMaintenance::ISystemMaintenanceService                *m_pSystemMaintenanceService;
    qnzkna::PositionManage::IPositionManageService                      *m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService                          *m_pISensorManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService      *m_pIUAVRealTimeStateManageService;
    qnzkna::JoystickManage::IJoystickManageService                      *m_pIJoystickManageService;
    qnzkna::TargetManage::IZKLYTargetManageService						 *m_pIZKLYTargetManageService;
};

#endif
