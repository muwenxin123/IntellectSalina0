#include "LY_AgriSprayPrescriptionActivator.h"

#include "LY_AgriySprayPrescriptionService.h"

LY_AgriSprayPrescriptionActivator::LY_AgriSprayPrescriptionActivator()
	: m_IRegistryImpl(nullptr)
	, m_pITextToSpeechManageService(nullptr)
    ,m_pIAgriFarmManagerService(nullptr)
	, m_pIZoneManageService(nullptr)
	, m_pIWeaponManageService(nullptr)
	, m_pIUAVRealTimeStateManageService(nullptr)
{

}


LY_AgriSprayPrescriptionActivator::~LY_AgriSprayPrescriptionActivator()
{

	m_IRegistryImpl = nullptr;
    m_pIAgriFarmManagerService = nullptr;
	m_pIZoneManageService = nullptr;
	m_pITextToSpeechManageService = nullptr;
	m_pIWeaponManageService = nullptr;
	m_pIUAVRealTimeStateManageService = nullptr;

}
 qnzkna::AgriFarmManager::IAgriFarmManagerService	 *LY_AgriSprayPrescriptionActivator::getPIAgriFarmManagerService() const
 {

     return m_pIAgriFarmManagerService;
 }
 qnzkna::ZoneManage::IZoneManageService	 *LY_AgriSprayPrescriptionActivator::getPIZoneManageService() const
 {
     return m_pIZoneManageService;
 }

 qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService   *LY_AgriSprayPrescriptionActivator::getPIUAVRealTimeStateManageService() const
 {
	 return m_pIUAVRealTimeStateManageService;
 }

 qnzkna::WeaponManage::IWeaponManageService * LY_AgriSprayPrescriptionActivator::getPIWeaponManageService() const
 {
	 return m_pIWeaponManageService;
 }

LY_AgriSprayPrescriptionActivator &LY_AgriSprayPrescriptionActivator::Get()
{
    static LY_AgriSprayPrescriptionActivator sLY_AgriSprayPrescriptionHomepageActivator;
    return sLY_AgriSprayPrescriptionHomepageActivator;
}

qnzkna::framework::IRegistry *LY_AgriSprayPrescriptionActivator::getIRegistryImpl() const
{
	return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *LY_AgriSprayPrescriptionActivator::getPIConfigProjectControlService() const
{
	return m_pIConfigProjectControlService;
}

qnzkna::TextToSpeechManage::ITextToSpeechManageService *LY_AgriSprayPrescriptionActivator::getPITextToSpeechManageService() const
{
	return m_pITextToSpeechManageService;
}

void LY_AgriSprayPrescriptionActivator::start(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->addServiceListener(this);

	m_IRegistryImpl->addMessageListener("MsgAirInfoRecv", this);
}

void LY_AgriSprayPrescriptionActivator::stop(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}

	m_IRegistryImpl->removeServiceListener(this);

	m_IRegistryImpl->removeMessageListener("MsgAirInfoRecv", this);
}

bool LY_AgriSprayPrescriptionActivator::addingService(const qnzkna::framework::ServiceReference &service)
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
    else if (service.getServiceName() == "Ly.IZoneManageService")
    {
        m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
        {
            m_pIAgriFarmManagerService = static_cast<qnzkna::AgriFarmManager::IAgriFarmManagerService*>(service.getService());
        }

	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
	{
		m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
	}
	return true;
}

bool LY_AgriSprayPrescriptionActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.IConfigProjectControlService")
	{
		m_pIConfigProjectControlService = nullptr;
	}

	else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
	{
		m_pITextToSpeechManageService = nullptr;
	}
    else if (service.getServiceName() == "Ly.IZoneManageService")
    {
        m_pIZoneManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
        {
            m_pIAgriFarmManagerService = nullptr;
        }

	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
	{
		m_pIUAVRealTimeStateManageService = nullptr;
	}
	return true;
}

bool LY_AgriSprayPrescriptionActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
	if (messageName == "MsgAirInfoRecv")
	{

	}
	return true;
}


