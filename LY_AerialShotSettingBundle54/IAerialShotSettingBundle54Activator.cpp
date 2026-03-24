#include "IAerialShotSettingBundle54Activator.h"

#include "LY_AerialShotSettingBundle54Service.h"
IAerialShotSettingBundle54Activator::IAerialShotSettingBundle54Activator()
    : m_IRegistryImpl(nullptr)
    , m_pSystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIJoystickManageService(nullptr)
{

}

IAerialShotSettingBundle54Activator::~IAerialShotSettingBundle54Activator()
{
    m_IRegistryImpl                 = nullptr;
    m_pSystemMaintenanceService		= nullptr;
    m_pIPositionManageService       = nullptr;
    m_pISensorManageService         = nullptr;
    m_pIJoystickManageService       = nullptr;
}

IAerialShotSettingBundle54Activator &IAerialShotSettingBundle54Activator::Get()
{
    static IAerialShotSettingBundle54Activator sIAerialShotSettingBundle54Activator;
    return sIAerialShotSettingBundle54Activator;
}

qnzkna::framework::IRegistry *IAerialShotSettingBundle54Activator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IAerialShotSettingBundle54Activator::getSystemMaintenanceService() const
{
    return m_pSystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IAerialShotSettingBundle54Activator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IAerialShotSettingBundle54Activator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IAerialShotSettingBundle54Activator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::JoystickManage::IJoystickManageService *IAerialShotSettingBundle54Activator::getPIJoystickManageService() const
{
    return m_pIJoystickManageService;
}

qnzkna::TargetManage::IZKLYTargetManageService *IAerialShotSettingBundle54Activator::getPTIZKLYTargetManageService() const
{
    return m_pIZKLYTargetManageService;
}

bool IAerialShotSettingBundle54Activator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID, const std::string &currentComponentID)
{
    if (sendMsg == nullptr || length <= 0)
    {
        return false;
    }
    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = getSystemMaintenanceService();
    if (pSystemMaintenanceService == nullptr)
    {
        return false;
    }
    qnzkna::NodeManage::NODE_TYPE_UINT nLocalType  = NODE_TYPE_CMDUNIT;
    std::string                        strLocalID;
    const qnzkna::NodeManage::NODE_TYPE_UINT nTargetType = NODE_TYPE_SENSOR;
    pSystemMaintenanceService->GetNodeInfo(&nLocalType, &strLocalID);

    qnzkna::framework::MessageReferenceProps props;
    props.put("SourceType",  std::to_string(nLocalType));
    props.put("SourceID",    strLocalID);
    props.put("TargetType",  std::to_string(nTargetType));
    props.put("TargetID",    currentSensorID);
    props.put("ComponentID", currentComponentID);
    if (getIRegistryImpl() == nullptr)
    {
        return false;
    }
    getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVLoad54Send", sendMsg, length, props));
    return true;
}

void IAerialShotSettingBundle54Activator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgPodAerialShot54", this);
    m_IRegistryImpl->addMessageListener("MsgDYZCControl", this);
    m_IRegistryImpl->addMessageListener("MsgDYZCControlAutoAiming", this);
    m_IRegistryImpl->addMessageListener("MsgZCTargetBleadXygszc", this);

}

void IAerialShotSettingBundle54Activator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgPodAerialShot54", this);
    m_IRegistryImpl->removeMessageListener("MsgDYZCControl", this);
    m_IRegistryImpl->removeMessageListener("MsgDYZCControlAutoAiming", this);
    m_IRegistryImpl->removeMessageListener("MsgZCTargetBleadXygszc", this);
}

bool IAerialShotSettingBundle54Activator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }

    if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pSystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = static_cast<qnzkna::PositionManage::IPositionManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IJoystickManageService")
    {
        m_pIJoystickManageService = static_cast<qnzkna::JoystickManage::IJoystickManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITargetManageService")
    {
        m_pIZKLYTargetManageService = static_cast<qnzkna::TargetManage::IZKLYTargetManageService *>(service.getService());
    }
    return true;
}

bool IAerialShotSettingBundle54Activator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pSystemMaintenanceService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IJoystickManageService")
    {
        m_pIJoystickManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITargetManageService")
    {
        m_pIZKLYTargetManageService = nullptr;
    }
    return true;
}

bool IAerialShotSettingBundle54Activator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgPodAerialShot54")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength != AMAPPOD54CONTROLSTATUS_LENGTH))
        {
            return true;
        }

        const std::string &strSensorId     = message.getMessageProperties("TargetID");
        if (strSensorId.empty())
        {
            return true;
        }

        if (LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog() != nullptr)
        {
            LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->on_Message_Recv(QString::fromStdString(strSensorId), QByteArray(msg, msgLength));
        }
    }
    else if (messageName == "MsgDYZCControl")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }

        const std::string &strTargetID     = message.getMessageProperties("TargetID");
        if (strTargetID.empty())
        {
            return true;
        }

        if (LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog() != nullptr)
        {
            emit LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->AutoAmimingSig(QString::fromStdString(strTargetID), QByteArray(msg, msgLength));
        }
    }
    else if (messageName == "MsgDYZCControlAutoAiming")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }

        const std::string &strTargetID     = message.getMessageProperties("TargetID");
        if (strTargetID.empty())
        {
            return true;
        }

        if (LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog() != nullptr)
        {
            emit LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->AutoAmimingSig(QString::fromStdString(strTargetID), QByteArray(msg, msgLength));
        }
    }
    return true;
}

