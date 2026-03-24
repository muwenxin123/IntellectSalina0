#include "IAgroTaskManagerActivator.h"
#include "lY_AgroTaskManage.h"
#include "QGeoCoordinate"

IAgroTaskManagerActivator::IAgroTaskManagerActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pITaskManageService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIFormationManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIMarkManageService(nullptr)
    , m_pICommandUnitManageService(nullptr)
    , m_pAmmunitionManageService(nullptr)
	, m_pIAgroTaskService(nullptr)
{

}

IAgroTaskManagerActivator::~IAgroTaskManagerActivator()
{
    m_IRegistryImpl = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pISystemMaintenanceService = nullptr;
    m_pITaskManageService = nullptr;
    m_pIWeaponManageService = nullptr;
    m_pIFormationManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pIMarkManageService = nullptr;
    m_pICommandUnitManageService = nullptr;
    m_pAmmunitionManageService = nullptr;
}

IAgroTaskManagerActivator &IAgroTaskManagerActivator::Get()
{
    static IAgroTaskManagerActivator sITaskManagerActivator;
    return sITaskManagerActivator;
}

qnzkna::framework::IRegistry *IAgroTaskManagerActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IAgroTaskManagerActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IAgroTaskManagerActivator::getISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::ZoneManage::IZoneManageService *IAgroTaskManagerActivator::getIZoneManageService() const
{
    return m_pIZoneManageService;
}

qnzkna::WeaponManage::IWeaponManageService *IAgroTaskManagerActivator::getPWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::TaskManage::INebulaTaskManageService *IAgroTaskManagerActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

qnzkna::UAVFormManage::IFormationManageService *IAgroTaskManagerActivator::getPFormationManageService() const
{
    return m_pIFormationManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IAgroTaskManagerActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::MarkManage::IMarkManageService *IAgroTaskManagerActivator::getPIMarkManageService() const
{
    return m_pIMarkManageService;
}

qnzkna::CCommandUnitManage::ICommandUnitManageService *IAgroTaskManagerActivator::getPICommandUnitManageService() const
{
    return m_pICommandUnitManageService;
}


qnzkna::AmmunitionManage::IAmmunitionManageService *IAgroTaskManagerActivator::getPAmmunitionManageService() const
{
    return m_pAmmunitionManageService;
}

qnzkna::AgroTask::IAgroTaskService* IAgroTaskManagerActivator::getPIAgroTaskService() const
{
	return m_pIAgroTaskService;
}

qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService* IAgroTaskManagerActivator::GetPIAgriSprayPrescriptionService() const
{
	return m_pAgriSprayPrescriptionService;
}

qnzkna::AgriFarmManager::IAgriFarmManagerService* IAgroTaskManagerActivator::GetPIAgriFarmManagerService() const
{
	return m_pIAgriFarmManagerService;
}

bool IAgroTaskManagerActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentComponentID)
{
    return true;
}

void IAgroTaskManagerActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgUAVParameterState", this);
    m_IRegistryImpl->addMessageListener("MsgStrikeSchemeList", this);
    m_IRegistryImpl->addMessageListener("MsgMarkAmmoID", this);
    m_IRegistryImpl->addMessageListener("MsgOnlyMarkCreateSend", this);
    m_IRegistryImpl->addMessageListener("MsgPlatAmmoID", this);
    m_IRegistryImpl->addMessageListener("MsgOnlyLinepointSend", this);
    m_IRegistryImpl->addMessageListener("TaskDataEdit", this);
    m_IRegistryImpl->addMessageListener("DeviationVoyageRoute", this);
	m_IRegistryImpl->addMessageListener("DistanceVoyageRoute", this);
}

void IAgroTaskManagerActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgUAVParameterState", this);
    m_IRegistryImpl->removeMessageListener("MsgStrikeSchemeList", this);
    m_IRegistryImpl->removeMessageListener("MsgMarkAmmoID", this);
    m_IRegistryImpl->removeMessageListener("MsgOnlyMarkCreateSend", this);
    m_IRegistryImpl->removeMessageListener("MsgPlatAmmoID", this);
    m_IRegistryImpl->removeMessageListener("MsgOnlyLinepointSend", this);
    m_IRegistryImpl->removeMessageListener("TaskDataEdit", this);
    m_IRegistryImpl->removeMessageListener("DeviationVoyageRoute", this);
    m_IRegistryImpl->removeMessageListener("DistanceVoyageRoute", this);
}

bool IAgroTaskManagerActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IFormManageService")
    {
        m_pIFormationManageService = static_cast<qnzkna::UAVFormManage::IFormationManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = static_cast<qnzkna::MarkManage::IMarkManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = static_cast<qnzkna::CCommandUnitManage::ICommandUnitManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IAmmunitionManageService")
    {
        m_pAmmunitionManageService = static_cast<qnzkna::AmmunitionManage::IAmmunitionManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IZoneManageService")
    {
        m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
    }
	else if (service.getServiceName() == "Ly.IAgroTaskService")
	{
		m_pIAgroTaskService = static_cast<qnzkna::AgroTask::IAgroTaskService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = static_cast<qnzkna::AgriFarmManager::IAgriFarmManagerService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pAgriSprayPrescriptionService = static_cast<qnzkna::AgriSprayPrescription::IAgriSprayPrescriptionService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IAgriFarmManagerService")
	{
		m_pIAgriFarmManagerService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IAgriSprayPrescriptionService")
	{
		m_pAgriSprayPrescriptionService = nullptr;
	}

    return true;
}

bool IAgroTaskManagerActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = nullptr;
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
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IAmmunitionManageService")
    {
        m_pAmmunitionManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IZoneManageService")
    {
        m_pIZoneManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.IAgroTaskService")
	{
		m_pIAgroTaskService = nullptr;
	}

    return true;
}

bool IAgroTaskManagerActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    return true;
}


