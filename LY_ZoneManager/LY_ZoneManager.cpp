#include "LY_ZoneManager.h"
#include "LY_ZoneManagerInterface.h"
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"
#include <IFunctionInterface.h>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "IProj.h"
#include "QMetaType"
#include "vector3.h"
#include "IZoneManagerActivator.h"
#include <QMouseEvent>
#include <QPushButton>

#include <map3d/EarthEvent.h>

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
CGetHeight LY_ZoneManager::s_CGetHeight("../data/dem/90");
LY_ZoneManager:: LY_ZoneManager()
{
    m_pProj = NULL;
}

void LY_ZoneManager::X_S(double x, double y, float *l, float *b)
{
	if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
	{
		int px, py;
		m_pProj->xyLtoD(x, y, &px, &py);
		*l = (float)px;
		*b = (float)py;
	}
}

void LY_ZoneManager::S_X(float x, float y, double *xx, double *yy)
{
	QPoint pt;
	pt.setX((int)x);
	pt.setY((int)y);
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, *xx, *yy);
	}
}

void LY_ZoneManager::L_X(double l, double b, double *x, double *y)
{
	float x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->jwxy(l, b, x, y);
	}
}

void LY_ZoneManager::X_L(double x, double y, double *l, double *b)
{
	float l1 = 0, b1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->xyjw(x, y, l, b);
	}
}

void LY_ZoneManager::L_S(double x, double y, int *sx, int *sy)
{
	if (m_pProj == nullptr)
	{
		return;
	}
	double zx, zy;
	m_pProj->jwxy((float)x, (float)y, &zx, &zy);
	m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void LY_ZoneManager::S_L(int sx, int sy, double *x, double *y)
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

void LY_ZoneManager::Startup()
{
	IFunctionInterface &ifi = LY_ZoneManagerInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			IZoneManagerActivator::Get().start(pIRegistry);
		}
	}
}

void LY_ZoneManager::Shutdown()
{
	IFunctionInterface &ifi = LY_ZoneManagerInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			IZoneManagerActivator::Get().stop(pIRegistry);
		}
	}
}

void LY_ZoneManager::Reset()
{

}
bool LY_ZoneManager::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_ZoneManager::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pZoneManageWidget != nullptr)
	{
		m_pZoneManageWidget->DrawQYZoneData(&painter);
	}
	if (m_pZoneManageWidget->b_IsShowTextAll)
	{
		m_pZoneManageWidget->DrawTextFunctionTwo(painter, pProj);
	}

	//土地图标
	m_strIconPath=(":/images/");
	if (!m_vlandIocn.empty())
	{
		QPixmap pixmap;
		int num = 1;
		for (auto data:m_vlandIocn)
		{
			if (num < 3) {
				pixmap.load(m_strIconPath + "copy001.png");
			}else if (num<6)
			{
				pixmap.load(m_strIconPath + "copy002.png");
			}
			else if (num < 9)
			{
				pixmap.load(m_strIconPath + "copy003.png");
			}
			else if (num < 12)
			{
				pixmap.load(m_strIconPath + "copy004.png");
			}
			else
			{
				pixmap.load(m_strIconPath + "copy005.png");
			}
			const QPointF drawpointF = pProj->Jwd2Screen(data.rx(), data.ry());
			painter.drawPixmap(drawpointF.x()-15, drawpointF.y()-15,30,30, pixmap);
			num++;
		}
	}


	return true;
}

