#include "IIntellectImageShangBoActivator.h"

IIntellectImageShangBoActivator::IIntellectImageShangBoActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
{

}

IIntellectImageShangBoActivator::~IIntellectImageShangBoActivator()
{
    m_IRegistryImpl                    = nullptr;
    m_pIConfigProjectControlService    = nullptr;
    m_pIWeaponManageService            = nullptr;
    m_pISystemMaintenanceService       = nullptr;
    m_pIPositionManageService          = nullptr;
    m_pISensorManageService            = nullptr;
    m_pIUAVRealTimeStateManageService  = nullptr;
}

IIntellectImageShangBoActivator &IIntellectImageShangBoActivator::Get()
{
    static IIntellectImageShangBoActivator sIIntellectImageShangBoActivator;
    return sIIntellectImageShangBoActivator;
}

qnzkna::framework::IRegistry *IIntellectImageShangBoActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IIntellectImageShangBoActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *IIntellectImageShangBoActivator::getPIWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IIntellectImageShangBoActivator::getPISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IIntellectImageShangBoActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IIntellectImageShangBoActivator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IIntellectImageShangBoActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::MarkManage::IMarkManageService* IIntellectImageShangBoActivator::getPITargetManageService() const
{
    return m_pITargetManageService;
}

void IIntellectImageShangBoActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

}

void IIntellectImageShangBoActivator::stop(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

}

bool IIntellectImageShangBoActivator::addingService( const qnzkna::framework::ServiceReference& service )
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

    return true;
}

bool IIntellectImageShangBoActivator::removedService( const qnzkna::framework::ServiceReference& service )
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

bool IIntellectImageShangBoActivator::dealingMessage(const qnzkna::framework::MessageReference& message)
{

    return true;
}

