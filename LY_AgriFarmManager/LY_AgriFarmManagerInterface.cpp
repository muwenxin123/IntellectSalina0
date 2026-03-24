#include "LY_AgriFarmManagerInterface.h"
#include "LY_AgriFarmManager.h"

extern "C" LY_AGRIFARMMANAGERSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriFarmManagerInterface::Get();
}

LY_AgriFarmManagerInterface &LY_AgriFarmManagerInterface::Get()
{
    static LY_AgriFarmManagerInterface gi;
    return gi;
}

void LY_AgriFarmManagerInterface::Startup()
{
    LY_AgriFarmManager::Get().Startup();
}

void LY_AgriFarmManagerInterface::Shutdown()
{
    LY_AgriFarmManager::Get().Shutdown();
}

void LY_AgriFarmManagerInterface::Reset()
{
    LY_AgriFarmManager::Get().Reset();
}

bool LY_AgriFarmManagerInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriFarmManager::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriFarmManagerInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriFarmManager::Get().DrawGraph(painter, pProj);
}

bool LY_AgriFarmManagerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriFarmManager::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriFarmManagerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriFarmManager::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriFarmManager::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriFarmManager::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriFarmManager::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriFarmManager::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriFarmManager::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriFarmManager::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriFarmManager::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
