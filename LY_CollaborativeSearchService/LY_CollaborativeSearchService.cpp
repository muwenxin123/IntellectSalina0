#include "LY_CollaborativeSearchService.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "LY_CollaborativeSearchServiceInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QDb.h>

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>
#include "lyUI/LyDockManager.h"
#include <LyDockWidget.h>
#include "RoutePlanningDialog.h"
#include "RoutePlanningDisplayDialog.h"
#include "LyMessageBox.h"
#include "IFunctionInterface.h"
#include <QDateTime>

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)
CGetHeight LY_CollaborativeSearchService::s_CGetHeight("../data/dem/90");

IUserMesh * createQuad1(IGraphics *pGraphics)
{
    VERTEX vertex[5];
    vertex[0].x = 0;
    vertex[0].y = 0;
    vertex[0].z = 0;

    vertex[1].x = 10;
    vertex[1].y = 0;
    vertex[1].z = 0;

    vertex[2].x = 10;
    vertex[2].y = 0;
    vertex[2].z = 10;

    vertex[3].x = 0;
    vertex[3].y = 0;
    vertex[3].z = 10;

    vertex[4].x = 5;
    vertex[4].y = 20;
    vertex[4].z = 5;

    unsigned short pIndex[18];
    pIndex[0] = 0;
    pIndex[2] = 2;
    pIndex[1] = 1;

    pIndex[3] = 0;
    pIndex[5] = 3;
    pIndex[4] = 2;

    pIndex[6] = 4;
    pIndex[7] = 3;
    pIndex[8] = 0;

    pIndex[9] = 4;
    pIndex[10] = 0;
    pIndex[11] = 1;

    pIndex[12] = 4;
    pIndex[13] = 1;
    pIndex[14] = 2;

    pIndex[15] = 4;
    pIndex[16] = 2;
    pIndex[17] = 3;

    IUserMesh2 *pUserMesh = pGraphics->CreateUserMesh2();
    pUserMesh->SetPoints(5, vertex);
    pUserMesh->SetIndexs(18, pIndex, IUserMesh::USERMESH_TRIANGLES);
    pUserMesh->SetPosition(116, 40, 20000);
    pUserMesh->SetScale(10000);
    pUserMesh->SetColor(TRGB(240, 0, 128, 128));

    pUserMesh->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
    return pUserMesh;
}

LY_CollaborativeSearchService::LY_CollaborativeSearchService()
{
	int colorIndex = 0;
	m_nColor[0] = 0xffff00;
	m_nColor[colorIndex++] = 0xffff00;
	m_nColor[colorIndex] = 0xffffff;
	m_nColor[colorIndex++] = 0xffa500;

	m_nColor[colorIndex++] = 0x800000;
	m_nColor[colorIndex++] = 0x808000;

	m_nColor[colorIndex++] = 0xaa5500;
	m_nColor[colorIndex++] = 0x808080;
	m_nColor[colorIndex++] = 0x00ff00;
	m_nColor[colorIndex++] = 0xc0c0c0;
	m_nColor[colorIndex++] = 0x00ffff;
	m_nColor[colorIndex++] = 0x008080;
	m_nColor[colorIndex++] = 0x0000ff;
	m_nColor[colorIndex++] = 0x000080;
	m_nColor[colorIndex++] = 0xff00ff;
	m_nColor[colorIndex++] = 0x800080;

	m_nColor[colorIndex++] = 0x404040;
	m_nColor[colorIndex++] = 0xc000c0;
	m_nColor[colorIndex++] = 0x00ffff;

	m_nColor[colorIndex++] = 0x008000;
}

void LY_CollaborativeSearchService::Startup()
{
	IFunctionInterface &ifi = LY_CollaborativeSearchServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			ICollaborativeSearchActivator::Get().start(pIRegistry);
		}
	}
}

void LY_CollaborativeSearchService::Shutdown()
{
	IFunctionInterface &ifi = LY_CollaborativeSearchServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			ICollaborativeSearchActivator::Get().stop(pIRegistry);
		}
	}
}

void LY_CollaborativeSearchService::Reset()
{

}

