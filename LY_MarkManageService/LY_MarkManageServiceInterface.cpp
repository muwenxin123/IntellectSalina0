#include "LY_MarkManageServiceInterface.h"
#include "LY_MarkManageService.h"
#include "Protocol/XygsSystemControl/SystemControl.h"
#include "IMarkManageServiceActivator.h"

extern "C" LY_MARKMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_MarkManageServiceInterface::Get();
}

LY_MarkManageServiceInterface &LY_MarkManageServiceInterface::Get()
{
    static LY_MarkManageServiceInterface gi;
    return gi;
}

void LY_MarkManageServiceInterface::Startup()
{
    LY_MarkManageService::Get().Startup();
}

void LY_MarkManageServiceInterface::Shutdown()
{
    LY_MarkManageService::Get().Shutdown();
}

void LY_MarkManageServiceInterface::Reset()
{
    LY_MarkManageService::Get().Reset();
}

bool LY_MarkManageServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_MarkManageService::Get().DrawGraph(painter, pProj);
}

bool LY_MarkManageServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_MarkManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_MarkManageServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_MarkManageService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_MarkManageService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_MarkManageService::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_MarkManageService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_MarkManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LY_MarkManageService::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LY_MarkManageService::Get().OnMouseMove((QMouseEvent *)lParam);
    }
    break;

    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_MarkManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_MarkManageService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_MarkManageService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}

bool LY_MarkManageServiceInterface::OnNetRecv(int nLength, char *lpData)
{
    if (nLength <= 0)
    {
        return false;
    }
    NetHeader *pHead = (NetHeader *)lpData;

    if (NETHEADER_UCTYPE_TELEGRAPH_CMD == pHead->ucType)
    {
        TeleMsgHead  *packHeader = (TeleMsgHead *)(lpData + sizeof(NetHeader));
        const int MasterCode = packHeader->MainCode;
        const int SubCode = packHeader->SubCode;
        const int MsgIndex = packHeader->MsgIndex;
        if (MasterCode == 0xFB && SubCode == 0x04)
        {
            TeleMsg_FB_04 *tfb04 = (TeleMsg_FB_04 *)(lpData + sizeof(NetHeader));
            LY_MarkManageService::Get().GetDlgMarkList()->AddDBMark(*tfb04);
        }
    }

    if (NETHEADER_UCTYPE_SYSTEM_UPDATE == pHead->ucType)
    {
        SystemUpdate *supdate = (SystemUpdate *)(lpData + sizeof(NetHeader));
        std::string loginName = IMarkManageServiceActivator::Get().getPTaskManageService()->getLoginName();
        std::string operatorName = supdate->Operator;
        if (loginName == operatorName)
        {
            return false;
        }

        switch (supdate->updateType)
        {
        case ESystemUpdateType::ESystemUpdateType_Mark:
        {
            LY_MarkManageService::Get().GetDlgMarkList()->updateMenu();
        }
        break;

        }
    }

    return true;
}
