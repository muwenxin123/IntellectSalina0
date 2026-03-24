#include "LY_AgriFertilizerPrescriptionActivator.h"

#include "LY_AgriFertilizerPrescription.h"

LY_AgriFertilizerPrescriptionActivator::LY_AgriFertilizerPrescriptionActivator()
	: m_IRegistryImpl(nullptr)
	, m_pITextToSpeechManageService(nullptr)
	, m_pIAgricultureHomePageService(nullptr)
	, m_pIZoneManageService(nullptr)
	, m_pAgriSoilTestingAnalysisService(nullptr)
	, m_pIWeaponManageService(nullptr)
	,m_pAgriSprayPrescriptionService(nullptr)
	, m_pIAgriFertilizerPrescriptionService(nullptr)
{

}


LY_AgriFertilizerPrescriptionActivator::~LY_AgriFertilizerPrescriptionActivator()
{

	m_IRegistryImpl = nullptr;

	m_pITextToSpeechManageService = nullptr;
	m_pIAgricultureHomePageService = nullptr;
	m_pIZoneManageService = nullptr;
	m_pAgriSoilTestingAnalysisService = nullptr;
	m_pIWeaponManageService = nullptr;
	m_pAgriSprayPrescriptionService = nullptr;
	m_pIAgriFertilizerPrescriptionService = nullptr;
}

LY_AgriFertilizerPrescriptionActivator &LY_AgriFertilizerPrescriptionActivator::Get()
{
    static LY_AgriFertilizerPrescriptionActivator sLY_AgriFertilizerPrescriptionHomepageActivator;
    return sLY_AgriFertilizerPrescriptionHomepageActivator;
}

qnzkna::framework::IRegistry *LY_AgriFertilizerPrescriptionActivator::getIRegistryImpl() const
{
	return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *LY_AgriFertilizerPrescriptionActivator::getPIConfigProjectControlService() const
{
	return m_pIConfigProjectControlService;
}

qnzkna::TextToSpeechManage::ITextToSpeechManageService *LY_AgriFertilizerPrescriptionActivator::getPITextToSpeechManageService() const
{
	return m_pITextToSpeechManageService;
}

void LY_AgriFertilizerPrescriptionActivator::start(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->addServiceListener(this);

	m_IRegistryImpl->addMessageListener("MsgAirInfoRecv", this);
}

void LY_AgriFertilizerPrescriptionActivator::stop(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->removeServiceListener(this);

	m_IRegistryImpl->removeMessageListener("MsgAirInfoRecv", this);
}

bool LY_AgriFertilizerPrescriptionActivator::addingService(const qnzkna::framework::ServiceReference &service)
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
	else if (service.getServiceName() == "Ly.IAgricultureHomePageService")
	{
		m_pIAgricultureHomePageService = static_cast<qnzkna::IAgricultureHomePage::IAgricultureHomePageService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriSoilTestingAnalysisService")
	{
		m_pAgriSoilTestingAnalysisService = static_cast<qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pAgriSprayPrescriptionService = static_cast<qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriFertilizerPrescriptionService")
	{
		m_pIAgriFertilizerPrescriptionService = static_cast<qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionService*>(service.getService());
	}

	return true;
}

bool LY_AgriFertilizerPrescriptionActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = nullptr;
	}

	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgricultureHomePageService")
	{
		m_pIAgricultureHomePageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriSoilTestingAnalysisService")
	{
		m_pAgriSoilTestingAnalysisService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pAgriSprayPrescriptionService = nullptr;
	}

	return true;
}

bool LY_AgriFertilizerPrescriptionActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
	if (messageName == "MsgAirInfoRecv")
	{

	}
	return true;
}
qnzkna::IAgricultureHomePage::IAgricultureHomePageService	*LY_AgriFertilizerPrescriptionActivator::getIAgricultureHomePageService() const
{
	return m_pIAgricultureHomePageService;
}
qnzkna::ZoneManage::IZoneManageService							*LY_AgriFertilizerPrescriptionActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}
qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService      *LY_AgriFertilizerPrescriptionActivator::getIAgriSoilTestingAnalysisService() const
{
	return m_pAgriSoilTestingAnalysisService;
}
qnzkna::WeaponManage::IWeaponManageService						*LY_AgriFertilizerPrescriptionActivator::getPIWeaponManageService() const
{
	return m_pIWeaponManageService;
}
qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService *		LY_AgriFertilizerPrescriptionActivator::getPAgriSprayPrescriptionService() const
{

	return m_pAgriSprayPrescriptionService;
}
qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionService *LY_AgriFertilizerPrescriptionActivator::getAgriFertilizerPrescriptionService() const
{
	return m_pIAgriFertilizerPrescriptionService;
}
