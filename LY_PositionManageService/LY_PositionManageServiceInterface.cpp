#include "LY_PositionManageServiceInterface.h"
#include "LY_PositionManageService.h"

extern "C" LY_POSITIONMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_PositionManageServiceInterface::Get();
}

LY_PositionManageServiceInterface& LY_PositionManageServiceInterface::Get()
{
    static LY_PositionManageServiceInterface gi;
    return gi;
}

void LY_PositionManageServiceInterface::Startup()
{
    LY_PositionManageService::Get().Startup();
}

void LY_PositionManageServiceInterface::Shutdown()
{
    LY_PositionManageService::Get().Shutdown();
}

void LY_PositionManageServiceInterface::Reset()
{
    LY_PositionManageService::Get().Reset();
}

bool LY_PositionManageServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_PositionManageService::Get().DrawGraph(painter, pProj);
}

bool LY_PositionManageServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_PositionManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_PositionManageServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_PositionManageService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_PositionManageService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_PositionManageService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_PositionManageService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_PositionManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_PositionManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_PositionManageService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_PositionManageService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
