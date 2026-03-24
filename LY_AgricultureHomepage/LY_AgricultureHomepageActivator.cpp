#include "LY_AgricultureHomepageActivator.h"

#include "LY_AgricultureHomepage.h"

LY_AgricultureHomepageActivator::LY_AgricultureHomepageActivator()
	: m_IRegistryImpl(nullptr)
	, m_pIZoneManageService(nullptr)
	, m_pITextToSpeechManageService(nullptr)
	, m_pIAgriFarmManagerService(nullptr)
	, m_pIAgricultureHomePageService(nullptr)
{

}


LY_AgricultureHomepageActivator::~LY_AgricultureHomepageActivator()
{
	m_IRegistryImpl = nullptr;
	m_pITextToSpeechManageService = nullptr;
	m_pIZoneManageService = nullptr;
	m_pIAgriFarmManagerService = nullptr;
	m_pIAgricultureHomePageService = nullptr;
}

LY_AgricultureHomepageActivator &LY_AgricultureHomepageActivator::Get()
{
    static LY_AgricultureHomepageActivator sLY_AgricultureHomepageActivator;
    return sLY_AgricultureHomepageActivator;
}

qnzkna::framework::IRegistry *LY_AgricultureHomepageActivator::getIRegistryImpl() const
{
	return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *LY_AgricultureHomepageActivator::getPIConfigProjectControlService() const
{
	return m_pIConfigProjectControlService;
}



qnzkna::TextToSpeechManage::ITextToSpeechManageService *LY_AgricultureHomepageActivator::getPITextToSpeechManageService() const
{
	return m_pITextToSpeechManageService;
}

void LY_AgricultureHomepageActivator::start(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->addServiceListener(this);

	m_IRegistryImpl->addMessageListener("MsgAirInfoRecv", this);
}

void LY_AgricultureHomepageActivator::stop(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->removeServiceListener(this);

	m_IRegistryImpl->removeMessageListener("MsgAirInfoRecv", this);
}

bool LY_AgricultureHomepageActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getService() == nullptr)
	{
		return false;
	}

	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = static_cast<qnzkna::TextToSpeechManage::ITextToSpeechManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = static_cast<qnzkna::AgriFarmManager::IAgriFarmManagerService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgricultureHomePageService")
	{
		m_pIAgricultureHomePageService = static_cast<qnzkna::IAgricultureHomePage::IAgricultureHomePageService*>(service.getService());
	}
	return true;
}

bool LY_AgricultureHomepageActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgricultureHomePageService")
	{
		m_pIAgricultureHomePageService = nullptr;
	}

	return true;
}
qnzkna::ZoneManage::IZoneManageService	*LY_AgricultureHomepageActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}
bool LY_AgricultureHomepageActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
	if (messageName == "MsgAirInfoRecv")
	{

	}
	return true;
}
qnzkna::AgriFarmManager::IAgriFarmManagerService				*LY_AgricultureHomepageActivator::getPIAgriFarmManagerService() const
{
	return m_pIAgriFarmManagerService;
}
qnzkna::IAgricultureHomePage::IAgricultureHomePageService	*LY_AgricultureHomepageActivator::getIAgricultureHomePageService() const
{
	return m_pIAgricultureHomePageService;
}
//qnzkna::TaskManage::INebulaTaskManageService					 *LY_AgricultureHomepageActivator::getPTaskManageService() const
//{
//	return m_pITaskManageService;
//}
