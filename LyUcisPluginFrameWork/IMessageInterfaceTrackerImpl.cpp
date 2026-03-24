#include "IMessageInterfaceTrackerImpl.h"

namespace qnzkna
{

namespace framework
{

IMessageInterfaceTracker::IMessageInterfaceTracker()
    : m_MessageInterfaceTrackerController(new IMessageInterfaceTrackerController(this))
{

}

IMessageInterfaceTracker::~IMessageInterfaceTracker()
{
    if (m_MessageInterfaceTrackerController != nullptr)
    {
        delete m_MessageInterfaceTrackerController;
        m_MessageInterfaceTrackerController = nullptr;
    }
}

bool IMessageInterfaceTrackerController::messageNotify(const qnzkna::framework::MessageReference &message) const
{
    QSharedPointer<const qnzkna::framework::MessageReference> messagePointer(
        new qnzkna::framework::MessageReference(message.getMessageName(), message.getMessage(), message.getMessageLength(), message.getMessageProperties()));
    return handleMessage(messagePointer);
}

bool IMessageInterfaceTrackerController::messageNotify(const QSharedPointer<const qnzkna::framework::MessageReference> &message) const
{
    return handleMessage(message);
}

}
}
