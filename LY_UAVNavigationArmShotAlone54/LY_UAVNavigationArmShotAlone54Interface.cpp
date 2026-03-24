#include "LY_UAVNavigationArmShotAlone54Interface.h"
#include "LY_UAVNavigationArmShotAlone54.h"

extern "C" LY_UAVNAVIGATIONARMSHOTALONE54SHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_UAVNavigationArmShotAlone54Interface::Get();
}

LY_UAVNavigationArmShotAlone54Interface &LY_UAVNavigationArmShotAlone54Interface::Get()
{
    static LY_UAVNavigationArmShotAlone54Interface gi;
    return gi;
}

void LY_UAVNavigationArmShotAlone54Interface::Startup()
{
    LY_UAVNavigationArmShotAlone54::Get().Startup();
}

void LY_UAVNavigationArmShotAlone54Interface::Shutdown()
{
    LY_UAVNavigationArmShotAlone54::Get().Shutdown();
}

void LY_UAVNavigationArmShotAlone54Interface::Reset()
{
    LY_UAVNavigationArmShotAlone54::Get().Reset();
}

bool LY_UAVNavigationArmShotAlone54Interface::OnNetRecv(int nLength, char *lpData)
{
    return LY_UAVNavigationArmShotAlone54::Get().OnNetRecv(nLength, lpData);
}

bool LY_UAVNavigationArmShotAlone54Interface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_UAVNavigationArmShotAlone54::Get().DrawGraph(painter, pProj);
}

bool LY_UAVNavigationArmShotAlone54Interface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_UAVNavigationArmShotAlone54::Get().DrawDynamic(painter, pProj);
}

bool LY_UAVNavigationArmShotAlone54Interface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_UAVNavigationArmShotAlone54::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_UAVNavigationArmShotAlone54::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_UAVNavigationArmShotAlone54::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_UAVNavigationArmShotAlone54::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_UAVNavigationArmShotAlone54::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_UAVNavigationArmShotAlone54::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_UAVNavigationArmShotAlone54::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_UAVNavigationArmShotAlone54::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
