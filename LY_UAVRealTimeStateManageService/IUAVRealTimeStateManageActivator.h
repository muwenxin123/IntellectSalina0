#ifndef IUAVREALTIMESTATEMANAGEACTIVATOR_H
#define IUAVREALTIMESTATEMANAGEACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "IUAVRealTimeStateManageImpl.h"

class IUAVRealTimeStateManageActivator
        : public qnzkna::framework::IBundleActivator
        , public qnzkna::framework::IMessageInterfaceTracker
{
public:
    IUAVRealTimeStateManageActivator();
    virtual ~IUAVRealTimeStateManageActivator();

    void OnTimer(int timeID);

    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );
    virtual bool dealingMessage( const qnzkna::framework::MessageReference& ref );

private:
    qnzkna::framework::IRegistry*                               m_IRegistryImpl;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageImpl m_IUAVRealTimeStateManageImpl;
};

#endif
