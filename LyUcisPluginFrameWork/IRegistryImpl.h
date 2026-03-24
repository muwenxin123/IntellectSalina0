#ifndef IREGISTRYIMPL_H
#define IREGISTRYIMPL_H

#include <map>
#include <list>

#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "IMessageInterfaceTrackerImpl.h"

#include <QReadWriteLock>
#include <QWriteLocker>
#include <QReadLocker>

#include <QCoreApplication>

namespace qnzkna
{

namespace framework
{

class IRegistryImpl : public IRegistry
{
public:
    IRegistryImpl()
    {
        m_mapServiceInfoMap.clear();
        m_mapServiceListenerMap.clear();

        m_mapMessageListenerMap.clear();
    }

    virtual ~IRegistryImpl() override {}

    virtual bool addServiceListener(qnzkna::framework::IServiceInterfaceTracker *const  serviceListener) override
    {
        if (serviceListener == nullptr)
        {
            return false;
        }

        QWriteLocker locker(&m_lockServiceListener);
        if (m_mapServiceListenerMap.find(serviceListener) != m_mapServiceListenerMap.end())
        {
            return false;
        }

        for (const auto &serviceInfo :  m_mapServiceInfoMap)
        {
            serviceListener->addingService(serviceInfo.second);
        }

        auto ret = m_mapServiceListenerMap.insert(std::make_pair(serviceListener, serviceListener));
        return (ret.second);
    }

    virtual bool removeServiceListener(qnzkna::framework::IServiceInterfaceTracker *const serviceListener) override
    {
        if (serviceListener == nullptr)
        {
            return false;
        }

        QWriteLocker locker(&m_lockServiceListener);
        if (m_mapServiceListenerMap.find(serviceListener) == m_mapServiceListenerMap.end())
        {
            return false;
        }

        for (const auto &serviceInfo :  m_mapServiceInfoMap)
        {
            serviceListener->removedService(serviceInfo.second);
        }

        auto ret = m_mapServiceListenerMap.erase(serviceListener);
        return (ret);
    }

    virtual bool registerService(const qnzkna::framework::ServiceReference &service) override
    {
        if (service.getService() == nullptr || service.getServiceName().empty())
        {
            return false;
        }

        QWriteLocker locker(&m_lockServiceListener);
        if (m_mapServiceInfoMap.find(service.getServiceName()) != m_mapServiceInfoMap.end())
        {
            return false;
        }

        for (const auto &serviceListener :  m_mapServiceListenerMap)
        {
            serviceListener.second->addingService(service);
        }

        auto ret = m_mapServiceInfoMap.insert(std::make_pair(service.getServiceName(), service));
        return (ret.second);
    }

    virtual bool unregisterService(const qnzkna::framework::ServiceReference &service) override
    {
        if (service.getService() == nullptr || service.getServiceName().empty())
        {
            return false;
        }

        QWriteLocker locker(&m_lockServiceListener);
        if (m_mapServiceInfoMap.find(service.getServiceName()) == m_mapServiceInfoMap.end())
        {
            return false;
        }

        for (const auto &serviceListener :  m_mapServiceListenerMap)
        {
            serviceListener.second->removedService(service);
        }

        auto ret = m_mapServiceInfoMap.erase(service.getServiceName());
        return (ret);
    }

    void unregisterAllService()
    {
        QWriteLocker locker(&m_lockServiceListener);
        for (const auto &serviceListener :  m_mapServiceListenerMap)
        {
            for (const auto &serviceInfo :  m_mapServiceInfoMap)
            {
                serviceListener.second->removedService(serviceInfo.second);
            }
        }

        m_mapServiceInfoMap.clear();
    }

    virtual bool addMessageListener(const std::string &messageName, qnzkna::framework::IMessageInterfaceTracker *const  messageListener) override
    {
        if (messageListener == nullptr)
        {
            return false;
        }

        QWriteLocker locker(&m_lockMessageListener);
        auto iter = m_mapMessageListenerMap.find(messageName);
        if (iter == m_mapMessageListenerMap.end())
        {
            auto ret = m_mapMessageListenerMap.insert(std::make_pair(messageName, std::map<IMessageInterfaceTracker *const, IMessageInterfaceTracker *const> {std::make_pair(messageListener, messageListener)}));
            return (ret.second);
        }

        auto ret = iter->second.insert(std::make_pair(messageListener, messageListener));
        return (ret.second);
    }

