#include "MeasureInterface.h"
#include "Measure_global.h"
#include "Measure.h"

extern "C" MEASURESHARED_EXPORT IInterface *RegisterInterface()
{
    return &MeasureInterface::Get();
}

MeasureInterface &MeasureInterface::Get()
{
    static MeasureInterface gi;
    return gi;
}

bool MeasureInterface::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool MeasureInterface::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return Measure::Get().DrawDynamic(painter, pProj);
    return true;
}

bool MeasureInterface::OnMessage(unsigned int uiMessage, int wParam, void *lParam)
{
    bool bReturn = false;

    switch (uiMessage)
    {
    case IInterface::MSG_SET_DATA_PATH:
    {
        Measure::Get().SetDataDir((const char *)lParam);
    }
    break;
    case IInterface::MSG_Create:
    {
        Measure::Get().OnCreate(wParam, (QWidget *)lParam);
    }
    break;
    case IInterface::MSG_Command:
    {
        Measure::Get().OnCommand(wParam);
    }
    break;
    case IInterface::MSG_MouseButtonPress:
    {
        bReturn = Measure::Get().OnMouseButtonPress((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseMove:
    {
        bReturn = Measure::Get().OnMouseButtonMove((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonRelease:
    {
        bReturn = Measure::Get().OnMouseButtonRelease((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_MouseButtonDblClick:
    {
        bReturn = Measure::Get().OnMouseButtonDblClick((QMouseEvent *)lParam);
    }
    break;
    case IInterface::MSG_SET_COORDINATE_TRANSFORMATION:
    {
        Measure::Get().SetIProj((IProj *)lParam);
    }
    break;
    case IInterface::MSG_2D3D_CHANGED:
    {
        Measure::Get().On2d3dChanged(wParam);
    }
    break;
    case IInterface::MSG_3D_EARTH:
    {
        Measure::Get().OnEarthMessage(wParam, lParam);
    }
    break;
    default:
        break;
    }
    return bReturn;
}
