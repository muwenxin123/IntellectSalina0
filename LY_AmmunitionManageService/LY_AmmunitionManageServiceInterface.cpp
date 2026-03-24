#include "LY_AmmunitionManageServiceInterface.h"
#include "LY_AmmunitionManageService.h"

extern "C" LY_AMMUNITIONMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AmmunitionManageServiceInterface::Get();
}

LY_AmmunitionManageServiceInterface &LY_AmmunitionManageServiceInterface::Get()
{
    static LY_AmmunitionManageServiceInterface gi;
    return gi;
}

void LY_AmmunitionManageServiceInterface::Startup()
{
    LY_AmmunitionManageService::Get().Startup();
}

void LY_AmmunitionManageServiceInterface::Shutdown()
{
    LY_AmmunitionManageService::Get().Shutdown();
}

void LY_AmmunitionManageServiceInterface::Reset()
{
    LY_AmmunitionManageService::Get().Reset();
}

bool LY_AmmunitionManageServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AmmunitionManageService::Get().DrawGraph(painter, pProj);
}

bool LY_AmmunitionManageServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AmmunitionManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_AmmunitionManageServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AmmunitionManageService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AmmunitionManageService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AmmunitionManageService::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AmmunitionManageService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AmmunitionManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AmmunitionManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AmmunitionManageService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AmmunitionManageService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
