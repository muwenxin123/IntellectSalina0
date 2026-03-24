#include "LY_CollaborativeSearchServiceInterface.h"
#include "LY_CollaborativeSearchService.h"

extern "C" LY_COLLABORATIVESEARCHSERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_CollaborativeSearchServiceInterface::Get();
}

LY_CollaborativeSearchServiceInterface& LY_CollaborativeSearchServiceInterface::Get()
{
    static LY_CollaborativeSearchServiceInterface gi;
    return gi;
}

void LY_CollaborativeSearchServiceInterface::Startup()
{
	LY_CollaborativeSearchService::Get().Startup();
}

void LY_CollaborativeSearchServiceInterface::Shutdown()
{
	LY_CollaborativeSearchService::Get().Shutdown();
}

void LY_CollaborativeSearchServiceInterface::Reset()
{
	LY_CollaborativeSearchService::Get().Reset();
}

bool LY_CollaborativeSearchServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_CollaborativeSearchService::Get().DrawGraph(painter, pProj);
}

bool LY_CollaborativeSearchServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_CollaborativeSearchService::Get().DrawDynamic(painter, pProj);
}

bool LY_CollaborativeSearchServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_CollaborativeSearchService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_CollaborativeSearchService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_CollaborativeSearchService::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_CollaborativeSearchService::Get().OnCommand(wParam, lParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_CollaborativeSearchService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_CollaborativeSearchService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_CollaborativeSearchService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_CollaborativeSearchService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
	case IInterface::MSG_MouseMove:
	{
		LY_CollaborativeSearchService::Get().mouseMoveEvent((QMouseEvent *)lParam);
	}
		break;
	case IInterface::MSG_MouseButtonRelease:
	{
		LY_CollaborativeSearchService::Get().mouseReleaseEvent((QMouseEvent *)lParam);
	}
		break;
	case IInterface::MSG_KeyPress:
	{
		LY_CollaborativeSearchService::Get().KeyPress((QKeyEvent*)lParam);
	}
		break;
	case IInterface::MSG_KeyRelease:
	{
		LY_CollaborativeSearchService::Get().KeyRelease((QKeyEvent*)lParam);
	}
		break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_CollaborativeSearchService::Get().OnEarthMessage(wParam, lParam);
	}
	break;
    default:
        break;
    }
    return bReturn;
}

bool LY_CollaborativeSearchServiceInterface::OnNetRecv(int nLength, char * lpData)
{
	return LY_CollaborativeSearchService::Get().OnNetRecv(nLength, lpData);
}
