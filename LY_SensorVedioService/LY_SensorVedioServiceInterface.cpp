
#include "LY_SensorVedioServiceInterface.h"
#include "LY_SensorVedioService.h"
#include "LY_SensorVedioService_global.h"

extern "C" LY_SENSORVEDIOSERVICE_EXPORT LY_SensorVedioServiceInterface *RegisterInterface()
{
    return &LY_SensorVedioServiceInterface::Get();
}

LY_SensorVedioServiceInterface &LY_SensorVedioServiceInterface::Get()
{
    static LY_SensorVedioServiceInterface gi;
    return gi;
}

void LY_SensorVedioServiceInterface::Startup()
{
	return LY_SensorVedioService::Get().Startup();
}

void LY_SensorVedioServiceInterface::Shutdown()
{
	return LY_SensorVedioService::Get().Shutdown();
}

bool LY_SensorVedioServiceInterface::OnNetRecv(int nLength, char *lpData)
{
	return LY_SensorVedioService::Get().OnNetRecv(nLength, lpData);
}

bool LY_SensorVedioServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_SensorVedioService::Get().DrawGraph(painter, pProj);
}

bool LY_SensorVedioServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_SensorVedioService::Get().DrawDynamic(painter, pProj);
}

bool LY_SensorVedioServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
		LY_SensorVedioService::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
		LY_SensorVedioService::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_SensorVedioService::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_SensorVedioService::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_SensorVedioService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
	case IInterface::MSG_MouseButtonRelease:
	{
		bReturn = LY_SensorVedioService::Get().OnMouseButtonUp((QMouseEvent *)lParam);
	}
	break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_SensorVedioService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
		LY_SensorVedioService::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
		LY_SensorVedioService::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
