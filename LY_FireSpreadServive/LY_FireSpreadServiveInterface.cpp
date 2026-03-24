#include "LY_FireSpreadServiveInterface.h"
#include "LY_FireSpreadServive.h"

extern "C" LY_FIRESPREADSERVIVESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_FireSpreadServiveInterface::Get();
}

LY_FireSpreadServiveInterface &LY_FireSpreadServiveInterface::Get()
{
    static LY_FireSpreadServiveInterface gi;
    return gi;
}

void LY_FireSpreadServiveInterface::Startup()
{
	LY_FireSpreadServive::Get().Startup();
}

void LY_FireSpreadServiveInterface::Shutdown()
{
	LY_FireSpreadServive::Get().Shutdown();
}

void LY_FireSpreadServiveInterface::Reset()
{
	LY_FireSpreadServive::Get().Reset();
}

bool LY_FireSpreadServiveInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_FireSpreadServive::Get().DrawGraph(painter, pProj);
}

bool LY_FireSpreadServiveInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_FireSpreadServive::Get().DrawDynamic(painter, pProj);
}

bool LY_FireSpreadServiveInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_FireSpreadServive::Get().SetConfDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_FireSpreadServive::Get().SetDataDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_FireSpreadServive::Get().OnCreate(wParam, (QWidget *)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_FireSpreadServive::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_FireSpreadServive::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_FireSpreadServive::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_FireSpreadServive::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_FireSpreadServive::Get().SetCoordinateTransformation((IProj *)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
