#include "LY_AgriRestfulManagerInterface.h"
#include "LY_AgriRestfulManager.h"

extern "C" LY_AGRIRESTFULMANAGERSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriRestfulManagerInterface::Get();
}

LY_AgriRestfulManagerInterface &LY_AgriRestfulManagerInterface::Get()
{
    static LY_AgriRestfulManagerInterface gi;
    return gi;
}

void LY_AgriRestfulManagerInterface::Startup()
{
    LY_AgriRestfulManager::Get().Startup();
}

void LY_AgriRestfulManagerInterface::Shutdown()
{
    LY_AgriRestfulManager::Get().Shutdown();
}

void LY_AgriRestfulManagerInterface::Reset()
{
    LY_AgriRestfulManager::Get().Reset();
}

bool LY_AgriRestfulManagerInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriRestfulManager::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriRestfulManagerInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriRestfulManager::Get().DrawGraph(painter, pProj);
}

bool LY_AgriRestfulManagerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriRestfulManager::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriRestfulManagerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriRestfulManager::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriRestfulManager::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriRestfulManager::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriRestfulManager::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriRestfulManager::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriRestfulManager::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriRestfulManager::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriRestfulManager::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_AgriRestfulManager::Get().On2d3dChanged(wParam);
	}
	break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgriRestfulManager::Get().OnEarthMessage(wParam, lParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
