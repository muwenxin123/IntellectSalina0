#include "LY_UAVFlyControl_DJI_DockInterface.h"
#include "LY_UAVFlyControl_DJI_Dock.h"

extern "C" LY_UAVFLYCONTROL_DJI_DOCK_EXPORT IInterface *RegisterInterface()
{
    return &LY_UAVFlyControl_DJI_DockInterface::Get();
}

LY_UAVFlyControl_DJI_DockInterface& LY_UAVFlyControl_DJI_DockInterface::Get()
{
    static LY_UAVFlyControl_DJI_DockInterface gi;
    return gi;
}

void LY_UAVFlyControl_DJI_DockInterface::Startup()
{
    LY_UAVFlyControl_DJI_Dock::Get().Startup();
}

void LY_UAVFlyControl_DJI_DockInterface::Shutdown()
{
    LY_UAVFlyControl_DJI_Dock::Get().Shutdown();
}

void LY_UAVFlyControl_DJI_DockInterface::Reset()
{
    LY_UAVFlyControl_DJI_Dock::Get().Reset();
}

bool LY_UAVFlyControl_DJI_DockInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_UAVFlyControl_DJI_Dock::Get().OnNetRecv(nLength, lpData);
}

bool LY_UAVFlyControl_DJI_DockInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_UAVFlyControl_DJI_Dock::Get().DrawGraph(painter, pProj);
}

bool LY_UAVFlyControl_DJI_DockInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_UAVFlyControl_DJI_Dock::Get().DrawDynamic(painter, pProj);
}

bool LY_UAVFlyControl_DJI_DockInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_UAVFlyControl_DJI_Dock::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_UAVFlyControl_DJI_Dock::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnCreate(wParam, (QWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnCommand(wParam, lParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LY_UAVFlyControl_DJI_Dock::Get().OnMouseMove((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_UAVFlyControl_DJI_Dock::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_UAVFlyControl_DJI_Dock::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
