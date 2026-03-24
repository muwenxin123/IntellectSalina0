#include "IScenariosReplayActivator.h"

IScenariosReplayActivator::IScenariosReplayActivator()
    : m_IRegistryImpl(nullptr)
	,m_pIConfigProjectControlService(nullptr)
{

}

IScenariosReplayActivator::~IScenariosReplayActivator()
{
    m_IRegistryImpl = nullptr;
	m_pIConfigProjectControlService = nullptr;
}

IScenariosReplayActivator &IScenariosReplayActivator::Get()
{
    static IScenariosReplayActivator sIScenariosReplayActivator;
    return sIScenariosReplayActivator;
}
bool IScenariosReplayActivator::addingService(const qnzkna::framework::ServiceReference& service)
{
	if (service.getService() == nullptr) return false;
	if (service.getServiceName() == "Ly.IConfigProjectControlService") {
		m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService*>(service.getService());
	}
	return true;
}
bool IScenariosReplayActivator::removedService(const qnzkna::framework::ServiceReference& service)
{
	if (service.getServiceName() == "Ly.IConfigProjectControlService") {
		m_pIConfigProjectControlService = nullptr;
	}
	return true;
}
qnzkna::framework::IRegistry *IScenariosReplayActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}
qnzkna::SystemConfig::IConfigProjectControlService		*IScenariosReplayActivator::getConfigProjectControlService()	const
{
	return m_pIConfigProjectControlService;
}
void IScenariosReplayActivator::start(qnzkna::framework::IRegistry * const context)
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }
	m_IRegistryImpl->addServiceListener(this);
}

void IScenariosReplayActivator::stop(qnzkna::framework::IRegistry * const context)
{
    m_IRegistryImpl = context;
    if(m_IRegistryImpl == nullptr){
        return ;
    }
	m_IRegistryImpl->removeServiceListener(this);
}

bool IScenariosReplayActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    return true;
}