void LY_ZoneManager::OnMouseButtonDown(QMouseEvent *e)
{
	QPoint pt1 = e->pos();
	double x = 0, y = 0;
	double jd = 0, wd = 0;
	m_pProj->xyDtoL(pt1.x(), pt1.y(), &x, &y);
	m_pProj->xyjw(x, y, &jd, &wd);

	if (e->button() == Qt::LeftButton) {
		if (m_pZoneManageWidget && m_pZoneManageWidget->m_bCreateFlag == true && m_pZoneManageWidget->m_pZoneAdditionDrawWidget && m_pZoneManageWidget->m_pZoneAdditionDrawWidget->isVisible())
		{
			auto zonePoly = m_pZoneManageWidget->m_pZoneAdditionDrawWidget->m_pZonePoly;
			auto dragPolyVec = m_pZoneManageWidget->m_pZoneAdditionDrawWidget->m_DragPolyVec;
			int n = dragPolyVec.size();
			for (int i = 0; i < n; i++) {
				if (dragPolyVec[i].containsPoint(pt1, Qt::OddEvenFill)) {
					m_pDragPointSelected = true;
					m_pDragIndex = i;
				}
			}
			if (!m_pDragPointSelected) {
				if (zonePoly.containsPoint(pt1, Qt::OddEvenFill)) {
					m_pZoneManageWidget->isZoneSelected(true);
					m_pZoneSelected = true;
				}
				else {
					m_pZoneManageWidget->insertJWDpt(jd, wd);
				}
			}
		}
	}


	//土地图标
	if (e->button()==Qt::LeftButton && m_pZoneManageWidget->isDrowIcon)
	{
		QPointF point(jd, wd);
		m_vlandIocn.push_back(point);
	}
}

void LY_ZoneManager::OnMouseButtonRelease(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{

		m_oriX = m_oriY = m_pyx = m_pyy = 0;
		m_mkind = -1;
		b_oriX = b_oriY = b_pyx = b_pyy = 0;
		m_bkind = -1;
		m_nModifyZone = 0;
		m_ZOriLat = m_ZOriLong = 0;

	}
	m_pDragPointSelected = false;
	m_pZoneSelected = false;
}

void LY_ZoneManager::OnMouseMove(QMouseEvent *e)
{
	bool bReturn = false;
	if (m_pProj == nullptr)
	{
		return;
	}
	QPoint pt1 = e->pos();
	double x = 0, y = 0;
	double jd = 0, wd = 0;
	m_pProj->xyDtoL(pt1.x(), pt1.y(), &x, &y);
	m_pProj->xyjw(x, y, &jd, &wd);

	if ((e->buttons()&Qt::LeftButton) && m_pZoneManageWidget && m_pZoneManageWidget->isVisible() && m_pZoneManageWidget->m_bCreateFlag == true)
	{
		if (m_pDragPointSelected) {
			m_pZoneManageWidget->m_pZoneAdditionDrawWidget->EditMouseMoveRadius(m_pDragIndex, jd, wd);
		}

		if (!m_pDragPointSelected && m_pZoneSelected) {
			m_pZoneManageWidget->EditMouseMoveJWD(jd, wd);
		}
	}

	return;
}

bool LY_ZoneManager::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ZoneManager::OnCommand(int nID)
{
    return false;
}

