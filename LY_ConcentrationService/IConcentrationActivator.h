#ifndef IAGRILAYERCONTROLACTIVATOR_H_
#define IAGRILAYERCONTROLACTIVATOR_H_
 
#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/INebulaTaskManageService.h"

class IConcentrationActivator
	: public qnzkna::framework::IBundleActivator

	, public qnzkna::framework::IServiceInterfaceTracker
{
public:
	IConcentrationActivator();
	virtual ~IConcentrationActivator();

	static IConcentrationActivator &Get();

	qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
	qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;
public:
	virtual void start(qnzkna::framework::IRegistry *const context);
	virtual void stop(qnzkna::framework::IRegistry *const context);

	virtual bool addingService(const qnzkna::framework::ServiceReference &service);
	virtual bool removedService(const qnzkna::framework::ServiceReference &service);
	virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
	qnzkna::framework::IRegistry*                                        m_IRegistryImpl;
	qnzkna::TaskManage::INebulaTaskManageService*			             m_pITaskManageService;
};

#endif  // IConcentrationActivator_H