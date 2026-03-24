#include "LY_ProtocolParse_XygsServerInterface.h"
#include "LY_ProtocolParse_XygsServer.h"

extern "C" LY_PROTOCOLPARSE_XYGSSERVERSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_ProtocolParse_XygsServerInterface::Get();
}

LY_ProtocolParse_XygsServerInterface& LY_ProtocolParse_XygsServerInterface::Get()
{
    static LY_ProtocolParse_XygsServerInterface gi;
    return gi;
}

void LY_ProtocolParse_XygsServerInterface::Startup()
{
    LY_ProtocolParse_XygsServer::Get().Startup();
}

void LY_ProtocolParse_XygsServerInterface::Shutdown()
{
    LY_ProtocolParse_XygsServer::Get().Shutdown();
}

void LY_ProtocolParse_XygsServerInterface::Reset()
{
    LY_ProtocolParse_XygsServer::Get().Reset();
}

bool LY_ProtocolParse_XygsServerInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_ProtocolParse_XygsServer::Get().OnNetRecv(nLength, lpData);
}

bool LY_ProtocolParse_XygsServerInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_ProtocolParse_XygsServer::Get().DrawGraph(painter, pProj);
}

bool LY_ProtocolParse_XygsServerInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_ProtocolParse_XygsServer::Get().DrawDynamic(painter, pProj);
}

bool LY_ProtocolParse_XygsServerInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_ProtocolParse_XygsServer::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_ProtocolParse_XygsServer::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_ProtocolParse_XygsServer::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_ProtocolParse_XygsServer::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_ProtocolParse_XygsServer::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_ProtocolParse_XygsServer::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_ProtocolParse_XygsServer::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_ProtocolParse_XygsServer::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
