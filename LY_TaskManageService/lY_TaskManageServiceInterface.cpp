#include "lY_TaskManageServiceInterface.h"
#include "lY_TaskManageService.h"

extern "C" LY_TASKMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_TaskManageServiceInterface::Get();
}

LY_TaskManageServiceInterface &LY_TaskManageServiceInterface::Get()
{
    static LY_TaskManageServiceInterface gi;
    return gi;
}

void LY_TaskManageServiceInterface::Startup()
{
    LY_TaskManageService::Get().Startup();
}

void LY_TaskManageServiceInterface::Shutdown()
{
    LY_TaskManageService::Get().Shutdown();
}

void LY_TaskManageServiceInterface::Reset()
{
    LY_TaskManageService::Get().Reset();
}

bool LY_TaskManageServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_TaskManageService::Get().DrawGraph(painter, pProj);
}

bool LY_TaskManageServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_TaskManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_TaskManageServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_TaskManageService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_TaskManageService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_TaskManageService::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_TaskManageService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_TaskManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_TaskManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_TaskManageService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_TaskManageService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
