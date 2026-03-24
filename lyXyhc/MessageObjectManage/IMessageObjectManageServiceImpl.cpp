#include "IMessageObjectManageServiceImpl.h"

using namespace qnzkna::MessageObjectManage;

IMessageObjectManageServiceImpl &IMessageObjectManageServiceImpl::Get()
{
    static IMessageObjectManageServiceImpl messageObjectManageServiceImpl;
    return messageObjectManageServiceImpl;
}

IMessageObjectManageServiceImpl::IMessageObjectManageServiceImpl()
    : IMessageObjectManageService()
{
    m_MessageObjectMap.clear();
    m_LinsterObjectMap.clear();
}

IMessageObjectManageServiceImpl::~IMessageObjectManageServiceImpl()
{
    QMap<QString, QObject *>::iterator iter = m_MessageObjectMap.begin();
    for (; iter != m_MessageObjectMap.end();)
    {

        const QString messageObjectName = iter.key();
        QObject *messageObject = iter.value();

        iter = m_MessageObjectMap.erase(iter);

        UpdateMessageObjectLinster(messageObjectName, messageObject);
    }
}

QObject *IMessageObjectManageServiceImpl::GetMessageObject(const QString &messageObjectName)
{
    QObject *messageObject = m_MessageObjectMap.value(messageObjectName, nullptr);

    return messageObject;
}

bool IMessageObjectManageServiceImpl::AddMessageObject(const QString &messageObjectName, QObject *messageObject)
{
    if (messageObjectName.isEmpty() || messageObject == nullptr)
    {
        return false;
    }

    if (m_MessageObjectMap.contains(messageObjectName))
    {
        return false;
    }

    m_MessageObjectMap.insert(messageObjectName, messageObject);

    UpdateMessageObjectLinster(messageObjectName, messageObject);

    return true;
}

bool IMessageObjectManageServiceImpl::RemoveMessageObject(const QString &messageObjectName)
{
    if (messageObjectName.isEmpty())
    {
        return false;
    }

    QMap<QString, QObject *>::iterator iter = m_MessageObjectMap.begin();
    for (; iter != m_MessageObjectMap.end();)
    {
        if (iter.key() == messageObjectName)
        {

            const QString messageObjectName = iter.key();
            QObject *messageObject = iter.value();

            iter = m_MessageObjectMap.erase(iter);

            UpdateMessageObjectLinster(messageObjectName, messageObject);

            continue;
        }
        iter++;
    }

    return true;
}

bool IMessageObjectManageServiceImpl::RemoveMessageObject(QObject *messageObject)
{
    if (messageObject == nullptr)
    {
        return false;
    }

    QMap<QString, QObject *>::iterator iter = m_MessageObjectMap.begin();
    for (; iter != m_MessageObjectMap.end();)
    {
        if (iter.value() == messageObject)
        {

            const QString messageObjectName = iter.key();
            QObject *messageObject = iter.value();

            iter = m_MessageObjectMap.erase(iter);

            UpdateMessageObjectLinster(messageObjectName, messageObject);

            continue;
        }
        iter++;
    }

    return true;
}

bool IMessageObjectManageServiceImpl::AddMessageObjectLinster(QObject *linsterObject, std::function<void (const QString &, QObject *)> fun)
{
    if (m_LinsterObjectMap.contains(linsterObject))
    {
        return false;
    }

    m_LinsterObjectMap.insert(linsterObject, fun);
    return true;
}

void IMessageObjectManageServiceImpl::UpdateMessageObjectLinster(const QString &messageObjectName, QObject *messageObject)
{
    for (auto &fun : m_LinsterObjectMap)
    {
        fun(messageObjectName, messageObject);
    }
}
