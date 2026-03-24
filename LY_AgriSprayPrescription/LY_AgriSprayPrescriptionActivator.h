#pragma once
//#ifndef LY_AgriSprayPrescriptionPloughActivator_H
//#define LY_AgriSprayPrescriptionPloughActivator_H


#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/TextToSpeechManage/ITextToSpeechManageService.h"
#include "DataManage/IAgriFarmManagerService.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/AgriSevice/IAgriSprayPrescriptionService.h"
#include "IWeaponManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"

class LY_AgriSprayPrescriptionActivator
	: public qnzkna::framework::IBundleActivator
	, public qnzkna::framework::IMessageInterfaceTracker
	, public qnzkna::framework::IServiceInterfaceTracker
{
public:

    LY_AgriSprayPrescriptionActivator();
    virtual ~LY_AgriSprayPrescriptionActivator();

    static LY_AgriSprayPrescriptionActivator &Get();


	qnzkna::framework::IRegistry									*getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService				*getPIConfigProjectControlService() const;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService			*getPITextToSpeechManageService() const;
    qnzkna::AgriFarmManager::IAgriFarmManagerService				*getPIAgriFarmManagerService() const;
	qnzkna::ZoneManage::IZoneManageService							*getPIZoneManageService() const;
	qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService 	*getPIAgriSprayPrescriptionService() const;
	qnzkna::WeaponManage::IWeaponManageService						*getPIWeaponManageService()const;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService   *getPIUAVRealTimeStateManageService() const;
public:

	virtual void start(qnzkna::framework::IRegistry *const context);
	virtual void stop(qnzkna::framework::IRegistry *const context);

	virtual bool addingService(const qnzkna::framework::ServiceReference &service);
	virtual bool removedService(const qnzkna::framework::ServiceReference &service);
	virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
	qnzkna::framework::IRegistry                                        *m_IRegistryImpl;
	qnzkna::SystemConfig::IConfigProjectControlService                  *m_pIConfigProjectControlService;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService              *m_pITextToSpeechManageService;

    qnzkna::ZoneManage::IZoneManageService*								 m_pIZoneManageService;
    qnzkna::AgriFarmManager::IAgriFarmManagerService*			         m_pIAgriFarmManagerService;
	qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService *		 m_pAgriSprayPrescriptionService;
	qnzkna::WeaponManage::IWeaponManageService*							 m_pIWeaponManageService;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*     m_pIUAVRealTimeStateManageService;

};


//#endif // LY_AgriSprayPrescriptionPloughActivator_H
