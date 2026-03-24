#ifndef IUAVFLYCONTROLACTIVATOR_H
#define IUAVFLYCONTROLACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/ISensorManageService.h"
#include "DataManage/JoystickManage/IJoystickManageService.h"
#include "DataManage/ICommandUnitManageService.h"

class ISystemMaintenanceActivator
        : public qnzkna::framework::IBundleActivator
        , public qnzkna::framework::IServiceInterfaceTracker
{
public:
	ISystemMaintenanceActivator();
    virtual ~ISystemMaintenanceActivator();
    static ISystemMaintenanceActivator &Get();

    qnzkna::framework::IRegistry							*getIRegistryImpl()					const;
    qnzkna::SystemConfig::IConfigProjectControlService		*getConfigProjectControlService()	const;
    qnzkna::WeaponManage::IWeaponManageService				*getWeaponManageService()			const;
    qnzkna::PositionManage::IPositionManageService			*getPositionManageService()			const;
    qnzkna::SensorManage::ISensorManageService				*getSensorManageService()			const;
    qnzkna::JoystickManage::IJoystickManageService			*getJoystickManageService()			const;
	qnzkna::CCommandUnitManage::ICommandUnitManageService	*getCommandUnitManageService()		const;

	bool InitWeaponSystemMap();
	qnzkna::WeaponManage::WeaponSystemMap					*getWeaponSystems();
	qnzkna::WeaponManage::MultiWeaponComponentMap			*getUAVComponents();

public:
    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );

    virtual bool addingService( const qnzkna::framework::ServiceReference& service );
    virtual bool removedService( const qnzkna::framework::ServiceReference& service );

private:
    qnzkna::framework::IRegistry*							m_IRegistryImpl;
    qnzkna::SystemConfig::IConfigProjectControlService*		m_pIConfigProjectControlService;
    qnzkna::WeaponManage::IWeaponManageService*				m_pIWeaponManageService;
    qnzkna::PositionManage::IPositionManageService*			m_pIPositionManageService;
    qnzkna::SensorManage::ISensorManageService*				m_pISensorManageService;
    qnzkna::JoystickManage::IJoystickManageService*			m_pIJoystickManageService;
	qnzkna::CCommandUnitManage::ICommandUnitManageService*	m_pICommandUnitManageService;
	qnzkna::WeaponManage::WeaponSystemMap					m_mapWeaponSystem;
	qnzkna::WeaponManage::MultiWeaponComponentMap			m_mapWeaponComponent;
	bool													m_bInitWeaponSystemMap;
};

#endif
