#include "lyXyhcInterface.h"
#include "lyXyhc.h"
#include "IXyhcActivator.h"

extern "C" LYXYHCSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LyXyhcInterface::Get();
}

LyXyhcInterface &LyXyhcInterface::Get()
{
    static LyXyhcInterface gi;
    return gi;
}

void LyXyhcInterface::Startup()
{
    LyXyhc::Get().Startup();
}

void LyXyhcInterface::Shutdown()
{
    LyXyhc::Get().Shutdown();
}

void LyXyhcInterface::Reset()
{
    LyXyhc::Get().Reset();
}

bool LyXyhcInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LyXyhc::Get().DrawGraph(painter, pProj);
}

bool LyXyhcInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LyXyhc::Get().DrawDynamic(painter, pProj);
}

bool LyXyhcInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LyXyhc::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LyXyhc::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LyXyhc::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        bReturn = LyXyhc::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LyXyhc::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = LyXyhc::Get().OnMouseButtonUp((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = LyXyhc::Get().OnMouseMove((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LyXyhc::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LyXyhc::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LyXyhc::Get().SetCoordinateTransformation((IProj *)lParam);
    }
    break;
    case IInterface::MSG_3D_EARTH:
    {
        LyXyhc::Get().OnEarthMessage(wParam, lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}

bool LyXyhcInterface::OnNetRecv(int nLength, char *lpData)
{
    NetHeader *netHead = (NetHeader *)lpData;
    if (netHead->ucType == NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE)
    {
        char Operator[32] = { 0 };
        memcpy(Operator, lpData + sizeof(NetHeader) + sizeof(unsigned short), sizeof(Operator));
        std::string loginName = IXyhcActivator::Get().getPTaskManageService()->getLoginName();
        std::string operatorName = Operator;
        if (loginName == operatorName)
        {
            return false;
        }
        LyXyhc::Get().getPDlgSystemMessageDistribute()->getNetMessage_distrubute(nLength, lpData);
    }
    if (netHead->ucType == NETHEADER_UCTYPE_INTERVENCE_CONTROL)
    {
        if (netHead->ucSubType == NETHEADER_UCTYPE_INTERVENCE_CONTROL_DISAPPER)
        {

            TrackInfo *TempData = (TrackInfo *)lpData;

            LyXyhc::Get().getTrackMgrPtr()->UpdateDrawm_Track(TempData->cmn.Z, TempData->cmn.P);
        }

    }
    return LyXyhc::Get().OnNetRecv(nLength, lpData);
}
