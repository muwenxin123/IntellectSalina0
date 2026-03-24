#include "IXyhcActivator.h"

#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

#include "lyXyhc.h"

IXyhcActivator::IXyhcActivator()
    : m_IRegistryImpl(nullptr)

    , m_pIConfigProjectControlService(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIJoystickManageService(nullptr)
    , m_pIMarkManageService(nullptr)
    , m_pIWeaponManageService(nullptr)
{
    m_SensorInfoMap.clear();
}

IXyhcActivator::~IXyhcActivator()
{
    m_IRegistryImpl                 = nullptr;

    m_pIConfigProjectControlService = nullptr;
    m_pISystemMaintenanceService    = nullptr;
    m_pIPositionManageService       = nullptr;
    m_pISensorManageService         = nullptr;
    m_pIMarkManageService     = nullptr;
    m_pIWeaponManageService = nullptr;
}

IXyhcActivator &IXyhcActivator::Get()
{
    static IXyhcActivator sIXyhcActivator;
    return sIXyhcActivator;
}

qnzkna::framework::IRegistry *IXyhcActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IXyhcActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IXyhcActivator::getISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IXyhcActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IXyhcActivator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IXyhcActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::JoystickManage::IJoystickManageService *IXyhcActivator::getPIJoystickManageService() const
{
    return m_pIJoystickManageService;
}

qnzkna::MarkManage::IMarkManageService *IXyhcActivator::getPIMarkManageService() const
{
    return m_pIMarkManageService;
}

qnzkna::WeaponManage::IWeaponManageService *IXyhcActivator::getPWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::TaskManage::INebulaTaskManageService *IXyhcActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

bool IXyhcActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID)
{
    if (sendMsg == nullptr || length <= 0 || currentSensorID.empty())
    {
        return false;
    }

    qnzkna::SystemConfig::IConfigProjectControlService *pIConfigProjectControlService = getPIConfigProjectControlService();
    if (pIConfigProjectControlService == nullptr)
    {
        return false;
    }

    return false;
}

void IXyhcActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgGimbalAttitude", this);
    m_IRegistryImpl->addMessageListener("MsgDYZCControlAutoAiming", this);
}

void IXyhcActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgGimbalAttitude", this);
    m_IRegistryImpl->removeMessageListener("MsgDYZCControlAutoAiming", this);
}

bool IXyhcActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }

    if (service.getServiceName() == "Ly.IMessageObjectManageServiceImpl")
    {

    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
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
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = static_cast<qnzkna::MarkManage::IMarkManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
    }

    if (m_pISensorManageService != nullptr)
    {
        m_SensorInfoMap.clear();
        m_pISensorManageService->GetSensorInfoList("Optoelectronic", &m_SensorInfoMap);
    }

    if (m_pISystemMaintenanceService != nullptr)
    {
        qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
        if (m_pISystemMaintenanceService->GetWeaponSystems(&weaponSystemMap))
        {
            for (const auto &weaponSystemIter : weaponSystemMap)
            {
                const auto &mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
                for (const auto &mapWeaponComponentIter : mapWeaponComponent)
                {
                    const qnzkna::WeaponManage::WeaponComponent &weaponComponent = (mapWeaponComponentIter.second);
                    bool ok = false;
                    const unsigned long long componentID = QString::fromStdString(weaponComponent.getComponentID()).toULongLong(&ok);
                    if (ok)
                    {
                        LyXyhc::Get().getTrackMgr().m_TrackName.insert(std::pair<unsigned long long, std::string>(componentID, weaponComponent.getComponentName()));
                    }
                }
            }
        }

        if (LyXyhc::Get().getPUAVNavigationArmShotAutoAiming() != nullptr)
        {
            LyXyhc::Get().getPUAVNavigationArmShotAutoAiming()->InitWeaponComponent();
        }
    }

    return true;
}

bool IXyhcActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IMessageObjectManageServiceImpl")
    {

    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = nullptr;
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
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = nullptr;
    }

    if (m_pISensorManageService == nullptr)
    {
        m_SensorInfoMap.clear();

    }

    return true;
}

bool IXyhcActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgGimbalAttitude")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength != sizeof(XygsZC_02)))
        {
            return true;
        }

        const std::string &strSensorId     = message.getMessageProperties("TargetID");
        if (strSensorId.empty())
        {
            return true;
        }

        const XygsZC_02 *const pXygsZC_02 = (const XygsZC_02 * const)msg;

        const auto &iter = m_SensorInfoMap.find(strSensorId);
        if (iter != m_SensorInfoMap.end())
        {
            bool ok = false;
            const unsigned long componentID  = QString::fromStdString(iter->second.GetComponentID()).toULong(&ok);
            if (ok)
            {
                LyXyhc::Get().getTrackMgr().Process_XygsZC_02(componentID, *pXygsZC_02);
            }
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

        const std::string &strMarkID     = message.getMessageProperties("TargetID");
        if (strMarkID.empty())
        {
            return true;
        }

        if (LyXyhc::Get().getPUAVNavigationArmShotAutoAiming() != nullptr)
        {
            emit LyXyhc::Get().getPUAVNavigationArmShotAutoAiming()->AutoAmimingSig(QString::fromStdString(strMarkID), QByteArray(msg, msgLength));
        }
    }
    return true;
}
