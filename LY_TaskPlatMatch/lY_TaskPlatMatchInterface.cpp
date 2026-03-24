#include "lY_TaskPlatMatchInterface.h"
#include "lY_TaskPlatMatch.h"

extern "C" LY_TASKPLATMATCHSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_TaskPlatMatchInterface::Get();
}

LY_TaskPlatMatchInterface &LY_TaskPlatMatchInterface::Get()
{
    static LY_TaskPlatMatchInterface gi;
    return gi;
}

void LY_TaskPlatMatchInterface::Startup()
{
    LY_TaskPlatMatch::Get().Startup();
}

void LY_TaskPlatMatchInterface::Shutdown()
{
    LY_TaskPlatMatch::Get().Shutdown();
}

void LY_TaskPlatMatchInterface::Reset()
{
    LY_TaskPlatMatch::Get().Reset();
}

bool LY_TaskPlatMatchInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_TaskPlatMatch::Get().DrawGraph(painter, pProj);
}

bool LY_TaskPlatMatchInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_TaskPlatMatch::Get().DrawDynamic(painter, pProj);
}

bool LY_TaskPlatMatchInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_TaskPlatMatch::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_TaskPlatMatch::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_TaskPlatMatch::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_TaskPlatMatch::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_TaskPlatMatch::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_TaskPlatMatch::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_TaskPlatMatch::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_TaskPlatMatch::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
