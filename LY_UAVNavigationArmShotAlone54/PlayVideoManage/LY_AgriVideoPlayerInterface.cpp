
#include "LY_AgriVideoPlayerInterface.h"
#include "LY_AgriVideoPlayer.h"


extern "C" LY_AGRIVIDEOPLAYERSHARED_EXPORT IInterface *RegisterInterface()
{
	return &LY_AgriVideoPlayerInterface::Get();
}

LY_AgriVideoPlayerInterface &LY_AgriVideoPlayerInterface::Get()
{
    static LY_AgriVideoPlayerInterface gi;
    return gi;
}

void LY_AgriVideoPlayerInterface::Startup()
{
    LY_AgriVideoPlayer::Get().Startup();
}

void LY_AgriVideoPlayerInterface::Shutdown()
{
    LY_AgriVideoPlayer::Get().Shutdown();
}

void LY_AgriVideoPlayerInterface::Reset()
{
    LY_AgriVideoPlayer::Get().Reset();
}

bool LY_AgriVideoPlayerInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriVideoPlayer::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriVideoPlayerInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriVideoPlayer::Get().DrawGraph(painter, pProj);
}

bool LY_AgriVideoPlayerInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriVideoPlayer::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriVideoPlayerInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
	bool bReturn = false;

	switch (uiMessage)
	{
	case IInterface::MSG_SET_CONFIGURE_PATH:
	{
        LY_AgriVideoPlayer::Get().SetConfDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_SET_DATA_PATH:
	{
        LY_AgriVideoPlayer::Get().SetDataDir((const char *)lParam);
	}
	break;
	case IInterface::MSG_Create:
	{
        bReturn = LY_AgriVideoPlayer::Get().OnCreate(wParam, (QWidget *)lParam);
	}
	break;
	case IInterface::MSG_Command:
	{
        bReturn = LY_AgriVideoPlayer::Get().OnCommand(wParam);
	}
	break;
	case IInterface::MSG_MouseButtonPress:
	{
        bReturn = LY_AgriVideoPlayer::Get().OnMouseButtonDown((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_MouseButtonDblClick:
	{
        bReturn = LY_AgriVideoPlayer::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
	}
	break;
	case IInterface::MSG_Timer:
	{
        LY_AgriVideoPlayer::Get().OnTimer(wParam);
	}
	break;
	case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
	{
        LY_AgriVideoPlayer::Get().SetCoordinateTransformation((IProj *)lParam);
	}
	break;
	default:
		break;
	}
	return bReturn;
}
