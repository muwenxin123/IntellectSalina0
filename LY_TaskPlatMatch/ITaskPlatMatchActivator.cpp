#include "ITaskPlatMatchActivator.h"
#include "lY_TaskPlatMatch.h"

ITaskPlatMatchActivator::ITaskPlatMatchActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pITaskManageService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIFormationManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIMarkManageService(nullptr)
    , m_pICommandUnitManageService(nullptr)
{

}

ITaskPlatMatchActivator::~ITaskPlatMatchActivator()
{
    m_IRegistryImpl = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pITaskManageService = nullptr;
    m_pIWeaponManageService = nullptr;
    m_pIFormationManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pIMarkManageService = nullptr;
    m_pICommandUnitManageService = nullptr;
}

ITaskPlatMatchActivator &ITaskPlatMatchActivator::Get()
{
    static ITaskPlatMatchActivator sITaskManagerActivator;
    return sITaskManagerActivator;
}

qnzkna::framework::IRegistry *ITaskPlatMatchActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *ITaskPlatMatchActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::WeaponManage::IWeaponManageService *ITaskPlatMatchActivator::getPWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::TaskManage::INebulaTaskManageService *ITaskPlatMatchActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

qnzkna::UAVFormManage::IFormationManageService *ITaskPlatMatchActivator::getPFormationManageService() const
{
    return m_pIFormationManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *ITaskPlatMatchActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::MarkManage::IMarkManageService *ITaskPlatMatchActivator::getPIMarkManageService() const
{
    return m_pIMarkManageService;
}

qnzkna::CCommandUnitManage::ICommandUnitManageService *ITaskPlatMatchActivator::getPICommandUnitManageService() const
{
    return m_pICommandUnitManageService;
}

bool ITaskPlatMatchActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentComponentID)
{
    return true;
}

void ITaskPlatMatchActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->addServiceListener(this);
    m_IRegistryImpl->addMessageListener("MsgPlatMateMerge", this);

}

void ITaskPlatMatchActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgPlatMateMerge", this);

}

bool ITaskPlatMatchActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
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

    return true;
}

bool ITaskPlatMatchActivator::removedService(const qnzkna::framework::ServiceReference &service)
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
    return true;
}

bool ITaskPlatMatchActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgPlatMateMerge")
    {
        const std::string &strPlatMateMerge = message.getMessageProperties("UpdatePlatMateList");
        if (strPlatMateMerge.empty())
        {
            return true;
        }
        if (strPlatMateMerge == "1")
        {

            LY_TaskPlatMatch::Get().isPlatMateMerge = true;
        }
    }
    return true;
}

