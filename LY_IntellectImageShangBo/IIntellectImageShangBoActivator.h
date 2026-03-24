#ifndef IINTELLECTIMAGESHANGBOACTIVATOR_H
#define IINTELLECTIMAGESHANGBOACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IWeaponManageService.h"

#include "DataManage/IUAVRealTimeStateManageService.h"

#include "DataManage/IMarkManageService.h"

class IIntellectImageShangBoActivator
    : public qnzkna::framework::IBundleActivator

    , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IIntellectImageShangBoActivator();
    virtual ~IIntellectImageShangBoActivator();

    static IIntellectImageShangBoActivator &Get();

    qnzkna::framework::IRegistry                                  *getIRegistryImpl() const;
    qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
    qnzkna::WeaponManage::IWeaponManageService                     *getPIWeaponManageService() const;
    qnzkna::SystemMaintenance::ISystemMaintenanceService           *getPISystemMaintenanceService() const;
    qnzkna::PositionManage::IPositionManageService                 *getPIPositionManageService() const;
    qnzkna::SensorManage::ISensorManageService                     *getPISensorManageService() const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getPIUAVRealTimeStateManageService() const;
    qnzkna::MarkManage::IMarkManageService                         *getPITargetManageService() const;
public:
    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );

    virtual bool addingService( const qnzkna::framework::ServiceReference& service );
    virtual bool removedService( const qnzkna::framework::ServiceReference& service );
    virtual bool dealingMessage( const qnzkna::framework::MessageReference& message );

private:
    qnzkna::framework::IRegistry*                                           m_IRegistryImpl;
    qnzkna::SystemConfig::IConfigProjectControlService*                     m_pIConfigProjectControlService;
    qnzkna::WeaponManage::IWeaponManageService*                             m_pIWeaponManageService;
    qnzkna::SystemMaintenance::ISystemMaintenanceService*                   m_pISystemMaintenanceService;
    qnzkna::PositionManage::IPositionManageService*                         m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService*                             m_pISensorManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*         m_pIUAVRealTimeStateManageService;
    qnzkna::MarkManage::IMarkManageService*                                 m_pITargetManageService;
};

#endif
