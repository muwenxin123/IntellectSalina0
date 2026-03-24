#include "LY_FireSpreadServive.h"
#include "LY_FireSpreadServiveInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry*           LY_FireSpreadServive::s_IRegistryImpl = nullptr;
qnzkna::FireSpread::IFireSpreadImpl LY_FireSpreadServive::s_IFireSpreadImpl;

LY_FireSpreadServive:: LY_FireSpreadServive()
{
    m_pProj = NULL;
}

void LY_FireSpreadServive::Startup()
{
	IFunctionInterface &ifi = LY_FireSpreadServiveInterface::Get().GetFunctionInterface();
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
		 s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IFireSpreadService", &(s_IFireSpreadImpl)));
	}
}

void LY_FireSpreadServive::Shutdown()
{
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IFireSpreadService", &(s_IFireSpreadImpl)));
	}
}

void LY_FireSpreadServive::Reset()
{

}
bool LY_FireSpreadServive::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_FireSpreadServive::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (&s_IRegistryImpl != nullptr)
	{
		s_IFireSpreadImpl.DrawDynamic(&painter);
	}
    return true;
}

bool LY_FireSpreadServive::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_FireSpreadServive::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_FireSpreadServive::OnCommand(int nID)
{
    return false;
}

bool LY_FireSpreadServive::OnCreate(int wParam, QWidget *pWidget)
{

    return false;
}

bool LY_FireSpreadServive::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_FireSpreadServiveInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

void LY_FireSpreadServive::X_S(double x, double y, float *l, float *b)
{
	if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
	{
		int px, py;
		m_pProj->xyLtoD(x, y, &px, &py);
		*l = (float)px;
		*b = (float)py;
	}
}

void LY_FireSpreadServive::S_X(float x, float y, double *xx, double *yy)
{
	QPoint pt;
	pt.setX((int)x);
	pt.setY((int)y);
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, *xx, *yy);
	}
}

void LY_FireSpreadServive::L_X(double l, double b, double *x, double *y)
{
	float x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->jwxy(l, b, x, y);
	}
}

void LY_FireSpreadServive::X_L(double x, double y, double *l, double *b)
{
	float l1 = 0, b1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->xyjw(x, y, l, b);
	}
}

void LY_FireSpreadServive::L_S(double x, double y, int *sx, int *sy)
{
	if (m_pProj == nullptr)
	{
		return;
	}
	double zx, zy;
	m_pProj->jwxy((float)x, (float)y, &zx, &zy);
	m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void LY_FireSpreadServive::S_L(int sx, int sy, double *x, double *y)
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
