#include "LY_AgriMachineManagerInterface.h"
#include "LY_AgriMachineManager.h"

extern "C" LY_AGRIMACHNIEMANAGERSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriMachineManagerInterface::Get();
}

LY_AgriMachineManagerInterface &LY_AgriMachineManagerInterface::Get()
{
    static LY_AgriMachineManagerInterface gi;
    return gi;
}

void LY_AgriMachineManagerInterface::Startup()
{
    LY_AgriMachineManager::Get().Startup();
}

void LY_AgriMachineManagerInterface::Shutdown()
{
    LY_AgriMachineManager::Get().Shutdown();
}

void LY_AgriMachineManagerInterface::Reset()
{
    LY_AgriMachineManager::Get().Reset();
}

bool LY_AgriMachineManagerInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriMachineManager::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriMachineManagerInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriMachineManager::Get().DrawGraph(painter, pProj);
}

bool LY_AgriMachineManagerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriMachineManager::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriMachineManagerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriMachineManager::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriMachineManager::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriMachineManager::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriMachineManager::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriMachineManager::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriMachineManager::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriMachineManager::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriMachineManager::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
