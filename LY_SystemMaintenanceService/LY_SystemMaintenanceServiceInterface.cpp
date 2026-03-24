#include "LY_SystemMaintenanceServiceInterface.h"
#include "LY_SystemMaintenanceService.h"

extern "C" LY_SYSTEMMAINTENANCESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_SystemMaintenanceServiceInterface::Get();
}

LY_SystemMaintenanceServiceInterface &LY_SystemMaintenanceServiceInterface::Get()
{
    static LY_SystemMaintenanceServiceInterface gi;
    return gi;
}

bool LY_SystemMaintenanceServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_SystemMaintenanceService::Get().DrawGraph(painter, pProj);
}

bool LY_SystemMaintenanceServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_SystemMaintenanceService::Get().DrawDynamic(painter, pProj);
}

void LY_SystemMaintenanceServiceInterface::Startup()
{
    LY_SystemMaintenanceService::Get().Startup();
}

void LY_SystemMaintenanceServiceInterface::Shutdown()
{
    LY_SystemMaintenanceService::Get().Shutdown();
}

void LY_SystemMaintenanceServiceInterface::Reset()
{
    LY_SystemMaintenanceService::Get().Reset();
}

bool LY_SystemMaintenanceServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {

    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {

    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_SystemMaintenanceService::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_SystemMaintenanceService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_SystemMaintenanceService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_SystemMaintenanceService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_SystemMaintenanceService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {

    }
    break;
    default:
        break;
    }
    return bReturn;
}
