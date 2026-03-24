#include "IAgriDataAnalysisPredictActivator.h"

#include "LY_AgriDataAnalysisPredict.h"

IAgriDataAnalysisPredictActivator::IAgriDataAnalysisPredictActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pITextToSpeechManageService(nullptr)
{

}

IAgriDataAnalysisPredictActivator::~IAgriDataAnalysisPredictActivator()
{
    m_IRegistryImpl           = nullptr;
    m_pIWeaponManageService   = nullptr;
    m_pIPositionManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pITextToSpeechManageService     = nullptr;
}

IAgriDataAnalysisPredictActivator &IAgriDataAnalysisPredictActivator::Get()
{
    static IAgriDataAnalysisPredictActivator sIAgriDataAnalysisPredictActivator;
    return sIAgriDataAnalysisPredictActivator;
}

qnzkna::framework::IRegistry *IAgriDataAnalysisPredictActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IAgriDataAnalysisPredictActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *IAgriDataAnalysisPredictActivator::getPIWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::PositionManage::IPositionManageService *IAgriDataAnalysisPredictActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IAgriDataAnalysisPredictActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::TextToSpeechManage::ITextToSpeechManageService *IAgriDataAnalysisPredictActivator::getPITextToSpeechManageService() const
{
    return m_pITextToSpeechManageService;
}

qnzkna::ZoneManage::IZoneManageService * IAgriDataAnalysisPredictActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}

qnzkna::TaskManage::INebulaTaskManageService* IAgriDataAnalysisPredictActivator::getPTaskManageService() const
{
	return m_pITaskManageService;
}

qnzkna::AgriFarmManager::IAgriFarmManagerService* IAgriDataAnalysisPredictActivator::getPIAgriFarmManagerService() const
{
	return m_pIAgriFarmManagerService;
}

void IAgriDataAnalysisPredictActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

}

void IAgriDataAnalysisPredictActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

}

bool IAgriDataAnalysisPredictActivator::addingService(const qnzkna::framework::ServiceReference &service)
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
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = static_cast<qnzkna::AgriFarmManager::IAgriFarmManagerService*>(service.getService());
	}
    return true;
}

bool IAgriDataAnalysisPredictActivator::removedService(const qnzkna::framework::ServiceReference &service)
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
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = nullptr;
	}
    return true;
}

bool IAgriDataAnalysisPredictActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgAirInfoRecv")
    {

    }
    return true;
}
