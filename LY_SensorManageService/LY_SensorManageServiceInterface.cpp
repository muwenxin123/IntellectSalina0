#include "LY_SensorManageServiceInterface.h"
#include "LY_SensorManageService.h"

extern "C" LY_SENSORMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_SensorManageServiceInterface::Get();
}

LY_SensorManageServiceInterface& LY_SensorManageServiceInterface::Get()
{
    static LY_SensorManageServiceInterface gi;
    return gi;
}

void LY_SensorManageServiceInterface::Startup()
{
    LY_SensorManageService::Get().Startup();
}

void LY_SensorManageServiceInterface::Shutdown()
{
    LY_SensorManageService::Get().Shutdown();
}

void LY_SensorManageServiceInterface::Reset()
{
    LY_SensorManageService::Get().Reset();
}

bool LY_SensorManageServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_SensorManageService::Get().DrawGraph(painter, pProj);
}

bool LY_SensorManageServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_SensorManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_SensorManageServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_SensorManageService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_SensorManageService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_SensorManageService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_SensorManageService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_SensorManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_SensorManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_SensorManageService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_SensorManageService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
