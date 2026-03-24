#ifndef ICOLLABORATIVESEARCHACTIVATOR_H
#define ICOLLABORATIVESEARCHACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/IFormationManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/ICommandUnitManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/IZoneManageService.h"

class ICollaborativeSearchActivator
        : public qnzkna::framework::IBundleActivator
            , public qnzkna::framework::IMessageInterfaceTracker
            , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    ICollaborativeSearchActivator();

    virtual ~ICollaborativeSearchActivator();

    static ICollaborativeSearchActivator &Get();

    qnzkna::framework::IRegistry									*getIRegistryImpl() const;
    qnzkna::SystemConfig::IConfigProjectControlService				*getPIConfigProjectControlService() const;
    qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;
    qnzkna::WeaponManage::IWeaponManageService						*getPWeaponManageService() const;
    qnzkna::UAVFormManage::IFormationManageService					*getPFormationManageService() const;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService	*getPIUAVRealTimeStateManageService() const;
    qnzkna::MarkManage::IMarkManageService					*getPIMarkManageService() const;
    qnzkna::CCommandUnitManage::ICommandUnitManageService			*getPICommandUnitManageService() const;
	qnzkna::SensorManage::ISensorManageService                     *getPISensorManageService() const;
	qnzkna::ZoneManage::IZoneManageService							 *getPIZoneManageService() const;

    bool sendMessage(const char* const sendMsg, const unsigned int length, const std::string& currentComponentID);

public:
    virtual void start(qnzkna::framework::IRegistry* const context);
    virtual void stop(qnzkna::framework::IRegistry* const context);

    virtual bool addingService(const qnzkna::framework::ServiceReference& service);
    virtual bool removedService(const qnzkna::framework::ServiceReference& service);
    virtual bool dealingMessage(const qnzkna::framework::MessageReference& message);

private:
    qnzkna::framework::IRegistry*							             m_IRegistryImpl;
    qnzkna::SystemConfig::IConfigProjectControlService*		             m_pIConfigProjectControlService;
    qnzkna::TaskManage::INebulaTaskManageService*			             m_pITaskManageService;
    qnzkna::WeaponManage::IWeaponManageService*				             m_pIWeaponManageService;
    qnzkna::UAVFormManage::IFormationManageService*			             m_pIFormationManageService;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*      m_pIUAVRealTimeStateManageService;
    qnzkna::MarkManage::IMarkManageService*						 m_pIMarkManageService;
    qnzkna::CCommandUnitManage::ICommandUnitManageService*				 m_pICommandUnitManageService;
	qnzkna::SensorManage::ISensorManageService                          *m_pISensorManageService;
	qnzkna::SensorManage::SensorInfoMap                                  m_SensorInfoMap;
	qnzkna::ZoneManage::IZoneManageService*								 m_pIZoneManageService;
};

#endif
