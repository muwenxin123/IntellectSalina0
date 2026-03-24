#include "IZoneManagerActivator.h"
#include "LY_ZoneManagerInterface.h"
#include "LY_ZoneManager.h"

IZoneManagerActivator::IZoneManagerActivator()
    : m_IRegistryImpl(nullptr)
	, m_pIZoneManageService(nullptr)
	, m_pINebulaTaskManageService(nullptr)
	, m_pIFireSpreadService(nullptr)
	, m_pIMarkManageService(nullptr)
    , m_SeatID(0)
{

}

IZoneManagerActivator::~IZoneManagerActivator()
{
    m_IRegistryImpl                 = nullptr;
	m_pIZoneManageService			= nullptr;
	m_pINebulaTaskManageService		= nullptr;
	m_pIFireSpreadService = nullptr;
	m_pIMarkManageService = nullptr;
}

IZoneManagerActivator &IZoneManagerActivator::Get()
{
    static IZoneManagerActivator sIAerialShotSettingBundleActivator;
    return sIAerialShotSettingBundleActivator;
}

qnzkna::framework::IRegistry *IZoneManagerActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::ZoneManage::IZoneManageService* IZoneManagerActivator::getPIZoneManageService() const
{
	return m_pIZoneManageService;
}

qnzkna::TaskManage::INebulaTaskManageService* IZoneManagerActivator::getPTaskManageService() const
{
	return m_pINebulaTaskManageService;
}

qnzkna::FireSpread::IFireSpreadService * IZoneManagerActivator::getPIFireSpreadService() const
{
	return m_pIFireSpreadService;
}

qnzkna::MarkManage::IMarkManageService * IZoneManagerActivator::getPIMarkManageService() const
{
	return m_pIMarkManageService;
}

void IZoneManagerActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);
}

void IZoneManagerActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);
}

bool IZoneManagerActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }

	if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pINebulaTaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IFireSpreadService")
	{
		m_pIFireSpreadService = static_cast<qnzkna::FireSpread::IFireSpreadService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IMarkManageService")
	{
		m_pIMarkManageService = static_cast<qnzkna::MarkManage::IMarkManageService *>(service.getService());
	}
    return true;
}

bool IZoneManagerActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pINebulaTaskManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IFireSpreadService")
	{
		m_pIFireSpreadService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IMarkManageService")
	{
		m_pIMarkManageService = nullptr;
	}
    return true;
}

bool IZoneManagerActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID)
{
	if (sendMsg == nullptr || length <= 0)
	{
		return false;
	}
	return false;
}

bool IZoneManagerActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
    return true;
}
