#pragma once
//#ifndef LY_AgriFertilizerPrescriptionPloughActivator_H
//#define LY_AgriFertilizerPrescriptionPloughActivator_H


#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/TextToSpeechManage/ITextToSpeechManageService.h"
#include "DataManage/AgriSevice/IAgricultureHomePageService.h"
#include "DataManage/AgriSevice/IAgriSprayPrescriptionService.h"
#include "DataManage/AgriSevice/IAgriFertilizerPrescripyionService.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/IAgriSoilTestingAnalysisService.h"
#include "IWeaponManageService.h"

class LY_AgriFertilizerPrescriptionActivator
	: public qnzkna::framework::IBundleActivator
	, public qnzkna::framework::IMessageInterfaceTracker
	, public qnzkna::framework::IServiceInterfaceTracker
{
public:

    LY_AgriFertilizerPrescriptionActivator();
    virtual ~LY_AgriFertilizerPrescriptionActivator();

    static LY_AgriFertilizerPrescriptionActivator &Get();


	qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService         *getPITextToSpeechManageService() const;
	qnzkna::IAgricultureHomePage::IAgricultureHomePageService				*getIAgricultureHomePageService() const;
	qnzkna::ZoneManage::IZoneManageService							*getPIZoneManageService() const;
	qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService*			         getIAgriSoilTestingAnalysisService() const;
	qnzkna::WeaponManage::IWeaponManageService						*getPIWeaponManageService() const;
	qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService *		getPAgriSprayPrescriptionService() const;
	qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionService *		getAgriFertilizerPrescriptionService() const;

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
	qnzkna::IAgricultureHomePage::IAgricultureHomePageService*			         m_pIAgricultureHomePageService;
	qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService *		 m_pAgriSprayPrescriptionService;
	qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService*			         m_pAgriSoilTestingAnalysisService;
	qnzkna::WeaponManage::IWeaponManageService                   *m_pIWeaponManageService;
	qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionService                   *m_pIAgriFertilizerPrescriptionService;

	
};


//#endif // LY_AgriFertilizerPrescriptionPloughActivator_H