bool LY_ZoneManager::OnCommand(int nID, void * lParam)
{
	switch (nID)
	{
	case 1001001:
	{
		if (m_pZoneManageWidget && m_pZoneManageWidget->m_bCreateFlag == true && m_pZoneManageWidget->m_pZoneAdditionDrawWidget && m_pZoneManageWidget->m_pZoneAdditionDrawWidget->isVisible())
		{

		}
	}
	break;
	case 1001002:
	{
		pos3d *p3d = (pos3d *)lParam;
		if (m_pZoneManageWidget && m_pZoneManageWidget->m_bCreateFlag == true && m_pZoneManageWidget->m_pZoneAdditionDrawWidget && m_pZoneManageWidget->m_pZoneAdditionDrawWidget->isVisible())
		{
			m_pZoneManageWidget->insertJWDpt(p3d->longitude, p3d->latitude);
		}
	}
	break;
	case 1001003:
	{
		float* radius = (float*)lParam;
		if (m_pZoneManageWidget && m_pZoneManageWidget->m_bCreateFlag == true && m_pZoneManageWidget->m_pZoneAdditionDrawWidget && m_pZoneManageWidget->m_pZoneAdditionDrawWidget->isVisible())
		{
			m_pZoneManageWidget->m_pZoneAdditionDrawWidget->getCDlgZoneParameterEdit()->EditMouseMoveRadius(*radius);

		}
	}
	break;
	case 1001004:
	{
		std::vector<casic_vector3d>* radius = (std::vector<casic_vector3d>*)lParam;
		if (m_pZoneManageWidget )
		{
			int vecSize = radius->size();
			qnzkna::ZoneManage::ZonePointVec zonpoints;
			auto zonebase = m_pZoneManageWidget->getCurZoneBase();
			for (int i = 0; i < vecSize; i++)
			{
				zonpoints.push_back(qnzkna::ZoneManage::ZonePoint(radius->at(i).x, radius->at(i).y));
			}
			zonebase.stZonePointVec = zonpoints;
			zonebase.nPointCount = vecSize;
			m_pZoneManageWidget->setCurZoneBase(zonebase);
		}
	}
	break;
	case 1001005:
	{
		std::vector<casic_vector3d>* radius = (std::vector<casic_vector3d>*)lParam;
		if (m_pZoneManageWidget )
		{
			int vecSize = radius->size();
			if (vecSize == 2)
			{
				qnzkna::ZoneManage::ZonePointVec zonpoints;
				auto zonebase = m_pZoneManageWidget->getCurZoneBase();
				zonebase.stRoundParameter.dLon = radius->at(0).x;
				zonebase.stRoundParameter.dLat = radius->at(0).y;
				zonebase.stRoundParameter.nRadius = radius->at(1).x;
				m_pZoneManageWidget->setCurZoneBase(zonebase);
			}
		}
	}
	break;
	case 1001006:
	{
		std::vector<casic_vector3d>* radius = (std::vector<casic_vector3d>*)lParam;
		if (m_pZoneManageWidget)
		{
			int vecSize = radius->size();
			if (vecSize == 2)
			{
				qnzkna::ZoneManage::ZonePointVec zonpoints;
				auto zonebase = m_pZoneManageWidget->getCurZoneBase();
				zonebase.stRoundParameter.dLon = radius->at(0).x;
				zonebase.stRoundParameter.dLat = radius->at(0).y;
				zonebase.stRoundParameter.nRaidusDuan = radius->at(1).x;
				zonebase.stRoundParameter.nRaidusChang = radius->at(1).y;
				zonebase.stRoundParameter.nNorthAngel = radius->at(1).z;
				m_pZoneManageWidget->setCurZoneBase(zonebase);
			}
		}
	}

	case 433388:
	{
	
		if (m_pZoneManageWidget)
		{
			if (m_pZoneManageWidget->isHidden())
			{
				m_pZoneManageWidget->show();
			}
		}

	}
	break;
	default:
	{
		if (m_pZoneManageWidget)
		{
			m_pZoneManageWidget->hide();
		}
	}
		break;
	}

	return false;
}

