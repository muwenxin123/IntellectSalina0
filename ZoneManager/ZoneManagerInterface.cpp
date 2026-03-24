#include "ZoneManagerInterface.h"
#include "ZoneManager_global.h"
#include "ZoneManager.h"

extern "C" ZoneManagerSHARED_EXPORT IInterface *RegisterInterface()
{
    return &ZoneManagerInterface::Get();
}

ZoneManagerInterface &ZoneManagerInterface::Get()
{
    static ZoneManagerInterface hx;
    return hx;
}

bool ZoneManagerInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return ZoneManager::Get().DrawGraph(painter, pProj);
}

bool ZoneManagerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return ZoneManager::Get().DrawDynamic(painter, pProj);
}

bool ZoneManagerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_Create:
        {
            bReturn = ZoneManager::Get().OnCreate(wParam, (QWidget *)lParam);
        }
        break;
    case IInterface::MSG_Command:
        {
            bReturn = ZoneManager::Get().OnCommand(wParam);
        }
        break;
    case IInterface::MSG_MouseButtonPress:
        {
            ZoneManager::Get().OnMouseButtonDown((QMouseEvent *)lParam);
        }
        break;
    case IInterface::MSG_MouseButtonRelease:
        {
            ZoneManager::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
        }
        break;
    case IInterface::MSG_MouseMove:
        {
            ZoneManager::Get().OnMouseMove((QMouseEvent *)lParam);
        }
        break;
    case IInterface::MSG_Timer:
        {
            bReturn = ZoneManager::Get().OnTimer(wParam);
        }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
        {
            ZoneManager::Get().SetCoordinateTransformation((IProj *)lParam);
        }
        break;
    case IInterface::MSG_SET_DATA_PATH:
        {
            ZoneManager::Get().SetDataDir((const char *)lParam);
        }
        break;
    case IInterface::MSG_SET_CONFIGURE_PATH:
        {
            ZoneManager::Get().SetConfDir((const char *)lParam);
        }
        break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		ZoneManager::Get().On2d3dChanged(wParam);
	}
	break;
	default:
        break;
    }
    return bReturn;
}
