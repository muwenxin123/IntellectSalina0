#ifndef IPROTOCOLPARSEXYGSSERVERIMPL_CONTROL_H
#define IPROTOCOLPARSEXYGSSERVERIMPL_CONTROL_H

#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "IProtocolParseXygsServerActivator.h"

#include "IProtocolParseXygsServerImpl_Control_Handle.h"

#include <list>

#include <QString>

namespace qnzkna {

namespace NetSeverClient {

class IProtocolParseXygsServerImpl_Control
{
public:
    virtual void start();
    virtual void stop();
    virtual bool Pack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) = 0;
    virtual bool UnPack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) = 0;
    virtual bool NetRecv_XygsServer(const int nLength, const char * const pRecvBuf) = 0;
    virtual bool NetHeader_XygsServer(const int nLength, const char * const pRecvBuf) = 0;

    static bool getNetServerClient() ;

    static unsigned int getSeatID() ;

    const std::string &getMessageName() const;

protected:
    IProtocolParseXygsServerImpl_Control(
            const std::string&  messageName,
            const unsigned char ucType,
            const unsigned char unNodeType
            );
    virtual ~IProtocolParseXygsServerImpl_Control();

    static int NetSend_XygsServer(const int nLength, const char * const pSendBuf);

private:
    IProtocolParseXygsServerImpl_Control(const IProtocolParseXygsServerImpl_Control&) = delete;
    IProtocolParseXygsServerImpl_Control(const IProtocolParseXygsServerImpl_Control&&) = delete;
    IProtocolParseXygsServerImpl_Control& operator=(const IProtocolParseXygsServerImpl_Control&) = delete;
    IProtocolParseXygsServerImpl_Control& operator=(const IProtocolParseXygsServerImpl_Control&&) = delete;

private:
    static bool                                             m_NetServerClient;
    static unsigned int                                     m_SeatID;

protected:
    const std::string                                       m_MessageName;
    const std::string                                       m_NodeTypeString;
    const unsigned char                                     m_ucType;
    const unsigned char                                     m_unNodeType;

    friend class IProtocolParseXygsServerImpl_Control_HandleList;
};

const std::list<IProtocolParseXygsServerImpl_Control *>& IProtocolParseXygsServerImpl_ControlList();

void IProtocolParseXygsServerImpl_Start();

void IProtocolParseXygsServerImpl_Stop();

void IProtocolParseXygsServerImpl_RecvMessage(const int nLength, const char * const lpData);

}

}

#endif
