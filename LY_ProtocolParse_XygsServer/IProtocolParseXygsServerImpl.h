#ifndef IPROTOCOLPARSEXYGSSERVERIMPL_H
#define IPROTOCOLPARSEXYGSSERVERIMPL_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "Protocol/XygsJL/Bag_XygsJLDefines.h"

#include "LogRecord.h"

class IProtocolParseXygsServerImpl
{
public:
    IProtocolParseXygsServerImpl(void);
    ~IProtocolParseXygsServerImpl(void);

    bool PackXygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext);
    bool UnPackXygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext);
};

#endif