bool LY_CollaborativeSearchService::DrawGraph( QPainter &painter, IProj *pProj )
{
    return false;
}

bool LY_CollaborativeSearchService::DrawDynamic( QPainter &painter, IProj *pProj )
{

	if (mouseLeftIsDown && m_RoutePlanningDialog->isGetUp)
	{
		m_RoutePlanningDialog->drawMarkRect(painter, pProj);
	}

// 	if (m_RoutePlanningDialog->gridLinesPoints.size() > 0)
// 	{
// 		m_RoutePlanningDialog->drawGrid(painter, pProj);
// 	}

	m_RoutePlanningDialog->drawNoFlayZone(painter, pProj);

	if (m_RoutePlanningDialog->obstacleArea.size() > 0)
	{
		m_RoutePlanningDialog->drawObstacleArea(painter, pProj);
	}

	if (!m_RoutePlanningDialog->oneKeyAreaPoints.empty())
	{
		m_RoutePlanningDialog->drawKeyAreaSelectPoly(painter, pProj);
	}

	if (!m_RoutePlanningDialog->keyXYPoint.empty())
	{
		m_RoutePlanningDialog->drawKeyArea(painter, pProj);
	}

	if (m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube.size() > 0 && m_RoutePlanningDialog->b_startDraw)
	{
		m_RoutePlanningDialog->drawCoverCube(painter, pProj);
	}

	if (mouseLeftIsDown && m_RoutePlanningDialog->isModifyPoint)
	{
		m_RoutePlanningDialog->drawSelectPoint(painter, pProj);
	}

	if (!m_RoutePlanningDialog->newAddPoints.empty())
	{
		m_RoutePlanningDialog->drawNewAddPoint(painter, pProj);
	}

	painter.restore();
	int colorIndex = 0;

	auto pointList = m_RoutePlanningDialog->AreaSearchPointMap;
	if (!pointList.empty())
	{
		for (auto item : pointList)
		{
			if (colorIndex >= sizeof(m_nColor) / sizeof(m_nColor[0]))
			{
				colorIndex = 0;
			}
			QPoint ptFirst;
			QColor greeb = 0x800000;

			QPen pen(greeb,5);
			QPen penText(QColor(255, 255, 255));
			painter.setPen(pen);
			auto waypoints = item.second;
			auto pointitor = waypoints.begin();
			QPointF ptFirst1 = pProj->Jwd2Screen(pointitor->x, pointitor->y);
			unsigned int pointNum = 0;
			for (auto point1 : waypoints)
			{
				double dLongitude = point1.x;
				double dLatitude = point1.y;
				int dHeight = point1.z;

				QPoint ptPoint = pProj->Jwd2Screen(dLongitude, dLatitude);
				painter.drawText(ptPoint, QString::number(pointNum));
				pointNum++;

				painter.setPen(pen);
				painter.setBrush(greeb);
				painter.drawLine(ptFirst1, ptPoint);
				ptFirst1 = ptPoint;

			}
			colorIndex++;
		}
	}

	if (m_uav_dispose_route_dialog  != NULL)
	{
		m_uav_dispose_route_dialog->drawPoint(painter, pProj);
	}

	if (m_uav_dispose_route_dialog != NULL)
	{
		m_uav_dispose_route_dialog->drawUAV_MarkPoisnts(painter, pProj);
		m_uav_dispose_route_dialog->drawUAV_Fire_3Test(painter, pProj);
	}

	painter.save();
    return true;
}

