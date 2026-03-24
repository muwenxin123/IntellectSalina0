#include "LY_AgriDataAnalysisPredictInterface.h"
#include "LY_AgriDataAnalysisPredict.h"

extern "C" LY_AGRIDATAANALYSISPREDICTSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriDataAnalysisPredictInterface::Get();
}

LY_AgriDataAnalysisPredictInterface &LY_AgriDataAnalysisPredictInterface::Get()
{
    static LY_AgriDataAnalysisPredictInterface gi;
    return gi;
}

void LY_AgriDataAnalysisPredictInterface::Startup()
{
    LY_AgriDataAnalysisPredict::Get().Startup();
}

void LY_AgriDataAnalysisPredictInterface::Shutdown()
{
    LY_AgriDataAnalysisPredict::Get().Shutdown();
}

void LY_AgriDataAnalysisPredictInterface::Reset()
{
    LY_AgriDataAnalysisPredict::Get().Reset();
}

bool LY_AgriDataAnalysisPredictInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriDataAnalysisPredict::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriDataAnalysisPredictInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriDataAnalysisPredict::Get().DrawGraph(painter, pProj);
}

bool LY_AgriDataAnalysisPredictInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriDataAnalysisPredict::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriDataAnalysisPredictInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriDataAnalysisPredict::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriDataAnalysisPredict::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriDataAnalysisPredict::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriDataAnalysisPredict::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriDataAnalysisPredict::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriDataAnalysisPredict::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriDataAnalysisPredict::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriDataAnalysisPredict::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_AgriDataAnalysisPredict::Get().On2d3dChanged(wParam);
	}
	break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgriDataAnalysisPredict::Get().OnEarthMessage(wParam, lParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
