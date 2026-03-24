#include "ServiceFramework/IMessageInterfaceTracker.h"
#include "IUAVRealTimeStateManageActivator.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"

IUAVRealTimeStateManageActivator::IUAVRealTimeStateManageActivator()
{

}

IUAVRealTimeStateManageActivator::~IUAVRealTimeStateManageActivator()
{

}

void IUAVRealTimeStateManageActivator::OnTimer(int timeID)
{
    m_IUAVRealTimeStateManageImpl.OnTimerCallbackFun(LOCAL_TIMER_ID);
}

void IUAVRealTimeStateManageActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IUAVRealTimeStateManageService", &(m_IUAVRealTimeStateManageImpl)));

    m_IRegistryImpl->addMessageListener("MsgUAVRealTimeState", this);
}

void IUAVRealTimeStateManageActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IUAVRealTimeStateManageService", &(m_IUAVRealTimeStateManageImpl)));

    m_IRegistryImpl->removeMessageListener("MsgUAVRealTimeState", this);
}

bool IUAVRealTimeStateManageActivator::dealingMessage(const qnzkna::framework::MessageReference &ref)
{
    const std::string &messageName = ref.getMessageName();
    if (messageName == "MsgUAVRealTimeState")
    {
        const char        *message       = ref.getMessage();
        const unsigned int messageLength = ref.getMessageLength();
        if ((0 == messageLength) || (messageLength != sizeof(xygsJL_20)))
        {
            return true;
        }

        const std::string &strWeaponId     = ref.getMessageProperties("TargetID");
        const std::string &strProtocolType = ref.getMessageProperties("ProtocolType");

        if (strWeaponId.empty())
        {
            return true;
        }

        const xygsJL_20 *pInfo = (const xygsJL_20 *)(message);
        m_IUAVRealTimeStateManageImpl.SetUAVRealTimeState(strWeaponId, strProtocolType, (pInfo->param));
    }

    return true;
}
