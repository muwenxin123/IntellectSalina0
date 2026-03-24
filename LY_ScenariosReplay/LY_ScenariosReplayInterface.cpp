#include "lY_ScenariosReplayInterface.h"
#include "lY_ScenariosReplay.h"

extern "C" LY_SCENARIOSREPLAYSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_ScenariosReplayInterface::Get();
}

LY_ScenariosReplayInterface& LY_ScenariosReplayInterface::Get()
{
    static LY_ScenariosReplayInterface gi;
    return gi;
}

void LY_ScenariosReplayInterface::Startup()
{
	LY_ScenariosReplay::Get().Startup();
}

void LY_ScenariosReplayInterface::Shutdown()
{
	LY_ScenariosReplay::Get().Shutdown();
}

void LY_ScenariosReplayInterface::Reset()
{

}

bool LY_ScenariosReplayInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_ScenariosReplay::Get().DrawGraph(painter, pProj);
}

bool LY_ScenariosReplayInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_ScenariosReplay::Get().DrawDynamic(painter, pProj);
}

bool LY_ScenariosReplayInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ScenariosReplay::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ScenariosReplay::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ScenariosReplay::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_ScenariosReplay::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_ScenariosReplay::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ScenariosReplay::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_ScenariosReplay::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ScenariosReplay::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
