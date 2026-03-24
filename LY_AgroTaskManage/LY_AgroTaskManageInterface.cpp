

#include "LY_AgroTaskManageInterface.h"
#include "lY_AgroTaskManage.h"
#include "netheader.h"
#include "Protocol/XygsSystemControl/SystemControl.h"
#include "IAgroTaskManagerActivator.h"
#include "zzTime.h"

extern "C" LY_AGROTASKMANAGESHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgroTaskManageInterface::Get();
}

LY_AgroTaskManageInterface &LY_AgroTaskManageInterface::Get()
{
    static LY_AgroTaskManageInterface gi;
    return gi;
}

void LY_AgroTaskManageInterface::Startup()
{
    LY_AgroTaskManage::Get().Startup();
}

void LY_AgroTaskManageInterface::Shutdown()
{
    LY_AgroTaskManage::Get().Shutdown();
}

void LY_AgroTaskManageInterface::Reset()
{
    LY_AgroTaskManage::Get().Reset();
}

bool LY_AgroTaskManageInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgroTaskManage::Get().DrawGraph(painter, pProj);
}

bool LY_AgroTaskManageInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgroTaskManage::Get().DrawDynamic(painter, pProj);
}

bool LY_AgroTaskManageInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgroTaskManage::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgroTaskManage::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgroTaskManage::Get().OnCreate(wParam, (QTabWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_AgroTaskManage::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgroTaskManage::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LY_AgroTaskManage::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LY_AgroTaskManage::Get().OnMouseMove((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgroTaskManage::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgroTaskManage::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgroTaskManage::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}

bool LY_AgroTaskManageInterface::OnNetRecv(int nLength, char *lpData)
{
    return false;
}