bool LY_CollaborativeSearchService::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    QPoint pt = e->pos();

    if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isGetUp)
	{

		mouseLeftIsDown = true;
		double lon;
		double lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);
		m_RoutePlanningDialog->targetLeftUp = llPoint;
		m_RoutePlanningDialog->targetRightDown = llPoint;
		m_RoutePlanningDialog->setMarkPloy();
	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording &&  m_RoutePlanningDialog->isAddKeyArea)
	{
		double lon, lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);
		m_RoutePlanningDialog->addKeyAreaPoint(llPoint);
	}

    else if (e->button() == Qt::RightButton && m_RoutePlanningDialog && m_SystemEventRecording &&  m_RoutePlanningDialog->isAddKeyArea)
	{
		LLQPoints points = m_RoutePlanningDialog->oneKeyAreaPoints;
		m_RoutePlanningDialog->oneKeyAreaPoints.clear();
		m_RoutePlanningDialog->KeyAreaVec.push_back(points);
		int index = m_RoutePlanningDialog->KeyAreaVec.size() - 1;
		std::pair<bool, int> sec(false, m_RoutePlanningDialog->KeyAreaNum);
		m_RoutePlanningDialog->KeyAreas[index] = sec;
		m_RoutePlanningDialog->setKeyAreaXyHcost();
	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording &&  m_RoutePlanningDialog->isDelKeyArea)
	{

		double lon;
		double lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);

		m_RoutePlanningDialog->delKeyArea(llPoint);
	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isAddNoFly)
	{

		double lon, lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);
		m_RoutePlanningDialog->addNoFlyZone(llPoint);
	}

    else if (e->button() == Qt::RightButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isAddNoFly)
	{
		m_RoutePlanningDialog->NoFlyZonePoints.push_back(m_RoutePlanningDialog->oneNoFlyZonePoint);
		m_RoutePlanningDialog->oneNoFlyZonePoint.clear();

		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DNoFlyArea();
	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isDelNoFly)
	{
		double lon, lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF mousePressPos(lon, lat);
		for (auto index = m_RoutePlanningDialog->NoFlyZonePoints.begin();
			index != m_RoutePlanningDialog->NoFlyZonePoints.end();)
		{
			bool pointIsInZone = m_RoutePlanningDialog->is_point_in_zone(mousePressPos, *index);
			if (pointIsInZone)
			{

				int ret = LyMessageBox::question(NULL, "", QObject::tr("sure to delete current NoFlyArea?"), QMessageBox::Yes | QMessageBox::No );

				switch (ret)
				{
				case QMessageBox::Yes:
				{
					index = m_RoutePlanningDialog->NoFlyZonePoints.erase(index);
					LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DNoFlyArea();
				}
				break;
				case QMessageBox::No:
				{
					index++;
				}
				break;

				}

			}
			else
			{
				index++;
			}

		}

	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isPushStartPoint)
	{
		double lon, lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon, lat);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}
		m_RoutePlanningDialog->curpos = casic_vector3d(lon, lat, elevationTemp);
		m_RoutePlanningDialog->setMarkPloy();

		m_RoutePlanningDialog->AreaSearchPointMap.clear();
		m_RoutePlanningDialog->del3DPathLine();
		m_RoutePlanningDialog->deleteKmeansArea();
		m_RoutePlanningDialog->delCoverArea();
		m_RoutePlanningDialog->kmeansSamples.clear();
		m_RoutePlanningDialog->kmeansLabels.clear();

		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DObstacleArea();
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();

		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
        if(m_SystemEventRecording != nullptr){
            m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, QString("操作事件"), QString("搜索任务规划-设置起飞位置"), QString("110301")));
        }

	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_uav_dispose_route_dialog && m_uav_dispose_route_dialog->selectPos_For != SELECTPOS_NO)
	{
		double lon, lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon, lat);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}
		m_uav_dispose_route_dialog->setUAVOriPos(casic_vector3d(lon, lat, elevationTemp));
		switch (m_uav_dispose_route_dialog->selectPos_For)
		{
		case SELECTPOS_UAV_Throw:
		{
			m_uav_dispose_route_dialog->routInfo_forA2A3[0].uavPos = casic_vector3d(lon, lat, elevationTemp);
		}break;
		case SELECTPOS_UAV_Fire:
		{
			m_uav_dispose_route_dialog->routInfo_forA2A3[1].uavPos = casic_vector3d(lon, lat, elevationTemp);
		}break;
		case SELECTPOS_TAR_Throw:
		{
			m_uav_dispose_route_dialog->routInfo_forA2A3[0].targetPos = casic_vector3d(lon, lat, elevationTemp);
		}break;
		case SELECTPOS_TAR_Fire:
		{
			m_uav_dispose_route_dialog->routInfo_forA2A3[1].targetPos = casic_vector3d(lon, lat, elevationTemp);
		}break;
		}

	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isModifyPoint && !key_CtrlDown)
	{
		mouseLeftIsDown = true;
		double lon;
		double lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);
		m_RoutePlanningDialog->selectFirstPoint = llPoint;
		m_RoutePlanningDialog->selectSecPoint = llPoint;
	}

    else if (e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && key_CtrlDown)
	{
		if (m_RoutePlanningDialog->selectedSerialPoints.empty())
		{
			LyMessageBox::information(NULL, QString("提示").toUtf8(), QString("尚未选择航点").toUtf8());
			key_CtrlDown = false;
			return false;
		}
		else
		{
			double lon, lat;
			m_pProj->Screen2Jwd(pt, lon, lat);
			casic_vector3d pos(lon, lat, m_RoutePlanningDialog->cruiseHeight);
			pos = m_RoutePlanningDialog->newPointTransGridPos(pos);
			m_RoutePlanningDialog->newAddPoints.emplace_back(pos);
		}
	}
    switch(e->button())
    {
    case Qt::LeftButton:
    {

    }
        break;
    case Qt::RightButton:

    {

    }

        break;
    case Qt::MiddleButton:
    {

    }
        break;
    }

    return false;
}

