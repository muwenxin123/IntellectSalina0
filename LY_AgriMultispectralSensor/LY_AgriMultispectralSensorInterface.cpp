#include "LY_AgriMultispectralSensorInterface.h"
#include "LY_AgriMultispectralSensor.h"

extern "C" LY_AGRIMULTISPECTRALSENSORSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriMultispectralSensorInterface::Get();
}

LY_AgriMultispectralSensorInterface &LY_AgriMultispectralSensorInterface::Get()
{
    static LY_AgriMultispectralSensorInterface gi;
    return gi;
}

void LY_AgriMultispectralSensorInterface::Startup()
{
    LY_AgriMultispectralSensor::Get().Startup();
}

void LY_AgriMultispectralSensorInterface::Shutdown()
{
    LY_AgriMultispectralSensor::Get().Shutdown();
}

void LY_AgriMultispectralSensorInterface::Reset()
{
    LY_AgriMultispectralSensor::Get().Reset();
}

bool LY_AgriMultispectralSensorInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriMultispectralSensor::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriMultispectralSensorInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriMultispectralSensor::Get().DrawGraph(painter, pProj);
}

bool LY_AgriMultispectralSensorInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriMultispectralSensor::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriMultispectralSensorInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriMultispectralSensor::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriMultispectralSensor::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriMultispectralSensor::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriMultispectralSensor::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriMultispectralSensor::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriMultispectralSensor::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriMultispectralSensor::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriMultispectralSensor::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_AgriMultispectralSensor::Get().On2d3dChanged(wParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
