#ifndef IAERIALSHOTSETTINGBUNDLEACTIVATOR_H
#define IAERIALSHOTSETTINGBUNDLEACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IFireSpreadService.h"
#include "IMarkManageService.h"

class IZoneManagerActivator
    : public qnzkna::framework::IBundleActivator
    , public qnzkna::framework::IMessageInterfaceTracker
    , public qnzkna::framework::IServiceInterfaceTracker
{
public:
    IZoneManagerActivator();
    virtual ~IZoneManagerActivator();

    static IZoneManagerActivator &Get();

    qnzkna::framework::IRegistry									*getIRegistryImpl() const;
	qnzkna::ZoneManage::IZoneManageService							*getPIZoneManageService() const;
	qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;
	qnzkna::FireSpread::IFireSpreadService *getPIFireSpreadService() const;
	qnzkna::MarkManage::IMarkManageService *getPIMarkManageService() const;

public:
    virtual void start(qnzkna::framework::IRegistry *const context);
    virtual void stop(qnzkna::framework::IRegistry *const context);

    virtual bool addingService(const qnzkna::framework::ServiceReference &service);
    virtual bool removedService(const qnzkna::framework::ServiceReference &service);
	bool sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID);
    virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
    qnzkna::framework::IRegistry                                        *m_IRegistryImpl;
	qnzkna::ZoneManage::IZoneManageService								*m_pIZoneManageService;
	qnzkna::TaskManage::INebulaTaskManageService						*m_pINebulaTaskManageService;
	qnzkna::FireSpread::IFireSpreadService *m_pIFireSpreadService;
	qnzkna::MarkManage::IMarkManageService* m_pIMarkManageService;

    unsigned int                                                         m_SeatID;

};

#endif
