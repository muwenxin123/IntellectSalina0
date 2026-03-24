#include "lyUcisPluginFrameWorkInterface.h"
#include "lyUcisPluginFrameWork.h"

extern "C" LYUCISPLUGINFRAMEWORKSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LyUcisPluginFrameWorkInterface::Get();
}

LyUcisPluginFrameWorkInterface &LyUcisPluginFrameWorkInterface::Get()
{
    static LyUcisPluginFrameWorkInterface gi;
    return gi;
}

void LyUcisPluginFrameWorkInterface::Startup()
{
    LyUcisPluginFrameWork::Get().Startup();
}

void LyUcisPluginFrameWorkInterface::Shutdown()
{
    LyUcisPluginFrameWork::Get().Shutdown();
}

void LyUcisPluginFrameWorkInterface::Reset()
{
    LyUcisPluginFrameWork::Get().Reset();
}

bool LyUcisPluginFrameWorkInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LyUcisPluginFrameWork::Get().DrawGraph(painter, pProj);
}

bool LyUcisPluginFrameWorkInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LyUcisPluginFrameWork::Get().DrawDynamic(painter, pProj);
}

bool LyUcisPluginFrameWorkInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LyUcisPluginFrameWork::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LyUcisPluginFrameWork::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LyUcisPluginFrameWork::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LyUcisPluginFrameWork::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LyUcisPluginFrameWork::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LyUcisPluginFrameWork::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LyUcisPluginFrameWork::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LyUcisPluginFrameWork::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
