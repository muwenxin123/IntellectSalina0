#include "IConcentrationActivator.h"

#include "LY_ConcentrationService.h"

IConcentrationActivator::IConcentrationActivator()
    : m_IRegistryImpl(nullptr)
{

}

IConcentrationActivator::~IConcentrationActivator()
{
    m_IRegistryImpl           = nullptr;
}

IConcentrationActivator &IConcentrationActivator::Get()
{
    static IConcentrationActivator sIConcentrationActivator;
    return sIConcentrationActivator;
}

qnzkna::framework::IRegistry *IConcentrationActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::TaskManage::INebulaTaskManageService* IConcentrationActivator::getPTaskManageService() const
{
	return m_pITaskManageService;
}

void IConcentrationActivator::start(qnzkna::framework::IRegistry* const context )
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

}

void IConcentrationActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

}

bool IConcentrationActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService*>(service.getService());
	}
    return true;
}

bool IConcentrationActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pITaskManageService = nullptr;
	}
    return true;
}

bool IConcentrationActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgAirInfoRecv")
    {

    }
    return true;
}
