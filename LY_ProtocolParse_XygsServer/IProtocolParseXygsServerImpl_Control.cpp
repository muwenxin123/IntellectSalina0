#include "IProtocolParseXygsServerImpl_Control.h"

#include "LY_ProtocolParse_XygsServer.h"

#include <QSettings>

using namespace qnzkna::NetSeverClient;

bool                                             IProtocolParseXygsServerImpl_Control::m_NetServerClient                                  = true;
unsigned int                                     IProtocolParseXygsServerImpl_Control::m_SeatID                                           = 0;

IProtocolParseXygsServerImpl_Control::IProtocolParseXygsServerImpl_Control(
        const std::string&  messageName,
        const unsigned char ucType,
        const unsigned char unNodeType)
    : m_MessageName(messageName)
    , m_NodeTypeString(std::to_string(unNodeType))
    , m_ucType(ucType)
    , m_unNodeType(unNodeType)
{

    QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);
    settings.beginGroup("NetServerClient");
    if(settings.contains("ServerClient")){
        m_NetServerClient = settings.value("ServerClient", true).toBool();
    }
    settings.endGroup();

    settings.beginGroup("NetServerClientSeat");
    if(settings.contains("ServerClientSeatID")){
        m_SeatID = settings.value("ServerClientSeatID", 0).toUInt();
    }
    settings.endGroup();
}

IProtocolParseXygsServerImpl_Control::~IProtocolParseXygsServerImpl_Control()
{
}

const std::string& IProtocolParseXygsServerImpl_Control::getMessageName() const
{
    return m_MessageName;
}

unsigned int IProtocolParseXygsServerImpl_Control::getSeatID()
{
    return m_SeatID;
}

void IProtocolParseXygsServerImpl_Control::start()
{

}

void IProtocolParseXygsServerImpl_Control::stop()
{

}

bool IProtocolParseXygsServerImpl_Control::getNetServerClient()
{
    return m_NetServerClient;
}

int IProtocolParseXygsServerImpl_Control::NetSend_XygsServer(const int nLength, const char * const pSendBuf)
{
    if(nLength <= 0 || pSendBuf == nullptr){
        return 0;
    }
    return LY_ProtocolParse_XygsServer::Get().SendData(pSendBuf, nLength);
}