    bool removeMessageListener(const std::string &messageName, qnzkna::framework::IMessageInterfaceTracker *const  messageListener) override
    {
        if (messageListener == nullptr)
        {
            return false;
        }

        QWriteLocker locker(&m_lockMessageListener);
        auto iter = m_mapMessageListenerMap.find(messageName);
        if (iter == m_mapMessageListenerMap.end())
        {
            return true;
        }

        auto iterMessageName = iter->second.find(messageListener);
        if (iterMessageName == iter->second.end())
        {
            return true;
        }

        auto ret = iter->second.erase(iterMessageName);
        return (ret != iter->second.end());
    }

    void unregisterAllMessage()
    {
        QWriteLocker locker(&m_lockMessageListener);
        for (auto &messageListener :  m_mapMessageListenerMap)
        {
            messageListener.second.clear();
        }

        m_mapMessageListenerMap.clear();
    }

    void stopAllMessageListener()
    {
        QReadLocker locker(&m_lockMessageListener);
        for (auto &messageListener :  m_mapMessageListenerMap)
        {
            for (auto &messageListeners :  messageListener.second)
            {
                if (messageListeners.first->m_MessageInterfaceTrackerController != nullptr)
                {
                    messageListeners.first->m_MessageInterfaceTrackerController->stopMessageInterfaceTracker();
                }
            }
        }

        locker.unlock();

        while (!m_lockMessageListener.tryLockForWrite(100))
        {
            QCoreApplication::processEvents();
        }

        for (auto &messageListener :  m_mapMessageListenerMap)
        {
            messageListener.second.clear();
        }

        m_mapMessageListenerMap.clear();

        m_lockMessageListener.unlock();
    }

    virtual bool messageNotify(const qnzkna::framework::MessageReference &message) const override
    {
        if (message.getMessage() == nullptr || message.getMessageName().empty())
        {
            return false;
        }

        QReadLocker locker(&m_lockMessageListener);
        auto iter = m_mapMessageListenerMap.find(message.getMessageName());
        if (iter == m_mapMessageListenerMap.end())
        {
            return false;
        }
        for (const auto &messageListener :  iter->second)
        {
            if (messageListener.first->m_MessageInterfaceTrackerController != nullptr)
            {
                messageListener.first->m_MessageInterfaceTrackerController->messageNotify(message);
            }
        }

        return true;
    }

    virtual bool messageNotify(const qnzkna::framework::MessageReference *const message) const override
    {
        QSharedPointer<const qnzkna::framework::MessageReference> messageSharedPointer(message);
        if (message == nullptr || message->getMessageName().empty())
        {
            return false;
        }

        QReadLocker locker(&m_lockMessageListener);
        auto iter = m_mapMessageListenerMap.find(message->getMessageName());
        if (iter == m_mapMessageListenerMap.end())
        {
            return false;
        }

        for (const auto &messageListener :  iter->second)
        {
            if (messageListener.first->m_MessageInterfaceTrackerController != nullptr)
            {
                messageListener.first->m_MessageInterfaceTrackerController->messageNotify(messageSharedPointer);
            }
        }

        return true;
    }

private:
    IRegistryImpl(const IRegistryImpl &) = delete;
    IRegistryImpl(const IRegistryImpl &&) = delete;
    IRegistryImpl &operator=(const IRegistryImpl &) = delete;
    IRegistryImpl &operator=(const IRegistryImpl &&) = delete;

private:
    QReadWriteLock                                                                                     m_lockServiceListener;
    std::map<std::string, ServiceReference>                                                            m_mapServiceInfoMap;
    std::map<IServiceInterfaceTracker *const, IServiceInterfaceTracker *const>                         m_mapServiceListenerMap;

    mutable  QReadWriteLock                                                                            m_lockMessageListener;
    std::map<std::string, std::map<IMessageInterfaceTracker *const, IMessageInterfaceTracker *const> > m_mapMessageListenerMap;
};

}
}
#endif
