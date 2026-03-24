#include "IProtocolParseXygsServerActivator.h"

#include "IProtocolParseXygsServerImpl_Control.h"

IProtocolParseXygsServerActivator::IProtocolParseXygsServerActivator()
    : m_IRegistryImpl(nullptr)
{

}

IProtocolParseXygsServerActivator::~IProtocolParseXygsServerActivator()
{
    m_IRegistryImpl = nullptr;
}

IProtocolParseXygsServerActivator &IProtocolParseXygsServerActivator::Get()
{
    static IProtocolParseXygsServerActivator sIProtocolParseXygsServerActivator;
    return sIProtocolParseXygsServerActivator;
}

qnzkna::framework::IRegistry *IProtocolParseXygsServerActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

void IProtocolParseXygsServerActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->addMessageListener("MsgXygsServerPack", this);
    m_IRegistryImpl->addMessageListener("MsgXygsServerUnPack", this);

	m_IRegistryImpl->addMessageListener("MsgSeatScenariosReplay", this);

    qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_Start();

    for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
    {
        IProtocolParseXygsServerImpl_Control->start();
    }

    for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
    {
        if(!IProtocolParseXygsServerImpl_Control->getMessageName().empty()){
            m_IRegistryImpl->addMessageListener(IProtocolParseXygsServerImpl_Control->getMessageName(), this);
        }
    }
}

void IProtocolParseXygsServerActivator::stop(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->removeMessageListener("MsgXygsServerPack", this);
    m_IRegistryImpl->removeMessageListener("MsgXygsServerUnPack", this);

	m_IRegistryImpl->removeMessageListener("MsgSeatScenariosReplay", this);

    qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_Stop();

    for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
    {
        IProtocolParseXygsServerImpl_Control->stop();
    }

    for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
    {
        if(!IProtocolParseXygsServerImpl_Control->getMessageName().empty()){
            m_IRegistryImpl->removeMessageListener(IProtocolParseXygsServerImpl_Control->getMessageName(), this);
        }
    }
}

bool IProtocolParseXygsServerActivator::dealingMessage( const qnzkna::framework::MessageReference& message )
{

    if ("MsgXygsServerPack" == message.getMessageName()) {

    }
    else if("MsgXygsServerUnPack" == message.getMessageName()) {

    }
	else if ("MsgSeatScenariosReplay" == message.getMessageName())
	{
		const char*        msg = message.getMessage();
		const unsigned int msgLength = message.getMessageLength();
		if ((nullptr == msg) || (msgLength <= 0)) {
			return true;
		}

		qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_RecvMessage(msgLength, msg);
	}

    for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
    {
        if(IProtocolParseXygsServerImpl_Control->getMessageName() == message.getMessageName())
        {
            IProtocolParseXygsServerImpl_Control->Pack_XygsServer(&message, m_IRegistryImpl);
            break;
        }
    }
    return true;
}
