#include "LY_AerialShotSettingBundle54Service.h"
#include "LY_AerialShotSettingBundle54ServiceInterface.h"

#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IAerialShotSettingBundle54Activator.h"

#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    #include "Protocol/XygsSeverClientControl/GimbalControlZC54_SeverClient.h"
    #include <Protocol/XygsZC/Bag_XygsZCDefines.h>
#endif

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_AerialShotSettingBundle54Service::LY_AerialShotSettingBundle54Service()
#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    : m_NetServerClient(true)
    , m_SeatID(0)
    , m_pWnd(nullptr)
    , m_pIAerialShotSettingBundle54Dialog(nullptr)
#else
    : m_pWnd(nullptr)
    , m_pIAerialShotSettingBundle54Dialog(nullptr)
#endif
{
    IAerialShotSettingBundle54Activator::Get();

#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);
    settings.beginGroup("NetServerClient");
    if (settings.contains("ServerClient"))
    {
        m_NetServerClient = settings.value("ServerClient", true).toBool();
    }
    settings.endGroup();

    settings.beginGroup("NetServerClientSeat");
    if (settings.contains("ServerClientSeatID"))
    {
        m_SeatID = settings.value("ServerClientSeatID", 0).toUInt();
    }
    settings.endGroup();
#endif

}

void LY_AerialShotSettingBundle54Service::Startup()
{
    IFunctionInterface &ifi = LY_AerialShotSettingBundle54ServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAerialShotSettingBundle54Activator::Get().start(pIRegistry);
        }
    }
}

void LY_AerialShotSettingBundle54Service::Shutdown()
{
    IFunctionInterface &ifi = LY_AerialShotSettingBundle54ServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAerialShotSettingBundle54Activator::Get().stop(pIRegistry);
        }
    }
}

void LY_AerialShotSettingBundle54Service::Reset()
{

}

bool LY_AerialShotSettingBundle54Service::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AerialShotSettingBundle54Service::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AerialShotSettingBundle54Service::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AerialShotSettingBundle54Service::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AerialShotSettingBundle54Service::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:
    {

    }
    break;
    case 44777:
    {

    }
    break;
    case 44778:
    {

    }
    break;
    default:
        break;
    }
    return false;
}

bool LY_AerialShotSettingBundle54Service::OnCreate(int wParam, QWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {
        m_pWnd = pTab;
    }
    break;
    case 1:
    {
        LyDockWidget *pDock = LyDockManager::instance()->dockWidget("AerialShotSettingBundle54");
        if (pDock == nullptr)
        {
            pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, IAerialShotSettingBundle54Dialog::tr("Photoelectric Scout 54 Setting"), "AerialShotSettingBundle54");
            LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
        }

        if (m_pIAerialShotSettingBundle54Dialog == nullptr)
        {
            IAerialShotSettingBundle54Dialog *pIAerialShotSettingBundle54DialogTemp = new IAerialShotSettingBundle54Dialog();
            pIAerialShotSettingBundle54DialogTemp->InitWeaponInfoList();
            pDock->setCentralWidget(pIAerialShotSettingBundle54DialogTemp);
            pDock->setWindowTitle(IAerialShotSettingBundle54Dialog::tr("Photoelectric Scout 54 Setting"));
            LyDockWidget::connect(pIAerialShotSettingBundle54DialogTemp, &IAerialShotSettingBundle54Dialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);

            m_pIAerialShotSettingBundle54Dialog = pIAerialShotSettingBundle54DialogTemp;
        }
    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
    case 4:
        break;
    }
    return false;
}

bool LY_AerialShotSettingBundle54Service::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_AerialShotSettingBundle54Service::OnNetRecv(int nLength, char *lpData)
{
#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    if ((!m_NetServerClient) || (nLength <= sizeof(NetHeader)))
    {
        return false;
    }

    const auto *const pNetMsg =
        (const qnzkna::NetSeverClient::XygsGimbalControlZC54::GimbalControlZC54MsgStatus * const)(lpData);

    if ((pNetMsg->header.ucType != NETHEADER_UCTYPE_ZC54_CONTROL) ||
            (pNetMsg->header.ucSubType != NETHEADER_UCSUBTYPE_ZC54_CONTROL_DOWN_TO_CLIENT) ||
            (pNetMsg->header.unNodeType != NODE_TYPE_SENSOR) ||
            (pNetMsg->header.unSeatType != NETHEADER_UNSEATTYPE_SERVER)
       )
    {
        return false;
    }

    const int msgLength = pNetMsg->header.unLength - sizeof(NetHeader);
    if (msgLength <= 0 || msgLength != AMAPPOD54CONTROLSTATUS_LENGTH)
    {
        return false;
    }

    if ((pNetMsg->aMapPod54ControlStatus_MsgStatus_Info.head[0] != AMAPPOD54CONTROLSTATUS_FIRST_HEAD) ||
            (pNetMsg->aMapPod54ControlStatus_MsgStatus_Info.head[1] != AMAPPOD54CONTROLSTATUS_SECOND_HEAD) ||
            (pNetMsg->aMapPod54ControlStatus_MsgStatus_Info.end != AMAPPOD54CONTROLSTATUS_TAIL)
       )
    {
        return true;
    }

    const QString &strSensorId = QString::number(pNetMsg->header.unNodeID);
    if (strSensorId.isEmpty())
    {
        return true;
    }

    const char *const msg = ((const char *const) & (pNetMsg->aMapPod54ControlStatus_MsgStatus_Info));

    if (m_pIAerialShotSettingBundle54Dialog != nullptr)
    {
        m_pIAerialShotSettingBundle54Dialog->on_Message_Recv(strSensorId, QByteArray(msg, msgLength));
    }
#else
    Q_UNUSED(nLength)
    Q_UNUSED(lpData)
#endif
    return false;
}

bool LY_AerialShotSettingBundle54Service::NetServerClientSend(const AMapPod54Control &aMapPod54Control, const QString &strSensorID)
{
#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    if ((!m_NetServerClient))
    {
        return false;
    }

    bool ok = false;
    const unsigned long long unNodeID = strSensorID.toULongLong(&ok);
    if (!ok)
    {
        return false;
    }

    qnzkna::NetSeverClient::XygsGimbalControlZC54::GimbalControlZC54 netMsg;

    netMsg.header.ucType       = NETHEADER_UCTYPE_ZC54_CONTROL;
    netMsg.header.ucSubType    = NETHEADER_UCSUBTYPE_ZC54_CONTROL_UP_TO_SERVER;
    netMsg.header.unLength     = 0;
    netMsg.header.unMask       = NETHEADER_UNMASK_SERVER_CLIENT;

    netMsg.header.unNodeType   = NODE_TYPE_SENSOR;
    netMsg.header.unNodeID     = unNodeID;
    netMsg.header.unSeatType   = NETHEADER_UNSEATTYPE_CLIENT;
    netMsg.header.unSeatID     = m_SeatID;

    netMsg.aMapPod54Control_Info = aMapPod54Control;

    netMsg.header.unLength = sizeof(aMapPod54Control) + sizeof(netMsg.header);

    LY_AerialShotSettingBundle54ServiceInterface::Get().SendData((const char *)(&netMsg), netMsg.header.unLength);
#else
    Q_UNUSED(aMapPod54Control)
    Q_UNUSED(strSensorID)
#endif

    return true;
}

IAerialShotSettingBundle54Dialog *LY_AerialShotSettingBundle54Service::getPIAerialShotSettingBundle54Dialog() const
{
    return m_pIAerialShotSettingBundle54Dialog;
}