bool LY_CollaborativeSearchService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

void LY_CollaborativeSearchService::mouseMoveEvent(QMouseEvent * e)
{
    if (m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isGetUp && mouseLeftIsDown)
	{
		mouseIsMove = true;
		QPoint nowPoint = e->pos();
		double lon;
		double lat;
		m_pProj->Screen2Jwd(nowPoint, lon, lat);
		QPointF point(lon, lat);
		m_RoutePlanningDialog->targetRightDown = point;
		m_RoutePlanningDialog->setMarkPloy();
	}
    else if (m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isModifyPoint && mouseLeftIsDown)
	{
		mouseIsMove = true;
		QPoint nowPoint = e->pos();
		double lon, lat;
		m_pProj->Screen2Jwd(nowPoint, lon, lat);
		QPointF point(lon, lat);
		m_RoutePlanningDialog->selectSecPoint = point;
	}
}

void LY_CollaborativeSearchService::mouseReleaseEvent(QMouseEvent * e)
{
    if (mouseIsMove && e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isGetUp)
	{
		mouseIsMove = false;
		mouseLeftIsDown = false;

		if (m_RoutePlanningDialog->leftup == m_RoutePlanningDialog->rightdowm)
		{
			return;
		}

		m_RoutePlanningDialog->calcolateGridLinePoints();
		m_RoutePlanningDialog->createGridsNode();
		m_RoutePlanningDialog->AreaSearchPointMap.clear();
		m_RoutePlanningDialog->del3DPathLine();
		m_RoutePlanningDialog->deleteKmeansArea();
		m_RoutePlanningDialog->delCoverArea();
		m_RoutePlanningDialog->kmeansSamples.clear();
		m_RoutePlanningDialog->kmeansLabels.clear();

		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DObstacleArea();
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();

		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
        m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, QString("操作事件"), QString("搜索任务规划-获取区域"), QString("110301")));
	}
    else if (mouseIsMove && e->button() == Qt::LeftButton && m_RoutePlanningDialog && m_SystemEventRecording && m_RoutePlanningDialog->isModifyPoint)
	{
		mouseIsMove = false;
		mouseLeftIsDown = false;

		if (m_RoutePlanningDialog->selectFirstPoint == m_RoutePlanningDialog->selectSecPoint)
		{
			return;
		}
		m_RoutePlanningDialog->pointSelects.clear();
		double p11 = m_RoutePlanningDialog->selectFirstPoint.x();
		double p12 = m_RoutePlanningDialog->selectFirstPoint.y();
		double p21 = m_RoutePlanningDialog->selectSecPoint.x();
		double p22 = m_RoutePlanningDialog->selectSecPoint.y();
		QPointF point1(p11, p12);
		QPointF point2(p11, p22);
		QPointF point3(p21, p22);
		QPointF point4(p21, p12);
		m_RoutePlanningDialog->pointSelects.emplace_back(point1);
		m_RoutePlanningDialog->pointSelects.emplace_back(point2);
		m_RoutePlanningDialog->pointSelects.emplace_back(point3);
		m_RoutePlanningDialog->pointSelects.emplace_back(point4);

		m_RoutePlanningDialog->calSelectedSerialPoint();
		m_RoutePlanningDialog->deleAllSelectedPoint();
	}

}

