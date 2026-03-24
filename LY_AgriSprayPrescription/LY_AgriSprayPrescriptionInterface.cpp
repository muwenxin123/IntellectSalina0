
#include "LY_AgriSprayPrescriptionInterface.h"
#include "LY_AgriSprayPrescription.h"


extern "C" LY_AGRISPRAYPRESCRIPTIONSHARED_EXPORT IInterface *RegisterInterface()
{
	return &LY_AgriSprayPrescriptionInterface::Get();
}

LY_AgriSprayPrescriptionInterface &LY_AgriSprayPrescriptionInterface::Get()
{
    static LY_AgriSprayPrescriptionInterface gi;
    return gi;
}

void LY_AgriSprayPrescriptionInterface::Startup()
{
    LY_AgriSprayPrescription::Get().Startup();
}

void LY_AgriSprayPrescriptionInterface::Shutdown()
{
    LY_AgriSprayPrescription::Get().Shutdown();
}

void LY_AgriSprayPrescriptionInterface::Reset()
{
    LY_AgriSprayPrescription::Get().Reset();
}

bool LY_AgriSprayPrescriptionInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriSprayPrescription::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriSprayPrescriptionInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriSprayPrescription::Get().DrawGraph(painter, pProj);
}

bool LY_AgriSprayPrescriptionInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriSprayPrescription::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriSprayPrescriptionInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
	bool bReturn = false;

	switch (uiMessage)
	{
	case IInterface::MSG_SET_CONFIGURE_PATH:
	{
        LY_AgriSprayPrescription::Get().SetConfDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_SET_DATA_PATH:
	{
        LY_AgriSprayPrescription::Get().SetDataDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_Create:
	{
        bReturn = LY_AgriSprayPrescription::Get().OnCreate(wParam, (QWidget *)lParam);
	}
	break;
	case IInterface::MSG_Command:
	{
        bReturn = LY_AgriSprayPrescription::Get().OnCommand(wParam);
	}
	break;
	case IInterface::MSG_MouseButtonPress:
	{
        bReturn = LY_AgriSprayPrescription::Get().OnMouseButtonDown((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_MouseButtonDblClick:
	{
        bReturn = LY_AgriSprayPrescription::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_Timer:
	{
        LY_AgriSprayPrescription::Get().OnTimer(wParam);
	}
	break;
	case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
	{
        LY_AgriSprayPrescription::Get().SetCoordinateTransformation((IProj *)lParam);
	}
	break;

	case IInterface::MSG_3D_EARTH:
	{
		LY_AgriSprayPrescription::Get().OnEarthMessage(wParam, lParam);
	}
	break;
	default:
		break;
	}
	return bReturn;
}
