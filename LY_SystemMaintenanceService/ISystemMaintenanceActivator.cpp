#include "ISystemMaintenanceActivator.h"

ISystemMaintenanceActivator::ISystemMaintenanceActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIJoystickManageService(nullptr)
{
    m_bInitWeaponSystemMap = false;
}

ISystemMaintenanceActivator::~ISystemMaintenanceActivator()
{
    m_IRegistryImpl                 = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pIWeaponManageService         = nullptr;
    m_pIPositionManageService       = nullptr;
    m_pISensorManageService         = nullptr;
    m_pIJoystickManageService       = nullptr;
}

ISystemMaintenanceActivator &ISystemMaintenanceActivator::Get()
{
    static ISystemMaintenanceActivator sIUAVFlyControlActivator;
    return sIUAVFlyControlActivator;
}

qnzkna::framework::IRegistry *ISystemMaintenanceActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *ISystemMaintenanceActivator::getConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *ISystemMaintenanceActivator::getWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::PositionManage::IPositionManageService *ISystemMaintenanceActivator::getPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *ISystemMaintenanceActivator::getSensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::JoystickManage::IJoystickManageService *ISystemMaintenanceActivator::getJoystickManageService() const
{
    return m_pIJoystickManageService;
}

qnzkna::CCommandUnitManage::ICommandUnitManageService	 *ISystemMaintenanceActivator::getCommandUnitManageService() const
{
    return m_pICommandUnitManageService;
}

bool ISystemMaintenanceActivator::InitWeaponSystemMap()
{

    m_mapWeaponSystem.clear();
    m_mapWeaponComponent.clear();
    if (!m_pIConfigProjectControlService ||
            !m_pIWeaponManageService ||
            !m_pIPositionManageService ||
            !m_pISensorManageService ||
            !m_pICommandUnitManageService)
    {
        return false;
    }

    std::string strId = m_pIConfigProjectControlService->systemSeatInfo().SystemNodeId();
    if ("" == strId)
    {
        return false;
    }
    StringSet weaponSysList;
    bool bGetWeaponSys = m_pICommandUnitManageService->GetCommandUnitWeaponList(strId, &weaponSysList);
    if (!bGetWeaponSys)
    {
        return false;
    }
    for (const auto &itrWeaponSys : weaponSysList)
    {
        std::string strWeaponSysId = itrWeaponSys;
        qnzkna::WeaponManage::WeaponSystem weaponSys;
        bool bGetSys = m_pIWeaponManageService->GetWeaponSystem(&weaponSys, strWeaponSysId);
        if (bGetSys)
        {
            m_mapWeaponSystem.insert(std::make_pair(strWeaponSysId, weaponSys));
        }
    }

    for (const auto &itr : m_mapWeaponSystem)
    {
        if (qnzkna::WeaponManage::WeaponSystem_Type_UAV == itr.second.getSystemType())
        {
            for (const auto &itrComponent : itr.second.getMapWeaponComponent())
            {
                qnzkna::WeaponManage::WeaponComponent_Type eComponentType = itrComponent.second.getPWeaponModel()->getComponentType();
                if (qnzkna::WeaponManage::WeaponComponent_Type_RotorUAV == eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_FixedWingUAV == eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_FlappingWingUAV == eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_PrarafoilUAV == eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_SpinWingUAV == eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_HelicopterUAV == eComponentType)
                {
                    m_mapWeaponComponent.insert(std::make_pair(itrComponent.second.getComponentID(), itrComponent.second));
                }
            }
        }
    }

    m_bInitWeaponSystemMap = true;
    return true;
}

qnzkna::WeaponManage::WeaponSystemMap					*ISystemMaintenanceActivator::getWeaponSystems()
{
    if (!m_bInitWeaponSystemMap)
    {
        InitWeaponSystemMap();
    }
    return &m_mapWeaponSystem;
}

qnzkna::WeaponManage::MultiWeaponComponentMap	*ISystemMaintenanceActivator::getUAVComponents()
{
    if (!m_bInitWeaponSystemMap)
    {
        InitWeaponSystemMap();
    }
    for (const auto &itr : m_mapWeaponSystem)
    {
        if (qnzkna::WeaponManage::WeaponSystem_Type_UAV == itr.second.getSystemType())
        {
            for (const auto &itrComponent : itr.second.getMapWeaponComponent())
            {
                qnzkna::WeaponManage::WeaponComponent_Type eComponentType = itrComponent.second.getPWeaponModel()->getComponentType();
                if (qnzkna::WeaponManage::WeaponComponent_Type_RotorUAV			== eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_FixedWingUAV		== eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_FlappingWingUAV	== eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_PrarafoilUAV		== eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_SpinWingUAV		== eComponentType ||
                        qnzkna::WeaponManage::WeaponComponent_Type_HelicopterUAV	== eComponentType)
                {
                    m_mapWeaponComponent.insert(std::make_pair(itrComponent.second.getComponentID(), itrComponent.second));
                }
            }
        }
    }

    return &m_mapWeaponComponent;
}

void ISystemMaintenanceActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }
    m_IRegistryImpl->addServiceListener(this);
}

void ISystemMaintenanceActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }
    m_IRegistryImpl->removeServiceListener(this);
}

bool ISystemMaintenanceActivator::addingService(const qnzkna::framework::ServiceReference &service)
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
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IJoystickManageService")
    {
        m_pIJoystickManageService = static_cast<qnzkna::JoystickManage::IJoystickManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = static_cast<qnzkna::CCommandUnitManage::ICommandUnitManageService *>(service.getService());
    }
    return true;
}

bool ISystemMaintenanceActivator::removedService(const qnzkna::framework::ServiceReference &service)
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
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IJoystickManageService")
    {
        m_pIJoystickManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = nullptr;
    }

    return true;
}
