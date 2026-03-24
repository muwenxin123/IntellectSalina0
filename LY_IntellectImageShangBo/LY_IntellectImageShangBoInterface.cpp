#include "LY_IntellectImageShangBoInterface.h"
#include "LY_IntellectImageShangBo.h"

extern "C" LY_INTELLECTIMAGESHANGBOSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_IntellectImageShangBoInterface::Get();
}

LY_IntellectImageShangBoInterface& LY_IntellectImageShangBoInterface::Get()
{
    static LY_IntellectImageShangBoInterface gi;
    return gi;
}

void LY_IntellectImageShangBoInterface::Startup()
{
    LY_IntellectImageShangBo::Get().Startup();
}

void LY_IntellectImageShangBoInterface::Shutdown()
{
    LY_IntellectImageShangBo::Get().Shutdown();
}

void LY_IntellectImageShangBoInterface::Reset()
{
    LY_IntellectImageShangBo::Get().Reset();
}

bool LY_IntellectImageShangBoInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_IntellectImageShangBo::Get().DrawGraph(painter, pProj);
}

bool LY_IntellectImageShangBoInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_IntellectImageShangBo::Get().DrawDynamic(painter, pProj);
}

bool LY_IntellectImageShangBoInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_IntellectImageShangBo::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_IntellectImageShangBo::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_IntellectImageShangBo::Get().OnCreate(wParam, (QTabWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_IntellectImageShangBo::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_IntellectImageShangBo::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_IntellectImageShangBo::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_IntellectImageShangBo::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_IntellectImageShangBo::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
