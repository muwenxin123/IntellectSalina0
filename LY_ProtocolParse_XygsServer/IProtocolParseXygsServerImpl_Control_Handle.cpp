#include "IProtocolParseXygsServerImpl_Control_Handle.h"

#include "IProtocolParseXygsServerImpl_Control.h"

#include "netheader.h"

namespace qnzkna {

namespace NetSeverClient {

void IProtocolParseXygsServerImpl_Control_Work::doDealingMessage()
{
    forever
    {
        QLinkedList<const QByteArray> listMessage;
        listMessage.clear();

        QMutexLocker lockerMessage(&m_mutexMessage);
        if(m_exited){
            return ;
        }

        if(m_listMessage.size() == 0){
            m_mutexMessageWaitCondition.wait(&m_mutexMessage);
        }
        listMessage = std::move(m_listMessage);
        m_listMessage.clear();
        lockerMessage.unlock();

        if(listMessage.size() > 0){
            QLinkedList<const QByteArray>::const_iterator messageReferenceIter;
            for (messageReferenceIter = listMessage.constBegin(); messageReferenceIter != listMessage.constEnd(); ++messageReferenceIter){
                if((!qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_Control::getNetServerClient())
                        || (messageReferenceIter->size() <= sizeof (NetHeader))
                        ) {
                    continue;
                }

                for(const auto& IProtocolParseXygsServerImpl_Control : qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_ControlList())
                {
                    if(IProtocolParseXygsServerImpl_Control->NetHeader_XygsServer(messageReferenceIter->size(), messageReferenceIter->constData()))
                    {
                        IProtocolParseXygsServerImpl_Control->NetRecv_XygsServer(messageReferenceIter->size(), messageReferenceIter->constData());
                        break;
                    }
                }
            }
        }

        listMessage.clear();
    }
}

IProtocolParseXygsServerImpl_Control_HandleList &IProtocolParseXygsServerImpl_Control_HandleList::Get()
{

    static IProtocolParseXygsServerImpl_Control_HandleList s_IProtocolParseXygsServerImpl_Control_HandleList;
    return s_IProtocolParseXygsServerImpl_Control_HandleList;
}

void IProtocolParseXygsServerImpl_Control_HandleList::addProtocolParseXygsServerImpl_ControlList(IProtocolParseXygsServerImpl_Control *pIProtocolParseXygsServerImpl_Control)
{
    m_ProtocolParseXygsServerImpl_ControlList.push_back(pIProtocolParseXygsServerImpl_Control);
}

const std::list<IProtocolParseXygsServerImpl_Control *> &IProtocolParseXygsServerImpl_Control_HandleList::getProtocolParseXygsServerImpl_ControlList() const{
    return m_ProtocolParseXygsServerImpl_ControlList;
}

IProtocolParseXygsServerImpl_Control_HandleList::IProtocolParseXygsServerImpl_Control_HandleList()
    : m_ProtocolParseXygsServerImpl_ControlList()
{

}

IProtocolParseXygsServerImpl_Control_HandleList::~IProtocolParseXygsServerImpl_Control_HandleList()
{

}

const std::list<IProtocolParseXygsServerImpl_Control *>& IProtocolParseXygsServerImpl_ControlList()
{
    return IProtocolParseXygsServerImpl_Control_HandleList::Get().getProtocolParseXygsServerImpl_ControlList();
}

void IProtocolParseXygsServerImpl_Start()
{
    IProtocolParseXygsServerImpl_Control_Controller::Get();
}

void IProtocolParseXygsServerImpl_Stop()
{
    IProtocolParseXygsServerImpl_Control_Controller::Get().stopMessageInterfaceTracker();
}

void IProtocolParseXygsServerImpl_RecvMessage(const int nLength, const char * const lpData)
{
    IProtocolParseXygsServerImpl_Control_Controller::Get().handleMessage(QByteArray(lpData, nLength));
}

}

}
