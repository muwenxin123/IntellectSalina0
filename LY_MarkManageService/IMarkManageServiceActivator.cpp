#include "IMarkManageServiceActivator.h"
#include "LY_MarkManageServiceInterface.h"
#include "LY_MarkManageService.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "IMarkManageImpl.h"
#include "track.h"

#include <QDateTime>
#include <QSettings>

IMarkManageServiceActivator::IMarkManageServiceActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pSystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIJoystickManageService(nullptr)
    , m_pINebulaTaskManageService(nullptr)
    , m_SeatID(0)
{

    QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);

    settings.beginGroup("NetServerClientSeat");
    if (settings.contains("ServerClientSeatID"))
    {
        m_SeatID = settings.value("ServerClientSeatID", 0).toUInt();
    }
    settings.endGroup();
}

IMarkManageServiceActivator::~IMarkManageServiceActivator()
{
    m_IRegistryImpl                 = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pIWeaponManageService         = nullptr;
    m_pSystemMaintenanceService     = nullptr;
    m_pIPositionManageService       = nullptr;
    m_pISensorManageService         = nullptr;
    m_pINebulaTaskManageService		= nullptr;
}

IMarkManageServiceActivator &IMarkManageServiceActivator::Get()
{
    static IMarkManageServiceActivator sIAerialShotSettingBundleActivator;
    return sIAerialShotSettingBundleActivator;
}

qnzkna::framework::IRegistry *IMarkManageServiceActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *IMarkManageServiceActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *IMarkManageServiceActivator::getPIWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IMarkManageServiceActivator::getPSystemMaintenanceService() const
{
    return m_pSystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IMarkManageServiceActivator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IMarkManageServiceActivator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IMarkManageServiceActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::JoystickManage::IJoystickManageService *IMarkManageServiceActivator::getPIJoystickManageService() const
{
    return m_pIJoystickManageService;
}

qnzkna::TaskManage::INebulaTaskManageService *IMarkManageServiceActivator::getPTaskManageService() const
{
    return m_pINebulaTaskManageService;
}

bool IMarkManageServiceActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID)
{
    if (sendMsg == nullptr || length <= 0)
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

void IMarkManageServiceActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgZCMarkPosition", this);
    m_IRegistryImpl->addMessageListener("MsgZCVideoMarkNoPosition", this);
    m_IRegistryImpl->addMessageListener("MsgGimbalAttitude", this);
    m_IRegistryImpl->addMessageListener("MsgUAVAssignMark", this);
}

void IMarkManageServiceActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgZCMarkPosition", this);
    m_IRegistryImpl->removeMessageListener("MsgZCVideoMarkNoPosition", this);
    m_IRegistryImpl->removeMessageListener("MsgGimbalAttitude", this);
    m_IRegistryImpl->removeMessageListener("MsgUAVAssignMark", this);
}

bool IMarkManageServiceActivator::addingService(const qnzkna::framework::ServiceReference &service)
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
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
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
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pINebulaTaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
    }
    return true;
}

bool IMarkManageServiceActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
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
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pINebulaTaskManageService = nullptr;
    }
    return true;
}

bool IMarkManageServiceActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();

    return true;
}

