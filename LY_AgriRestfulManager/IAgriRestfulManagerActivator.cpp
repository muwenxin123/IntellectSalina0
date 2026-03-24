#include "IAgriRestfulManagerActivator.h"

#include "LY_AgriRestfulManager.h"

IAgriRestfulManagerActivator::IAgriRestfulManagerActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pITextToSpeechManageService(nullptr)
{

}

IAgriRestfulManagerActivator::~IAgriRestfulManagerActivator()
{
    m_IRegistryImpl           = nullptr;
    m_pIWeaponManageService   = nullptr;
    m_pIPositionManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pITextToSpeechManageService     = nullptr;
}

IAgriRestfulManagerActivator &IAgriRestfulManagerActivator::Get()
{
    static IAgriRestfulManagerActivator sIAgriRestfulManagerActivator;
    return sIAgriRestfulManagerActivator;
}

qnzkna::framework::IRegistry *IAgriRestfulManagerActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IAgriRestfulManagerActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *IAgriRestfulManagerActivator::getPIWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::PositionManage::IPositionManageService *IAgriRestfulManagerActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IAgriRestfulManagerActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::TextToSpeechManage::ITextToSpeechManageService *IAgriRestfulManagerActivator::getPITextToSpeechManageService() const
{
    return m_pITextToSpeechManageService;
}

qnzkna::ZoneManage::IZoneManageService * IAgriRestfulManagerActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}

qnzkna::TaskManage::INebulaTaskManageService* IAgriRestfulManagerActivator::getPTaskManageService() const
{
	return m_pITaskManageService;
}

void IAgriRestfulManagerActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

}

void IAgriRestfulManagerActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

}

bool IAgriRestfulManagerActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }

    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = static_cast<qnzkna::PositionManage::IPositionManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
    {
        m_pITextToSpeechManageService = static_cast<qnzkna::TextToSpeechManage::ITextToSpeechManageService *>(service.getService());
    }
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriDataAnalysisPredictService")
	{
		m_pIAgriDataAnalysisPredictervice = static_cast<qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pIAgriSprayPrescriptionService = static_cast<qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriSoilTestingAnalysisService")
	{
		m_pAgriSoilTestingAnalysisService = static_cast<qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService *>(service.getService());
	}
    return true;
}

bool IAgriRestfulManagerActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITextToSpeechManageService")
    {
        m_pITextToSpeechManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pITaskManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriDataAnalysisPredictService")
	{
		m_pIAgriDataAnalysisPredictervice = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pIAgriSprayPrescriptionService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriSoilTestingAnalysisService")
	{
		m_pAgriSoilTestingAnalysisService = nullptr;
	}
    return true;
}

bool IAgriRestfulManagerActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgAirInfoRecv")
    {

    }
    return true;
}

qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictService	* IAgriRestfulManagerActivator::getPIAgriDataAnalysisPredictervice() const
{
	return m_pIAgriDataAnalysisPredictervice;
}

qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService	* IAgriRestfulManagerActivator::getPIAgriSprayPrescriptionService() const
{
	return m_pIAgriSprayPrescriptionService;
}

qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisService* IAgriRestfulManagerActivator::getIAgriSoilTestingAnalysisService() const
{
	return m_pAgriSoilTestingAnalysisService;
}
