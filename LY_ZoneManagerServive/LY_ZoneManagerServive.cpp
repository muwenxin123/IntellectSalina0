#include "LY_ZoneManagerServive.h"
#include "LY_ZoneManagerServiveInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry*           LY_ZoneManagerServive::s_IRegistryImpl = nullptr;
qnzkna::ZoneManage::IZoneManagerImpl	LY_ZoneManagerServive::s_IZoneManagerImpl;

LY_ZoneManagerServive:: LY_ZoneManagerServive()
{
    m_pProj = NULL;
}

void LY_ZoneManagerServive::Startup()
{
	IFunctionInterface &ifi = LY_ZoneManagerServiveInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			s_IRegistryImpl = pIRegistry;
		}
	}

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IZoneManageService", &(s_IZoneManagerImpl)));
	}
}

void LY_ZoneManagerServive::Shutdown()
{
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IZoneManageService", &(s_IZoneManagerImpl)));
	}
}

void LY_ZoneManagerServive::Reset()
{

}
bool LY_ZoneManagerServive::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_ZoneManagerServive::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (&s_IRegistryImpl != nullptr)
	{
		s_IZoneManagerImpl.DrawQYZoneData(&painter);
	}
    return true;
}

bool LY_ZoneManagerServive::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_ZoneManagerServive::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ZoneManagerServive::OnCommand(int nID)
{
    return false;
}

bool LY_ZoneManagerServive::OnCreate(int wParam, QWidget *pWidget)
{

    return false;
}

bool LY_ZoneManagerServive::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_ZoneManagerServiveInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

void LY_ZoneManagerServive::OnEarthMessage(int wParam, void * lParam)
{

	{
		s_IZoneManagerImpl.OnEarthMessage_Pick(wParam, lParam);
	}
}

void LY_ZoneManagerServive::X_S(double x, double y, float *l, float *b)
{
	if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
	{
		int px, py;
		m_pProj->xyLtoD(x, y, &px, &py);
		*l = (float)px;
		*b = (float)py;
	}
}

void LY_ZoneManagerServive::S_X(float x, float y, double *xx, double *yy)
{
	QPoint pt;
	pt.setX((int)x);
	pt.setY((int)y);
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, *xx, *yy);
	}
}

void LY_ZoneManagerServive::L_X(double l, double b, double *x, double *y)
{
	float x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->jwxy(l, b, x, y);
	}
}

void LY_ZoneManagerServive::X_L(double x, double y, double *l, double *b)
{
	float l1 = 0, b1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->xyjw(x, y, l, b);
	}
}

void LY_ZoneManagerServive::L_S(double x, double y, int *sx, int *sy)
{
	if (m_pProj == nullptr)
	{
		return;
	}
	double zx, zy;
	m_pProj->jwxy((float)x, (float)y, &zx, &zy);
	m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void LY_ZoneManagerServive::S_L(int sx, int sy, double *x, double *y)
{
	QPoint pt;
	pt.setX((int)sx);
	pt.setY((int)sy);
	double x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, x1, y1);
		m_pProj->xyjw((float)x1, (float)y1, x, y);
	}
	else
	{
		return;
	}
}
