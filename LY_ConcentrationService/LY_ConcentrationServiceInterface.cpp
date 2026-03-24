#include "LY_ConcentrationServiceInterface.h"
#include "LY_ConcentrationService.h"

extern "C" LY_CONCENTRATIONSERVICE_EXPORT IInterface *RegisterInterface()
{
    return &LY_ConcentrationServiceInterface::Get();
}

LY_ConcentrationServiceInterface &LY_ConcentrationServiceInterface::Get()
{
    static LY_ConcentrationServiceInterface gi;
    return gi;
}

void LY_ConcentrationServiceInterface::Startup()
{
    LY_ConcentrationService::Get().Startup();
}

void LY_ConcentrationServiceInterface::Shutdown()
{
    LY_ConcentrationService::Get().Shutdown();
}

void LY_ConcentrationServiceInterface::Reset()
{
    LY_ConcentrationService::Get().Reset();
}

bool LY_ConcentrationServiceInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_ConcentrationService::Get().OnNetRecv(nLength, lpData);
}

bool LY_ConcentrationServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_ConcentrationService::Get().DrawGraph(painter, pProj);
}

bool LY_ConcentrationServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_ConcentrationService::Get().DrawDynamic(painter, pProj);
}

bool LY_ConcentrationServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ConcentrationService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ConcentrationService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ConcentrationService::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_ConcentrationService::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_ConcentrationService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ConcentrationService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_ConcentrationService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ConcentrationService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_ConcentrationService::Get().On2d3dChanged(wParam);
	}
	break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_ConcentrationService::Get().OnEarthMessage(wParam, lParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
