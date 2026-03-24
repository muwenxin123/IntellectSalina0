#include "LY_UAVNavigationArmShotAloneFireProtInterface.h"
#include "LY_UAVNavigationArmShotAloneFireProt.h"

extern "C" LY_UAVNAVIGATIONARMSHOTALONEFIREPROTSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_UAVNavigationArmShotAloneFireProtInterface::Get();
}

LY_UAVNavigationArmShotAloneFireProtInterface& LY_UAVNavigationArmShotAloneFireProtInterface::Get()
{
    static LY_UAVNavigationArmShotAloneFireProtInterface gi;
    return gi;
}

void LY_UAVNavigationArmShotAloneFireProtInterface::Startup()
{
    LY_UAVNavigationArmShotAloneFireProt::Get().Startup();
}

void LY_UAVNavigationArmShotAloneFireProtInterface::Shutdown()
{
    LY_UAVNavigationArmShotAloneFireProt::Get().Shutdown();
}

void LY_UAVNavigationArmShotAloneFireProtInterface::Reset()
{
    LY_UAVNavigationArmShotAloneFireProt::Get().Reset();
}

bool LY_UAVNavigationArmShotAloneFireProtInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_UAVNavigationArmShotAloneFireProt::Get().OnNetRecv(nLength, lpData);
}

bool LY_UAVNavigationArmShotAloneFireProtInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_UAVNavigationArmShotAloneFireProt::Get().DrawGraph(painter, pProj);
}

bool LY_UAVNavigationArmShotAloneFireProtInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_UAVNavigationArmShotAloneFireProt::Get().DrawDynamic(painter, pProj);
}

bool LY_UAVNavigationArmShotAloneFireProtInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_UAVNavigationArmShotAloneFireProt::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_UAVNavigationArmShotAloneFireProt::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_UAVNavigationArmShotAloneFireProt::Get().OnCreate(wParam, (QWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_UAVNavigationArmShotAloneFireProt::Get().OnCommand(wParam, lParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_UAVNavigationArmShotAloneFireProt::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_UAVNavigationArmShotAloneFireProt::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_UAVNavigationArmShotAloneFireProt::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_UAVNavigationArmShotAloneFireProt::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
