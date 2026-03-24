#include "LY_AerialShotSettingBundle54ServiceInterface.h"
#include "LY_AerialShotSettingBundle54Service.h"

extern "C" LY_AERIALSHOTSETTINGBUNDLE54SHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AerialShotSettingBundle54ServiceInterface::Get();
}

LY_AerialShotSettingBundle54ServiceInterface &LY_AerialShotSettingBundle54ServiceInterface::Get()
{
    static LY_AerialShotSettingBundle54ServiceInterface gi;
    return gi;
}

void LY_AerialShotSettingBundle54ServiceInterface::Startup()
{
    LY_AerialShotSettingBundle54Service::Get().Startup();
}

void LY_AerialShotSettingBundle54ServiceInterface::Shutdown()
{
    LY_AerialShotSettingBundle54Service::Get().Shutdown();
}

void LY_AerialShotSettingBundle54ServiceInterface::Reset()
{
    LY_AerialShotSettingBundle54Service::Get().Reset();
}

bool LY_AerialShotSettingBundle54ServiceInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AerialShotSettingBundle54Service::Get().OnNetRecv(nLength, lpData);
}

bool LY_AerialShotSettingBundle54ServiceInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AerialShotSettingBundle54Service::Get().DrawGraph(painter, pProj);
}

bool LY_AerialShotSettingBundle54ServiceInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AerialShotSettingBundle54Service::Get().DrawDynamic(painter, pProj);
}

bool LY_AerialShotSettingBundle54ServiceInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AerialShotSettingBundle54Service::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AerialShotSettingBundle54Service::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AerialShotSettingBundle54Service::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AerialShotSettingBundle54Service::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AerialShotSettingBundle54Service::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AerialShotSettingBundle54Service::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AerialShotSettingBundle54Service::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AerialShotSettingBundle54Service::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
