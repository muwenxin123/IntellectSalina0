#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSettings>

#include "IProtocolParseXygsServerImpl.h"

#include "DataManage/Common/TypeDefine_Weapon.h"
#include "DataManage/NebulaUAVState/NebulaStatePointInfo.h"
#include "DataManage/Common/TypeDefine_WeaponSystem.h"
#include "DataManage/KeyEvent/SimuPosition.h"

IProtocolParseXygsServerImpl::IProtocolParseXygsServerImpl(void)
{

}

IProtocolParseXygsServerImpl::~IProtocolParseXygsServerImpl(void)
{
}

bool IProtocolParseXygsServerImpl::PackXygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{
    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    const unsigned int nInBufLen = msg->getMessageLength();
    if (0 >= nInBufLen){
        return false;
    }

    return false;
}

bool IProtocolParseXygsServerImpl::UnPackXygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{
    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    return false;
}
