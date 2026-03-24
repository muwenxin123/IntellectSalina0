#include "LY_ZoneManagerServiveInterface.h"
#include "LY_ZoneManagerServive.h"

extern "C" LY_ZONEMANAGERSERVIVESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_ZoneManagerServiveInterface::Get();
}

LY_ZoneManagerServiveInterface &LY_ZoneManagerServiveInterface::Get()
{
    static LY_ZoneManagerServiveInterface gi;
    return gi;
}

void LY_ZoneManagerServiveInterface::Startup()
{
	LY_ZoneManagerServive::Get().Startup();
}

void LY_ZoneManagerServiveInterface::Shutdown()
{
	LY_ZoneManagerServive::Get().Shutdown();
}

void LY_ZoneManagerServiveInterface::Reset()
{
	LY_ZoneManagerServive::Get().Reset();
}

bool LY_ZoneManagerServiveInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_ZoneManagerServive::Get().DrawGraph(painter, pProj);
}

bool LY_ZoneManagerServiveInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_ZoneManagerServive::Get().DrawDynamic(painter, pProj);
}

bool LY_ZoneManagerServiveInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ZoneManagerServive::Get().SetConfDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ZoneManagerServive::Get().SetDataDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ZoneManagerServive::Get().OnCreate(wParam, (QWidget *)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_ZoneManagerServive::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_ZoneManagerServive::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ZoneManagerServive::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_ZoneManagerServive::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ZoneManagerServive::Get().SetCoordinateTransformation((IProj *)lParam);
    }
        break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_ZoneManagerServive::Get().OnEarthMessage(wParam, lParam);
	}
	break;
    default:
        break;
    }
    return bReturn;
}
