#ifndef IVIDEOREALTIMEPREVIEWACTIVATOR_H
#define IVIDEOREALTIMEPREVIEWACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "DataManage/ISensorVedioService.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/ISystemMaintenanceService.h"
#include "DataManage/IZKLYTargetManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/ISensorManageService.h"

#include<QMap>

class IVideoRealTimePreviewActivator 
	: public qnzkna::framework::IBundleActivator
	, public qnzkna::framework::IMessageInterfaceTracker
	, public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IVideoRealTimePreviewActivator();
	virtual ~IVideoRealTimePreviewActivator();

	static IVideoRealTimePreviewActivator &Get();

	qnzkna::framework::IRegistry                                *getIRegistryImpl() const;
	qnzkna::SensorVedio::ISensorVedioService					*getPISensorVedioService() const;
	qnzkna::WeaponManage::IWeaponManageService						*getPWeaponManageService() const;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService				*getPIUAVRealTimeStateManageService() const;
	qnzkna::SystemMaintenance::ISystemMaintenanceService           *getISystemMaintenanceService() const;
	qnzkna::TargetManage::IZKLYTargetManageService						*getPZKLYTargetManageService() const;
	qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;
	qnzkna::SensorManage::ISensorManageService						*getPISensorManageService() const;

public:
	virtual void start(qnzkna::framework::IRegistry *const context);
	virtual void stop(qnzkna::framework::IRegistry *const context);

	virtual bool addingService(const qnzkna::framework::ServiceReference &service);
	virtual bool removedService(const qnzkna::framework::ServiceReference &service);
	virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
	qnzkna::framework::IRegistry                        *m_IRegistryImpl;
	qnzkna::SensorVedio::ISensorVedioService			*m_pISensorVedioService;
	qnzkna::WeaponManage::IWeaponManageService				             *m_pIWeaponManageService;
	qnzkna::TargetManage::IZKLYTargetManageService						 *m_pITargetManageService;
	qnzkna::SystemMaintenance::ISystemMaintenanceService                *m_pISystemMaintenanceService;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService      *m_pIUAVRealTimeStateManageService;
	qnzkna::TaskManage::INebulaTaskManageService						 *m_pINebulaTaskManageService;
	qnzkna::SensorManage::ISensorManageService							*m_pISensorManageService;
	QMap<std::string, long long>										m_sensorID2ComponentIDMap;
};

#endif // IVIDEOREALTIMEPREVIEWACTIVATOR_H