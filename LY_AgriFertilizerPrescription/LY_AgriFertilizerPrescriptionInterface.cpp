
#include "LY_AgriFertilizerPrescriptionInterface.h"
#include "LY_AgriFertilizerPrescription.h"


extern "C" LY_AGRIFERTILIZERPRESCRIPTION_EXPORT IInterface *RegisterInterface()
{
	return &LY_AgriFertilizerPrescriptionInterface::Get();
}

LY_AgriFertilizerPrescriptionInterface &LY_AgriFertilizerPrescriptionInterface::Get()
{
    static LY_AgriFertilizerPrescriptionInterface gi;
    return gi;
}

void LY_AgriFertilizerPrescriptionInterface::Startup()
{
    LY_AgriFertilizerPrescription::Get().Startup();
}

void LY_AgriFertilizerPrescriptionInterface::Shutdown()
{
    LY_AgriFertilizerPrescription::Get().Shutdown();
}

void LY_AgriFertilizerPrescriptionInterface::Reset()
{
    LY_AgriFertilizerPrescription::Get().Reset();
}

bool LY_AgriFertilizerPrescriptionInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriFertilizerPrescription::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriFertilizerPrescriptionInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriFertilizerPrescription::Get().DrawGraph(painter, pProj);
}

bool LY_AgriFertilizerPrescriptionInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriFertilizerPrescription::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriFertilizerPrescriptionInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
	bool bReturn = false;

	switch (uiMessage)
	{
	case IInterface::MSG_SET_CONFIGURE_PATH:
	{
        LY_AgriFertilizerPrescription::Get().SetConfDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_SET_DATA_PATH:
	{
        LY_AgriFertilizerPrescription::Get().SetDataDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_Create:
	{
        bReturn = LY_AgriFertilizerPrescription::Get().OnCreate(wParam, (QWidget *)lParam);
	}
	break;
	case IInterface::MSG_Command:
	{
        bReturn = LY_AgriFertilizerPrescription::Get().OnCommand(wParam);
	}
	break;
	case IInterface::MSG_MouseButtonPress:
	{
        bReturn = LY_AgriFertilizerPrescription::Get().OnMouseButtonDown((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_MouseButtonDblClick:
	{
        bReturn = LY_AgriFertilizerPrescription::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_Timer:
	{
        LY_AgriFertilizerPrescription::Get().OnTimer(wParam);
	}
	break;
	case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
	{
        LY_AgriFertilizerPrescription::Get().SetCoordinateTransformation((IProj *)lParam);
	}
	break;
	default:
		break;
	}
	return bReturn;
}
