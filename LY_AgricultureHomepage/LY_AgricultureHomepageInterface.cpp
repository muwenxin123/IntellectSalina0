
#include "LY_AgricultureHomepageInterface.h"
#include "LY_AgricultureHomepage.h"


extern "C" LY_AGRICULTUREHOMEPAGESHARED_EXPORT IInterface *RegisterInterface()
{
	return &LY_AgricultureHomepageInterface::Get();
}

LY_AgricultureHomepageInterface &LY_AgricultureHomepageInterface::Get()
{
    static LY_AgricultureHomepageInterface gi;
    return gi;
}

void LY_AgricultureHomepageInterface::Startup()
{
    LY_AgricultureHomepage::Get().Startup();
}

void LY_AgricultureHomepageInterface::Shutdown()
{
    LY_AgricultureHomepage::Get().Shutdown();
}

void LY_AgricultureHomepageInterface::Reset()
{
    LY_AgricultureHomepage::Get().Reset();
}

bool LY_AgricultureHomepageInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgricultureHomepage::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgricultureHomepageInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgricultureHomepage::Get().DrawGraph(painter, pProj);
}

bool LY_AgricultureHomepageInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgricultureHomepage::Get().DrawDynamic(painter, pProj);
}

bool LY_AgricultureHomepageInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
	bool bReturn = false;

	switch (uiMessage)
	{
	case IInterface::MSG_SET_CONFIGURE_PATH:
	{
        LY_AgricultureHomepage::Get().SetConfDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_SET_DATA_PATH:
	{
        LY_AgricultureHomepage::Get().SetDataDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_Create:
	{
        bReturn = LY_AgricultureHomepage::Get().OnCreate(wParam, (QWidget *)lParam);
	}
	break;
	case IInterface::MSG_Command:
	{
        bReturn = LY_AgricultureHomepage::Get().OnCommand(wParam);
	}
	break;
	case IInterface::MSG_MouseButtonPress:
	{
        bReturn = LY_AgricultureHomepage::Get().OnMouseButtonDown((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_MouseButtonDblClick:
	{
        bReturn = LY_AgricultureHomepage::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_Timer:
	{
        LY_AgricultureHomepage::Get().OnTimer(wParam);
	}
	break;
	case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
	{
        LY_AgricultureHomepage::Get().SetCoordinateTransformation((IProj *)lParam);
	}
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgricultureHomepage::Get().OnEarthMessage(wParam, lParam);
	}
	break;
	default:
		break;
	}
	return bReturn;
}
