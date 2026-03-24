#ifndef ISCENARIOSREPLAYACTIVATOR_H
#define ISCENARIOSREPLAYACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"

class IScenariosReplayActivator: public qnzkna::framework::IBundleActivator ,
	                                               public  qnzkna::framework::IServiceInterfaceTracker
{
public:
    IScenariosReplayActivator();
    virtual ~IScenariosReplayActivator();

    static IScenariosReplayActivator &Get();

    qnzkna::framework::IRegistry *getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService		*getConfigProjectControlService()	const;

public:
    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );

	virtual bool addingService(const qnzkna::framework::ServiceReference& service);
	virtual bool removedService(const qnzkna::framework::ServiceReference& service);
    virtual bool dealingMessage( const qnzkna::framework::MessageReference& message );

private:
    qnzkna::framework::IRegistry*                        m_IRegistryImpl;
	qnzkna::SystemConfig::IConfigProjectControlService*		m_pIConfigProjectControlService;
};

#endif
