#pragma once
//#ifndef LY_AgricultureHomepageActivator_H
//#define LY_AgricultureHomepageActivator_H


#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
//#include "DataManage/INebulaTaskManageService.h"
#include "SystemConfig/IConfigProjectControlService.h"
//#include "DataManage/IWeaponManageService.h"
//#include "DataManage/IPositionManageService.h"
//#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/TextToSpeechManage/ITextToSpeechManageService.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/IAgriFarmManagerService.h"
#include "DataManage/AgriSevice/IAgricultureHomePageService.h"

class LY_AgricultureHomepageActivator
	: public qnzkna::framework::IBundleActivator
	, public qnzkna::framework::IMessageInterfaceTracker
	, public qnzkna::framework::IServiceInterfaceTracker
{
public:

    LY_AgricultureHomepageActivator();
    virtual ~LY_AgricultureHomepageActivator();

    static LY_AgricultureHomepageActivator &Get();


	qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
	qnzkna::ZoneManage::IZoneManageService							 *getPIZoneManageService() const;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService         *getPITextToSpeechManageService() const;
	qnzkna::AgriFarmManager::IAgriFarmManagerService				*getPIAgriFarmManagerService() const;
	qnzkna::IAgricultureHomePage::IAgricultureHomePageService				*getIAgricultureHomePageService() const;

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
	qnzkna::IAgricultureHomePage::IAgricultureHomePageService*			         m_pIAgricultureHomePageService;
};


//#endif // LY_AgricultureHomepageActivator_H
