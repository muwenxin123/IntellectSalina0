#include "LY_AgriySprayPrescriptionServiceInterface.h"
#include "LY_AgriySprayPrescriptionService.h"

extern "C" LY_AGRIYSPRAYPRESCRIPTIONSERVICESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriySprayPrescriptionServiceInterface::Get();
}

LY_AgriySprayPrescriptionServiceInterface& LY_AgriySprayPrescriptionServiceInterface::Get()
{
    static LY_AgriySprayPrescriptionServiceInterface gi;
    return gi;
}

void LY_AgriySprayPrescriptionServiceInterface::Startup()
{
    LY_AgriySprayPrescriptionService::Get().Startup();
}

void LY_AgriySprayPrescriptionServiceInterface::Shutdown()
{
    LY_AgriySprayPrescriptionService::Get().Shutdown();
}

void LY_AgriySprayPrescriptionServiceInterface::Reset()
{
    LY_AgriySprayPrescriptionService::Get().Reset();
}

bool LY_AgriySprayPrescriptionServiceInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_AgriySprayPrescriptionService::Get().DrawGraph(painter, pProj);
}

bool LY_AgriySprayPrescriptionServiceInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_AgriySprayPrescriptionService::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriySprayPrescriptionServiceInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriySprayPrescriptionService::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriySprayPrescriptionService::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriySprayPrescriptionService::Get().OnCreate(wParam, (QWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AgriySprayPrescriptionService::Get().OnCommand(wParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriySprayPrescriptionService::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriySprayPrescriptionService::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_AgriySprayPrescriptionService::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriySprayPrescriptionService::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
    default:
        break;
    }
    return bReturn;
}
