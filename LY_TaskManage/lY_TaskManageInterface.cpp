#include "lY_TaskManageInterface.h"
#include "lY_TaskManage.h"
#include "netheader.h"
#include "Protocol/XygsSystemControl/SystemControl.h"
#include "ITaskManagerActivator.h"
#include "zzTime.h"

extern "C" LY_TASKMANAGESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_TaskManageInterface::Get();
}

LY_TaskManageInterface &LY_TaskManageInterface::Get()
{
    static LY_TaskManageInterface gi;
    return gi;
}

void LY_TaskManageInterface::Startup()
{
    LY_TaskManage::Get().Startup();
}

void LY_TaskManageInterface::Shutdown()
{
    LY_TaskManage::Get().Shutdown();
}

void LY_TaskManageInterface::Reset()
{
    LY_TaskManage::Get().Reset();
}

bool LY_TaskManageInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_TaskManage::Get().DrawGraph(painter, pProj);
}

bool LY_TaskManageInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_TaskManage::Get().DrawDynamic(painter, pProj);
}

bool LY_TaskManageInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_TaskManage::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_TaskManage::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_TaskManage::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_TaskManage::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_TaskManage::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LY_TaskManage::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LY_TaskManage::Get().OnMouseMove((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_TaskManage::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_TaskManage::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_TaskManage::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}

bool LY_TaskManageInterface::OnNetRecv(int nLength, char *lpData)
{
    NetHeader *netHead = (NetHeader *)lpData;
    if (netHead->ucType == NETHEADER_UCTYPE_SYSTEM_UPDATE)
    {
        SystemUpdate *supdate = (SystemUpdate *)(lpData + sizeof(NetHeader));
        std::string loginName = ITaskManagerActivator::Get().getPTaskManageService()->getLoginName();
        std::string operatorName = supdate->Operator;
        if (loginName == operatorName)
        {
            return false;
        }
        switch (supdate->updateType)
        {
        case ESystemUpdateType::ESystemUpdateType_Route:
        {
            LY_TaskManage::Get().GetDlgMissionLineList()->updateMenu();
        }
        break;
        case ESystemUpdateType::ESystemUpdateType_Task:
        {
            LY_TaskManage::Get().GetDlgMissionList()->updateMenu();
        }
        break;

        }
    }
    else if (netHead->ucType == NETHEADER_UCTYPE_BATTLE_TIME && netHead->ucSubType == 1)
    {
        ZzTime *pZzTime = (ZzTime *)lpData;
        QLabel *pLabel = LY_TaskManage::Get().GetZzTimeLable();
        if (pLabel != NULL)
        {
            pLabel->setText(QDateTime::fromMSecsSinceEpoch(pZzTime->nTime).toString("yyyy-MM-dd hh:mm:ss"));
        }
    }

    return false;
}
