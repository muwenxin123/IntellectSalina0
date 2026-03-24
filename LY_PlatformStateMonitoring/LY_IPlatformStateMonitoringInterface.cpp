#include "LY_IPlatformStateMonitoringInterface.h"
#include "LY_IPlatformStateMonitoring.h"

extern "C" LY_IPLATFORMSTATEMONITORINGSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_IPlatformStateMonitoringInterface::Get();
}

LY_IPlatformStateMonitoringInterface &LY_IPlatformStateMonitoringInterface::Get()
{
    static LY_IPlatformStateMonitoringInterface gi;
    return gi;
}

void LY_IPlatformStateMonitoringInterface::Startup()
{
    LY_IPlatformStateMonitoring::Get().Startup();
}

void LY_IPlatformStateMonitoringInterface::Shutdown()
{
    LY_IPlatformStateMonitoring::Get().Shutdown();
}

void LY_IPlatformStateMonitoringInterface::Reset()
{
    LY_IPlatformStateMonitoring::Get().Reset();
}

bool LY_IPlatformStateMonitoringInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_IPlatformStateMonitoring::Get().OnNetRecv(nLength, lpData);
}

bool LY_IPlatformStateMonitoringInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_IPlatformStateMonitoring::Get().DrawGraph(painter, pProj);
}

bool LY_IPlatformStateMonitoringInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_IPlatformStateMonitoring::Get().DrawDynamic(painter, pProj);
}

bool LY_IPlatformStateMonitoringInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_IPlatformStateMonitoring::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_IPlatformStateMonitoring::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_IPlatformStateMonitoring::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_IPlatformStateMonitoring::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_IPlatformStateMonitoring::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_IPlatformStateMonitoring::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_IPlatformStateMonitoring::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_IPlatformStateMonitoring::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
