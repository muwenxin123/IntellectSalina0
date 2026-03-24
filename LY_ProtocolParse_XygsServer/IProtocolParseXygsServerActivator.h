#ifndef IPROTOCOLPARSEXYGSSERVERACTIVATOR_H
#define IPROTOCOLPARSEXYGSSERVERACTIVATOR_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "DataManage/IWeaponManageService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/NebulaUAVState/NebulaStatePointInfo.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"

#include "IProtocolParseXygsServerImpl.h"

class IProtocolParseXygsServerActivator
        : public qnzkna::framework::IBundleActivator
        , public qnzkna::framework::IMessageInterfaceTracker
{
public:
    IProtocolParseXygsServerActivator();
    virtual ~IProtocolParseXygsServerActivator();

    static IProtocolParseXygsServerActivator &Get();

    qnzkna::framework::IRegistry *getIRegistryImpl() const;

public:
    virtual void start(qnzkna::framework::IRegistry* const context );
    virtual void stop(qnzkna::framework::IRegistry* const context );

    virtual bool dealingMessage( const qnzkna::framework::MessageReference& message );

private:
    qnzkna::framework::IRegistry*                        m_IRegistryImpl;
    IProtocolParseXygsServerImpl                         m_implProtocolXygsServer;
};

#endif