bool LY_ZoneManager::OnCreate(int wParam, QWidget *pWidget)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
	switch (wParam)
	{
	case 1000:
	{
// 		LyDockWidget *pDockZoneManager = LyDockManager::instance()->dockWidget("ZoneManager");
// 		if (pDockZoneManager == nullptr)
// 		{
// 			pDockZoneManager = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QString("区域管理"), "ZoneManager");
// 			LyDockManager::instance()->embedToolDockWidget(pDockZoneManager, LyDockDef::Right);
// 		}
// 
// 		if (m_pZoneManageWidget == nullptr)
// 		{
// 			m_pZoneManageWidget = new ZoneManageWidget;
// 			pDockZoneManager->setCentralWidget(m_pZoneManageWidget);
// 			m_pZoneManageWidget->setWindowTitle(QString("区域管理"));
// 			LyDockWidget::connect(m_pZoneManageWidget, &ZoneManageWidget::windowTitleChanged, pDockZoneManager, &LyDockWidget::setWindowTitle);
// 		}

		if (m_pZoneManageWidget == nullptr)
		{
			m_pZoneManageWidget = new ZoneManageWidget(pWidget);
			m_pZoneManageWidget->hide();
			m_pZoneManageWidget->move(0, 60);
			m_pZoneManageWidget->resize(QSize(screenSize.width(), screenSize.height() - 60));
		}

		if (m_pDlgZoneRuleManage == nullptr)
		{
			m_pDlgZoneRuleManage = new CDlgZoneRuleManage(pWidget);
			m_pDlgZoneRuleManage->hide();
		}

		if (m_pDlgZoneFileManage == nullptr)
		{
			m_pDlgZoneFileManage = new CDlgZoneFileManage(pWidget);
			m_pDlgZoneFileManage->hide();
		}

		if (m_pZoneTypeMatchDialog==nullptr)
		{
			m_pZoneTypeMatchDialog = new IZoneTypeMatchDialog(pWidget);
			m_pZoneTypeMatchDialog->hide();
		}

		if (m_pDlgZoneScale == nullptr)
		{
			m_pDlgZoneScale = new CDlgZoneScale(pWidget);
			m_pDlgZoneScale->hide();
		}

		LyDockWidget *pDockMeteorologicalIntelligenceWidget = LyDockManager::instance()->dockWidget("MeteorologicalIntelligenceWidget");
		if (pDockMeteorologicalIntelligenceWidget == nullptr)
		{
			pDockMeteorologicalIntelligenceWidget = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QString("气象情报"), "MeteorologicalIntelligenceWidget");
			LyDockManager::instance()->embedToolDockWidget(pDockMeteorologicalIntelligenceWidget, LyDockDef::Right);
		}

		if (m_pMeteorologicalIntelligenceWidget == nullptr)
		{
			m_pMeteorologicalIntelligenceWidget = new MeteorologicalIntelligenceWidget;
			pDockMeteorologicalIntelligenceWidget->setCentralWidget(m_pMeteorologicalIntelligenceWidget);
			m_pMeteorologicalIntelligenceWidget->setWindowTitle(QString("气象情报"));
			LyDockWidget::connect(m_pMeteorologicalIntelligenceWidget, &ZoneManageWidget::windowTitleChanged, pDockMeteorologicalIntelligenceWidget, &LyDockWidget::setWindowTitle);
		}

		LyDockWidget *pDockFireSpreadWidget = LyDockManager::instance()->dockWidget("FireSpreadWidget");
		if (pDockFireSpreadWidget == nullptr)
		{
			pDockFireSpreadWidget = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QString("火情蔓延"), "FireSpreadWidget");
			LyDockManager::instance()->embedToolDockWidget(pDockFireSpreadWidget, LyDockDef::Right);
		}

		if (m_pFireSpreadWidget == nullptr)
		{
			m_pFireSpreadWidget = new FireSpreadWidget;
			pDockFireSpreadWidget->setCentralWidget(m_pFireSpreadWidget);
			m_pFireSpreadWidget->setWindowTitle(QString("火情蔓延"));
			LyDockWidget::connect(m_pFireSpreadWidget, &ZoneManageWidget::windowTitleChanged, pDockFireSpreadWidget, &LyDockWidget::setWindowTitle);
		}

		if (m_pCDlgSet3DLableColor == nullptr)
		{
			m_pCDlgSet3DLableColor = new ICDlgSet3DLableColor(pWidget);
			m_pCDlgSet3DLableColor->hide();
		}
	}break;
	default:
		break;

	}

    return false;
}

bool LY_ZoneManager::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_ZoneManagerInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

