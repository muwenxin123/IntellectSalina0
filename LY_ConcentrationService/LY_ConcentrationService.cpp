#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/Viewpoint>
#include <osgViewer/Viewer>
#include <osg/Math>

#include <QDebug>

#include "LY_ConcentrationService.h"
#include "LY_ConcentrationServiceInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IConcentrationActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)


LY_ConcentrationService::LY_ConcentrationService()
{
    IConcentrationActivator::Get();
}

void LY_ConcentrationService::Startup()
{
    IFunctionInterface &ifi = LY_ConcentrationServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IConcentrationActivator::Get().start(pIRegistry);
        }
    }
}

void LY_ConcentrationService::Shutdown()
{
    IFunctionInterface &ifi = LY_ConcentrationServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IConcentrationActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_ConcentrationService::Reset()
{

}

bool LY_ConcentrationService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_ConcentrationService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_ConcentrationService::OnMouseButtonDown(QMouseEvent *e)
{
	return false;
}

bool LY_ConcentrationService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ConcentrationService::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 33370:
	{

	}
	break;
	default:
	break;
	}
	return false;
}

bool LY_ConcentrationService::OnCreate(int wParam, QWidget *pWgt)
{
    switch (wParam)
    {
    case 0:
    {

    }
    break;
	case 1:
	{

	}
	break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
    case 4:
        break;
	case 1000:
	{
		
	}
    break;
    }
    return false;
}

bool LY_ConcentrationService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_ConcentrationService::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_ConcentrationService::OnEarthMessage(int wParam, void *lParam)
{
	return;
}

bool LY_ConcentrationService::On2d3dChanged(int wParam)
{
	return false;
}

void LY_ConcentrationService::SetScreenCenter(double dLon, double dLat)
{
	return;
}