bool LY_CollaborativeSearchService::OnCommand( int nID, void *lParam)
{
	switch (nID)
	{
	case 433390:
	{
		if (m_RoutePlanningDialog != nullptr)
		{
				m_RoutePlanningDialog->show();
		}
	}
	break;
	case 900100:
	{
		if (m_uav_dispose_route_dialog != nullptr) {
			m_uav_dispose_route_dialog->init_targets();
		}
	}
	break;
	case 900200:
	{
		if (m_SystemEventRecording != nullptr && lParam != nullptr) {
			auto param1 = (EventInfo*)(lParam);
			m_SystemEventRecording->sltRecvEvent(*param1);
		}
	}
	break;
	case 433001:
	{
		if (m_RoutePlanningDialog != nullptr)
		{
			if (m_RoutePlanningDialog->isHidden())
			{
				m_RoutePlanningDialog->show();
			}
		}
		if (m_mission_load_dialog)
		{
			m_mission_load_dialog->show();
		}
	}
	break;
	case 50009:
	{
// 		if (m_RoutePlanningDialog != nullptr)
// 		{
// 			if (m_RoutePlanningDialog->isHidden())
// 			{
// 				m_RoutePlanningDialog->show();
// 			}
// 		}
		break;
	}
	break;
	default:
	{
		if (m_RoutePlanningDialog)
		{
			m_RoutePlanningDialog->hide();
		}
		if (m_mission_load_dialog)
		{
			m_mission_load_dialog->hide();
		}
		break;
	}
	}
    return false;
}

bool LY_CollaborativeSearchService::OnCreate( int wParam, QWidget *pTab )
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch(wParam)
    {
    case 0:
    {

    }
        break;
    case 1000:
    {
		if (m_RoutePlanningDialog == nullptr)
		{
			m_RoutePlanningDialog = new RoutePlanningDialog(pTab);
			m_RoutePlanningDialog->hide();
			m_RoutePlanningDialog->move(0, 80);
			m_RoutePlanningDialog->resize(QSize(screenSize.width(), screenSize.height() - 80));
			 
		}

		if (m_mission_load_dialog == nullptr)
		{
			m_mission_load_dialog = new MissionLoadDialog(pTab);
		}
		if (m_UAVAreaChooseDialog == nullptr)
		{
			m_UAVAreaChooseDialog = new UAVAreaChooseDialog;
		}

		if (ICollaborativeSearchActivator::Get().getPTaskManageService() != NULL)
		{
			ICollaborativeSearchActivator::Get().getPTaskManageService()->Attach(m_RoutePlanningDialog);
		}

		LyDockWidget *pCollaborationSearchDlg1 = LyDockManager::instance()->dockWidget("UAV_Dispose_route_dialog");
		if (pCollaborationSearchDlg1 == nullptr)
		{
            pCollaborationSearchDlg1 = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("灭火任务规划"), "UAV_Dispose_route_dialog");

			LyDockManager::instance()->embedToolDockWidget(pCollaborationSearchDlg1, LyDockDef::Bottom);

		}

		if (m_uav_dispose_route_dialog == nullptr)
		{
			m_uav_dispose_route_dialog = new UAV_DisPose_Route_Dialog;
			pCollaborationSearchDlg1->setCentralWidget(m_uav_dispose_route_dialog);
            pCollaborationSearchDlg1->setWindowTitle(tr2("灭火任务规划"));
		}

		if (ICollaborativeSearchActivator::Get().getPTaskManageService() != NULL)
		{
			ICollaborativeSearchActivator::Get().getPTaskManageService()->Attach(m_uav_dispose_route_dialog);
		}

		LyDockWidget *pCollaborationSearchDlg2 = LyDockManager::instance()->dockWidget("SystemEventRecording");
		if (pCollaborationSearchDlg2 == nullptr)
		{
			pCollaborationSearchDlg2 = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("系统事件记录"), "SystemEventRecording");

			LyDockManager::instance()->embedToolDockWidget(pCollaborationSearchDlg2, LyDockDef::Bottom);

		}

		if (m_SystemEventRecording == nullptr)
		{
			m_SystemEventRecording = new SystemEventRecording;
			pCollaborationSearchDlg2->setCentralWidget(m_SystemEventRecording);
			pCollaborationSearchDlg2->setWindowTitle(tr2("系统事件记录"));
		}

		if (ICollaborativeSearchActivator::Get().getPTaskManageService() != NULL)
		{
			ICollaborativeSearchActivator::Get().getPTaskManageService()->Attach(m_SystemEventRecording);
		}
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
    case 4: m_pWnd = pTab; break;
    }
    return false;
}

