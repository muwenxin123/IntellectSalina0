#include "LY_UAVRealTimeStateManageServiceInterface.h"
#include "LY_UAVRealTimeStateManageService.h"

extern "C" LY_UAVREALTIMESTATEMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_UAVRealTimeStateManageServiceInterface::Get();
}

LY_UAVRealTimeStateManageServiceInterface& LY_UAVRealTimeStateManageServiceInterface::Get()
{
    static LY_UAVRealTimeStateManageServiceInterface gi;
    return gi;
}

void LY_UAVRealTimeStateManageServiceInterface::Startup()
{
    LY_UAVRealTimeStateManageService::Get().Startup();
}

void LY_UAVRealTimeStateManageServiceInterface::Shutdown()
{
    LY_UAVRealTimeStateManageService::Get().Shutdown();
}

void LY_UAVRealTimeStateManageServiceInterface::Reset()
{
    LY_UAVRealTimeStateManageService::Get().Reset();
}

bool LY_UAVRealTimeStateManageServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_UAVRealTimeStateManageService::Get().DrawGraph(painter, pProj);
}

bool LY_UAVRealTimeStateManageServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_UAVRealTimeStateManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_UAVRealTimeStateManageServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_UAVRealTimeStateManageService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_UAVRealTimeStateManageService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_UAVRealTimeStateManageService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_UAVRealTimeStateManageService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_UAVRealTimeStateManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_UAVRealTimeStateManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_UAVRealTimeStateManageService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_UAVRealTimeStateManageService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
