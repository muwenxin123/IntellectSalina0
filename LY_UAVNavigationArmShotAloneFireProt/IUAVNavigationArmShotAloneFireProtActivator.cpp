#include "IUAVNavigationArmShotAloneFireProtActivator.h"

#include "LY_UAVNavigationArmShotAloneFireProt.h"

IUAVNavigationArmShotAloneFireProtActivator::IUAVNavigationArmShotAloneFireProtActivator()
    : m_IRegistryImpl(nullptr)

    , m_pIConfigProjectControlService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
{

}

IUAVNavigationArmShotAloneFireProtActivator::~IUAVNavigationArmShotAloneFireProtActivator()
{
    m_IRegistryImpl                    = nullptr;

    m_pIConfigProjectControlService = nullptr;
    m_pIWeaponManageService         = nullptr;
    m_pISystemMaintenanceService       = nullptr;
    m_pIPositionManageService          = nullptr;
    m_pISensorManageService            = nullptr;
    m_pIUAVRealTimeStateManageService  = nullptr;
}

IUAVNavigationArmShotAloneFireProtActivator &IUAVNavigationArmShotAloneFireProtActivator::Get()
{
    static IUAVNavigationArmShotAloneFireProtActivator sIUAVNavigationArmShotAloneFireProtActivator;
    return sIUAVNavigationArmShotAloneFireProtActivator;
}

qnzkna::framework::IRegistry *IUAVNavigationArmShotAloneFireProtActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IUAVNavigationArmShotAloneFireProtActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *IUAVNavigationArmShotAloneFireProtActivator::getPIWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IUAVNavigationArmShotAloneFireProtActivator::getPISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IUAVNavigationArmShotAloneFireProtActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IUAVNavigationArmShotAloneFireProtActivator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IUAVNavigationArmShotAloneFireProtActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

void IUAVNavigationArmShotAloneFireProtActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

}

void IUAVNavigationArmShotAloneFireProtActivator::stop(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

}

bool IUAVNavigationArmShotAloneFireProtActivator::addingService( const qnzkna::framework::ServiceReference& service )
{
    if(service.getService() == nullptr){
        return false;
    }

    if(service.getServiceName() == "Ly.IConfigProjectControlService"){
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IWeaponManageService"){
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IMessageObjectManageServiceImpl"){

    }
    else if(service.getServiceName() == "Ly.ISystemMaintenanceService"){
        m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IPositionManageService"){
        m_pIPositionManageService = static_cast<qnzkna::PositionManage::IPositionManageService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.ISensorManageService"){
        m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService*>(service.getService());
    }
	else if (service.getServiceName() == "Ly.ITargetManageService") {
        m_pITargetManageService = static_cast<qnzkna::MarkManage::IMarkManageService*>(service.getService());
	}
    else if(service.getServiceName() == "Ly.IUAVRealTimeStateManageService"){
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*>(service.getService());
    }

    if (m_Component_Property_ComponentFireProtMap.size() == 0)
    {
        InitWeaponInfoList();
    }

    return true;
}

bool IUAVNavigationArmShotAloneFireProtActivator::removedService( const qnzkna::framework::ServiceReference& service )
{
    if(service.getServiceName() == "Ly.IConfigProjectControlService"){
        m_pIConfigProjectControlService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IWeaponManageService"){
        m_pIWeaponManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IMessageObjectManageServiceImpl"){

    }
    else if(service.getServiceName() == "Ly.ISystemMaintenanceService"){
        m_pISystemMaintenanceService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IPositionManageService"){
        m_pIPositionManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.ITargetManageService") {
		m_pITargetManageService = nullptr;
	}
    else if(service.getServiceName() == "Ly.ISensorManageService"){
        m_pISensorManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IUAVRealTimeStateManageService"){
        m_pIUAVRealTimeStateManageService = nullptr;
    }

    return true;
}

bool IUAVNavigationArmShotAloneFireProtActivator::dealingMessage(const qnzkna::framework::MessageReference& message)
{

    return true;
}

void IUAVNavigationArmShotAloneFireProtActivator::InitWeaponInfoList()
{
    return;

    if (!m_pISystemMaintenanceService || !m_pISensorManageService) { return; }
    std::set<std::string>  strPlatformList;
    m_pISystemMaintenanceService->GetWeaponComponentPlatforms(&strPlatformList);
    for (const auto& itrPlatform : strPlatformList)
    {
        std::string strPlatformId = itrPlatform;
        qnzkna::WeaponManage::WeaponComponent weaponInfo;
        if (!m_pISystemMaintenanceService->GetWeaponComponent(strPlatformId, &weaponInfo)) continue;
        std::string strSysId;
        unsigned int nIndex = 0;
        if (!qnzkna::WeaponManage::WeaponSystem::DecodeWeaponComponentID(strSysId, nIndex, strPlatformId)) {
            continue;
        }
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
        bool bGetSensor = m_pISensorManageService->GetSensorInfosInWeaponComponent(strPlatformId, &tmpSensorInfoMap);
        if (bGetSensor)
        {
            qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
            for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++) {
                qnzkna::SensorManage::CSensorInfo& tmpSensorInfo = itrSensor->second;
                std::string typeOptoelectronic("");
                tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                std::transform(typeOptoelectronic.begin(), typeOptoelectronic.end(), typeOptoelectronic.begin(), ::tolower);
                if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "FireProt"))
                {

                    tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                }
            }
        }

        for (const auto& tmpSensorInfoOptoelectronic : tmpSensorInfoOptoelectronicMap)
        {
            Component_Property_ComponentFireProt component_Property_ComponentFireProt;
            component_Property_ComponentFireProt.sysID = QString::fromStdString(strSysId).toULong();
            component_Property_ComponentFireProt.component_ComponentSysID = strSysId;
            component_Property_ComponentFireProt.component_ComponentName = weaponInfo.getComponentName();
            component_Property_ComponentFireProt.component_ComponentID = weaponInfo.getComponentID();
            component_Property_ComponentFireProt.component_SensorName = tmpSensorInfoOptoelectronic.second.GetSensorName();
            component_Property_ComponentFireProt.component_SensorID = tmpSensorInfoOptoelectronic.second.GetSensorID();
            m_Component_Property_ComponentFireProtMap[QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID())] = component_Property_ComponentFireProt;
        }
    }
}

qnzkna::MarkManage::IMarkManageService* IUAVNavigationArmShotAloneFireProtActivator::getPITargetManageService() const
{
	return m_pITargetManageService;
}

