#ifndef IUAVNAVIGATIONARMSHOTALONE54ACTIVATOR_H
#define IUAVNAVIGATIONARMSHOTALONE54ACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/IZKLYTargetManageService.h"
#include "Protocol/XygsZC54/Bag_XygsZC54Defines.h"
#include "QMap"
#include "QMutex"
#include "DataManage/ISensorVedioService.h"

typedef struct __Component_Property_Component54
{
    unsigned long int			sysID;
    std::string					component_ComponentSysID;
    std::string					component_ComponentName;
    std::string					component_ComponentID;
    std::string					component_SensorName;
    std::string					component_SensorID;
    AMapPod54ControlStatus		aMapPod54ControlStatus;

    __Component_Property_Component54()
    {
        sysID = 0;
        component_ComponentSysID = "";
        component_ComponentName = "";
        component_ComponentID = "";
        component_SensorName = "";
        component_SensorID = "";
        memset(&aMapPod54ControlStatus, 0x00, sizeof(AMapPod54ControlStatus));
    }
} Component_Property_Component54;

class IUAVNavigationArmShotAlone54Activator
    : public qnzkna::framework::IBundleActivator
    , public qnzkna::framework::IMessageInterfaceTracker
    , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IUAVNavigationArmShotAlone54Activator();
    virtual ~IUAVNavigationArmShotAlone54Activator();

    static IUAVNavigationArmShotAlone54Activator &Get();

    qnzkna::framework::IRegistry                                  *getIRegistryImpl() const;
    qnzkna::SystemMaintenance::ISystemMaintenanceService           *getPISystemMaintenanceService() const;
    qnzkna::PositionManage::IPositionManageService                 *getPIPositionManageService() const;
    qnzkna::SensorManage::ISensorManageService                     *getPISensorManageService() const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getPIUAVRealTimeStateManageService() const;
    qnzkna::TargetManage::IZKLYTargetManageService					*getPITargetManageService() const;
	qnzkna::SensorVedio::ISensorVedioService					*getPISensorVedioService() const;

public:
    virtual void start(qnzkna::framework::IRegistry *const context);
    virtual void stop(qnzkna::framework::IRegistry *const context);

    virtual bool addingService(const qnzkna::framework::ServiceReference &service);
    virtual bool removedService(const qnzkna::framework::ServiceReference &service);
    virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

    bool sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID, const std::string &currentComponentID);

    void InitWeaponInfoList();
    void SetPod54ControlStatusInfo(const QString &strID, const AMapPod54ControlStatus &stData);
    AMapPod54ControlStatus GetPod54ControlStatusInfo(const QString strID);
    AMapPod54ControlStatus GetPod54ControlStatusInfoByUAVID(const QString strID);
private:
    qnzkna::framework::IRegistry                                           *m_IRegistryImpl;
    qnzkna::SystemMaintenance::ISystemMaintenanceService                   *m_pISystemMaintenanceService;
    qnzkna::PositionManage::IPositionManageService                         *m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService                             *m_pISensorManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService         *m_pIUAVRealTimeStateManageService;
    qnzkna::TargetManage::IZKLYTargetManageService						 *m_pITargetManageService;
	qnzkna::SensorVedio::ISensorVedioService*									m_pISensorVedioService;
    QMap<QString, Component_Property_Component54>							m_Component_Property_Component54Map;
    QMutex																	sMutex;
};

#endif
