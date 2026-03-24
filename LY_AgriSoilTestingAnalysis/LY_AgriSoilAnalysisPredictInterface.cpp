#include "LY_AgriSoilAnalysisPredictInterface.h"
#include "LY_AgriSoilAnalysisPredict.h"

extern "C" LY_AGRISOILTESTINGANALYSISSHARED_EXPORT IInterface *RegisterInterface()
{
    return &LY_AgriSoilAnalysisPredictInterface::Get();
}

LY_AgriSoilAnalysisPredictInterface &LY_AgriSoilAnalysisPredictInterface::Get()
{
    static LY_AgriSoilAnalysisPredictInterface gi;
    return gi;
}

void LY_AgriSoilAnalysisPredictInterface::Startup()
{
    LY_AgriSoilAnalysisPredict::Get().Startup();
}

void LY_AgriSoilAnalysisPredictInterface::Shutdown()
{
    LY_AgriSoilAnalysisPredict::Get().Shutdown();
}

void LY_AgriSoilAnalysisPredictInterface::Reset()
{
    LY_AgriSoilAnalysisPredict::Get().Reset();
}

bool LY_AgriSoilAnalysisPredictInterface::OnNetRecv(int nLength, char *lpData)
{
    return LY_AgriSoilAnalysisPredict::Get().OnNetRecv(nLength, lpData);
}

bool LY_AgriSoilAnalysisPredictInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return LY_AgriSoilAnalysisPredict::Get().DrawGraph(painter, pProj);
}

bool LY_AgriSoilAnalysisPredictInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return LY_AgriSoilAnalysisPredict::Get().DrawDynamic(painter, pProj);
}

bool LY_AgriSoilAnalysisPredictInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_CONFIGURE_PATH:
    {
        LY_AgriSoilAnalysisPredict::Get().SetConfDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_SET_DATA_PATH:
    {
        LY_AgriSoilAnalysisPredict::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        bReturn = LY_AgriSoilAnalysisPredict::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
		bReturn = LY_AgriSoilAnalysisPredict::Get().OnCommand(wParam, lParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = LY_AgriSoilAnalysisPredict::Get().OnMouseButtonDown((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = LY_AgriSoilAnalysisPredict::Get().OnMouseDoubleClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_Timer:
    {
        LY_AgriSoilAnalysisPredict::Get().OnTimer(wParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        LY_AgriSoilAnalysisPredict::Get().SetCoordinateTransformation((IProj *)lParam);
    }
	break;
	case IInterface::MSG_2D3D_CHANGED:
	{
		LY_AgriSoilAnalysisPredict::Get().On2d3dChanged(wParam);
	}
	break;
	case IInterface::MSG_3D_EARTH:
	{
		LY_AgriSoilAnalysisPredict::Get().OnEarthMessage(wParam, lParam);
	}
    break;
    default:
        break;
    }
    return bReturn;
}