int LY_ZoneManager::IfOnQYLine(double jd, double wd, const QUYU_Zone &zone)
{
	if (m_pProj == nullptr)
	{
		return -1;
	}
	long   pms;
	QPoint  pt = m_pProj->Jwd2Screen(jd, wd);
	QPoint cpoint1, cpoint2;
	if (zone.type == 3 || zone.type == 8)
	{
		double dDistance = 0;
		double dAngle = 0;
		int val = 5;
		for (int i = 0; i < zone.data.n; i++)
		{
			cpoint1 = m_pProj->Jwd2Screen(zone.data.h[i].x, zone.data.h[i].y);
			if (i == zone.data.n - 1)
			{
				cpoint2 = m_pProj->Jwd2Screen(zone.data.h[0].x, zone.data.h[0].y);
				m_pProj->L_A(zone.data.h[i].x, zone.data.h[i].y, zone.data.h[0].x, zone.data.h[0].y, dDistance, dAngle);
			}
			else
			{
				cpoint2 = m_pProj->Jwd2Screen(zone.data.h[i + 1].x, zone.data.h[i + 1].y);
				m_pProj->L_A(zone.data.h[i].x, zone.data.h[i].y, zone.data.h[i + 1].x, zone.data.h[i + 1].y, dDistance, dAngle);
			}
			if ((abs(pt.x() - cpoint1.x()) < val && abs(pt.y() - cpoint1.y()) < val) || (abs(pt.x() - cpoint2.x()) < val && abs(pt.y() - cpoint2.y()) < val))
			{
				return -1;
			}
			if (IfOnLine(pt.x(), pt.y(), cpoint1.x(), cpoint1.y(), cpoint2.x(), cpoint2.y()))
			{
				return i;
			}
		}

	}
	if (zone.type == 0 || zone.type == 5)
	{
		double  r = (double)zone.data.h[2].x;
		pms = m_pProj->Map2ScreenDistance(r * 1000);
		long pms1 = m_pProj->Map2ScreenDistance(zone.data.h[2].x * 1000);
		double  s;
		double a;
		double b;
		cpoint1 = m_pProj->Jwd2Screen(zone.data.h[0].x, zone.data.h[0].y);
		a = pt.x() - cpoint1.x();
		b = pt.y() - cpoint1.y();
		s = sqrt(a * a + b * b);
		if (fabs(s - pms) < 2 || fabs(s - pms1) < 2)
		{
			return 1;
		}
	}
	if (zone.type == 1 || zone.type == 6)
	{
		double dDistance = 0;
		double dAngle = 0;
		double  r = (double)zone.data.h[2].x;
		double r1 = (double)zone.data.h[2].y;
		long dj;
		m_pProj->L_A(zone.data.h[0].x, zone.data.h[0].y, jd, wd, dDistance, dAngle);

		if (((zone.data.h[1].y - zone.data.h[1].x) >= 0 && (dAngle >= zone.data.h[1].x && dAngle <= zone.data.h[1].y))
			|| ((zone.data.h[1].y - zone.data.h[1].x) < 0 && (dAngle + 360 >= zone.data.h[1].x && dAngle <= zone.data.h[1].y)))
		{
			dj = m_pProj->Map2ScreenDistance(dDistance);
			pms = m_pProj->Map2ScreenDistance(r * 1000);
			long pms1 = m_pProj->Map2ScreenDistance(r1 * 1000);
			if (abs(dj - pms) < 2 || abs(dj - pms1) < 2)
			{
				return 1;
			}
		}
	}
	if (zone.type == 4)
	{
		float l, b;
		double dis;
		double j1, j2, j3, j4;
		double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
		double x1, y1;
		L_X(zone.data.h[0].x, zone.data.h[0].y, &x1, &y1);
		if (zone.data.h[2].y == 0)
		{
			dis = sqrt(zone.data.h[1].x * zone.data.h[1].x + zone.data.h[1].y * zone.data.h[1].y) * 1000 / 2;
			if (m_pProj != nullptr)
			{
				j1 = zone.data.h[2].x - atan(zone.data.h[1].y / zone.data.h[1].x) * 180 / PI;
				m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
				j2 = zone.data.h[2].x + atan(zone.data.h[1].y / zone.data.h[1].x) * 180 / PI;
				m_pProj->A_X(x1, y1, dis, j2, dx2, dy2);
				j3 = j1 + 180;
				m_pProj->A_X(x1, y1, dis, j3, dx3, dy3);
				j4 = j2 + 180;
				m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);
			}
		}
		else
		{
			if (m_pProj != nullptr)
			{
				dis = zone.data.h[1].y * 1000 / 2;
				j1 = zone.data.h[2].x - 90;
				m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
				j4 = zone.data.h[2].x + 90;
				m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);
				dis = zone.data.h[1].x * 1000;
				j2 = j3 = zone.data.h[2].x;
				m_pProj->A_X(dx1, dy1, dis, j2, dx2, dy2);
				m_pProj->A_X(dx4, dy4, dis, j3, dx3, dy3);
			}
		}
		QPoint zonept[4];
		X_S(dx1, dy1, &l, &b);
		zonept[0].setX(l);
		zonept[0].setY(b);
		X_S(dx2, dy2, &l, &b);
		zonept[1].setX(l);
		zonept[1].setY(b);
		X_S(dx3, dy3, &l, &b);
		zonept[2].setX(l);
		zonept[2].setY(b);
		X_S(dx4, dy4, &l, &b);
		zonept[3].setX(l);
		zonept[3].setY(b);
		for (int n = 0; n < 4; n++)
		{
			if (n == 3)
			{
				if (IfOnLine(pt.x(), pt.y(), zonept[n].x(), zonept[n].y(), zonept[0].x(), zonept[0].y()))
				{
					return 1;
				}
			}
			else
			{
				if (IfOnLine(pt.x(), pt.y(), zonept[n].x(), zonept[n].y(), zonept[n + 1].x(), zonept[n + 1].y()))
				{
					return 1;
				}
			}
		}
	}
	return -1;
}

