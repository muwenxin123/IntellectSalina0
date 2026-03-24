#include "LY_AgricultureHomepageServiceInterface.h"
#include "LY_AgricultureHomepageService.h"

extern "C" LY_AGRICULTUREHOMEPAGESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgricultureHomepageServiceInterface::Get();
}

LY_AgricultureHomepageServiceInterface& LY_AgricultureHomepageServiceInterface::Get()
{
    static LY_AgricultureHomepageServiceInterface gi;
    return gi;
}

void LY_AgricultureHomepageServiceInterface::Startup()
{
	LY_AgricultureHomepageService::Get().Startup();
}

void LY_AgricultureHomepageServiceInterface::Shutdown()
{
	LY_AgricultureHomepageService::Get().Shutdown();
}

void LY_AgricultureHomepageServiceInterface::Reset()
{
	LY_AgricultureHomepageService::Get().Reset();
}

bool LY_AgricultureHomepageServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_AgricultureHomepageService::Get().DrawGraph(painter, pProj);
}

bool LY_AgricultureHomepageServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_AgricultureHomepageService::Get().DrawDynamic(painter, pProj);
}

bool LY_AgricultureHomepageServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
		LY_AgricultureHomepageService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
		LY_AgricultureHomepageService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgricultureHomepageService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AgricultureHomepageService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgricultureHomepageService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgricultureHomepageService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
		LY_AgricultureHomepageService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
		LY_AgricultureHomepageService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgricultureHomepageService::Get().OnEarthMessage(wParam, lParam);
	}
	break;
    default:
        break;
    }
    return bReturn;
}
