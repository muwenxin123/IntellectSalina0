#include "LY_ZoneManagerInterface.h"
#include "LY_ZoneManager.h"

extern "C" LY_ZONEMANAGERSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_ZoneManagerInterface::Get();
}

LY_ZoneManagerInterface &LY_ZoneManagerInterface::Get()
{
    static LY_ZoneManagerInterface gi;
    return gi;
}

void LY_ZoneManagerInterface::Startup()
{
	LY_ZoneManager::Get().Startup();
}

void LY_ZoneManagerInterface::Shutdown()
{
	LY_ZoneManager::Get().Shutdown();
}

void LY_ZoneManagerInterface::Reset()
{
	LY_ZoneManager::Get().Reset();
}

bool LY_ZoneManagerInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_ZoneManager::Get().DrawGraph(painter, pProj);
}

bool LY_ZoneManagerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_ZoneManager::Get().DrawDynamic(painter, pProj);
}

bool LY_ZoneManagerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ZoneManager::Get().SetConfDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ZoneManager::Get().SetDataDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ZoneManager::Get().OnCreate(wParam, (QWidget *)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_ZoneManager::Get().OnCommand(wParam);
		bReturn = LY_ZoneManager::Get().OnCommand(wParam, lParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        LY_ZoneManager::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
	case IInterface::MSG_MouseButtonRelease:
	{
		LY_ZoneManager::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_MouseMove:
	{
		LY_ZoneManager::Get().OnMouseMove((QMouseEvent *)lParam);
	}
	break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ZoneManager::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_ZoneManager::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ZoneManager::Get().SetCoordinateTransformation((IProj *)lParam);
    }
        break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_ZoneManager::Get().On2d3dChanged(wParam);
	}
	break;
    default:
        break;
    }
    return bReturn;
}
