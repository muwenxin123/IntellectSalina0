#include "LY_WeaponManageServiceInterface.h"
#include "LY_WeaponManageService.h"

extern "C" LY_WEAPONMANAGESERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_WeaponManageServiceInterface::Get();
}

LY_WeaponManageServiceInterface& LY_WeaponManageServiceInterface::Get()
{
    static LY_WeaponManageServiceInterface gi;
    return gi;
}

void LY_WeaponManageServiceInterface::Startup()
{
    LY_WeaponManageService::Get().Startup();
}

void LY_WeaponManageServiceInterface::Shutdown()
{
    LY_WeaponManageService::Get().Shutdown();
}

void LY_WeaponManageServiceInterface::Reset()
{
    LY_WeaponManageService::Get().Reset();
}

bool LY_WeaponManageServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_WeaponManageService::Get().DrawGraph(painter, pProj);
}

bool LY_WeaponManageServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_WeaponManageService::Get().DrawDynamic(painter, pProj);
}

bool LY_WeaponManageServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_WeaponManageService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_WeaponManageService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_WeaponManageService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_WeaponManageService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_WeaponManageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_WeaponManageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_WeaponManageService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_WeaponManageService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
