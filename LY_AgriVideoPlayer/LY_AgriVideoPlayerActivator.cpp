#include "LY_AgriVideoPlayerActivator.h"

#include "LY_AgriVideoPlayer.h"

LY_AgriVideoPlayerActivator::LY_AgriVideoPlayerActivator()
	: m_IRegistryImpl(nullptr)
	, m_pITextToSpeechManageService(nullptr)
{

}


LY_AgriVideoPlayerActivator::~LY_AgriVideoPlayerActivator()
{

	m_IRegistryImpl = nullptr;

	m_pITextToSpeechManageService = nullptr;

}

LY_AgriVideoPlayerActivator &LY_AgriVideoPlayerActivator::Get()
{
    static LY_AgriVideoPlayerActivator sLY_AgriQuickSprayServiceHomepageActivator;
    return sLY_AgriQuickSprayServiceHomepageActivator;
}

qnzkna::framework::IRegistry *LY_AgriVideoPlayerActivator::getIRegistryImpl() const
{
	return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *LY_AgriVideoPlayerActivator::getPIConfigProjectControlService() const
{
	return m_pIConfigProjectControlService;
}

qnzkna::TextToSpeechManage::ITextToSpeechManageService *LY_AgriVideoPlayerActivator::getPITextToSpeechManageService() const
{
	return m_pITextToSpeechManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService * LY_AgriVideoPlayerActivator::getPIUAVRealTimeStateManageService() const
{
	return m_pIUAVRealTimeStateManageService;
}

void LY_AgriVideoPlayerActivator::start(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->addServiceListener(this);

	m_IRegistryImpl->addMessageListener("MsgAirInfoRecv", this);
}

void LY_AgriVideoPlayerActivator::stop(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->removeServiceListener(this);

	m_IRegistryImpl->removeMessageListener("MsgAirInfoRecv", this);
}

bool LY_AgriVideoPlayerActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getService() == nullptr)
	{
		return false;
	}

	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
	}

	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = static_cast<qnzkna::TextToSpeechManage::ITextToSpeechManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
	{
		m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
	}
	return true;
}

bool LY_AgriVideoPlayerActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = nullptr;
	}

	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = nullptr;
	}

	return true;
}

bool LY_AgriVideoPlayerActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
	if (messageName == "MsgAirInfoRecv")
	{

	}
	return true;
}

