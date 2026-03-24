#include "LY_CommandUnitManageServiceInterface.h"
#include "LY_CommandUnitManageService.h"

extern "C" LY_COMMANDUNITMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_CommandUnitManageServiceInterface::Get();
}

LY_CommandUnitManageServiceInterface &LY_CommandUnitManageServiceInterface::Get()
{
    static LY_CommandUnitManageServiceInterface gi;
    return gi;
}

void LY_CommandUnitManageServiceInterface::Startup()
{
    LY_CommandUnitManageService::Get().Startup();
}

void LY_CommandUnitManageServiceInterface::Shutdown()
{
    LY_CommandUnitManageService::Get().Shutdown();
}

void LY_CommandUnitManageServiceInterface::Reset()
{
    LY_CommandUnitManageService::Get().Reset();
}

bool LY_CommandUnitManageServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_CommandUnitManageService::Get().DrawGraph(painter, pProj);
}

bool LY_CommandUnitManageServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_CommandUnitManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_CommandUnitManageServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_CommandUnitManageService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_CommandUnitManageService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        LY_CommandUnitManageService::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        LY_CommandUnitManageService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        LY_CommandUnitManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        LY_CommandUnitManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_CommandUnitManageService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_CommandUnitManageService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
