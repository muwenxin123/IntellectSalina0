#include "IUAVFlyControl_MavlinkActivator.h"

#include "LY_UAVFlyControl_Mavlink.h"

#include "QGCApplication.h"
#include "LocalLink.h"

IUAVFlyControl_MavlinkActivator::IUAVFlyControl_MavlinkActivator()
    : m_IRegistryImpl(nullptr)

    , m_pIConfigProjectControlService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pSystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIJoystickManageService(nullptr)
    , m_pITaskManageService(nullptr)
	, m_pIZoneManageService(nullptr)
{
}

IUAVFlyControl_MavlinkActivator::~IUAVFlyControl_MavlinkActivator()
{
    m_IRegistryImpl                         = nullptr;
    m_pIConfigProjectControlService         = nullptr;
    m_pIWeaponManageService                 = nullptr;
    m_pSystemMaintenanceService             = nullptr;
    m_pIPositionManageService               = nullptr;
    m_pISensorManageService                 = nullptr;
    m_pIUAVRealTimeStateManageService       = nullptr;
    m_pIJoystickManageService               = nullptr;
    m_pITaskManageService                   = nullptr;
	m_pIZoneManageService					= nullptr;

}

IUAVFlyControl_MavlinkActivator &IUAVFlyControl_MavlinkActivator::Get()
{
    static IUAVFlyControl_MavlinkActivator sIUAVFlyControlActivator_Mavlink;
    return sIUAVFlyControlActivator_Mavlink;
}

qnzkna::framework::IRegistry *IUAVFlyControl_MavlinkActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::PositionManage::IPositionManageService *IUAVFlyControl_MavlinkActivator::getPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IUAVFlyControl_MavlinkActivator::getSensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IUAVFlyControl_MavlinkActivator::getIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::JoystickManage::IJoystickManageService *IUAVFlyControl_MavlinkActivator::getJoystickManageService() const
{
    return m_pIJoystickManageService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IUAVFlyControl_MavlinkActivator::getSystemMaintenanceService() const
{
    return m_pSystemMaintenanceService;
}

qnzkna::TaskManage::INebulaTaskManageService *IUAVFlyControl_MavlinkActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

qnzkna::ZoneManage::IZoneManageService *IUAVFlyControl_MavlinkActivator::getZoneManageService() const
{
	return m_pIZoneManageService;
}

bool IUAVFlyControl_MavlinkActivator::sendMessage(const char* const sendMsg, const unsigned int msgLength, const unsigned long long weaponId)
{

#ifdef QGC_APPLICATION_MAVLINK_VEHICLE_CONTROL_SEND_COMMAN_DMESSAGE
    if(sendMsg == nullptr || msgLength <= 0){
        return false;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT nNodeType  = NODE_TYPE_CMDUNIT;
    std::string                        strNodeId  = "0";
    if(m_pSystemMaintenanceService){
        m_pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId);
    }

    const qnzkna::NodeManage::NODE_TYPE_UINT nTargetType = NODE_TYPE_WEAPONCOMPONENT;
    const std::string &strComponentID            = std::to_string(weaponId);

    qnzkna::framework::MessageReferenceProps props;
    props.put("SourceType",   std::to_string(nNodeType));
    props.put("SourceID",     strNodeId);
    props.put("TargetType",   std::to_string(nTargetType));
    props.put("TargetID",     strComponentID);
    props.put("WeaponID",     strComponentID);
    props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK);
    if(getIRegistryImpl() == nullptr){
        return false;
    }
    getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVMavlinkControlSend", sendMsg, msgLength, props));
#endif
    return true;
}

void IUAVFlyControl_MavlinkActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgUAVMavlinkControlRecv", this);
}

void IUAVFlyControl_MavlinkActivator::stop(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgUAVMavlinkControlRecv", this);
}

bool IUAVFlyControl_MavlinkActivator::addingService( const qnzkna::framework::ServiceReference& service )
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
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService") {
        m_pSystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IPositionManageService"){
        m_pIPositionManageService = static_cast<qnzkna::PositionManage::IPositionManageService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.ISensorManageService"){
        m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IUAVRealTimeStateManageService"){
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*>(service.getService());
    }
    else if(service.getServiceName() == "Ly.IJoystickManageService"){
        m_pIJoystickManageService = static_cast<qnzkna::JoystickManage::IJoystickManageService*>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITaskManageService"){
        m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService*>(service.getService());
    }
	else if (service.getServiceName() == "Ly.IZoneManageService") {
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService*>(service.getService());
	}

    if(m_pSystemMaintenanceService != nullptr && m_pIConfigProjectControlService != nullptr
            && m_pIWeaponManageService != nullptr && m_pIPositionManageService != nullptr)
    {
        qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
        if(m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
            std::string strNodeId = m_pIConfigProjectControlService->systemSeatInfo().SystemNodeId();
            {
                qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
                if(m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
                    for(const auto& weaponSystemIter : weaponSystemMap){
                        const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
                        for(const auto& mapWeaponComponentIter : mapWeaponComponent){
                            if((std::string(mapWeaponComponentIter.second.getProtocolType()) == std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK))
                                    )
                            {
                                if (mapWeaponComponentIter.second.getPWeaponModel() && mapWeaponComponentIter.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_RotorUAV) {
                                    const qnzkna::WeaponManage::WeaponComponent& weaponComponent = (mapWeaponComponentIter.second);
                                    bool ok = false;
                                    qDebug() << __FILE__ << __LINE__ << QString::fromStdString(weaponComponent.getComponentID());
                                    const unsigned long long componentID = QString::fromStdString(weaponComponent.getComponentID()).toULongLong(&ok);
                                    if(ok){
                                        qDebug() << __FILE__ << __LINE__ << componentID << QString::fromStdString(weaponComponent.getComponentName());
                                        QGCApplication::updateVehicleName(componentID, QString::fromStdString(weaponComponent.getComponentName()));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }
    return true;
}

bool IUAVFlyControl_MavlinkActivator::removedService( const qnzkna::framework::ServiceReference& service )
{
    if(service.getServiceName() == "Ly.IConfigProjectControlService"){
        m_pIConfigProjectControlService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IWeaponManageService"){
        m_pIWeaponManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.ISystemMaintenanceService"){
        m_pSystemMaintenanceService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IPositionManageService"){
        m_pIPositionManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.ISensorManageService"){
        m_pISensorManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IUAVRealTimeStateManageService"){
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if(service.getServiceName() == "Ly.IJoystickManageService"){
        m_pIJoystickManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITaskManageService"){
        m_pITaskManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.IZoneManageService") {
		m_pIZoneManageService = nullptr;
	}
    return true;
}

bool IUAVFlyControl_MavlinkActivator::dealingMessage(const qnzkna::framework::MessageReference& message)
{
    const std::string& messageName = message.getMessageName();
    if(messageName == "MsgUAVMavlinkControlRecv")
    {
        const char*        msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0)){
            return true;
        }

        if(message.getMessageProperties("ProtocolType") != WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK){
            return true;
        }

        const std::string& strWeaponId     = message.getMessageProperties("TargetID");
        if (strWeaponId.empty()){
            return true;
        }

        bool ok = false;
        unsigned long long weaponId = QString::fromStdString(strWeaponId).toULongLong(&ok);
        if(!ok){
            return true;
        }

        if(MavlinkApp() && MavlinkApp()->localLink()){
            MavlinkApp()->localLink()->dealingMessage(msg, msgLength, weaponId);
        }

    }
    return true;
}

