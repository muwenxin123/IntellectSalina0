#include "ICollaborativeSearchActivator.h"

ICollaborativeSearchActivator::ICollaborativeSearchActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pITaskManageService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIFormationManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIMarkManageService(nullptr)
    , m_pICommandUnitManageService(nullptr)
	, m_pISensorManageService(nullptr)
{

}

ICollaborativeSearchActivator::~ICollaborativeSearchActivator()
{
    m_IRegistryImpl = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pITaskManageService = nullptr;
    m_pIWeaponManageService = nullptr;
    m_pIFormationManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pIMarkManageService = nullptr;
    m_pICommandUnitManageService = nullptr;
	m_pISensorManageService = nullptr;
}

ICollaborativeSearchActivator &ICollaborativeSearchActivator::Get()
{
    static ICollaborativeSearchActivator sICollaborativeSearchActivator;
    return sICollaborativeSearchActivator;
}

qnzkna::framework::IRegistry *ICollaborativeSearchActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *ICollaborativeSearchActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::TaskManage::INebulaTaskManageService *ICollaborativeSearchActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

qnzkna::WeaponManage::IWeaponManageService *ICollaborativeSearchActivator::getPWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::UAVFormManage::IFormationManageService *ICollaborativeSearchActivator::getPFormationManageService() const
{
    return m_pIFormationManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *ICollaborativeSearchActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::MarkManage::IMarkManageService *ICollaborativeSearchActivator::getPIMarkManageService() const
{
    return m_pIMarkManageService;
}

qnzkna::CCommandUnitManage::ICommandUnitManageService *ICollaborativeSearchActivator::getPICommandUnitManageService() const
{
    return m_pICommandUnitManageService;
}

qnzkna::SensorManage::ISensorManageService * ICollaborativeSearchActivator::getPISensorManageService() const
{
	return m_pISensorManageService;
}

bool ICollaborativeSearchActivator::sendMessage(const char * const sendMsg, const unsigned int length, const string &currentComponentID)
{
    return true;
}

void ICollaborativeSearchActivator::start(qnzkna::framework::IRegistry * const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgUAVParameterState", this);
}

void ICollaborativeSearchActivator::stop(qnzkna::framework::IRegistry * const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgUAVParameterState", this);
}

bool ICollaborativeSearchActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IFormManageService")
    {
        m_pIFormationManageService = static_cast<qnzkna::UAVFormManage::IFormationManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService") {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IMarkManageService") {
        m_pIMarkManageService = static_cast<qnzkna::MarkManage::IMarkManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService") {
        m_pICommandUnitManageService = static_cast<qnzkna::CCommandUnitManage::ICommandUnitManageService*>(service.getService());
    }
	else if (service.getServiceName() == "Ly.ISensorManageService")
	{
		m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}

	if (m_pISensorManageService != nullptr)
	{
		m_SensorInfoMap.clear();
		m_pISensorManageService->GetSensorInfoList("Optoelectronic", &m_SensorInfoMap);
	}

    return true;
}

bool ICollaborativeSearchActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IFormManageService")
    {
        m_pIFormationManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService") {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IMarkManageService") {
        m_pIMarkManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService") {
        m_pICommandUnitManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.ISensorManageService")
	{
		m_pISensorManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}

	if (m_pISensorManageService == nullptr)
	{
		m_SensorInfoMap.clear();

	}
    return true;
}

bool ICollaborativeSearchActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string& messageName = message.getMessageName();
    if (messageName == "MsgUAVParameterState")
    {
        const char*        msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();

        if ((nullptr == msg) || (msgLength <= 0))
            return true;

        const std::string& strWeaponId = message.getMessageProperties("TargetID");

        if (strWeaponId.empty())
            return true;
    }
    return true;
}

qnzkna::ZoneManage::IZoneManageService	*ICollaborativeSearchActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}
