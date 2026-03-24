#include "LY_UAVFlyControl_MavlinkInterface.h"
#include "LY_UAVFlyControl_Mavlink.h"

extern "C" LY_UAVFLYCONTROLSHARED_MAVLINK_EXPORT IInterface *RegisterInterface()
{
    return &LY_UAVFlyControl_MavlinkInterface::Get();
}

LY_UAVFlyControl_MavlinkInterface& LY_UAVFlyControl_MavlinkInterface::Get()
{
    static LY_UAVFlyControl_MavlinkInterface gi;
    return gi;
}

void LY_UAVFlyControl_MavlinkInterface::Startup()
{
    LY_UAVFlyControl_Mavlink::Get().Startup();
}

void LY_UAVFlyControl_MavlinkInterface::Shutdown()
{
    LY_UAVFlyControl_Mavlink::Get().Shutdown();
}

void LY_UAVFlyControl_MavlinkInterface::Reset()
{
    LY_UAVFlyControl_Mavlink::Get().Reset();
}

bool LY_UAVFlyControl_MavlinkInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_UAVFlyControl_Mavlink::Get().OnNetRecv(nLength, lpData);
}

bool LY_UAVFlyControl_MavlinkInterface::DrawGraph( QPainter &painter, IProj *pProj )
{
    return LY_UAVFlyControl_Mavlink::Get().DrawGraph(painter, pProj);
}

bool LY_UAVFlyControl_MavlinkInterface::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return LY_UAVFlyControl_Mavlink::Get().DrawDynamic(painter, pProj);
}

bool LY_UAVFlyControl_MavlinkInterface::OnMessage( unsigned int uiMessage, int wParam, void *lParam )
{
    bool bReturn = false;

    switch(uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_UAVFlyControl_Mavlink::Get().SetConfDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_UAVFlyControl_Mavlink::Get().SetDataDir((const char*)lParam);
    }
        break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnCreate(wParam, (QWidget*)lParam);
    }
        break;
    case IInterface::MSG_Command:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnCommand(wParam, lParam);
    }
        break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LY_UAVFlyControl_Mavlink::Get().OnMouseMove((QMouseEvent *)lParam);
    }
        break;
    case IInterface::MSG_Timer:
    {
        LY_UAVFlyControl_Mavlink::Get().OnTimer(wParam);
    }
        break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_UAVFlyControl_Mavlink::Get().SetCoordinateTransformation((IProj*)lParam);
    }
        break;
	case IInterface::MSG_MAP_IDENTIFY_MENU + 100:
	{
		LY_UAVFlyControl_Mavlink::Get().SetFireProt((FireProt&)lParam);
    }break;
	case IInterface::MSG_MAP_IDENTIFY_MENU + 200:
	{
		if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkTaskManageDialog() != NULL)
		{
			if(wParam == 50009 + 100 * 1)
				LY_UAVFlyControl_Mavlink::Get().setChangeUAVFlyManage_MavlinkTaskManageDialog(QString::fromLocal8Bit("罐"));
			else if (wParam == 50009 + 100 * 2)
			{
				LY_UAVFlyControl_Mavlink::Get().setChangeUAVFlyManage_MavlinkTaskManageDialog(QString::fromLocal8Bit("弹"));
			}
		}
	}
	break;
    default:
        break;
    }
    return bReturn;
}
