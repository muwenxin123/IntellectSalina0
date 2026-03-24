#include "LY_ConfigBundleControlServiceInterface.h"
#include "LY_ConfigBundleControlService.h"

extern "C" LY_CONFIGBUNDLECONTROLSERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_ConfigBundleControlServiceInterface::Get();
}

LY_ConfigBundleControlServiceInterface& LY_ConfigBundleControlServiceInterface::Get()
{
    static LY_ConfigBundleControlServiceInterface gi;
    return gi;
}

void LY_ConfigBundleControlServiceInterface::Startup()
{
    LY_ConfigBundleControlService::Get().Startup();
}

void LY_ConfigBundleControlServiceInterface::Shutdown()
{
    LY_ConfigBundleControlService::Get().Shutdown();
}

void LY_ConfigBundleControlServiceInterface::Reset()
{
    LY_ConfigBundleControlService::Get().Reset();
}

bool LY_ConfigBundleControlServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_ConfigBundleControlService::Get().DrawGraph(painter, pProj);
}

bool LY_ConfigBundleControlServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_ConfigBundleControlService::Get().DrawDynamic(painter, pProj);
}

bool LY_ConfigBundleControlServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ConfigBundleControlService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ConfigBundleControlService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ConfigBundleControlService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_ConfigBundleControlService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_ConfigBundleControlService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ConfigBundleControlService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_ConfigBundleControlService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ConfigBundleControlService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
