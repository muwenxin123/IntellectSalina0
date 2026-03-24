#ifndef IPROTOCOLPARSEXYGSSERVERIMPL_CONTROL_HANDLE_H
#define IPROTOCOLPARSEXYGSSERVERIMPL_CONTROL_HANDLE_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QLinkedList>
#include <QByteArray>
#include <QDateTime>
#include <QThread>
#include <QDebug>

#include <list>

namespace qnzkna {

namespace NetSeverClient {

class IProtocolParseXygsServerImpl_Control;
class IProtocolParseXygsServerImpl_Control_Work : public QObject
{
    Q_OBJECT

public:
    IProtocolParseXygsServerImpl_Control_Work()
        : QObject(nullptr)
        , m_exited(false)
    {
        m_listMessage.clear();
    }

    ~IProtocolParseXygsServerImpl_Control_Work()
    {

    }

    void handleMessage(const QByteArray& message)
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
    void doDealingMessage();

private:
    IProtocolParseXygsServerImpl_Control_Work(const IProtocolParseXygsServerImpl_Control_Work&) = delete;
    IProtocolParseXygsServerImpl_Control_Work& operator=(const IProtocolParseXygsServerImpl_Control_Work&) = delete;
    IProtocolParseXygsServerImpl_Control_Work(const IProtocolParseXygsServerImpl_Control_Work&&) = delete;
    IProtocolParseXygsServerImpl_Control_Work& operator=(const IProtocolParseXygsServerImpl_Control_Work&&) = delete;

private:
    volatile bool                   m_exited;
    QWaitCondition                  m_mutexMessageWaitCondition;
    QMutex                          m_mutexMessage;
    QLinkedList<const QByteArray>   m_listMessage;
};

class IProtocolParseXygsServerImpl_Control_Controller : public QObject
{
    Q_OBJECT

public:
    static IProtocolParseXygsServerImpl_Control_Controller& Get()
    {
        static IProtocolParseXygsServerImpl_Control_Controller s_IProtocolParseXygsServerImpl_Control_Controller;
        return s_IProtocolParseXygsServerImpl_Control_Controller;
    }

    ~IProtocolParseXygsServerImpl_Control_Controller()
    {
        m_ProtocolParseXygsServerImpl_Control_Work.setExited(true);

        m_workerThread.quit();
        m_workerThread.wait();
    }

    void stopMessageInterfaceTracker()
    {
        m_ProtocolParseXygsServerImpl_Control_Work.setExited(true);

        m_workerThread.quit();
        m_workerThread.wait();
    }

    void handleMessage(const QByteArray& message)
    {
        m_ProtocolParseXygsServerImpl_Control_Work.handleMessage(message);
    }

signals:
    void dealingMessage();

private:
    IProtocolParseXygsServerImpl_Control_Controller()
        : QObject()
        , m_ProtocolParseXygsServerImpl_Control_Work()
    {
        m_ProtocolParseXygsServerImpl_Control_Work.moveToThread(&m_workerThread);

        connect(&m_workerThread, &QThread::finished, [this](){
            m_ProtocolParseXygsServerImpl_Control_Work.setExited(true);
        });

        connect(this, &IProtocolParseXygsServerImpl_Control_Controller::dealingMessage,
                         &m_ProtocolParseXygsServerImpl_Control_Work, &IProtocolParseXygsServerImpl_Control_Work::doDealingMessage, Qt::QueuedConnection);

        m_workerThread.start();

        emit dealingMessage();
    }

    IProtocolParseXygsServerImpl_Control_Controller(const IProtocolParseXygsServerImpl_Control_Controller&) = delete;
    IProtocolParseXygsServerImpl_Control_Controller& operator=(const IProtocolParseXygsServerImpl_Control_Controller&) = delete;
    IProtocolParseXygsServerImpl_Control_Controller(const IProtocolParseXygsServerImpl_Control_Controller&&) = delete;
    IProtocolParseXygsServerImpl_Control_Controller& operator=(const IProtocolParseXygsServerImpl_Control_Controller&&) = delete;

private:
    IProtocolParseXygsServerImpl_Control_Work        m_ProtocolParseXygsServerImpl_Control_Work;
    QThread                                          m_workerThread;
};

class IProtocolParseXygsServerImpl_Control_HandleList
{
public:
    static IProtocolParseXygsServerImpl_Control_HandleList& Get();

    void addProtocolParseXygsServerImpl_ControlList(IProtocolParseXygsServerImpl_Control *pIProtocolParseXygsServerImpl_Control);

    const std::list<IProtocolParseXygsServerImpl_Control *>& getProtocolParseXygsServerImpl_ControlList() const;

private:
    IProtocolParseXygsServerImpl_Control_HandleList();

    virtual ~IProtocolParseXygsServerImpl_Control_HandleList();

private:
    std::list<IProtocolParseXygsServerImpl_Control*> m_ProtocolParseXygsServerImpl_ControlList;
    template<class classname>
    friend class IProtocolParseXygsServerImpl_Control_Handle;
};

template<class T>
class IProtocolParseXygsServerImpl_Control_Handle
{
public:
    IProtocolParseXygsServerImpl_Control_Handle()
    {
       IProtocolParseXygsServerImpl_Control_HandleList::Get().addProtocolParseXygsServerImpl_ControlList(new T);
    }

    virtual ~IProtocolParseXygsServerImpl_Control_Handle(){}
};

#define ADD_PROTOCOLPARSEXYGSSERVERIMPL_CONTROL_HANDLE(classname) static IProtocolParseXygsServerImpl_Control_Handle<classname> g_##classname;

}

}

#endif