int LY_ZoneManager::IfOnQYPot(double jd, double wd, const QUYU_Zone &zone)
{
	if (m_pProj == nullptr)
	{
		return -1;
	}
	QPoint pt = m_pProj->Jwd2Screen(jd, wd);
	QPoint cpoint;
	for (int i = 0; i < zone.data.n; i++)
	{
		cpoint = m_pProj->Jwd2Screen(zone.data.h[i].x, zone.data.h[i].y);
		if ((pt.x() - cpoint.x()) * (pt.x() - cpoint.x()) + (pt.y() - cpoint.y()) * (pt.y() - cpoint.y()) <= 9)
		{
			return i;
		}
	}
	return -1;
}

int LY_ZoneManager::IfOnLine(double x, double y, double x1, double y1, double x2, double y2, int nPixel, bool bScreen )
{
	double a = y1 - y2;
	double b = x2 - x1;
	double c = x1 * y2 - x2 * y1;
	if (a == 0 && b == 0)
	{
		return false;
	}
	double aa = (x - x1) * (x - x1) + (y - y1) * (y - y1);
	double bb = (x - x2) * (x - x2) + (y - y2) * (y - y2);
	double cc = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cc + aa >= bb && cc + bb >= aa)
	{
		double L = -1.0 * (a * x + b * y + c) / sqrt(a * a + b * b);
		if (bScreen)
		{
			L = m_pProj->Screen2MapDistance(fabs(L));
		}
		double dDelta = nPixel == 0 ? 30 : m_pProj->Screen2MapDistance(nPixel);
		if (fabs(L) <= dDelta)
		{
			return true;
		}
	}
	return false;
}

bool LY_ZoneManager::On2d3dChanged(int wParam)
{
	if (m_pZoneManageWidget != NULL)
	{
		m_pZoneManageWidget->On2d3dChanged(wParam);
	}
	return false;
}

QString LY_ZoneManager::RtnDir_and_Dis_str(double jd1, double wd1, double jd2, double wd2, int kind )
{
	QString  str = "";
	if (m_pProj == nullptr)
	{
		return str;
	}
	double dDistance = 0;
	double dAngle = 0;
	m_pProj->L_A(jd1, wd1, jd2, wd2, dDistance, dAngle);

	if (kind == 0)
	{
		int Ly;
		Ly = (int)(dAngle + 0.5);
		dDistance = dDistance / 1000.0 + 0.5;
		str = QStringLiteral("X:%1 S:%2").arg(Ly, 3, 10, QChar('0')).arg((int)dDistance);
	}
	else
	{
		str = QStringLiteral("(%1--%2)").arg((int)(dAngle + 0.5)).arg((int)(dDistance / 1000.0 + 0.5));
	}
	return str;
}

double LY_ZoneManager::StrToJWD(const QString &str)
{
	int du, fen, miao;
	sscanf(str.toLocal8Bit().data(), "%d.%d'%d\"", &du, &fen, &miao);
	return 1.0 * du + 1.0 / 60 * fen + 1.0 / 3600 * miao;
}

ZoneManageWidget* LY_ZoneManager::GetPZoneManageWidget() const
{
	return m_pZoneManageWidget;
}

CDlgZoneRuleManage* LY_ZoneManager::GetPDlgZoneRuleManage() const
{
	return m_pDlgZoneRuleManage;
}

CDlgZoneFileManage* LY_ZoneManager::GetPDlgZoneFileManage() const
{
	return m_pDlgZoneFileManage;
}

IZoneTypeMatchDialog* LY_ZoneManager::GetPZoneTypeMatchDialog() const
{
	return m_pZoneTypeMatchDialog;
}

ICDlgSet3DLableColor* LY_ZoneManager::GetCDlgSet3DLableColor() const
{
	return m_pCDlgSet3DLableColor;
}
