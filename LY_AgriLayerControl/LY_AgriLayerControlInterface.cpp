#include "LY_AgriLayerControlInterface.h"
#include "LY_AgriLayerControl.h"

extern "C" LY_AGRILAYERCONTROLSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriLayerControlInterface::Get();
}

LY_AgriLayerControlInterface &LY_AgriLayerControlInterface::Get()
{
    static LY_AgriLayerControlInterface gi;
    return gi;
}

void LY_AgriLayerControlInterface::Startup()
{
    LY_AgriLayerControl::Get().Startup();
}

void LY_AgriLayerControlInterface::Shutdown()
{
    LY_AgriLayerControl::Get().Shutdown();
}

void LY_AgriLayerControlInterface::Reset()
{
    LY_AgriLayerControl::Get().Reset();
}

bool LY_AgriLayerControlInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriLayerControl::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriLayerControlInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriLayerControl::Get().DrawGraph(painter, pProj);
}

bool LY_AgriLayerControlInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriLayerControl::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriLayerControlInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriLayerControl::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriLayerControl::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriLayerControl::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriLayerControl::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriLayerControl::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriLayerControl::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriLayerControl::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriLayerControl::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_AgriLayerControl::Get().On2d3dChanged(wParam);
	}
	break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgriLayerControl::Get().OnEarthMessage(wParam, lParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
