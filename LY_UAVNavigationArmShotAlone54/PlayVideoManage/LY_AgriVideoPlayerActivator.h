#pragma once
//#ifndef LY_AgriQuickSprayServicePloughActivator_H
//#define LY_AgriQuickSprayServicePloughActivator_H


#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/TextToSpeechManage/ITextToSpeechManageService.h"

class LY_AgriVideoPlayerActivator
	: public qnzkna::framework::IBundleActivator
	, public qnzkna::framework::IMessageInterfaceTracker
	, public qnzkna::framework::IServiceInterfaceTracker
{
public:

	LY_AgriVideoPlayerActivator();
    virtual ~LY_AgriVideoPlayerActivator();

    static LY_AgriVideoPlayerActivator &Get();


	qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService         *getPITextToSpeechManageService() const;

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
};


//#endif // LY_AgriQuickSprayServicePloughActivator_H
