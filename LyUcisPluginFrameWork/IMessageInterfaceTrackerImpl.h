#ifndef IMESSAGEINTERFACETRACKERIMPL_H
#define IMESSAGEINTERFACETRACKERIMPL_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QLinkedList>

#include "ServiceFramework/MessageReference.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

namespace qnzkna
{

namespace framework
{

class IMessageInterfaceTrackerWorker : public QObject
{
    Q_OBJECT

public:
    IMessageInterfaceTrackerWorker(qnzkna::framework::IMessageInterfaceTracker *const messageInterfaceTracker)
        : m_exited(false)
        , m_messageInterfaceTracker(messageInterfaceTracker)
    {
        m_listMessage.clear();
    }

    ~IMessageInterfaceTrackerWorker()
    {

    }

    void handleMessage(const QSharedPointer<const qnzkna::framework::MessageReference> &message)
    {
        QMutexLocker locker(&m_mutexMessage);
        m_listMessage.append(message);
        locker.unlock();
        m_mutexMessageWaitCondition.wakeAll();
    }

    bool getExited() const
    {
        return m_exited;
    }

    void setExited(bool exited)
    {
        QMutexLocker locker(&m_mutexMessage);
        m_exited = exited;
        locker.unlock();
        m_mutexMessageWaitCondition.wakeAll();
    }

public slots:
    void doDealingMessage()
    {
        forever
        {
            QLinkedList<const QSharedPointer<const qnzkna::framework::MessageReference>> listMessage;
            listMessage.clear();

            QMutexLocker lockerMessage(&m_mutexMessage);
            if (m_exited)
            {
                return ;
            }

            if (m_listMessage.size() == 0)
            {
                m_mutexMessageWaitCondition.wait(&m_mutexMessage);
            }
            listMessage = std::move(m_listMessage);
            m_listMessage.clear();
            lockerMessage.unlock();

            if (m_messageInterfaceTracker != nullptr)
            {
                QLinkedList<const QSharedPointer<const qnzkna::framework::MessageReference>>::const_iterator messageReferenceIter;
                for (messageReferenceIter = listMessage.constBegin(); messageReferenceIter != listMessage.constEnd(); ++messageReferenceIter)
                {
                    if (!messageReferenceIter->isNull())
                    {
                        m_messageInterfaceTracker->dealingMessage(*(messageReferenceIter->data()));
                    }
                }
            }

            listMessage.clear();
        }
    }

private:
    IMessageInterfaceTrackerWorker(const IMessageInterfaceTrackerWorker &) = delete;
    IMessageInterfaceTrackerWorker &operator=(const IMessageInterfaceTrackerWorker &) = delete;

private:
    volatile bool                                                                m_exited;
    qnzkna::framework::IMessageInterfaceTracker *const                           m_messageInterfaceTracker = nullptr;
    QWaitCondition                                                               m_mutexMessageWaitCondition;
    QMutex                                                                       m_mutexMessage;
    QLinkedList<const QSharedPointer<const qnzkna::framework::MessageReference>> m_listMessage;
};

class IMessageInterfaceTrackerController : public QObject
{
    Q_OBJECT

public:
    IMessageInterfaceTrackerController(qnzkna::framework::IMessageInterfaceTracker *const  messageInterfaceTracker)
        : m_messageInterfaceTrackerWorker(new IMessageInterfaceTrackerWorker(messageInterfaceTracker))
    {
        m_messageInterfaceTrackerWorker->moveToThread(&m_workerThread);

        connect(&m_workerThread, &QThread::finished, [this]()
        {
            if (m_messageInterfaceTrackerWorker != nullptr)
            {
                delete  m_messageInterfaceTrackerWorker;
                m_messageInterfaceTrackerWorker = nullptr;
            }
        });
        connect(this, &IMessageInterfaceTrackerController::dealingMessage, m_messageInterfaceTrackerWorker, &IMessageInterfaceTrackerWorker::doDealingMessage, Qt::QueuedConnection);

        m_workerThread.start();

        emit dealingMessage();
    }

    ~IMessageInterfaceTrackerController()
    {
        if (m_messageInterfaceTrackerWorker != nullptr)
        {
            m_messageInterfaceTrackerWorker->setExited(true);
        }

        m_workerThread.quit();
        m_workerThread.wait();
    }

    void stopMessageInterfaceTracker()
    {
        if (m_messageInterfaceTrackerWorker != nullptr)
        {
            m_messageInterfaceTrackerWorker->setExited(true);
        }

        m_workerThread.quit();
        m_workerThread.wait();
    }

    bool handleMessage(const QSharedPointer< const qnzkna::framework::MessageReference> &message) const
    {
        if (m_messageInterfaceTrackerWorker != nullptr)
        {
            m_messageInterfaceTrackerWorker->handleMessage(message);
            return true;
        }
        return false;
    }

    bool messageNotify(const qnzkna::framework::MessageReference &message) const;
    bool messageNotify(const QSharedPointer< const qnzkna::framework::MessageReference> &message) const;

signals:
    void dealingMessage();

private:
    IMessageInterfaceTrackerController(const IMessageInterfaceTrackerController &) = delete;
    IMessageInterfaceTrackerController &operator=(const IMessageInterfaceTrackerController &) = delete;

private:
    QThread                         m_workerThread;
    IMessageInterfaceTrackerWorker *m_messageInterfaceTrackerWorker = nullptr;
};

}
}

#endif