bool LY_CollaborativeSearchService::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        LY_CollaborativeSearchServiceInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

bool LY_CollaborativeSearchService::OnNetRecv(int nLength, char * lpData)
{
	if (m_uav_dispose_route_dialog != NULL)
		return m_uav_dispose_route_dialog->deal_TrackInfo(nLength, lpData);
	else
		return false;

}

void LY_CollaborativeSearchService::KeyPress(QKeyEvent * e)
{
	int nkey = e->key();
	switch (e->key())
	{
	case Qt::Key_Control:
	{
		key_CtrlDown = true;
	}
	break;
	}
}

void LY_CollaborativeSearchService::KeyRelease(QKeyEvent * e)
{
	switch (e->key())
	{
	case Qt::Key_Control:
	{
		key_CtrlDown = false;
        if (m_RoutePlanningDialog && m_SystemEventRecording && !m_RoutePlanningDialog->newAddPoints.empty())
		{

			auto pathItor = m_RoutePlanningDialog->selectedSerialPoints.begin();
			while (pathItor != m_RoutePlanningDialog->selectedSerialPoints.end())
			{
				if (pathItor->second.empty())
				{
					pathItor++;
				}
				else
				{
					break;
				}
			}
			if (pathItor != m_RoutePlanningDialog->selectedSerialPoints.end())
			{
				int pathIndex = pathItor->first;
				auto AreaPathItor = m_RoutePlanningDialog->AreaSearchPointMap.find(pathIndex);
				if (AreaPathItor == m_RoutePlanningDialog->AreaSearchPointMap.end())
				{
					return;
				}
				m_RoutePlanningDialog->newPointsAssem2Path(pathIndex, AreaPathItor->second,
					m_RoutePlanningDialog->newAddPoints);
				m_RoutePlanningDialog->newAddPoints.clear();
				m_RoutePlanningDialog->selectedSerialPoints.clear();
				m_RoutePlanningDialog->selectedFBPoints.clear();
			}
			m_RoutePlanningDialog->CalculatePoint();
		}
	}
	break;
	}
}

void LY_CollaborativeSearchService::OnEarthMessage(int wParam, void * lParam)
{
	pos3d *p3d = (pos3d *)lParam;
	switch (wParam)
	{
	case IEarthEventHandler::Earth_LDOUBLECLICK:
	{
		if (m_RoutePlanningDialog && m_SystemEventRecording && m_uav_dispose_route_dialog && m_uav_dispose_route_dialog->selectPos_For != SELECTPOS_NO)
		{
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(p3d->longitude, p3d->latitude);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			m_uav_dispose_route_dialog->setUAVOriPos(casic_vector3d(p3d->longitude, p3d->latitude, elevationTemp > p3d->altitude ? elevationTemp : p3d->altitude));
		}
	}
	break;
	case IEarthEventHandler::Earth_MOUSE_MOVE:
	{
		// qDebug() << "____________________________________________________Earth_Mouse_Move";
	}
	break;
	case IEarthEventHandler::Earth_TRACK_CONTENT:
	{
		// qDebug() << "____________________________________________________Earth_TRACK_CONTENT";
	}
	break;
	case IEarthEventHandler::Earth_LBUTTONDOWN:
	{
		// qDebug() << "_______________________________________________________Earth_LBUTTONDOWN";
	}
	break;
	}
}
