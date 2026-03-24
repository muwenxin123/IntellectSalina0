
#include "Ly_VideoRealTimePreviewInterface.h"
#include "Ly_VideoRealTimePreview.h"


extern "C" LY_VIDEOREALTIMEPREVIEWSHARED_EXPORT IInterface *RegisterInterface()
{
    return &Ly_VideoRealTimePreviewInterface::Get();
}

Ly_VideoRealTimePreviewInterface &Ly_VideoRealTimePreviewInterface::Get()
{
    static Ly_VideoRealTimePreviewInterface gi;
    return gi;
}

void Ly_VideoRealTimePreviewInterface::Startup()
{
	Ly_VideoRealTimePreview::Get().Startup();
}

void Ly_VideoRealTimePreviewInterface::Shutdown()
{
	Ly_VideoRealTimePreview::Get().Shutdown();
}

void Ly_VideoRealTimePreviewInterface::Reset()
{

}

bool Ly_VideoRealTimePreviewInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return Ly_VideoRealTimePreview::Get().DrawGraph(painter, pProj);
}

bool Ly_VideoRealTimePreviewInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return Ly_VideoRealTimePreview::Get().DrawDynamic(painter, pProj);
}

bool Ly_VideoRealTimePreviewInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        Ly_VideoRealTimePreview::Get().SetConfDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        Ly_VideoRealTimePreview::Get().SetDataDir((const char *)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = Ly_VideoRealTimePreview::Get().OnCreate(wParam, (QWidget *)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = Ly_VideoRealTimePreview::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = Ly_VideoRealTimePreview::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = Ly_VideoRealTimePreview::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        Ly_VideoRealTimePreview::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        Ly_VideoRealTimePreview::Get().SetCoordinateTransformation((IProj *)lParam);
    }
        break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		Ly_VideoRealTimePreview::Get().On2d3dChanged(wParam);
	}
	break;
    default:
        break;
    }
    return bReturn;
}
