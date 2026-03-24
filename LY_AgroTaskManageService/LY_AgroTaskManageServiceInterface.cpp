
#include "LY_AgroTaskManageServiceInterface.h"
#include "LY_AgroTaskManageService.h"

extern "C" LY_AGROTASKMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgroTaskManageServiceInterface::Get();
}

LY_AgroTaskManageServiceInterface &LY_AgroTaskManageServiceInterface::Get()
{
    static LY_AgroTaskManageServiceInterface gi;
    return gi;
}

void LY_AgroTaskManageServiceInterface::Startup()
{
    LY_AgroTaskManageService::Get().Startup();
}

void LY_AgroTaskManageServiceInterface::Shutdown()
{
    LY_AgroTaskManageService::Get().Shutdown();
}

void LY_AgroTaskManageServiceInterface::Reset()
{
    LY_AgroTaskManageService::Get().Reset();
}

bool LY_AgroTaskManageServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgroTaskManageService::Get().DrawGraph(painter, pProj);
}

bool LY_AgroTaskManageServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgroTaskManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_AgroTaskManageServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgroTaskManageService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgroTaskManageService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgroTaskManageService::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AgroTaskManageService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgroTaskManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgroTaskManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgroTaskManageService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgroTaskManageService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
