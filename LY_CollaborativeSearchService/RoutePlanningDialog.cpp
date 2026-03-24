#include "RoutePlanningDialog.h"
#include "ui_RoutePlanningDialog.h"
#include "LY_CollaborativeSearchService.h"
#include <QGeoCoordinate>
#include "aStar.h"
#include "RRTStar.h"
#include <QPainter>
#include "QMessageBox"
#include "LyMessageBox.h"
#include "FloodFill.hpp"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include <fstream>
#include <sstream>
#include <QFile>
#include "readAndWriteXML.h"
#include "QRegExpExample.h"
#include "LY_CollaborativeSearchServiceInterface.h"
#include "SystemEventRecording.h"
#include <QDateTime>
#include "QCString.h"
#include "lib_ly_ccip/ly_ccip.h"
#include "CuttingCone.h"

#define PLAY_HZ		2
#define PI       3.14159265358979323846

casic_vector3d CalcPosByDis_Angle(double &lon, double &lat, double distance, double& angle)
{
	casic_vector3d newPosition(0.0, 0.0, 0.0);

	double ea = 6378137;
	double eb = 6356725;

	double geoLon = lon * (180 / PI);
	double geoLat = lat * (180 / PI);

	double DLon = distance * sin(angle * (PI / 180));
	double DLat = distance * cos(angle * (PI / 180));

	double latLong = 2 * PI * ea * cos(lat );
	double LonLong = 2 * PI * eb;

	double newGeoLon = geoLon + DLon / latLong * 360;
	double newGeoLat = geoLat + DLat / LonLong * 360;

	newPosition.x = newGeoLon * (PI / 180);
	newPosition.y = newGeoLat * (PI / 180);

	short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(newPosition.x, newPosition.y);
	if (elevationTemp <-10000)
	{
		elevationTemp = 0;
	}
	newPosition.z = elevationTemp;

	return newPosition;
}

double get_distance_from_lat_lon(double lat1, double lon1, double lat2, double lon2)
{

	double lat1Rad = lat1 * PI / 180.0;
	double lon1Rad = lon1 * PI / 180.0;
	double lat2Rad = lat2 * PI / 180.0;
	double lon2Rad = lon2 * PI / 180.0;
	double radius = _earth_r_a;
	double dLat = (lat2Rad - lat1Rad);
	double dLon = (lon2Rad - lon1Rad);
	double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
		cos(lat1Rad) *  cos(lat2Rad) *
		sin(dLon / 2.0) * sin(dLon / 2.0);
	double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
	return (radius * c);
}

RoutePlanningDialog::RoutePlanningDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoutePlanningDialog)
{
    ui->setupUi(this);

	this->setWindowFlags(Qt::FramelessWindowHint);
	m_pITaskManageService = ICollaborativeSearchActivator::Get().getPTaskManageService();


	leftup = casic_vector3d(116.43104189418962, 40.172360416425263,cruiseHeight);
	rightdowm = casic_vector3d(116.48184845111903, 40.142128297212281, cruiseHeight);

	curpos = casic_vector3d(116.45588385357065, 40.156253443307641, cruiseHeight);

	stagingPoint = casic_vector3d(0, 0, 0);
	currTakeOffPoint = casic_vector3d(0, 0, 0);
	m_nPlayTime = 0;

	m_routePlanningPlayDialog = new RoutePlanningDisplayDialog;
	m_routePlanningPlayDialog->hide();
	init();

	ui->pbtnKmeans->hide();
	ui->groupBox_5->hide();
	ui->pbthDisplay->hide();
}

RoutePlanningDialog::~RoutePlanningDialog()
{
    delete ui;
}

void RoutePlanningDialog::autoPlanning()
{

	double lon_curpos = 91.4489829, lat_curpos = 38.5451368;
	short int elevationTemp = 38.00;

	qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
	bool res = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService()->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap);
	if (!res) {
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("未成功获取到无人机的信息"), QMessageBox::Ok);
		lon_curpos = 116.49055701, lat_curpos = 39.89479082;
	}

	elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon_curpos, lat_curpos);
	if (elevationTemp < -10000)
	{
		elevationTemp = 0;
	}
	curpos = casic_vector3d(lon_curpos, lat_curpos, elevationTemp);
	setMarkPloy();
	AreaSearchPointMap.clear();
	del3DPathLine();
	deleteKmeansArea();
	delCoverArea();
	kmeansSamples.clear();
	kmeansLabels.clear();

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DObstacleArea();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();

	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("系统事件"), tr2("区域搜索规划添加起飞点")));

	QPoint ltPoint1;
	double lon = lon_curpos - 0.005, lat = lat_curpos - 0.001;

	QPointF ltPoint(lon, lat);
	targetLeftUp = ltPoint;
	setMarkPloy();

	QPoint rdPoint1;
	lon = lon_curpos + 0.01, lat = lat_curpos - 0.01;

	QPointF rdPoint(lon, lat);
	targetRightDown = rdPoint;
	setMarkPloy();

	calcolateGridLinePoints();
	createGridsNode();
	AreaSearchPointMap.clear();
	del3DPathLine();
	deleteKmeansArea();
	delCoverArea();
	kmeansSamples.clear();
	kmeansLabels.clear();
	kmeansArea3D.clear();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DObstacleArea();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();

	curdt = QDateTime::currentDateTime();
	strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("系统事件"), tr2("区域搜索规划绘制网格")));

	m_pAutoPlanning = true;

	m_pAutoPlanning = false;

	curdt = QDateTime::currentDateTime();
	strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("系统事件"), tr2("区域搜索规划规划并绘制航线")));
}

void RoutePlanningDialog::init()
{
	ui->ledtUavCruiseHeight->setValidator(new QRegExpValidator(regNum));
	ui->ledtspeed->setValidator(new QRegExpValidator(regNum));
	ui->ledtflyTime->setValidator(new QRegExpValidator(regNum));
	ui->ledtAngularSpeed->setValidator(new QRegExpValidator(regNum));
	ui->ledtUpSpeed->setValidator(new QRegExpValidator(regNum));
	ui->ledtsafeDis->setValidator(new QRegExpValidator(regNum));
	ui->ledtuavNum->setValidator(new QRegExpValidator(regNum));
	ui->ledtPicWidth->setValidator(new QRegExpValidator(regNum));
	ui->ledtFocalDis->setValidator(new QRegExpValidator(regNum));
	ui->ledtHeadingOverlap->setValidator(new QRegExpValidator(regIntDouble));
	ui->ledtSideOverlap->setValidator(new QRegExpValidator(regIntDouble));
	ui->ledtMinAngle->setValidator(new QRegExpValidator(allNumber));
	ui->ledtMaxAngle->setValidator(new QRegExpValidator(allNumber));
	ui->ledtAreaLength->setValidator(new QRegExpValidator(regNum));
	ui->ledtAreaWidth->setValidator(new QRegExpValidator(regNum));
	ui->ledtRealCoverRate->setValidator(new QRegExpValidator(regNum));
	ui->ledtFlyTimeRemain->setValidator(new QRegExpValidator(regNum));
	ui->cbPlanWay->clear();
	QStringList PlanWayList;
	PlanWayList <<tr2(" ")<< tr2("左上") << tr2("左下") << tr2("右上") << tr2("右下");
	ui->cbPlanWay->addItems(PlanWayList);


	m_pIZoneManageService = ICollaborativeSearchActivator::Get().getPIZoneManageService();
	qnzkna::TaskManage::INebulaTaskManageService* pTaskService = ICollaborativeSearchActivator::Get().getPTaskManageService();
	if (nullptr == pTaskService) {
		return;
	}
	ui->ledtActionName->setText(tr2("智慧盐田一体化巡检与管控"));
	ui->ledtActionName->setEnabled(false);

	ui->tedtMissionInfo->clear();

	ui->ledtflyTime->setText(QString::number(flyTime));
	ui->ledtspeed->setText(QString::number(cruiseSpeed));
	ui->ledtUavCruiseHeight->setText(QString::number(cruiseHeight));
	ui->ledtAngularSpeed->setText(QString::number(angularSpeed));
	ui->ledtUpSpeed->setText(QString::number(upSpeed));
	ui->ledtsafeDis->setText(QString::number(safeDistance));
	ui->ledtuavNum->setText(QString::number(uavNum));

	ui->ledtPicLength->setText(QString::number(pic_Length));
	ui->ledtPicWidth->setText(QString::number(pic_width));
	ui->ledtFocalDis->setText(QString::number(focal_dis));
	ui->ledtHeadingOverlap->setText(QString::number(heading_overlap));
	ui->ledtSideOverlap->setText(QString::number(side_overlap));
	ui->letdCoverRate->setText(QString::number(coverRate));

    ui->ledtCurposLon->setText(QString::number(curpos.x, 'f', 8));
	ui->ledtCurposLat->setText(QString::number(curpos.y, 'f', 8));
    ui->ledtCurposHei->setText(QString::number(curpos.z, 'f', 2));

    ui->ledtLeftUpLon->setText(QString::number(leftup.x, 'f', 8));
    ui->ledtLeftUpLat->setText(QString::number(leftup.y, 'f', 8));

    ui->ledtRightDownLon->setText(QString::number(rightdowm.x, 'f', 8));
    ui->ledtRightDownLat->setText(QString::number(rightdowm.y, 'f', 8));

	ui->cbximportanceLevel->setMaxVisibleItems(10);
	ui->cbximportanceLevel->setEditable(true);
	//ui->cbximportanceLevel->setInsertPolicy(QComboBox::InsertAtBottom);

	ui->pbtnGetArea->setCheckable(true);

	ui->pbtnTakeOff->setCheckable(true);

	ui->ledtMinAngle->setText(QString::number(minAngle));
	ui->ledtMaxAngle->setText(QString::number(maxAngle));

	for (int i = 0; i < 20; i++)
	{
		ui->cbximportanceLevel->addItem(QString::number(i + 1));
	}

	connect(ui->ledtUavCruiseHeight, &QLineEdit::textChanged, [&] {
        int maxHeight = calcolateMaxHeight();
        if((int)(ui->ledtUavCruiseHeight->text().toDouble()) > maxHeight){
            LyMessageBox::warning(nullptr, tr2("警告"), tr2("飞行高度过高，请降低高度或调大焦距！当前最大飞行高度为%1").arg(maxHeight), QMessageBox::Yes);
			ui->ledtUavCruiseHeight->setText(QString::number(maxHeight));
        }
		cruiseHeight = ui->ledtUavCruiseHeight->text().toDouble();
	});
	connect(ui->ledtspeed, &QLineEdit::textChanged, [&] {
		cruiseSpeed = ui->ledtspeed->text().toDouble();
	});
	connect(ui->ledtflyTime, &QLineEdit::textChanged, [&] {
		flyTime = ui->ledtflyTime->text().toDouble();
	});
	connect(ui->ledtAngularSpeed, &QLineEdit::textChanged, [&] {
		angularSpeed = ui->ledtAngularSpeed->text().toDouble();
	});
	connect(ui->ledtUpSpeed, &QLineEdit::textChanged, [&] {
		upSpeed = ui->ledtUpSpeed->text().toDouble();
	});
	connect(ui->ledtsafeDis, &QLineEdit::textChanged, [&] {
		safeDistance = ui->ledtsafeDis->text().toDouble();
	});
	connect(ui->ledtuavNum, &QLineEdit::textChanged, [&] {
		uavNum = ui->ledtuavNum->text().toDouble();
	});

	connect(ui->ledtPicLength, &QLineEdit::textChanged, [&] {
		pic_Length = ui->ledtPicLength->text().toDouble();
	});
	connect(ui->ledtPicWidth, &QLineEdit::textChanged, [&] {
		pic_width = ui->ledtPicWidth->text().toDouble();
	});
	connect(ui->ledtFocalDis, &QLineEdit::textChanged, [&] {
		focal_dis = ui->ledtFocalDis->text().toDouble();
	});
	connect(ui->ledtHeadingOverlap, &QLineEdit::textChanged, [&] {
		heading_overlap = ui->ledtHeadingOverlap->text().toDouble();
	});
	connect(ui->ledtSideOverlap, &QLineEdit::textChanged, [&] {
		side_overlap = ui->ledtSideOverlap->text().toDouble();
	});
    connect(ui->ledtCurposLon, &QLineEdit::textChanged, [&] {
        double lon_curpos = QString(ui->ledtCurposLon->text()).toDouble();
        double lat_curpos = QString(ui->ledtCurposLat->text()).toDouble();
        short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon_curpos, lat_curpos);
        if (elevationTemp < -10000)
        {
            elevationTemp = 0;
        }
        double m_alt = elevationTemp;
        ui->ledtCurposHei->setText(QString::number(m_alt, 'f', 2));
    });
    connect(ui->ledtCurposLat, &QLineEdit::textChanged, [&] {
        double lon_curpos = QString(ui->ledtCurposLon->text()).toDouble();
        double lat_curpos = QString(ui->ledtCurposLat->text()).toDouble();
        short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon_curpos, lat_curpos);
        if (elevationTemp < -10000)
        {
            elevationTemp = 0;
        }
        double m_alt = elevationTemp;
        ui->ledtCurposHei->setText(QString::number(m_alt, 'f', 2));
    });

	connect(ui->pbtnGetArea,&QPushButton::clicked, this, &RoutePlanningDialog::on_push_GetArea_clicked);
	connect(ui->pbtnAddnoFlyArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushAddNoFly_clicked);
	connect(ui->pbtnDeletenoflyArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushDelNoFly_clicked);
	connect(ui->pbtnTakeOff, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushStartPoint_clicked);
	connect(ui->pbtnAddImpArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushAddImpArea_clicked);
	connect(ui->pbtnImportImpArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushImportImpArea_clicked);
	connect(ui->pbtnDelImpArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushDelImpArea_clicked);
	connect(ui->pbtnImportnoFlyArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushImportNoFly_clicked);
	connect(ui->cbximportanceLevel, &QComboBox::currentTextChanged, this, &RoutePlanningDialog::on_cbximportanceLevel_Changed);
	connect(ui->pbtnClearAll, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushClearAll);
	connect(m_routePlanningPlayDialog, &RoutePlanningDisplayDialog::updatePosition, this, &RoutePlanningDialog::drawcoverslot);
	connect(ui->pbtnSaveImpArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushSaveImpArea_clicked);
	connect(ui->pbtnSavenoFlyArea, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushSaveNoFly_clicked);
	connect(ui->pbthArticModify, &QPushButton::clicked, this, &RoutePlanningDialog::on_pushArticModify_clicked);

	connect(ui->pbtnLoadMission, &QPushButton::clicked, this, &RoutePlanningDialog::on_pbtnLoadMission_Clicked);
	connect(ui->pbtnAirLineUpLoad, &QPushButton::clicked, this, &RoutePlanningDialog::on_pbtnRouteUpLoad_clicked);
	connect(ui->ledtMinAngle, &QLineEdit::editingFinished, this, &RoutePlanningDialog::minAngleText_isChanged);
	connect(ui->ledtMaxAngle, &QLineEdit::editingFinished, this, &RoutePlanningDialog::maxAngleText_isChanged);

	ui->pbtnLoadMission->setEnabled(true);
// 	ui->pbtnTakeOff->setEnabled(false);
// 	ui->pbtnGetArea->setEnabled(false);
// 	ui->pbtnKmeans->setEnabled(false);
// 	ui->pbtnPlanning->setEnabled(false);
// 	ui->pbthArticModify->setEnabled(false);
// 	ui->pbtnAirLineUpLoad->setEnabled(false);
// 	ui->pbthDisplay->setEnabled(false);
// 	ui->pbtnClearAll->setEnabled(false);

//	ui->pbthArticModify->hide();
}

void RoutePlanningDialog::Update(qnzkna::TaskManage::INebulaTaskManageService* sub)
{
	UpdateActionData();
}

void RoutePlanningDialog::UpdateActionData()
{
}

double RoutePlanningDialog::getGridLength(double height, double frame, double focal, double heading_ratio)
{
	focal = focal == 0 ? 24 : focal;

	focal = focal / 1000;
	frame = frame / 1000;

	double dis = frame * height / focal;

	double fold_dis = dis * heading_ratio;

	dis = dis - fold_dis;

	return dis;
}

double RoutePlanningDialog::getGridWidth(double height, double frame, double focal, double side_ratio)
{
	focal = focal == 0 ? 24 : focal;

	focal = focal / 1000;
	frame = frame / 1000;

	double dis = frame * height / focal;

	double fold_dis = dis * side_ratio;

	dis = dis - fold_dis;

	return dis;
}

double RoutePlanningDialog::calcolateMaxHeight()
{
	double maxHeight = focal_dis * GSD_max / (pic_Length / p_Length);
	return maxHeight;
}

double RoutePlanningDialog::calcolateGrid()
{

	int maxHeight = calcolateMaxHeight();
	if (maxHeight < cruiseHeight)
    {
		QMessageBox::warning(this, u8"警告", u8"飞行高度过高，请降低高度或调大焦距");
		return -1;
	}
	double length = cruiseHeight * pic_Length / focal_dis * (1 - side_overlap);
	return (int)length;

}

std::vector<casic_vector2d> RoutePlanningDialog::calcTerrianObstacle(casic_vector2d start, casic_vector2d goal)
{
	std::vector<casic_vector2d>obstacleVec;

	casic_vector2d leftdown = casic_vector2d(start.x, goal.y);
	casic_vector2d rightup = casic_vector2d(goal.x, start.y);

	QGeoCoordinate point1(start.x, start.y);
	QGeoCoordinate point3(leftdown.x, leftdown.y);
	QGeoCoordinate point4(rightup.x, rightup.y);
	QGeoCoordinate point2(goal.x, goal.y);

	double mapSize = point1.distanceTo(point4);
	double gridSize = point1.distanceTo(point3);

	double ddis = point1.distanceTo(point2);

	double  dis_step = ddis / 40.0;
	double m_lonPointNum = mapSize / (dis_step)+1;
	double m_latPointNum = gridSize / (dis_step)+1;

	double delta_lon = (goal.x - start.x) / m_lonPointNum;
	double delta_lat = (goal.y - start.y) / m_latPointNum;

	for (int i = 0; i < m_lonPointNum; ++i)
	{
		std::vector<std::vector<casic_vector2d>> tmp;
		for (int j = 0; j < m_latPointNum; ++j)
		{
			casic_vector2d pos_lonlat = new casic_vector2d(start.x + delta_lon * i, start.y + delta_lat * j);
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(pos_lonlat.x, pos_lonlat.y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			double m_alt = elevationTemp;

			if (m_alt >= cruiseHeight)
			{
				obstacleVec.push_back(casic_vector2d(pos_lonlat.x, pos_lonlat.y));
			}
		}

	}
	return obstacleVec;
}

void RoutePlanningDialog::setAllObstacle(std::vector<std::vector<casic_vector2d>> no_flyAreaList, casic_vector2d start, casic_vector2d goal)
{
	std::vector<casic_vector2d>terrianAreaList = calcTerrianObstacle(start, goal);
	allObstacleVec.push_back(terrianAreaList);
	for (auto item : no_flyAreaList)
	{
		allObstacleVec.push_back(item);
	}
}

void RoutePlanningDialog::stagingPathPlanning(int uid, casic_vector3d start, casic_vector3d goal, double safedis, std::map<int, std::vector<casic_vector3d>>& stagePointPlanMap)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.x, start.y, start.z);
	QGeoCoordinate point2(sg1.x, sg1.y, sg1.z);
	QGeoCoordinate point3(goal.x, goal.y, goal.z);
	QGeoCoordinate point4(sg2.x, sg2.y, sg2.z);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = goal.z;

	AStar astar;
	astar.ObstacleUAVs = ObstacleVec1;
	std::vector<casic_vector3d>posVec;
	astar.init(length_, width_, alt_,safedis, start, goal);
	astar.pathPlan(posVec);
	for (auto pos_itor = posVec.rbegin(); pos_itor != posVec.rend(); ++pos_itor)
	{
		ObstacleVec1.push_back(*pos_itor);
	}

	std::pair<int, std::vector<casic_vector3d>> mpair = std::make_pair(uid, posVec);
	stagingPointList.insert(mpair);
	stagePointPlanMap.insert(mpair);
}

void RoutePlanningDialog::drawMarkRect(QPainter & painter, IProj * pProj)
{
	QPen targetPen(Qt::blue, 3);

	QPointF jwdLeftUp = targetLeftUp;
	QPointF jwdRightDown = targetRightDown;
	QPoint pointLeftUp = pProj->Jwd2Screen(jwdLeftUp.x(), jwdLeftUp.y());
	QPoint pointRightDown = pProj->Jwd2Screen(jwdRightDown.x(), jwdRightDown.y());
	double width = pointRightDown.x() - pointLeftUp.x();
	double heigth = pointRightDown.y() - pointLeftUp.y();

	QRect targetRect(pointLeftUp.x(), pointLeftUp.y(), width, heigth);
	painter.setPen(targetPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(targetRect);
}

void RoutePlanningDialog::initDialog()
{
}

void RoutePlanningDialog::setMarkPloy()
{
	if (!targetLeftUp.isNull() && targetRightDown.isNull())
	{
		QString lon = QString::number(targetLeftUp.x(), 'f', 8);
		QString lat = QString::number(targetLeftUp.y(), 'f', 8);
		ui->ledtLeftUpLat->setText(lat);
		ui->ledtLeftUpLon->setText(lon);
	}
	if (!targetRightDown.isNull())
	{

		double lon1 = targetLeftUp.x();
		double lon2 = targetRightDown.x();
		double lat1 = targetLeftUp.y();
		double lat2 = targetRightDown.y();
		QPointF jwdUp(lon1 > lon2 ? lon2 : lon1, lat1 > lat2 ? lat1 : lat2);
		QPointF jwdDown(lon1 > lon2 ? lon1 : lon2, lat1 > lat2 ? lat2 : lat1);

		QString Downlon = QString::number(jwdDown.x(), 'f', 8);
		QString Downlat = QString::number(jwdDown.y(), 'f', 8);
		QString uplon = QString::number(jwdUp.x(), 'f', 8);
		QString uplat = QString::number(jwdUp.y(), 'f', 8);
		ui->ledtLeftUpLat->setText(uplat);
		ui->ledtLeftUpLon->setText(uplon);
		ui->ledtRightDownLat->setText(Downlat);
		ui->ledtRightDownLon->setText(Downlon);

// 		if (ICollaborativeSearchActivator::Get().getPIMarkManageService() != nullptr)
// 		{
// 			ICollaborativeSearchActivator::Get().getPIMarkManageService()->SetThreatenSequencePosition((lon1 + lon2) / 2.0, (lat1+lat2) / 2, 0);
// 			LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900501, nullptr);
// 		}

	}

    QString lon = QString::number(curpos[0], 'f', 8);
    QString lat = QString::number(curpos[1], 'f', 8);
    QString hei = QString::number(curpos[2], 'f', 2);
	ui->ledtCurposLon->setText(lon);
	ui->ledtCurposLat->setText(lat);
    ui->ledtCurposHei->setText(hei);

// 	if (ICollaborativeSearchActivator::Get().getPIMarkManageService() != nullptr)
// 	{
// 		ICollaborativeSearchActivator::Get().getPIMarkManageService()->SetThreatenSequenceTakeOffpoint(curpos[0], curpos[1], curpos[2]);
// 		LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900502, nullptr);
// 	}
}

QString RoutePlanningDialog::radian2dms(double radian)
{
	double tempRadian = radian;
	int d = (int)tempRadian;
	double tempM = (radian - d) * 60;
	int m = (int)tempM;
	double s = (tempM - m) * 60;

	QString str;
	str.clear();
	str = QString::number(d) + u8"° " + QString::number(m) + "' " + QString::number(s) + "\"";
	return str;
}

void RoutePlanningDialog::calcolateGridLinePoints()
{
	gridsidePoints.clear();
	gridLinesPoints.clear();
	double gridWidth = calcolateGrid();
	casic_vector3d leftup(targetLeftUp.x(), targetLeftUp.y(), 0);
	casic_vector3d rightdown(targetRightDown.x(), targetRightDown.y(), 0);
	sortGridSide(leftup, rightdown, curpos);

	if (gridSidePoint.size() < 4)
	{
		return;
	}

	QGeoCoordinate min1(gridSidePoint[0][1], gridSidePoint[0][0]);
	QGeoCoordinate min2(gridSidePoint[1][1], gridSidePoint[1][0]);
	QGeoCoordinate min1Min(gridSidePoint[3][1], gridSidePoint[3][0]);
	QGeoCoordinate min2Min(gridSidePoint[2][1], gridSidePoint[2][0]);

	double min1Dismin2 = min1.distanceTo(min2);
	double min1DisMin = min1.distanceTo(min1Min);

	double min1Anglemin2 = min1.azimuthTo(min2);
	double min1AngleMin = min1.azimuthTo(min1Min);
	double AngleLine = (min1AngleMin + min1Anglemin2) / 2;

	int widthGrids = min1DisMin / gridWidth;
	int heightGrids = min1Dismin2 / gridWidth;

	QGeoCoordinate min1toMin = min1.atDistanceAndAzimuth((widthGrids + 1) * gridWidth, min1AngleMin);
	QGeoCoordinate min1toMin2 = min1.atDistanceAndAzimuth((heightGrids + 1) * gridWidth, min1Anglemin2);
	QGeoCoordinate min2ToMin = min2.atDistanceAndAzimuth((widthGrids + 1) * gridWidth, min1AngleMin);
	int GridMin1Min = min1.distanceTo(min1toMin);
	int GridMin1Min2 = min1.distanceTo(min1toMin2);

	bool isOnetoMin = false;
	int indexMin = -1;
	for (double lonDis = 0; lonDis < min1DisMin + gridWidth; lonDis += gridWidth)
	{
		QGeoCoordinate point = min1.atDistanceAndAzimuth(lonDis, min1AngleMin);

		QPointF pointStart(QPointF(point.longitude(), point.latitude()));
		QPointF pointStop;

		QGeoCoordinate stop1(point.latitude(), min1toMin2.longitude());
		QGeoCoordinate stop2(min1toMin2.latitude(), point.longitude());
		std::vector<QGeoCoordinate> stops;
		stops.clear();
		stops.push_back(stop1);
		stops.push_back(stop2);
		int startStopDis = point.distanceTo(stop1);

		if (!isOnetoMin)
		{
			if (fabs(startStopDis - GridMin1Min2) < 10)
			{
				indexMin = 0;
			}
			else
			{
				indexMin = 1;
			}
			isOnetoMin = true;
		}
		if (indexMin != -1)
		{
			QPointF Stop(stops[indexMin].longitude(), stops[indexMin].latitude());
			pointStop = Stop;
		}
		else
		{
			continue;
		}
		gridLinesPoints.push_back(pointStart);
		gridLinesPoints.push_back(pointStop);
	}

	bool isOnetoMin2 = false;
	int indexMin2 = -1;
	for (double latDis = 0; latDis < min1Dismin2 + gridWidth; latDis += gridWidth)
	{
		QGeoCoordinate point = min1.atDistanceAndAzimuth(latDis, min1Anglemin2);

		QPointF pointStart(QPointF(point.longitude(), point.latitude()));
		QPointF pointStop;

		QGeoCoordinate stop1(point.latitude(), min1toMin.longitude());
		QGeoCoordinate stop2(min1toMin.latitude(), point.longitude());

		std::vector<QGeoCoordinate> stops;
		stops.clear();
		stops.push_back(stop1);
		stops.push_back(stop2);
		int startStopDis = point.distanceTo(stop1);
		if (!isOnetoMin2)
		{
			if (fabs(startStopDis - GridMin1Min) < 10)
			{
				indexMin2 = 0;
			}
			else
			{
				indexMin2 = 1;
			}
			isOnetoMin2 = true;
		}
		if (indexMin2 != -1)
		{
			QPointF Stop(stops[indexMin2].longitude(), stops[indexMin2].latitude());
			pointStop = Stop;
		}
		else
		{
			continue;
		}

		gridLinesPoints.push_back(pointStart);
		gridLinesPoints.push_back(pointStop);
	}

	gridsidePoints.push_back(casic_vector3d(min1.longitude(), min1.latitude(), min1.altitude()));
	gridsidePoints.push_back(casic_vector3d(min2.longitude(), min2.latitude(), min2.altitude()));
	gridsidePoints.push_back(casic_vector3d(min2ToMin.longitude(), min2ToMin.latitude(), min2ToMin.altitude()));
	gridsidePoints.push_back(casic_vector3d(min1toMin.longitude(), min1toMin.latitude(), min1toMin.altitude()));
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DMarkArea();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DGrid();

	nearlyLenSide.clear();
	nearlyLenSide.push_back(casic_vector3d(min1.longitude(), min1.latitude(), min1.altitude()));
	double dis1 = min1.distanceTo(min2);
	double dis2 = min1.distanceTo(min1toMin);
	if (dis1 > dis2)
	{
		nearlyLenSide.push_back(casic_vector3d(min2.longitude(), min2.latitude(), min2.altitude()));
		nearlyLenSide.push_back(casic_vector3d(min1toMin.longitude(), min1toMin.latitude(), min1toMin.altitude()));
	}
	else
	{
		nearlyLenSide.push_back(casic_vector3d(min1toMin.longitude(), min1toMin.latitude(), min1toMin.altitude()));
		nearlyLenSide.push_back(casic_vector3d(min2.longitude(), min2.latitude(), min2.altitude()));
	}
	nearlyLenSide.push_back(casic_vector3d(min2ToMin.longitude(), min2ToMin.latitude(), min2ToMin.altitude()));

}

void RoutePlanningDialog::sortGridSide(casic_vector3d leftUp, casic_vector3d rightDown, casic_vector3d startPoint)
{
	gridSidePoint.clear();

	double lon1 = leftUp[0];
	double lon2 = rightDown[0];
	double lat1 = leftUp[1];
	double lat2 = rightDown[1];

	QGeoCoordinate GeoPoint1(lat1, lon1);
	QGeoCoordinate GeoPoint2(lat1, lon2);
	QGeoCoordinate GeoPoint3(lat2, lon1);
	QGeoCoordinate GeoPoint4(lat2, lon2);
	QGeoCoordinate GeoStartPoint(startPoint[1], startPoint[0]);

	std::vector<QGeoCoordinate> GeoSidePoints{
		GeoPoint1, GeoPoint2, GeoPoint3, GeoPoint4 };

	std::map<double, QGeoCoordinate> minGeoPoints;
	minGeoPoints.clear();

	for (int i = 0; i < GeoSidePoints.size(); i++)
	{
		double dis = GeoStartPoint.distanceTo(GeoSidePoints[i]);
		minGeoPoints[dis] = GeoSidePoints[i];
	}

	auto sidePointMinItor = minGeoPoints.begin();
	if (sidePointMinItor == minGeoPoints.end())
	{
		return;
	}
	QGeoCoordinate minGeoPoint = sidePointMinItor->second;
	casic_vector3d minPoint(minGeoPoint.longitude(), minGeoPoint.latitude(), 0);
	gridSidePoint.push_back(minPoint);

	sidePointMinItor++;

	std::map<double, QGeoCoordinate> pointToMinPoint;
	pointToMinPoint.clear();
	while (sidePointMinItor != minGeoPoints.end())
	{
		double dis = minGeoPoint.distanceTo(sidePointMinItor->second);
		pointToMinPoint[dis] = sidePointMinItor->second;
		sidePointMinItor++;
	}
	minGeoPoints.clear();

	casic_vector3d targetCenterPoint((lon1 + lon2) / 2, (lat1 + lat2) / 2, (leftUp[2] + rightDown[2]) / 2);
	casic_vector3d startPoint3d(startPoint[0], startPoint[1], 0);

	if (pointToMinPoint.empty())
	{
		return;
	}
	auto pointToMinItor = pointToMinPoint.begin();
	casic_vector3d min1Point(pointToMinItor->second.longitude(), pointToMinItor->second.latitude(), 0);
	pointToMinItor++;
	casic_vector3d min2Point(pointToMinItor->second.longitude(), pointToMinItor->second.latitude(), 0);
	pointToMinItor++;
	casic_vector3d min3Point(pointToMinItor->second.longitude(), pointToMinItor->second.latitude(), 0);
	AreaSearchAlgorithm m_areaSearch;
	if (m_areaSearch.is_lineA_intersect_lineB(targetCenterPoint, startPoint3d, minPoint, min1Point))
	{
		gridSidePoint.push_back(min1Point);
		gridSidePoint.push_back(min3Point);
		gridSidePoint.push_back(min2Point);
	}
	else
	{
		gridSidePoint.push_back(min2Point);
		gridSidePoint.push_back(min3Point);
		gridSidePoint.push_back(min1Point);
	}
}

void RoutePlanningDialog::drawGrid(QPainter & painter, IProj * pProj)
{

	double mapScale = pProj->GetMapScale();
	QPen pen(Qt::black, 1);
	if (gridLinesPoints.size() > 0)
	{
		for (int pointIndex = 0; pointIndex < gridLinesPoints.size(); pointIndex += 2)
		{
			QPoint point1 = pProj->Jwd2Screen(gridLinesPoints[pointIndex].x(), gridLinesPoints[pointIndex].y());
			QPoint point2 = pProj->Jwd2Screen(gridLinesPoints[pointIndex + 1].x(), gridLinesPoints[pointIndex + 1].y());
			QLine line(point1, point2);

			painter.setPen(pen);
			painter.drawLine(line);
		}
	}
}

void RoutePlanningDialog::addNoFlyZone(QPointF point)
{
	oneNoFlyZonePoint.push_back(point);
}

void RoutePlanningDialog::drawNoFlayZone(QPainter & painter, IProj * pProj)
{
	double mapScale = pProj->GetMapScale();
	QPen pen(Qt::red, 1);
	if (oneNoFlyZonePoint.size() > 0)
	{
		QVector<QPoint> points;
		points.clear();
		for (int pointIndex = 0; pointIndex < oneNoFlyZonePoint.size(); pointIndex++)
		{
			QPoint point1 = pProj->Jwd2Screen(oneNoFlyZonePoint[pointIndex].x(),
				oneNoFlyZonePoint[pointIndex].y());
			points.push_back(point1);
		}
		QPolygon poly(points);
		painter.setPen(pen);
		painter.drawPolygon(poly);
	}
	for (auto ZonePoints : NoFlyZonePoints)
	{
		QVector<QPoint> points;
		points.clear();
		for (int pointIndex = 0; pointIndex < ZonePoints.size(); pointIndex++)
		{
			QPoint point1 = pProj->Jwd2Screen(ZonePoints[pointIndex].x(),
				ZonePoints[pointIndex].y());
			points.push_back(point1);
		}
		QPolygon poly(points);
		painter.setPen(pen);
		painter.drawPolygon(poly);
	}
}

bool RoutePlanningDialog::is_point_in_zone(const QPointF point, const std::vector<QPointF> polyPoints)
{
	int count = polyPoints.size();
	int nCross = 0;
	for (int i = 0; i < count; i++)
	{
		QPointF p1 = polyPoints[i];
		QPointF p2 = polyPoints[(i + 1) % count];

		if (p1.y() == p2.y())
			continue;

		if (point.y() < min(p1.y(), p2.y()))
			continue;

		if (point.y() >= max(p1.y(), p2.y()))
			continue;

		double x = (point.y() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();

		if (x > point.x())
			nCross++;
	}

	return (nCross % 2 == 1);
}

void RoutePlanningDialog::sparsePath(std::map<int, std::vector<casic_vector3d>>& AreaSearchPointMap)
{
	double angleDiffMax = 10;

	auto PathItor = AreaSearchPointMap.begin();
	for (PathItor; PathItor != AreaSearchPointMap.end(); PathItor++)
	{
		int planIndex = PathItor->first;
		std::vector<casic_vector3d> startreturnpoint;
		if (searchStartReturnPointMap.size() > 0)
			startreturnpoint = searchStartReturnPointMap[planIndex];
		std::vector<casic_vector3d>* tempPath = &(PathItor->second);
		auto pointItor = tempPath->begin();
		QGeoCoordinate point1(pointItor->y, pointItor->x, pointItor->z);
		pointItor++;

		if (pointItor == tempPath->end())
		{
			break;
		}

		auto point2Itor = pointItor;
		QGeoCoordinate point2(pointItor->y, pointItor->x, pointItor->z);
		pointItor++;
		for (pointItor; pointItor != tempPath->end(); )
		{
			bool is_startreturnPoint = false;
			for (auto& poi : startreturnpoint) {
				if (std::fabs(point2Itor->x - poi.x) <= 1e-6 && std::fabs(point2Itor->y - poi.y) <= 1e-6)
					is_startreturnPoint = true;
			}

			QGeoCoordinate point3(pointItor->y, pointItor->x, pointItor->z);

			casic_vector3d nowPoint = *pointItor;
			bool isEqual = false;
			for (int stagIndex = 0; stagIndex < flyRoadPointsVec.size(); stagIndex++)
			{
				QGeoCoordinate newPointGeo(nowPoint.y, nowPoint.x);
				QGeoCoordinate stagingPoint(flyRoadPointsVec[stagIndex].y, flyRoadPointsVec[stagIndex].x);
				double dis = newPointGeo.distanceTo(stagingPoint);
				if (nowPoint.x == flyRoadPointsVec[stagIndex].x && nowPoint.y == flyRoadPointsVec[stagIndex].y)
				{
					point1 = point2;
					point2 = point3;
					point2Itor = pointItor;
					pointItor++;
					isEqual = true;
					break;
				}
			}
			if (isEqual)
			{
				continue;
			}

			double angle1_2 = point1.azimuthTo(point2);
			double angle2_3 = point2.azimuthTo(point3);
			double angleDiff = fabs(angle1_2 - angle2_3);
			if (angleDiff < angleDiffMax)
			{
				if (!is_startreturnPoint)
					pointItor = tempPath->erase(point2Itor);
				point2 = point3;
				point2Itor = pointItor;
				pointItor++;
			}
			else
			{
				point1 = point2;
				point2 = point3;
				point2Itor = pointItor;
				pointItor++;
			}
		}
	}
}

void RoutePlanningDialog::drawStartPoint(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::green, 4);
	QPoint startPoint = pProj->Jwd2Screen(curpos[0], curpos[1]);
	painter.setPen(pen);
	painter.drawPoint(startPoint);
	painter.drawText(startPoint, QString::fromLocal8Bit("起降区"));

	casic_vector3d pos = casic_vector3d(curpos[0], curpos[1], 0);
	std::vector<casic_vector3d>tmpVec;
	QGeoCoordinate mpos(pos.y, pos.x);

	casic_vector3d tmppos;
	QGeoCoordinate tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth *sqrt(5) / 2, 58.285);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 121.715);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 238.285);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 301.715);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	QVector<QPoint> points;
	for (auto pos1 : tmpVec)
	{
		QPoint mpoint = pProj->Jwd2Screen(pos1[0], pos1[1]);
		points.push_back(mpoint);
	}
	painter.setPen(QPen(Qt::lightGray, 2));

	QColor colorTemp(200, 200, 200, 60);
	painter.setBrush(QBrush(colorTemp));
	QPolygon poly(points);
	painter.drawPolygon(poly);

	CZoneInfo area;

	draw3DTakfOffLandArea(curpos[0], curpos[1], 10);
}

void RoutePlanningDialog::transToObstacleArea(const std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> GradMap)
{

	std::vector<QPoint> motion = {
		QPoint( 0, 1),
		QPoint( 1, 1),
		QPoint( 1, 0),
		QPoint( 1, -1),
		QPoint( 0, -1),
		QPoint( -1, -1),
		QPoint( -1, 0),
		QPoint( -1, 1)
	};

	std::map<int, QPoint> obstasMap;
	obstasMap.clear();
	for (int i = 0; i < ObstactAreaXY.size(); i++)
	{
		obstasMap[ObstactAreaXY[i].x() * 100 + ObstactAreaXY[i].y()] = ObstactAreaXY[i];
	}

	std::map<int, QPoint> havedMap;
	havedMap.clear();

	std::vector<std::vector<QPoint>> obsAreas;
	obsAreas.clear();
	std::vector<QPoint> obsArea;
	obsArea.clear();
	std::vector<QPoint> singlePoint;
	singlePoint.clear();

	auto obstasItor = obstasMap.begin();
	while (!obstasMap.empty())
	{
		if (obstasItor == obstasMap.end())
		{
			obstasItor = obstasMap.begin();
		}
		auto pointXY = obstasItor->second;
		int xIndex = pointXY.x();
		int yIndex = pointXY.y();

		std::vector<QPoint> nextPoints;
		nextPoints.clear();
		int sidePoints = 0;
		for (int i = 0; i < motion.size(); i++)
		{
			QPoint tmpPoint = pointXY + motion[i];
			if (tmpPoint.x() < 0 || tmpPoint.x() > GradMap.size() - 1 || tmpPoint.y() < 0 || tmpPoint.y() > GradMap[0].size() - 1)
			{
				continue;
			}
			sidePoints++;
			auto tmpPlanNode = GradMap[tmpPoint.x()][tmpPoint.y()];
			bool isObst = tmpPlanNode->isObstacle();
			if (isObst)
			{
				nextPoints.push_back(tmpPoint);
			}
		}

		if (nextPoints.size() == 8)
		{
			obstasItor = obstasMap.erase(obstasItor);
			continue;
		}
		else if (nextPoints.size() == 0)
		{
			singlePoint.push_back(pointXY);
			obsAreas.push_back(singlePoint);
			singlePoint.clear();
			obstasItor = obstasMap.erase(obstasItor);
		}
		else
		{
			int haveCount = 0;
			for (int i = 0; i < nextPoints.size(); i++)
			{
				QPoint nextPoint = nextPoints[i];
				auto isHaveItor = havedMap.find(nextPoint.x() * 100 + nextPoint.y());
				if (isHaveItor == havedMap.end())
				{
					std::vector<QPoint> sidePoints;
					sidePoints.clear();
					for (int i = 0; i < motion.size(); i++)
					{
						QPoint tmpPoint = nextPoint + motion[i];
						if (tmpPoint.x() < 0 || tmpPoint.x() > GradMap.size() - 1 || tmpPoint.y() < 0 || tmpPoint.y() > GradMap[0].size() - 1)
						{
							continue;
						}
						auto tmpPlanNode = GradMap[tmpPoint.x()][tmpPoint.y()];
						bool isObst = tmpPlanNode->isObstacle();
						if (isObst)
						{
							sidePoints.push_back(tmpPoint);
						}
					}
					if (sidePoints.size() == 8)
					{
						haveCount++;
						auto isDel = obstasMap.find(nextPoint.x() * 100 + nextPoint.y());
						if (isDel != obstasMap.end())
						{
							isDel = obstasMap.erase(isDel);
						}

						continue;
					}

					obsArea.push_back(pointXY);
					havedMap[pointXY.x() * 100 + pointXY.y()] = pointXY;
					obstasItor = obstasMap.erase(obstasItor);
					obstasItor = obstasMap.find(nextPoint.x() * 100 + nextPoint.y());
					break;
				}
				else
				{
					haveCount++;
				}
			}
			if (haveCount == nextPoints.size())
			{
				obsArea.push_back(pointXY);
				obsAreas.push_back(obsArea);
				obsArea.clear();
				havedMap.clear();
				obstasItor = obstasMap.erase(obstasItor);
			}
		}

	}

	for (int i = 0; i < obsAreas.size(); i++)
	{
		ObstacleAreaPoints.clear();
		auto area = obsAreas[i];
		std::vector<QPointF> areaLL;
		areaLL.clear();
		for (int j = 0; j < area.size(); j++)
		{
			auto tmpPlanNode = GradMap[area[j].x()][area[j].y()];
			QPointF pointLL(tmpPlanNode->coordinateX(), tmpPlanNode->coordinateY());
			areaLL.push_back(pointLL);
		}
		ObstacleAreaPoints.push_back(areaLL);
	}
}

void RoutePlanningDialog::drawObstacleArea(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 15);
	painter.setPen(pen);
	for (int i = 0; i < obstacleArea.size(); i++)
	{
		QPoint point = pProj->Jwd2Screen(obstacleArea[i][0], obstacleArea[i][1]);
		painter.drawPoint(point);
	}
}

void RoutePlanningDialog::drawObstacleZone(QPainter & painter, IProj * pProj)
{
	double mapScale = pProj->GetMapScale();
	QPen pen(Qt::red, 1);

	for (auto ZonePoints : ObstacleAreaPoints)
	{
		if (ZonePoints.size() < 3)
		{
			QPen pen(Qt::red, 15);
			painter.setPen(pen);
			for (int i = 0; i < ZonePoints.size(); i++)
			{
				QPoint point = pProj->Jwd2Screen(ZonePoints[i].x(), ZonePoints[i].y());
				painter.drawPoint(point);
			}
		}

		QVector<QPoint> points;
		points.clear();
		for (int pointIndex = 0; pointIndex < ZonePoints.size(); pointIndex++)
		{
			QPoint point1 = pProj->Jwd2Screen(ZonePoints[pointIndex].x(),
				ZonePoints[pointIndex].y());
			points.push_back(point1);
		}
		QPolygon poly(points);
		painter.setPen(pen);
		painter.drawPolygon(poly);
	}
}

std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> RoutePlanningDialog::createGridsNode()
{
	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> GridsMap;

	GridsMap.clear();
	obstacleArea.clear();

	double gridWidth = calcolateGrid();
	cameraGridWidth = gridWidth;

	if (gridSidePoint.size() < 4)
	{
		return GridsMap;
	}

	QGeoCoordinate min1(gridSidePoint[0][1], gridSidePoint[0][0]);
	QGeoCoordinate min2(gridSidePoint[1][1], gridSidePoint[1][0]);
	QGeoCoordinate min1Min(gridSidePoint[3][1], gridSidePoint[3][0]);
	QGeoCoordinate min2Min(gridSidePoint[2][1], gridSidePoint[2][0]);

    double jueduihei =  QString(ui->ledtCurposHei->text()).toDouble() + cruiseHeight;

	double min1Dismin2 = min1.distanceTo(min2);
	double min1DisMin = min1.distanceTo(min1Min);

	areaLength = min1Dismin2;
	areaWidth = min1DisMin;

	double min1Anglemin2 = min1.azimuthTo(min2);
	double min1AngleMin = min1.azimuthTo(min1Min);
	if (fabs(min1AngleMin - 0) < 1 && fabs(min1Anglemin2 - 270) < 1)
	{
		min1AngleMin += 360.0;
	}
	if (fabs(min1Anglemin2 - 0) < 1 && fabs(min1AngleMin - 270) < 1)
	{
		min1Anglemin2 += 360.0;
	}
	double AngleLine = (min1AngleMin + min1Anglemin2) / 2;

	int i = 0;
	for (int min1Tomin2Dis = 0; min1Tomin2Dis < min1Dismin2; min1Tomin2Dis += gridWidth)
	{
		QGeoCoordinate pointMin1To = min1.atDistanceAndAzimuth(min1Tomin2Dis, min1Anglemin2);
		std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>> pointsMin1toMin;
		pointsMin1toMin.clear();
		int j = 0;
		for (int min1ToMinDis = 0; min1ToMinDis < min1DisMin; min1ToMinDis += gridWidth)
		{
			QGeoCoordinate pointMinTo = pointMin1To.atDistanceAndAzimuth(min1ToMinDis, min1AngleMin);
			QGeoCoordinate gridCenterPoint = pointMinTo.atDistanceAndAzimuth(gridWidth * sqrt(2) / 2,
				AngleLine);
			auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), i, j);
			m_node->setAreaAttribute(0);
			pointsMin1toMin.push_back(m_node);

			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(gridCenterPoint.longitude(), gridCenterPoint.latitude());
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			double m_alt = elevationTemp;
            if (m_alt > jueduihei)
			{
				obstacleArea.push_back(casic_vector2d(gridCenterPoint.longitude(), gridCenterPoint.latitude()));
				pointsMin1toMin[j]->setObstacle();
				ObstactAreaXY.push_back(QPoint(i, j));
			}
			j++;
		}
		GridsMap.push_back(pointsMin1toMin);
		i++;
	}

	GridPointMap = GridsMap;
	keyXYPoint.clear();

	setKeyAreaNoTrans();
	setKeyAreaXyHcost();

	ui->ledtAreaLength->setText(QString::number(min1DisMin));
	ui->ledtAreaWidth->setText(QString::number(areaWidth));
	ui->ledtgridWidth->setText(QString::number(cameraGridWidth));

	return GridsMap;
}

void RoutePlanningDialog::setpbtnText()
{
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
}

void RoutePlanningDialog::draw3DCoverCube()
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;
	QColor colorTemp(255, 0, 0, 90);
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube.size() > 0)
	{
		static int index = 0;
		auto tmpVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube;
		for (auto mpair : tmpVec)
		{
			std::vector<double> pdLon;
			pdLon.clear();
			std::vector<double> pdLat;
			pdLat.clear();
			std::vector<double> pdHeight;
			pdHeight.clear();
			casic_vector3d pos = mpair.second;
			auto endPoints = getEndPoints(pos, gridwidth);

			pdLon.push_back(pos.x);
			pdLat.push_back(pos.y);
			pdHeight.push_back(100);
			for (auto pos : endPoints)
			{
				pdLon.push_back(pos.x);
				pdLat.push_back(pos.y);
				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
				(pos.x, pos.y);
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				pdHeight.push_back(elevationTemp);
			}

			double azimth = 180;
			if (index == 0)
			{
				lastPos = pos;
			}
			else
			{
				QGeoCoordinate nextPos(pos.y, pos.x);
				QGeoCoordinate currPos(lastPos.y, lastPos.x);
				azimth = currPos.azimuthTo(nextPos);
				lastPos = pos;
			}

			draw3DFireLine(mpair.second.x, mpair.second.y, 100, azimth, 0);

			draw3DSightViewCube(mpair.second.x, mpair.second.y,100);

			index++;

		}
	}

}

void RoutePlanningDialog::draw3DSightViewCube(double dLon, double dLat, double dHeight)
{
	IEarth* pEarth = IEarth::GetEarth();
	osg::Group* mrGroup = (osg::Group*)pEarth->GetPtr(IEarth::EPtrType_RootNode);
	if (pEarth == nullptr)
	{
		return;
	}

	for (int index = 0; index < cover3DConeVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(cover3DConeVec[index]);
	}
	cover3DConeVec.clear();

	for (int index = 0; index < m_CPolygonConeVec.size(); index++)
	{
		mrGroup->removeChild(m_CPolygonConeVec[index]);
	}
	m_CPolygonConeVec.clear();
	QColor colorTemp(235, 209, 26,90);

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();
	casic_vector3d pos = casic_vector3d(dLon,dLat, dHeight);
	auto endPoints = getEndPoints(pos, gridwidth);

	pdLon.push_back(pos.x);
	pdLat.push_back(pos.y);
	pdHeight.push_back(100);
	for (auto pos : endPoints)
	{
		pdLon.push_back(pos.x);
		pdLat.push_back(pos.y);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
		(pos.x, pos.y);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}
		pdHeight.push_back(0);
	}

	pdLon.push_back(endPoints[0].x);
	pdLat.push_back(endPoints[0].y);
	short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
	(endPoints[0].x, endPoints[0].y);
	pdHeight.push_back(0);

	CCreateShape cs;
	IUserMesh1 *pUM = nullptr;
	SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
		100, true, nullptr, pdHeight.data());
	if (pShp != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(colorTemp.rgba());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
			pUM->SetState(IUserMesh::LINEWIDTH, 2);
			cover3DConeVec.push_back(pUM);
		}
	}

	osg::EllipsoidModel *pEllipsoidModel = (osg::EllipsoidModel *)pEarth->GetPtr(IEarth::EPtrType_EllipsoidModel);
	CPolygonCone* m_CPolygonCone = new CPolygonCone(pEllipsoidModel, pdLon.data(), pdLat.data(), 5, 100, 100, true);
	m_CPolygonCone->SetColor(colorTemp.rgba());
	m_CPolygonConeVec.push_back(m_CPolygonCone);
	mrGroup->addChild(m_CPolygonCone);
}

void RoutePlanningDialog::draw3DTakfOffLandArea(double dLon, double dLat, double dHeight)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr || pEarth->GetGraphics() == nullptr)
	{
		return;
	}
	for (int index = 0; index < takeoffLandVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(takeoffLandVec[index]);
	}
	takeoffLandVec.clear();

	for (int index = 0; index < takeoffLandTextVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(takeoffLandTextVec[index]);
	}
	takeoffLandTextVec.clear();

	QColor colorTemp(200, 200, 200, 90);

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();
	casic_vector3d pos = casic_vector3d(dLon, dLat, dHeight);
	std::vector<casic_vector3d>tmpVec;
	QGeoCoordinate mpos(pos.y, pos.x);

	casic_vector3d tmppos;
	QGeoCoordinate tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth *sqrt(5) / 2, 58.285);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 121.715);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 238.285);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(takeoffLandGridWidth*sqrt(5) / 2, 301.715);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);

	auto endPoints = tmpVec;
	for (auto pos : endPoints)
	{
		pdLon.push_back(pos.x);
		pdLat.push_back(pos.y);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
		(pos.x, pos.y);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}
		pdHeight.push_back(2);
	}

	double dM = 2;
	CCreateShape cs;
	IUserMesh1 *pUM = nullptr;
	SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
		100, true, nullptr, pdHeight.data());
	if (pShp != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(colorTemp.rgba());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
			pUM->SetState(IUserMesh::LINEWIDTH, 5);
			takeoffLandVec.push_back(pUM);

			double dZ = 2;
			SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
			IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			pUserMesh->SetColor(colorTemp.rgba());
			takeoffLandVec.push_back(pUserMesh);

		}
	}
	IText* mshpText = pEarth->GetGraphics()->CreateText("起降区", dLon, dLat, dM);
	if (mshpText != nullptr)
	{
		takeoffLandTextVec.push_back(mshpText);
	}
}

void RoutePlanningDialog::draw3DDropBoomArea(double dlatitude, double dlongitude, double dheight, double dradius)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < dropBoomAreaVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(dropBoomAreaVec[index]);
	}
	dropBoomAreaVec.clear();
	for (int index = 0; index < dropBoomAreaTextVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(dropBoomAreaTextVec[index]);
	}
	dropBoomAreaTextVec.clear();

	QColor colorBombTrack(0xffff00);
	colorBombTrack.setAlpha(90);
	QColor colorTemp;
	colorTemp.setAlpha(60);
	colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorTemp.alpha());

	CCreateShape cs;
	SHPObject *pShp = cs.CreateEllipse(0, 0, dradius, dradius, 60000);
	if (pShp == nullptr)
		return;

	IUserMesh2 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp);

	pUserMesh->SetColor(colorTemp.rgba());
	pUserMesh->SetPosition(dlongitude, dlatitude, dheight);
	dropBoomAreaVec.push_back(pUserMesh);

	double dZ = 0;
	double dM = 2000;
	SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dheight);
	pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp1);

	pUserMesh->SetColor(colorTemp.rgba());
	pUserMesh->SetPosition(dlongitude, dlatitude, 0);
	dropBoomAreaVec.push_back(pUserMesh);

	IText* mshpText = pEarth->GetGraphics()->CreateText("投弹区", dlongitude, dlatitude, dheight);
	if (mshpText != nullptr)
	{
		dropBoomAreaTextVec.push_back(mshpText);
	}
}

void RoutePlanningDialog::draw3DPathLine(std::map<int, std::vector<casic_vector3d>>& PointsVec, QColor pathColor, int lineWidth)
{

	if (m_draw3DFirst) {
		del3DPathLine();
		Path3DLines.clear();
	}

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;

	double mheight = ui->ledtUavCruiseHeight->text().toDouble();
	auto pointsItor = PointsVec.begin();
	while (pointsItor != PointsVec.end())
	{

		std::vector<casic_vector3d> points = pointsItor->second;
		std::vector<double> pdLon;
		pdLon.clear();
		std::vector<double> pdLat;
		pdLat.clear();
		std::vector<double> pdHeight;
		pdHeight.clear();

		for (int pointIndex = 0; pointIndex < points.size(); pointIndex++)
		{
			pdLon.push_back(points[pointIndex].x);
			pdLat.push_back(points[pointIndex].y);
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
			(points[pointIndex].x, points[pointIndex].y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}

			double tempHei = points[pointIndex].z;
			pdHeight.push_back(tempHei);

			double pdLon1[2] = { points[pointIndex].x, points[pointIndex].x };
			double pdLat1[2] = { points[pointIndex].y, points[pointIndex].y };
			double pdHeight1[2] = { pdHeight[pointIndex] * 1.0, 0 };

			IUserMesh1	*pUM1 = nullptr;
			CCreateShape cs1;
			SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
			if (pShp1 != nullptr) {
				pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
				if (pUM1 != nullptr) {
					QColor col(255, 255, 255, 100);
					pUM1->SetColor(col.rgba());
					pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
					pUM1->SetState(IUserMesh::LINEWIDTH, 2);
					pUM1->SetState(GL_DEPTH_TEST, 1);
					Path3DLines.push_back(pUM1);
				}
			}

			IUserMesh2	*pUM2 = nullptr;
			SHPObject *pShp2 = cs1.CreateCircle(0, 0, 2, 1e6);
			if (pShp2 != nullptr) {
				pUM2 = pEarth->GetGraphics()->CreateUserMesh2(pShp2);
				if (pUM2 != nullptr) {
					QColor col(255, 255, 255, 160);
					pUM2->SetColor(col.rgba());
					pUM2->SetState(GL_DEPTH_TEST, 1);
					pUM2->SetPosition(points[pointIndex].x, points[pointIndex].y, 0);
					Path3DLines.push_back(pUM2);
				}
			}

		}
		IUserMesh1 *pUM = nullptr;
		SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdLon.size(),
			100, false, nullptr, pdHeight.data());
		if (pShp != nullptr)
		{
			pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
			if (pUM != nullptr)
			{
				pUM->SetColor(pathColor.rgb());
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);
				Path3DLines.push_back(pUM);
			}
		}
		pointsItor++;
	}
}

void RoutePlanningDialog::del3DPathLine()
{
	IEarth *pEarth = IEarth::GetEarth();
	for (int index = 0; index < Path3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines[index]);
	}

	return;
}

void RoutePlanningDialog::draw3DMarkArea()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	if (pMarkArea != nullptr)
	{
		pEarth->GetGraphics()->RemoveObject(pMarkArea);
		pMarkArea = nullptr;
	}

	for (int index = 0; index < Grid3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Grid3DLines[index]);
	}
	Grid3DLines.clear();

	for (int index = 0; index < m_3DMarkAreaVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(m_3DMarkAreaVec[index]);
	}
	m_3DMarkAreaVec.clear();
	for (int index = 0; index < m_3DMarkAreaTextVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(m_3DMarkAreaTextVec[index]);
	}
	m_3DMarkAreaTextVec.clear();

	QColor colorTmp(127, 255, 255, 40);
	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();

	for (int pointindex = 0; pointindex < gridsidePoints.size(); pointindex++)
	{
		short int m_alt = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
		(gridsidePoints[pointindex].x, gridsidePoints[pointindex].y);
		if (m_alt < -10000)
		{
			m_alt = 0;
		}
		pdLon.push_back(gridsidePoints[pointindex].x);
		pdLat.push_back(gridsidePoints[pointindex].y);
		pdHeight.push_back(0);
	}

	CCreateShape cs;
	IUserMesh1 *pUM = nullptr;
	SHPObject* tmpShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(), 100, true, nullptr, pdHeight.data());
	if (tmpShp != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh1(tmpShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(colorTmp.rgba());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
			pUM->SetState(IUserMesh::LINEWIDTH, 2);
			m_3DMarkAreaVec.push_back(pUM);

			double dZ = 0;
			double dM = 5;
			SHPObject *pShp1 = cs.AddZM2SHPObject(true, tmpShp, &dZ, &dM);
			IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			pUserMesh->SetColor(colorTmp.rgba());

			m_3DMarkAreaVec.push_back(pUserMesh);
		}
	}

	if (pdLon.size() > 0 && pdLat.size() > 0) {
		IText* mshpText = pEarth->GetGraphics()->CreateText("搜索区", pdLon[0], pdLat[0], 5);
		if (mshpText != nullptr)
		{
			m_3DMarkAreaTextVec.push_back(mshpText);
		}
	}
}

void RoutePlanningDialog::draw3DGrid()
{
	Grid3DLines.clear();
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;
	QColor colorTemp(0, 0, 0, 200);

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();

	if (gridLinesPoints.size() > 0)
	{
		int pindex = 0;
		for (int pointIndex = 0; pointIndex < gridLinesPoints.size(); pointIndex += 2)
		{
			IUserMesh1 *pUM = nullptr;
			SHPObject *pShp = cs.CreateLine(gridLinesPoints[pointIndex].x(), gridLinesPoints[pointIndex].y(), gridLinesPoints[pointIndex + 1].x(), gridLinesPoints[pointIndex + 1].y(),
				100, NULL,NULL);
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					pUM->SetColor(colorTemp.rgba());
					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
					pUM->SetState(IUserMesh::LINEWIDTH, 2);
					Grid3DLines.push_back(pUM);
				}
			}
		}
	}
}

void RoutePlanningDialog::draw3DNoFlyArea()
{

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < NoFlyArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(NoFlyArea3D[index]);
	}
	NoFlyArea3D.clear();
	for (int index = 0; index < NoFlyArea3DTextVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(NoFlyArea3DTextVec[index]);
	}
	NoFlyArea3DTextVec.clear();

	CCreateShape cs;
	QColor colorTemp(0, 0, 255, 100);
	if (NoFlyZonePoints.size() > 0)
	{

		for (auto oneNoFlyZonePoint : NoFlyZonePoints)
		{
			std::vector<double> pdLon;
			pdLon.clear();
			std::vector<double> pdLat;
			pdLat.clear();
			std::vector<double> pdHeight;
			pdHeight.clear();

			for (auto point : oneNoFlyZonePoint)
			{
				pdLon.push_back(point.x());
				pdLat.push_back(point.y());
				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
				(point.x(), point.y());
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				pdHeight.push_back(30);
			}
			auto point = oneNoFlyZonePoint.begin();
			if (point == oneNoFlyZonePoint.end())
			{
				return;
			}
			pdLon.push_back((*point).x());
			pdLat.push_back((*point).y());
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
			((*point).x(), (*point).y());
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			pdHeight.push_back(30);
			double dM = 100;

			IUserMesh1 *pUM = nullptr;
			SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
				100, true, nullptr, pdHeight.data());
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					pUM->SetColor(colorTemp.rgba());
					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
					pUM->SetState(IUserMesh::LINEWIDTH, 5);
					NoFlyArea3D.push_back(pUM);

					double dZ = 0;
					SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
					IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
					pUserMesh->SetColor(colorTemp.rgba());
					NoFlyArea3D.push_back(pUserMesh);
				}
			}
			IText* mshpText = pEarth->GetGraphics()->CreateText("禁飞区", pdLon[0], pdLat[0], dM);
			if (mshpText != nullptr)
			{
				NoFlyArea3DTextVec.push_back(mshpText);
			}

		}
	}
}

void RoutePlanningDialog::drawCoverArea()
{
	return;
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	delCoverArea();

	CCreateShape cs;
	QColor colorTemp(0, 255, 0, 60);
	if (_CoveredAreaVec.size() != 0)
	{
		for (auto areaIndex : _CoveredAreaVec)
		{
			std::vector<double> pdLon;
			pdLon.clear();
			std::vector<double> pdLat;
			pdLat.clear();
			std::vector<double> pdHeight;
			pdHeight.clear();
			std::vector<casic_vector3d>areaVec = areaIndex;

			for (auto point : areaVec)
			{
				pdLon.push_back(point.x);
				pdLat.push_back(point.y);
				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
				(point.x, point.y);
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				pdHeight.push_back(12);
			}
			IUserMesh1 *pUM = nullptr;
			SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
				100, true, nullptr, pdHeight.data());
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					pUM->SetColor(colorTemp.rgba());
					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
					pUM->SetState(IUserMesh::LINEWIDTH, 2);
					coverArea3D.push_back(pUM);
				}
			}
		}
	}
}

void RoutePlanningDialog::delCoverArea()
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < coverArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(coverArea3D[index]);
	}
	coverArea3D.clear();
}

void RoutePlanningDialog::draw3DObstacleArea()
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < obstacleArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(obstacleArea3D[index]);
	}
	obstacleArea3D.clear();

	CCreateShape cs;
	QColor colorTemp(0, 0, 255, 150);
	if (obstacleArea.size() != 0)
	{
		for (int i = 0; i < obstacleArea.size(); i++)
		{
			std::vector<double> pdLon;
			pdLon.clear();
			std::vector<double> pdLat;
			pdLat.clear();
			std::vector<double> pdHeight;
			pdHeight.clear();
			casic_vector3d pos = casic_vector3d(obstacleArea[i][0], obstacleArea[i][1], 10);
			std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridwidth);
			for (auto area : edpoints)
			{
				pdLon.push_back(area.x);
				pdLat.push_back(area.y);
				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
				(area.x, area.y);
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				pdHeight.push_back(elevationTemp);
			}

			IUserMesh1 *pUM = nullptr;
			SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
				100, true, nullptr, pdHeight.data());
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					pUM->SetColor(colorTemp.rgba());
					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
					pUM->SetState(IUserMesh::LINEWIDTH, 2);
					obstacleArea3D.push_back(pUM);
				}
			}
		}
	}
}

void RoutePlanningDialog::drawKeyArea3D()
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < keyArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(keyArea3D[index]);
	}
	keyArea3D.clear();

	CCreateShape cs;
	QColor colorTemp(255, 255, 0, 150);
	if (keyXYPoint.size() != 0)
	{
		auto keyXYPointItor = keyXYPoint.begin();
		while (keyXYPointItor != keyXYPoint.end())
		{
			std::vector<vector<double>> points = keyXYPointItor->second;
			auto xyItor = points.begin();
			while (xyItor != points.end())
			{
				std::vector<double> pdLon;
				pdLon.clear();
				std::vector<double> pdLat;
				pdLat.clear();
				std::vector<double> pdHeight;
				pdHeight.clear();

				std::vector<double> lonlatValue = *xyItor;
				casic_vector3d pos = casic_vector3d(lonlatValue[2], lonlatValue[3], 10);
				std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridwidth);
				for (auto area : edpoints)
				{
					pdLon.push_back(area.x);
					pdLat.push_back(area.y);
					short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
					(area.x, area.y);
					if (elevationTemp < -10000)
					{
						elevationTemp = 0;
					}
					pdHeight.push_back(elevationTemp);
				}
				IUserMesh1 *pUM = nullptr;
				SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
					100, true, nullptr, pdHeight.data());
				if (pShp != nullptr)
				{
					pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
					if (pUM != nullptr)
					{
						pUM->SetColor(colorTemp.rgba());
						pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
						pUM->SetState(IUserMesh::LINEWIDTH, 2);
						keyArea3D.push_back(pUM);
					}
				}

				xyItor++;
			}

			keyXYPointItor++;
		}
	}

}

void RoutePlanningDialog::drawKmeansArea()
{
	return;
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < kmeansArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(kmeansArea3D[index]);
	}
	kmeansArea3D.clear();

	CCreateShape cs;
	std::vector<QColor> Colors{
		QColor(0, 255, 0, 50),
		QColor(0, 0, 255, 50),
		QColor(255, 0, 0, 50),
		QColor(125, 125, 0, 50),
		QColor(125, 0, 125, 50),
		QColor(0, 125, 125, 50)
	};

	if (kmeansSamples.size() != 0 && kmeansLabels.size() != 0)
	{
		int index = 0;
		for (auto areaIndex : kmeansSamples)
		{
			std::vector<double> pdLon;
			pdLon.clear();
			std::vector<double> pdLat;
			pdLat.clear();
			std::vector<double> pdHeight;
			pdHeight.clear();
			std::vector<double>areaVec = areaIndex;

			casic_vector3d pos = casic_vector3d(areaIndex[0], areaIndex[1], 10);
			std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridwidth);
			for (auto area : edpoints)
			{
				pdLon.push_back(area.x);
				pdLat.push_back(area.y);
				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
				(area.x, area.y);
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				pdHeight.push_back(elevationTemp);
			}

			IUserMesh1 *pUM = nullptr;
			SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
				100, true, nullptr, pdHeight.data());
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					if (kmeansLabels.size() == kmeansSamples.size())
					{
						int labelNum = kmeansLabels[index];
						labelNum = labelNum % Colors.size();
						pUM->SetColor(Colors[labelNum].rgba());
					}

					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
					pUM->SetState(IUserMesh::LINEWIDTH, 2);
					kmeansArea3D.push_back(pUM);
				}
			}
			index++;
		}
	}
}

void RoutePlanningDialog::deleteKmeansArea()
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < kmeansArea3D.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(kmeansArea3D[index]);
	}
	kmeansArea3D.clear();
}

void RoutePlanningDialog::addKeyAreaPoint(QPointF point)
{
	oneKeyAreaPoints.push_back(point);
}

void RoutePlanningDialog::finishaddKeyAreaPoint()
{
	LLQPoints points = oneKeyAreaPoints;
	oneKeyAreaPoints.clear();
	KeyAreaVec.push_back(points);
	int index = KeyAreaVec.size() - 1;
	std::pair<bool, int> sec(false, KeyAreaNum);
	KeyAreas[index] = sec;
	setKeyAreaXyHcost();
}

void RoutePlanningDialog::setKeyAreaXyHcost()
{
	if (GridPointMap.empty())
	{
		return;
	}
	auto KeyAreaItor = KeyAreas.begin();
	while (KeyAreaItor != KeyAreas.end())
	{
		if (KeyAreaItor->second.first == true)
		{
			KeyAreaItor++;
			continue;
		}
		int KeyNum = KeyAreaItor->second.second;
		int keyAreaVecIndex = KeyAreaItor->first;

		LLQPoints areaPoints = KeyAreaVec[keyAreaVecIndex];

		auto KeyAreaXYItor = keyXYPoint.find(keyAreaVecIndex);
		if (KeyAreaXYItor == keyXYPoint.end())
		{
			std::vector<vector<double>> tmp;
			tmp.clear();
			keyXYPoint[keyAreaVecIndex] = tmp;
			KeyAreaXYItor = keyXYPoint.find(keyAreaVecIndex);
		}
		bool isHave = false;

		for (int xIndex = 0; xIndex < GridPointMap.size(); xIndex++)
		{
			for (int yIndex = 0; yIndex < GridPointMap[xIndex].size(); yIndex++)
			{
				auto nowNodeItor = GridPointMap[xIndex][yIndex];
				QPointF NodePoint(nowNodeItor->coordinateX(), nowNodeItor->coordinateY());

				bool isIn = is_point_in_zone(NodePoint, areaPoints);
				if (isIn)
				{

					nowNodeItor->setHCost(KeyNum);

					std::vector<double> xyLonlatVal = {
						double(xIndex), double(yIndex),
						nowNodeItor->coordinateX(), nowNodeItor->coordinateY(), double(KeyNum)
					};
					KeyAreaXYItor->second.push_back(xyLonlatVal);
					isHave = true;
				}
			}
		}
		if (!isHave)
		{

			KeyAreaXYItor = keyXYPoint.erase(KeyAreaXYItor);
		}
		else
		{
			KeyAreaItor->second.first = true;
		}
		KeyAreaItor++;
	}
}

void RoutePlanningDialog::setKeyAreaNoTrans()
{
	auto keyAreaItor = KeyAreas.begin();
	while (keyAreaItor != KeyAreas.end())
	{
		keyAreaItor->second.first = false;
		keyAreaItor++;
	}
}

void RoutePlanningDialog::delKeyArea(QPointF mousePoint)
{
	auto keyAreaXYItor = keyXYPoint.begin();
	while (keyAreaXYItor != keyXYPoint.end())
	{
		int keyAreaVecIndex = keyAreaXYItor->first;
		LLQPoints keyArea = KeyAreaVec[keyAreaVecIndex];

		bool isIn = is_point_in_zone(mousePoint, keyArea);
		if (isIn)
		{
			std::vector<vector<double>> xyPoints = keyAreaXYItor->second;

			for (int i = 0; i < xyPoints.size(); i++)
			{
				vector<double> xyPoint = xyPoints[i];
				GridPointMap[xyPoint[0]][xyPoint[1]]->setHCost(0);
			}
			keyAreaXYItor = keyXYPoint.erase(keyAreaXYItor);

			auto keyAreaItor = KeyAreas.find(keyAreaVecIndex);
			if (keyAreaItor != KeyAreas.end())
			{
				keyAreaItor = KeyAreas.erase(keyAreaItor);
			}

		}
		else
		{
			keyAreaXYItor++;
		}

	}
}

void RoutePlanningDialog::drawKeyAreaSelectPoly(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 1);
	if (oneKeyAreaPoints.size() > 0)
	{
		QVector<QPoint> points;
		points.clear();
		for (int pointIndex = 0; pointIndex < oneKeyAreaPoints.size(); pointIndex++)
		{
			QPoint point1 = pProj->Jwd2Screen(oneKeyAreaPoints[pointIndex].x(),
				oneKeyAreaPoints[pointIndex].y());
			points.push_back(point1);
		}
		QPolygon poly(points);
		painter.setPen(pen);
		painter.drawPolygon(poly);
	}
}

void RoutePlanningDialog::drawKeyArea(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::blue, 9);
	auto keyXYPointItor = keyXYPoint.begin();
	while (keyXYPointItor != keyXYPoint.end())
	{
		std::vector<vector<double>> points = keyXYPointItor->second;
		auto xyItor = points.begin();
		while (xyItor != points.end())
		{
			std::vector<double> lonlatValue = *xyItor;
			QPoint point = pProj->Jwd2Screen(lonlatValue[2], lonlatValue[3]);
			painter.setPen(pen);
			painter.drawText(point, QString::number(lonlatValue[4]));
			painter.drawPoint(point);
			xyItor++;
		}

		keyXYPointItor++;
	}
}

void RoutePlanningDialog::drawCoverCube(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::green, 1);
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube.size() > 0)
	{
		auto tmpVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube;
		for (auto mpair : tmpVec)
		{
			casic_vector3d pos = mpair.second;
			auto endPoints = getEndPoints(pos, gridwidth);
			QVector<QPoint> points;
			points.clear();
			QPen pen(Qt::green, 2);
			painter.setPen(pen);
			for (int pointIndex = 0; pointIndex < endPoints.size(); pointIndex++)
			{
				QPoint point1 = pProj->Jwd2Screen(endPoints[pointIndex].x,
					endPoints[pointIndex].y);
				points.push_back(point1);
			}
			QPolygon poly(points);

			QColor colorTemp(0, 255, 0, 60);
			painter.setBrush(QBrush(colorTemp));
			painter.drawPolygon(poly,Qt::WindingFill);

			draw3DCoverCube();
		}
	}
}

void RoutePlanningDialog::drawSelectPoint(QPainter & painter, IProj * pProj)
{
	QPen targetPen(Qt::yellow, 3);

	QPointF jwdLeftUp = selectFirstPoint;
	QPointF jwdRightDown = selectSecPoint;
	QPoint pointLeftUp = pProj->Jwd2Screen(jwdLeftUp.x(), jwdLeftUp.y());
	QPoint pointRightDown = pProj->Jwd2Screen(jwdRightDown.x(), jwdRightDown.y());
	double width = pointRightDown.x() - pointLeftUp.x();
	double heigth = pointRightDown.y() - pointLeftUp.y();

	QRect targetRect(pointLeftUp.x(), pointLeftUp.y(), width, heigth);
	painter.setPen(targetPen);
	painter.drawRect(targetRect);
}

std::vector<std::vector<double>> RoutePlanningDialog::getClusterPoints(int clusterNum)
{
	std::vector<std::vector<double>> tmpVec;
	if (!gridSidePoint.empty() || gridSidePoint.size() >= 4)
	{
		QGeoCoordinate firstPoint(gridSidePoint[0].y, gridSidePoint[0].x);
		QGeoCoordinate secPoint(gridSidePoint[1].y, gridSidePoint[1].x);

		double angle = firstPoint.azimuthTo(secPoint);
		double dis_step = (firstPoint.distanceTo(secPoint) - gridwidth) / (clusterNum - 1);
		double m_alt = gridSidePoint[0].z;

		casic_vector3d startPos = (gridSidePoint[0] + gridSidePoint[3]) / 2;

		QGeoCoordinate startGeo(startPos.y, startPos.x);
		startGeo = startGeo.atDistanceAndAzimuth(gridwidth / 2, angle);

		QGeoCoordinate tmpGeo;
		for (int i = 0; i < clusterNum; i++)
		{
			std::vector<double>tmpPoint;
			if (i == 0)
			{
				tmpPoint = { startGeo.longitude(),startGeo.latitude(),m_alt };
				tmpGeo = startGeo;
				tmpVec.push_back(tmpPoint);
				continue;
			}
			tmpGeo  = tmpGeo.atDistanceAndAzimuth(dis_step, angle);
			tmpPoint = { tmpGeo.longitude(),tmpGeo.latitude(),m_alt };
			tmpVec.push_back(tmpPoint);
		}
	}
	return tmpVec;
}

std::vector<std::vector<casic_vector3d>> RoutePlanningDialog::getClusterAreas(int clusterNum)
{
	std::vector<std::vector<casic_vector3d>> tmpVec;
	if (!gridSidePoint.empty() || gridSidePoint.size() >= 4)
	{
		QGeoCoordinate firstPoint(gridSidePoint[0].y, gridSidePoint[0].x);
		QGeoCoordinate secPoint(gridSidePoint[1].y, gridSidePoint[1].x);
		QGeoCoordinate thdPoint(gridSidePoint[2].y, gridSidePoint[2].x);
		QGeoCoordinate furthPoint(gridSidePoint[3].y, gridSidePoint[3].x);

		double angle = firstPoint.azimuthTo(secPoint);
		double dis_step = (firstPoint.distanceTo(secPoint) - gridwidth) / (clusterNum - 1);
		double m_alt = gridSidePoint[0].z;

		QGeoCoordinate startGeo = firstPoint;

		QGeoCoordinate tmpGeo = firstPoint;
		QGeoCoordinate tmpGeo1 = furthPoint;
		for (int i = 0; i < clusterNum; i++)
		{
			std::vector<casic_vector3d>tmpPoint;
			casic_vector3d tmppos(tmpGeo.longitude(), tmpGeo.latitude(), m_alt);
			tmpPoint.push_back(tmppos);

			tmpGeo = tmpGeo.atDistanceAndAzimuth(dis_step, angle);
			tmppos = casic_vector3d(tmpGeo.longitude(), tmpGeo.latitude(), m_alt);
			tmpPoint.push_back(tmppos);

			QGeoCoordinate lastGeo;
			lastGeo = tmpGeo1;

			tmpGeo1 = tmpGeo1.atDistanceAndAzimuth(dis_step, angle);
			tmppos = casic_vector3d(tmpGeo.longitude(), tmpGeo.latitude(), m_alt);
			tmpPoint.push_back(tmppos);

			tmppos = casic_vector3d(lastGeo.longitude(), lastGeo.latitude(), m_alt);
			tmpPoint.push_back(tmppos);

			lastGeo = tmpGeo1;

			tmpVec.push_back(tmpPoint);

		}
	}
	return tmpVec;
}

int RoutePlanningDialog::getClusterCount()
{

	double dampingRate = 1;
	int count;
	if (GridPointMap.size() == 0 || GridPointMap[0].size() == 0)
	{
		return 0;
	}
	int gridsCount = GridPointMap.size() * GridPointMap[0].size();
	double gridLength = calcolateGrid();
	double allDis = gridsCount * gridLength;
	double planeFlyDis = cruiseSpeed * flyTime ;

	double singleAngle = 90 / angularSpeed;
	double allAngleTime = gridsCount * singleAngle;
	double allDisTime = allDis / cruiseSpeed;
	double allTime = allAngleTime + allDisTime;

	count = allTime / (flyTime / 2) + 1;
	if (count > 4 && count % 2 !=  0)
	{
		count += 1;
	}
	return count;

}

double RoutePlanningDialog::getFlyTimeCoef(double &suggestFlyTime)
{
	double flyTimeParam = 0.0;
	double count = GridPointMap.size() > GridPointMap[0].size() ? GridPointMap.size() : GridPointMap[0].size();
	double gridLength = calcolateGrid();
	double t1 = count * gridLength * coverRate / cruiseSpeed;
	flyTimeParam = flyTime / 2 / t1;
	if (flyTimeParam < 2)
	{
		suggestFlyTime = t1 * 2;
	}
	return flyTimeParam;
}

casic_vector2d RoutePlanningDialog::getMaxPlanNodeIndex(std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>oneAreaPlanNode)
{
	int rowMax = 0;
	int colMax = 0;
	for (auto plannode : oneAreaPlanNode)
	{
		int row = plannode->indexX();
		int col = plannode->indexY();
		if (rowMax < row)
		{
			rowMax = row;
		}
		if (colMax < col)
		{
			colMax = col;
		}
	}
	return casic_vector2d(rowMax, colMax);
}

casic_vector2d RoutePlanningDialog::getMinPlanNodeIndex(std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>> oneAreaPlanNode)
{
	auto first = oneAreaPlanNode.begin();
	int rowMin = (*first)->indexX();
	int colMin = (*first)->indexY();
	for (auto plannode : oneAreaPlanNode)
	{
		int row = plannode->indexX();
		int col = plannode->indexY();
		if (rowMin > row)
		{
			rowMin = row;
		}
		if (colMin > col)
		{
			colMin = col;
		}
	}
	return casic_vector2d(rowMin, colMin);
}

std::vector<casic_vector3d> RoutePlanningDialog::getEndPoints(casic_vector3d pos, double gridWidth)
{
	std::vector<casic_vector3d>tmpVec;
	QGeoCoordinate mpos(pos.y, pos.x);
	casic_vector3d tmppos;
	QGeoCoordinate tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 45);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 135);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 225);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 315);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	return tmpVec;
}

void RoutePlanningDialog::CalculatePoint()
{
	m_vecCalculateSumTime = 0;
	m_vecCalculateMaxNum = 0;
	m_vecCalculatePoint.clear();
	int index = 110201;

	int index_ = 1;
	for (auto uav : AreaSearchPointMap)
	{
		std::string name_head = "A";
		name_head = name_head + std::to_string(index_);
		CCalculateLine stCalculatePoint;
		stCalculatePoint.strWeaponID = std::to_string(index);
		stCalculatePoint.strWeaponName = name_head;
		index = index + 100;
		index_++;
		stCalculatePoint.stTelemetryPointVec.clear();
		QGeoCoordinate QPoint1, QPoint2;
		auto itrPoint = uav.second.begin();
		for (; itrPoint != uav.second.end(); itrPoint++)
		{
			if ((itrPoint + 1) != uav.second.end())
			{
				double dLongStart(0), dLatStart(0), dLongEnd(0), dLatEnd(0), dHeightEnd(0), dHeight(0);
				dLongStart = itrPoint->x;
				dLatStart = itrPoint->y;
				dLongEnd = (itrPoint + 1)->x;
				dLatEnd = (itrPoint + 1)->y;
				dHeightEnd = (itrPoint + 1)->z;
				dHeight = itrPoint->z;
				QPoint1.setLongitude(dLongStart);
				QPoint1.setLatitude(dLatStart);
				QPoint2.setLongitude(dLongEnd);
				QPoint2.setLatitude(dLatEnd);

				double localdistance = QPoint1.distanceTo(QPoint2);

				localdistance = get_distance_from_lat_lon(dLatStart, dLongStart, dLatEnd, dLongEnd);

				double speedPlay = (cruiseSpeed *1.0 / PLAY_HZ);
				if (speedPlay <= 0) {
					speedPlay = 0.5;
				}
				double dTime = localdistance / speedPlay ;
				m_vecCalculateSumTime = m_vecCalculateSumTime + dTime;
				double dbearing = QPoint1.azimuthTo(QPoint2);
				if (dbearing < 0)
				{
					dbearing += 360;
				}

				double dDisOnce = cruiseSpeed / PLAY_HZ *1.0;
				if (dDisOnce <= 0) {
					dDisOnce = 0.5;
				}
				double dNewLong(0), dNewLat(0);

				telemetryInformation stTelemetryInformation;
				stTelemetryInformation.position.lng = dLongStart;
				stTelemetryInformation.position.lat = dLatStart;
				stTelemetryInformation.gpsHeigth = dHeight;
				stTelemetryInformation.angleofNose = dbearing;
				stTelemetryInformation.groundVelocity = cruiseSpeed;
				stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformation);

				double dlocaldistanceOnce = localdistance;
				while ((localdistance - dDisOnce) > 0.001)
				{

					QPoint1 = QPoint1.atDistanceAndAzimuth(dDisOnce, dbearing);

					localdistance = QPoint1.distanceTo(QPoint2);

					telemetryInformation stTelemetryInformationTmp;
					stTelemetryInformationTmp.position.lng = QPoint1.longitude();
					stTelemetryInformationTmp.position.lat = QPoint1.latitude();
					stTelemetryInformationTmp.gpsHeigth = dHeight;

					if (dlocaldistanceOnce != 0)
					{
						stTelemetryInformationTmp.gpsHeigth = dHeight + ((dlocaldistanceOnce - localdistance) * (dHeightEnd - dHeight)) / dlocaldistanceOnce;
					}
					stTelemetryInformationTmp.angleofNose = dbearing;
					stTelemetryInformationTmp.groundVelocity = cruiseSpeed;
					stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformationTmp);
				}
			}
			else
			{
				double dLongStart(0), dLatStart(0), dHeight(0);
				dLongStart = itrPoint->x;
				dLatStart = itrPoint->y;
				dHeight = itrPoint->z;

				telemetryInformation stTelemetryInformation;
				memset(&stTelemetryInformation, 0x00, sizeof(telemetryInformation));
				stTelemetryInformation.position.lng = dLongStart;
				stTelemetryInformation.position.lat = dLatStart;
				stTelemetryInformation.gpsHeigth = dHeight;
				stTelemetryInformation.groundVelocity = cruiseSpeed;

				stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformation);
			}
		}
		if (stCalculatePoint.stTelemetryPointVec.size() >= m_vecCalculateMaxNum)
		{
			m_vecCalculateMaxNum = stCalculatePoint.stTelemetryPointVec.size();

			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->m_vecCalculateMaxNum =
				stCalculatePoint.stTelemetryPointVec.size();
		}
		m_vecCalculatePoint.push_back(stCalculatePoint);
	}

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->m_vecCalculatePoint = m_vecCalculatePoint;
}

void RoutePlanningDialog::setTimeShow()
{
	QString hh = "00";
	QString mm = "00";
	QString ss = "00";
	QString dd = "0";
	static int	dex_i = 0;
	static int	m_temp = 0;
	static int	m_m = 0;
	static int	h_h = 0;
	static int  d_d = 0;
	if (isStopClicked)
	{
		m_temp = 0;
		dex_i = 0;
		m_m = 0;
		h_h = 0;
		isStopClicked = false;
	}

	if (m_temp >= 60)
	{
		m_m++;
		if (m_m >= 60)
		{
			h_h++;
			if (h_h >= 24)
			{
				d_d++;
				h_h = h_h - 24;
			}
			m_m = m_m - 60;

		}

		m_temp = 0;
	}
	if (m_temp < 10)
	{
		QString tmp = "0" + QString::number(m_temp);
		ss = tmp;
	}
	else
	{
		ss = QString::number(m_temp);
	}

	if (m_m < 10)
	{
		QString tmp = "0" + QString::number(m_m);
		mm = tmp;
	}
	else
	{
		mm = QString::number(m_m);
	}

	if (h_h < 10)
	{
		QString tmp = "0" + QString::number(h_h);
		hh = tmp;
	}
	else
	{
		hh = QString::number(h_h);
	}
	m_temp++;
	dd = QString::number(d_d);
	QString tim = dd + tr("Day") + " " + hh + ":" + mm + ":" + ss;

	dex_i++;
}

void RoutePlanningDialog::calSelectedSerialPoint()
{
	selectedSerialPoints.clear();
	selectedFBPoints.clear();
	auto pathItor = AreaSearchPointMap.begin();
	while (pathItor != AreaSearchPointMap.end())
	{
		std::vector<std::vector<IndexPos>> tmpSelectedPoints;
		tmpSelectedPoints.clear();
		std::vector<IndexPos> serialPoints;
		serialPoints.clear();
		std::vector<std::pair<IndexPos, IndexPos>> FBvec;
		FBvec.clear();

		int lastIndex = -1;
		auto path = pathItor->second;
		for (int pointIndex = 0; pointIndex < path.size(); pointIndex++)
		{

			casic_vector3d pos = path[pointIndex];
			QPointF point(pos[0], pos[1]);
			bool isIn = is_point_in_zone(point, pointSelects);
			if (isIn)
			{
				IndexPos index_pos = std::make_pair(pointIndex, pos);
				if (lastIndex == pointIndex - 1)
				{
					serialPoints.emplace_back(index_pos);
					lastIndex = pointIndex;
				}
				else if (lastIndex != pointIndex - 1)
				{
					if (serialPoints.empty())
					{
						serialPoints.emplace_back(index_pos);
						lastIndex = pointIndex;
					}
					else
					{
						tmpSelectedPoints.emplace_back(serialPoints);

						int count = serialPoints.size();
						int Findex = serialPoints[0].first - 1;
						int Bindex = serialPoints[count - 1].first + 1;
						if (Findex >= 0 && Bindex < path.size())
						{
							IndexPos fIndexPos = std::make_pair(Findex, path[Findex]);
							IndexPos BindexPos = std::make_pair(Bindex, path[Bindex]);

							FBvec.emplace_back(std::make_pair(fIndexPos, BindexPos));
						}
						serialPoints.clear();
						serialPoints.emplace_back(index_pos);
						lastIndex = pointIndex;
					}
				}
			}
			if (pointIndex == path.size() - 1 && !serialPoints.empty())
			{
				tmpSelectedPoints.emplace_back(serialPoints);

				int count = serialPoints.size();
				int Findex = serialPoints[0].first - 1;
				int Bindex = serialPoints[count - 1].first + 1;
				if (Findex >= 0 && Bindex < path.size())
				{
					IndexPos fIndexPos = std::make_pair(Findex, path[Findex]);
					IndexPos BindexPos = std::make_pair(Bindex, path[Bindex]);

					FBvec.emplace_back(std::make_pair(fIndexPos, BindexPos));
				}
			}
		}
		if (tmpSelectedPoints.empty())
		{
			pathItor++;
		}
		else
		{
			selectedSerialPoints[pathItor->first] = tmpSelectedPoints;
			selectedFBPoints[pathItor->first] = FBvec;
			pathItor++;
		}
	}

}

void RoutePlanningDialog::delSelectedPoint(int pathIndex, IndexPos index_pos)
{
	auto pathItor = AreaSearchPointMap.find(pathIndex);
	if (pathItor != AreaSearchPointMap.end())
	{
		int deledIndex = getIndexInPath(pathIndex, index_pos);

		if (deledIndex != -1)
		{
			std::vector<casic_vector3d>::iterator deledItor = pathItor->second.begin();
			for (int index = 0; index < deledIndex; index++)
			{
				deledItor++;
			}
			deledItor = pathItor->second.erase(deledItor);
		}
		else
		{
			return;
		}

	}
	else
	{
		return;
	}
}

void RoutePlanningDialog::deleAllSelectedPoint()
{
	auto pathItor = AreaSearchPointMap.begin();
	while (pathItor != AreaSearchPointMap.end())
	{
		auto serialPointsItor = selectedSerialPoints.find(pathItor->first);
		if (serialPointsItor == selectedSerialPoints.end())
		{
			pathItor++;
			continue;
		}
		std::vector<std::vector<IndexPos>> serialPoints = serialPointsItor->second;
		for (int serialIndex = 0; serialIndex < serialPoints.size(); serialIndex++)
		{
			std::vector<IndexPos> points = serialPoints[serialIndex];
			for (int index = 0; index < points.size(); index++)
			{
				IndexPos index_pos = points[index];
				delSelectedPoint(pathItor->first, index_pos);
			}
		}

		pathItor++;
	}

	CalculatePoint();
}

int RoutePlanningDialog::getIndexInPath(int pathIndex, IndexPos indexpos)
{
	int deledIndex = -1;
	auto pathItor = AreaSearchPointMap.find(pathIndex);
	if (pathItor != AreaSearchPointMap.end())
	{
		std::vector<casic_vector3d> points = pathItor->second;
		int pointsCount = points.size();
		int posIndex = indexpos.first;
		casic_vector3d deledPos = indexpos.second;
		posIndex = posIndex < pointsCount - 1 ? posIndex : pointsCount - 1;

		if (points[posIndex] == deledPos)
		{
			deledIndex = posIndex;
			return deledIndex;
		}
		else
		{
			int findex = posIndex - 1;
			int bindex = posIndex + 1;
			for (findex, bindex; bindex < pointsCount || findex >= 0; findex--, bindex++)
			{
				if (bindex < pointsCount)
				{
					if (points[bindex] == deledPos)
					{
						deledIndex = bindex;
						return deledIndex;;
					}
				}

				if (findex >= 0)
				{
					if (points[findex] == deledPos)
					{
						deledIndex = findex;
						return deledIndex;;
					}
				}
			}
		}
	}
	else
	{
		return deledIndex;
	}
	return deledIndex;
}

void RoutePlanningDialog::drawNewAddPoint(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 8);
	for (int i = 0; i < newAddPoints.size(); i++)
	{
		QPoint DrawPoint = pProj->Jwd2Screen(newAddPoints[i][0], newAddPoints[i][1]);
		painter.setPen(pen);
		painter.drawPoint(DrawPoint);
	}
}

void RoutePlanningDialog::newPointsAssem2Path(int pathIndex, std::vector<casic_vector3d>& path, std::vector<casic_vector3d> newPoints)
{
	auto selectFBItor = selectedFBPoints.find(pathIndex);
	if (selectFBItor == selectedFBPoints.end())
	{
		return;
	}
	else
	{
		std::vector<std::pair<IndexPos, IndexPos>> FBvec = selectFBItor->second;
		for (int FBindex = 0; FBindex < FBvec.size(); FBindex++)
		{
			if (newPoints.empty())
			{
				return;
			}

			if (FBvec.size() == 1)
			{
				std::vector<casic_vector3d> rePoss = newPoints;
				std::reverse(rePoss.begin(), rePoss.end());
				newPoints.insert(newPoints.end()--, rePoss.begin(), rePoss.end());
			}

			std::pair<IndexPos, IndexPos> FBIndexPos = FBvec[FBindex];
			IndexPos FindexPos = FBIndexPos.first;
			IndexPos BindexPos = FBIndexPos.second;

			QGeoCoordinate FGeo(FindexPos.second[1], FindexPos.second[0]);
			QGeoCoordinate BGeo(BindexPos.second[1], BindexPos.second[0]);
			QGeoCoordinate newP0(newPoints[0][1], newPoints[0][0]);
			double fdis = FGeo.distanceTo(newP0);
			double bdis = BGeo.distanceTo(newP0);

			int FNowIndex = getIndexInPath(pathIndex, FindexPos);
			if (FNowIndex >= 0)
			{
				std::vector<casic_vector3d>::iterator FNowItor = path.begin();
				for (int i = 0; i < FNowIndex + 1; i++)
				{
					FNowItor++;
				}
				path.insert(FNowItor, newPoints.begin(), newPoints.end());
				std::reverse(newPoints.begin(), newPoints.end());
			}
		}
	}
}

casic_vector3d RoutePlanningDialog::newPointTransGridPos(casic_vector3d newPoint)
{
	casic_vector3d nearPoint = newPoint;
	int iCount = GridPointMap.size();
	int jCount = GridPointMap[0].size();

	QGeoCoordinate newP(newPoint[1], newPoint[0]);

	QGeoCoordinate p00(GridPointMap[0][0]->coordinateY(), GridPointMap[0][0]->coordinateX());
	QGeoCoordinate pi0(GridPointMap[iCount - 1][0]->coordinateY(), GridPointMap[iCount - 1][0]->coordinateX());
	QGeoCoordinate p0j(GridPointMap[0][jCount - 1]->coordinateY(), GridPointMap[0][jCount - 1]->coordinateX());

	double Disj = p00.distanceTo(pi0);
	double Disi = p00.distanceTo(p0j);

	int i = -1;
	int j = -1;
	std::map<double, int> disiMap;
	std::map<double, int> disjMap;
	disiMap.clear();
	disjMap.clear();

	for (int iIndex = 0; iIndex < iCount; iIndex++)
	{
		QGeoCoordinate piIndex0(GridPointMap[iIndex][0]->coordinateY(), GridPointMap[iIndex][0]->coordinateX());
		QGeoCoordinate piIndexJConnt(GridPointMap[iIndex][jCount - 1]->coordinateY(),
			GridPointMap[iIndex][jCount - 1]->coordinateX());
		double dis1 = newP.distanceTo(piIndex0);
		double dis2 = newP.distanceTo(piIndexJConnt);
		if (dis1 + dis2 < Disi + gridwidth)
		{
			disiMap[dis1 + dis2] = iIndex;
		}
	}

	for (int jIndex = 0; jIndex < jCount; jIndex++)
	{
		QGeoCoordinate p0jIndex(GridPointMap[0][jIndex]->coordinateY(), GridPointMap[0][jIndex]->coordinateX());
		QGeoCoordinate pIcountJIndex(GridPointMap[iCount - 1][jIndex]->coordinateY(),
			GridPointMap[iCount - 1][jIndex]->coordinateX());
		double dis1 = newP.distanceTo(p0jIndex);
		double dis2 = newP.distanceTo(pIcountJIndex);
		if (dis1 + dis2 < Disj + gridwidth / 2)
		{
			disjMap[dis1 + dis2] = jIndex;
		}
	}
	if (!disiMap.empty())
	{
		i = disiMap.begin()->second;
	}
	if (!disjMap.empty())
	{
		j = disjMap.begin()->second;
	}

	if (i != -1 && j != -1)
	{
		nearPoint = casic_vector3d(GridPointMap[i][j]->coordinateX(), GridPointMap[i][j]->coordinateY(),
			cruiseHeight);
	}
	else if (i != -1 && j == -1)
	{

		auto planNodes = GridPointMap[i];
		for (int j = 0; j < planNodes.size(); j++)
		{
			QGeoCoordinate tmpPoint(planNodes[j]->coordinateY(), planNodes[j]->coordinateX());
			double dis = tmpPoint.distanceTo(newP);
			if (dis < gridwidth / 2)
			{
				nearPoint = casic_vector3d(planNodes[j]->coordinateX(), planNodes[j]->coordinateY(), cruiseHeight);
				return nearPoint;
			}
		}
	}
	else if (i == -1 && j != -1)
	{

		for (int i = 0; i < iCount; i++)
		{
			QGeoCoordinate tmpPoint(GridPointMap[i][j]->coordinateY(), GridPointMap[i][j]->coordinateX());
			double dis = tmpPoint.distanceTo(newP);
			if (dis < gridwidth / 2)
			{
				nearPoint = casic_vector3d(GridPointMap[i][j]->coordinateX(), GridPointMap[i][j]->coordinateY(), cruiseHeight);
				return nearPoint;
			}
		}
	}

	return nearPoint;
}

void RoutePlanningDialog::changeEvent(QEvent * event)
{
	QWidget::changeEvent(event);
}

void RoutePlanningDialog::showEvent(QShowEvent * event)
{
	if (!ICollaborativeSearchActivator::Get().getPWeaponManageService()) {
		return;
	}
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	if (!ICollaborativeSearchActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap)) {
		return;
	}
	ui->comboBox_clooseEquip->clear();
	for (const auto& weaponSystemIter : weaponSystemMap) {
		const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();

		for (const auto& mapWeaponComponentIter : mapWeaponComponent) {
			QString strWeaponComponentID = QString::fromStdString(mapWeaponComponentIter.second.getComponentID());
			QString strWeaponComponentName = QString::fromStdString(mapWeaponComponentIter.second.getComponentName());
			if (mapWeaponComponentIter.second.getPWeaponModel() && mapWeaponComponentIter.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_RotorUAV) {
				{
					m_WeaponComponentMap.insert(strWeaponComponentID, strWeaponComponentName);
				}
			}
		}
	}
	if (m_WeaponComponentMap.size() <=0){
		return;
	}
	for (const auto& weapon : m_WeaponComponentMap.keys())
	{
		ui->comboBox_clooseEquip->addItem(weapon);
	}
	m_show = true;
}

void RoutePlanningDialog::hideEvent(QHideEvent * event)
{
	m_show = false;
}

void RoutePlanningDialog::on_pbtnKmeans_clicked()
{
	_CoveredAreaVec.clear();
	ObstacleVec1.clear();
	isAddNoFly = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}

	isDelKeyArea = false;
	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");

	deleteKmeansArea();

	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> m_roadmap;

	if (GridPointMap.empty())
	{
		return;
	}
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calcolateGridLinePoints();
	createGridsNode();
	auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;

	m_roadmap = GridPointMap;

	if (m_roadmap.size() == 0)
	{
		return;
	}
	kmeansSamples.clear();
	kmeansLabels.clear();

	std::vector<std::vector<int>>FloodGrids;
	for (int i = 0; i < m_roadmap.size(); i++)
	{
		std::vector<int>line;
		for (int j = 0; j < m_roadmap[i].size(); j++)
		{
			double lon = m_roadmap[i][j]->coordinateX();
			double lat = m_roadmap[i][j]->coordinateY();
			QPointF point = QPointF(lon, lat);
			vector<double> cord = { lon,lat,cruiseHeight };
			kmeansSamples.push_back(cord);
			bool is_inside = false;

			for (auto area : noflyZonePointVec)
			{
				if (is_point_in_zone(point, area))
				{
					is_inside = true;
					break;
				}
			}

			if (is_inside == false)
			{

				short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(lon, lat);
				if (elevationTemp < -10000)
				{
					elevationTemp = 0;
				}
				if (elevationTemp > cruiseHeight)
				{
					std::vector<int> gid = { 0 };
					FloodFill_grid.push_back(gid);
					line.push_back(0);
				}
				else
				{
					std::vector<int> gid = { 1 };
					FloodFill_grid.push_back(gid);
					line.push_back(1);
				}
			}
			else
			{
				std::vector<int> gid = { 0 };
				FloodFill_grid.push_back(gid);
				line.push_back(0);
			}
		}
		FloodGrids.push_back(line);
	}

    clusterNum = getClusterCount();
    if (clusterNum > uavNum)
    {
        clusterNum = uavNum;
    }

	std::vector<std::vector<double>>selectData = getClusterPoints(clusterNum);

	std::vector<int> dimVec;
	dimVec.clear();
	QGeoCoordinate point1(gridSidePoint[0][1], gridSidePoint[0][0]);
	QGeoCoordinate point4(gridSidePoint[3][1], gridSidePoint[3][0]);
	int angle1to4 = point1.azimuthTo(point4) + 2;
	if (fabs(angle1to4 % 180) < 10)
	{
		dimVec.push_back(0);
	}
	else
	{
		dimVec.push_back(1);
	}

	KMeans kmeans(dimVec, clusterNum, 3);
	kmeans.setInitMode(KMeans::INIT_MANUAL);
	kmeans.cluster(kmeansSamples, selectData, kmeansLabels);

	std::vector<std::vector<std::vector<double>>>kmeansAreas;
	int Lcount = FloodGrids.size();
	int Wcount = FloodGrids[0].size();

	for (int i = 0; i < clusterNum; i++)
	{

		std::vector<std::vector<double>>chord;
		for (int j = 0; j < kmeansSamples.size(); j++)
		{
			if (kmeansLabels[j] == i)
			{
				chord.push_back(kmeansSamples[j]);
			}
		}
		kmeansAreas.push_back(chord);
	}

	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> allAreaPlanNode;

	for (int i = 0; i < kmeansAreas.size(); i++)
	{
		std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>oneAreaPlanNode;
		for (int j = 0; j < kmeansAreas[i].size(); j++)
		{

			for (int m = 0; m < m_roadmap.size(); m++)
			{
				int curIndex = -1;
				for (int n = 0; n < m_roadmap[0].size(); n++)
				{
					double lon = m_roadmap[m][n]->coordinateX();
					double lat = m_roadmap[m][n]->coordinateY();
					if (lon == kmeansAreas[i][j][0] && lat == kmeansAreas[i][j][1])
					{
						curIndex = n;
						break;
					}
				}
				if (curIndex != -1)
				{
					int is_nofly = FloodGrids[m][curIndex];
					shared_ptr<AreaSearchAlgorithm::PlanNode> tmpNodePointer = m_roadmap[m][curIndex];
					tmpNodePointer->setAreaAttribute(is_nofly);
					oneAreaPlanNode.push_back(tmpNodePointer);
					break;
				}
			}
		}
		allAreaPlanNode.push_back(oneAreaPlanNode);
	}

	std::vector<std::vector<std::vector<int>>>FloodAreaGirds;
	std::vector<std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>>> FloodPlanNode;
	for (auto oneAreaPlaneNode : allAreaPlanNode)
	{

		casic_vector2d minPos = getMinPlanNodeIndex(oneAreaPlaneNode);
		casic_vector2d maxPos = getMaxPlanNodeIndex(oneAreaPlaneNode);
		std::vector<std::vector<int>>oneAreaFlood;
		std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>>oneAreaPlanNode;
		for (int row = minPos.x; row <= maxPos.x; row++)
		{
			std::vector<int>onePosFlood;
			std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>onePosPlanNode;
			for (int col = minPos.y; col <= maxPos.y; col++)
			{
				auto tmpNode = m_roadmap[row][col];
				onePosFlood.push_back(tmpNode->getAreaAttribute());
				onePosPlanNode.push_back(tmpNode);
			}
			oneAreaFlood.push_back(onePosFlood);
			oneAreaPlanNode.push_back(onePosPlanNode);
		}
		FloodAreaGirds.push_back(oneAreaFlood);
		FloodPlanNode.push_back(oneAreaPlanNode);
	}
	std::vector<int>areaNumber;
	for (int i = 0; i < FloodAreaGirds.size(); i++)
	{
		auto grid = FloodAreaGirds[i];

		int row = grid.size();
		if (row == 0) {
			continue;
		}
		int col = grid[0].size();
		Floodfill floodFill(grid);

		int regionNum = floodFill.getRegionNum();
		if (regionNum > 1)
		{
			areaNumber.push_back(i);
		}
	}

	if (FloodAreaGirds.size() > 1)
	{
		auto itor = FloodAreaGirds.begin();
		int lineNum = itor->size();
		for (itor;itor != FloodAreaGirds.end();itor++)
		{
			if (itor->size() != lineNum)
			{
				for (int m = itor->size();m < lineNum;m++)
				{
					std::vector<int>onePosFlood = {0};
					itor->push_back(onePosFlood);
				}
			}
		}
	}

	for (int i = 0; i < areaNumber.size(); i++)
	{
		int number = areaNumber[i];
		if (number == FloodAreaGirds.size() -1 )
		{
			if (FloodAreaGirds.size() >1)
			{
				auto pointsV0 = FloodAreaGirds[number];
				auto pointsV1 = FloodAreaGirds[number - 1];
				for (int i = FloodAreaGirds[number].size(); i <  FloodAreaGirds[number].size() + FloodAreaGirds[number - 1].size(); i++)
				{
					auto line = FloodAreaGirds[number - 1][FloodAreaGirds[number - 1].size() - 1];
					FloodAreaGirds[number].emplace(FloodAreaGirds[number].begin(), line);
					auto line_Pos = FloodPlanNode[number - 1][FloodPlanNode[number - 1].size() - 1];
					FloodPlanNode[number].emplace(FloodPlanNode[number].begin(), line_Pos);

					int row = FloodAreaGirds[number].size();
					if (row == 0) {
						continue;
					}
					int col = FloodAreaGirds[number][0].size();
					Floodfill floodFill(FloodAreaGirds[number]);

					int regionNum = floodFill.getRegionNum();
					if (regionNum > 1)
					{
						FloodAreaGirds[number - 1].pop_back();
						FloodPlanNode[number - 1].pop_back();
					}
					else
					{
						FloodAreaGirds[number - 1].pop_back();
						FloodPlanNode[number - 1].pop_back();
						break;
					}
				}
			}
		}
		else
		{
			if (FloodAreaGirds.size() > 1)
			{
				auto pointsV0 = FloodAreaGirds[number];
				auto pointsV1 = FloodAreaGirds[number + 1];
				for (int i = FloodAreaGirds[number].size(); i < FloodAreaGirds[number].size() + FloodAreaGirds[number + 1].size(); i++)
				{
					auto line = FloodAreaGirds[number + 1][0];
					FloodAreaGirds[number].push_back(line);

					auto line_Pos = FloodPlanNode[number + 1][0];
					FloodPlanNode[number].push_back(line_Pos);

					int row = FloodAreaGirds[number].size();
					if (row == 0) {
						continue;
					}
					int col = FloodAreaGirds[number][0].size();
					Floodfill floodFill(FloodAreaGirds[number]);

					int regionNum = floodFill.getRegionNum();
					if (regionNum > 1)
					{
						FloodAreaGirds[number + 1].erase(FloodAreaGirds[number + 1].begin());
						FloodPlanNode[number + 1].erase(FloodPlanNode[number + 1].begin());
					}
					else
					{
						FloodAreaGirds[number + 1].erase(FloodAreaGirds[number + 1].begin());
						FloodPlanNode[number + 1].erase(FloodPlanNode[number + 1].begin());
						break;
					}
				}
			}
		}

	}
	if (areaNumber.size() != 0)
	{
		kmeansSamples.clear();
		kmeansLabels.clear();
		clusterNum = FloodAreaGirds.size();
		for (int areaNum = 0; areaNum < FloodAreaGirds.size(); areaNum++)
		{
			auto oneArea = FloodAreaGirds[areaNum];
			for (int i = 0; i < oneArea.size(); i++)
			{
				for (int j = 0; j < oneArea[i].size(); j++)
				{
					auto tmpNode = FloodPlanNode[areaNum][i][j];
					vector<double> cord = { tmpNode->coordinateX(),tmpNode->coordinateY(),cruiseHeight };
					kmeansSamples.push_back(cord);
					kmeansLabels.push_back(areaNum);
				}
			}
		}
	}

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKmeansArea();

	if (!m_pAutoPlanning) {
		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("区域搜索规划-区域聚类")));
	}
}

void RoutePlanningDialog::on_push_GetArea_clicked()
{
	isGetUp = !isGetUp;
	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	isAddNoFly = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}
	isDelKeyArea = false;
	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");
}

void RoutePlanningDialog::on_pushAddNoFly_clicked()
{

	isAddNoFly = !isAddNoFly;
	isGetUp = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}
	isDelKeyArea = false;
	if (isAddNoFly)
	{
		ui->pbtnAddnoFlyArea->tr("Add complete");
	}
	else
	{
		ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DNoFlyArea();
	}

	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}

	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");

	if (oneNoFlyZonePoint.size() > 0)
	{
		NoFlyZonePoints.push_back(oneNoFlyZonePoint);
		oneNoFlyZonePoint.clear();
	}
}

void RoutePlanningDialog::on_pushDelNoFly_clicked()
{
	isDelNoFly = !isDelNoFly;
	if (isDelNoFly)
	{
		ui->pbtnDeletenoflyArea->tr("Delete complete");
	}
	else
	{
		ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	}

	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}

	isGetUp = false;
	isAddNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}
	isDelKeyArea = false;
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DNoFlyArea();
}

void RoutePlanningDialog::on_pushSaveNoFly_clicked()
{
	if (NoFlyZonePoints.empty())
	{

		LyMessageBox::information(NULL, tr("Prompt"), tr("The no fly area is empty, no need to save it!"));
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, tr("file"),
        "../data",
		tr("XML Files (*.xml)"));
	std::string cStr = fileName.toLocal8Bit().data();
	const char* ofileName = cStr.c_str();

	WriteNoFlyArea2File(ofileName, NoFlyZonePoints);
}

void RoutePlanningDialog::on_pushImportNoFly_clicked()
{

	QString fileName = QFileDialog::getOpenFileName(this, tr("file"),
        "../data",
		tr("XML Files (*.xml)"));
    std::string cStr = fileName.toLocal8Bit().data();
	const char* ifileName = cStr.c_str();
	std::vector<std::vector<QPointF>> ImportAreas;
	ImportAreas = readNoFlyAreaFromFile(ifileName);
	for (int i = 0; i < ImportAreas.size(); i++)
	{
		NoFlyZonePoints.emplace_back(ImportAreas[i]);
	}
}

void RoutePlanningDialog::on_pushStartPoint_clicked()
{
	isPushStartPoint = !isPushStartPoint;
	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	isGetUp = false;
	isAddNoFly = false;
	isDelNoFly = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}
	isDelKeyArea = false;
	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");

}

void RoutePlanningDialog::on_cbximportanceLevel_Changed(const QString &)
{
	QString tmpLevel = ui->cbximportanceLevel->currentText();
	if (ui->cbximportanceLevel->findText(tmpLevel) == -1)
	{
		ui->cbximportanceLevel->addItem(tmpLevel);
	}
	KeyAreaNum = tmpLevel.toDouble();

}

void RoutePlanningDialog::on_pushAddImpArea_clicked()
{
	isAddKeyArea = !isAddKeyArea;
	if (isAddKeyArea)
	{
		ui->pbtnAddImpArea->tr("Add complete");
	}
	else
	{
		finishaddKeyAreaPoint();
		ui->pbtnAddImpArea->tr("AddImpArea");
	}

	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	isGetUp = false;
	isAddNoFly = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	isDelKeyArea = false;
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnDelImpArea->tr("DelImpArea");
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();
}

void RoutePlanningDialog::on_pushDelImpArea_clicked()
{
	isDelKeyArea = !isDelKeyArea;
	if (isDelKeyArea)
	{
		ui->pbtnDelImpArea->tr("Delete complete");
	}
	else
	{
		ui->pbtnDelImpArea->tr("DelImpArea");
	}

	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	isGetUp = false;
	isAddNoFly = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();
}

void RoutePlanningDialog::on_pushSaveImpArea_clicked()
{
	if (KeyAreaVec.empty() && KeyAreas.empty())
	{

		LyMessageBox::information(NULL, tr("Prompt"), tr("Important area is empty, no need to save!"));

		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, tr("file"),
        "../data",
		tr("XML Files (*.xml"));
    std::string cStr = fileName.toLocal8Bit().data();
	const char* ofileName = cStr.c_str();

	WriteKeyArea2File(ofileName, KeyAreaVec, KeyAreas);
}

void RoutePlanningDialog::on_pushImportImpArea_clicked()
{

	QString fileName = QFileDialog::getOpenFileName(this, tr("file"),
        "../data",
		tr("XML Files (*.xml)"));
	std::string cStr = fileName.toLocal8Bit().data();
	const char* ifileName = cStr.c_str();
	std::vector<std::vector<QPointF>> ImportAreas;
	std::vector<int> keyNums;
	ImportAreas = readKeyAreaFromFile(ifileName, keyNums);
	for (int i = 0; i < ImportAreas.size(); i++)
	{
		KeyAreaVec.emplace_back(ImportAreas[i]);
		int index = KeyAreaVec.size() - 1;
		std::pair<bool, int> sec(false, keyNums[i]);
		KeyAreas[index] = sec;
	}

	setKeyAreaXyHcost();
}

void RoutePlanningDialog::on_pushClearAll()
{

	AreaSearchPointMap.clear();
	gridLinesPoints.clear();
	NoFlyZonePoints.clear();
	obstacleArea.clear();
	oneKeyAreaPoints.clear();
	keyXYPoint.clear();
	KeyAreas.clear();
	gridsidePoints.clear();

	GridPointMap.clear();

	draw3DMarkArea();
	draw3DGrid();
	kmeansSamples.clear();
	kmeansLabels.clear();
	draw3DObstacleArea();
	drawKeyArea3D();

	del3DPathLine();
	deleteKmeansArea();
	delCoverArea();

	b_isStart = false;

	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog != nullptr )
	{
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->on_pbtnStop_clicked();
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->shadowCube.clear();
	}
	b_startDraw = false;
	draw3DCoverCube();

	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("区域搜索规划-清空")));
}

void RoutePlanningDialog::on_pbthDisplay_clicked()
{
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->isHidden())
	{
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->show();
	}
	else
	{
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_routePlanningPlayDialog->hide();
	}

}

void RoutePlanningDialog::on_timeOut()
{
	for (auto itrlie : m_vecCalculatePoint)
	{
		auto itrPoint = itrlie.stTelemetryPointVec.begin();
		if (m_nPlayTime < itrlie.stTelemetryPointVec.size())
		{
			itrPoint += m_nPlayTime;

			const std::string& vehicleID = itrlie.strWeaponID;
			telemetryInformation telemetryInfo;
			telemetryInfo = *itrPoint;

			xygsJL_20 bag;
			bag.flags = 20;
			bag.param = telemetryInfo;
#if 0

			if (ITaskManagerActivator::Get().getIRegistryImpl())
			{
				qnzkna::framework::MessageReferenceProps props;
				props.put("ChannelID", "ChannelID");
				props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
                props.put("TargetID", vehicleID);
				ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVRealTimeState", (char*)&bag, sizeof(bag), props));
			}
#else

			TelemetryInformation_t stTelemetryInformation_t;
			memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
			stTelemetryInformation_t.lat = telemetryInfo.position.lat* 1e7;
			stTelemetryInformation_t.lng = telemetryInfo.position.lng* 1e7;
			stTelemetryInformation_t.alt = telemetryInfo.gpsHeigth * 10;
			stTelemetryInformation_t.angleofNose = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t.angleofRudder = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t._rudder = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t.targetVelocity = telemetryInfo.groundVelocity * 100;
			stTelemetryInformation_t.groundVelocity = telemetryInfo.groundVelocity * 100;

			for (auto IPItr : m_UAVServerIPVec)
			{
				std::string strName = IPItr.strName.toStdString();
				if (0 == strcmp(strName.c_str(), vehicleID.c_str()))
				{
					QHostAddress stQHostAddress;
					stQHostAddress.setAddress(IPItr.strIP);
					m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), stQHostAddress, IPItr.nPort, IPItr.nPort);
					break;
				}
			}
#endif
		}
	}
	m_nPlayTime++;

}

void RoutePlanningDialog::LoadIPIni()
{
	m_UAVServerIPVec.clear();
	std::string strPath = "../data/PlaySendToServer.ini";
	std::ifstream inFile(strPath);
	std::string oneLine;
	std::stringstream ss;
	QStringList nameList, ipList;
	while (getline(inFile, oneLine))
	{
		std::string name, ip, port;
		ss = std::stringstream(oneLine);
		ss >> name >> ip >> port;

		UAVServerIP stUAVServerIPVec;
		stUAVServerIPVec.strIP = QString::fromStdString(ip);
		stUAVServerIPVec.strName = QString::fromStdString(name);
		stUAVServerIPVec.nPort = QString::fromStdString(port).toInt();
		m_UAVServerIPVec.push_back(stUAVServerIPVec);
	}
}

void RoutePlanningDialog::drawcoverslot()
{
	b_startDraw = true;
	draw3DCoverCube();
}

void RoutePlanningDialog::on_pushArticModify_clicked()
{
	LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
}

void RoutePlanningDialog::on_pbtnLoadMission_Clicked()
{
	LY_CollaborativeSearchService::Get().m_mission_load_dialog->init();
	//LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 433001, nullptr);
	if (LY_CollaborativeSearchService::Get().m_mission_load_dialog)
	{
		if(LY_CollaborativeSearchService::Get().m_mission_load_dialog->isHidden())
			LY_CollaborativeSearchService::Get().m_mission_load_dialog->show();
	}
        ui->ledtCurposLon->setText(QString::number(116.19663670, 'f', 8));
        ui->ledtCurposLat->setText(QString::number(40.26477610, 'f', 8));
        ui->ledtCurposHei->setText(QString::number(110.00, 'f', 2));

	ui->ledtspeed->setText(QString::number(15));
	ui->ledtflyTime->setText(QString::number(700));
	ui->ledtAngularSpeed->setText(QString::number(150));
	ui->ledtUpSpeed->setText(QString::number(1));
	ui->ledtuavNum->setText(QString::number(1));

	QPalette palette_gray;
	palette_gray.setColor(QPalette::Text, Qt::gray);
	ui->ledtPicLength->setText(QString::number(36));
	ui->ledtPicLength->setReadOnly(true);
	ui->ledtPicLength->setEnabled(false);
	ui->ledtPicLength->setPalette(palette_gray);

	ui->ledtPicWidth->setText(QString::number(24));
	ui->ledtPicWidth->setReadOnly(true);
	ui->ledtPicWidth->setEnabled(false);
	ui->ledtPicWidth->setPalette(palette_gray);

	ui->cbdiaocang->addItem(tr2("可见光"));
	ui->cbdiaocang->setCurrentText(tr2("可见光"));

	ui->ledtFocalDis->setText(QString::number(26));

	autoPlanning();

	ui->pbtnTakeOff->setEnabled(true);
	ui->pbtnGetArea->setEnabled(true);
	ui->pbtnKmeans->setEnabled(true);
	ui->pbtnPlanning->setEnabled(true);
	ui->pbthArticModify->setEnabled(true);
	ui->pbtnAirLineUpLoad->setEnabled(true);
	ui->pbthDisplay->setEnabled(true);
	ui->pbtnClearAll->setEnabled(true);
}

void RoutePlanningDialog::on_pbtnRouteUpLoad_clicked()
{
	if (!AreaSearchPointMap.empty())
	{
		if (m_pIZoneManageService == nullptr) {
			return;
		}
		
		CAirTrafficLine stLine;
		stLine.nID = m_pIZoneManageService->GetAirTrafficLineMaxID();
		QString strTmp = tr2("HX") + QString::number(stLine.nID);
		strncpy(stLine.strName, strTmp.toLocal8Bit(), MAX_AIR_LEN_1);
		QString strUAVID = ui->comboBox_clooseEquip->currentText();
		double dspeed = ui->ledtspeed->text().toDouble();
		strncpy(stLine.strUAVID, strUAVID.toLocal8Bit(), MAX_AIR_LEN_1);

		int nPontID = m_pIZoneManageService->GetAirTrafficLinePointMaxID();

		int nCount = 0;

		for (const auto& routeline : AreaSearchPointMap) {
			for (const auto& line : routeline.second)
			{
				CAirTrafficLinePoint stPoint;
				stPoint.nNo = nPontID + nCount;
				stPoint.nLineID = stLine.nID;
				stPoint.dLon = line.x;
				stPoint.dLat = line.y;
				stPoint.dAlt = line.z;
				stPoint.dSpeed = dspeed;
				stPoint.dHoverTime = 0;
				stLine.LinePointVec.push_back(stPoint);
				nCount++;
			}
		}
		m_pIZoneManageService->AddAirTrafficLine(stLine);
	}
	struct FireProt
	{
		bool UpdateRoute = true;
		bool UpDownRroute = true;
	}prot;

	void *lParam = (void *)(&prot);
	if (lParam == nullptr) {
		return;
	}
	LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
	//LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_MAP_IDENTIFY_MENU + 100, 50009, lParam);

	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("区域搜索规划-航线上传")));
}

void RoutePlanningDialog::minAngleText_isChanged()
{
	minAngle = ui->ledtMinAngle->text().toInt();
}

void RoutePlanningDialog::maxAngleText_isChanged()
{
	maxAngle = ui->ledtMaxAngle->text().toInt();
	double maxDis = calcolateMaxHeight();
	double dis = calSpyDis(ui->ledtUavCruiseHeight->text().toDouble(), maxAngle);
	if (dis > maxDis)
	{
		maxAngle = int(acos(ui->ledtUavCruiseHeight->text().toDouble() / maxDis) * (180 / PI) - 90) - 2;
		QString warnStr;
		warnStr = tr("The overhead angle is too large and the resolution is limited.The recommended overhead angle is:") + QString::number(maxAngle);
		QMessageBox::warning(this, tr("Prompt"), warnStr);
		ui->ledtMaxAngle->setText(QString::number(maxAngle));
	}
}

void RoutePlanningDialog::on_pbtnPlanning_clicked()
{
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calcolateGridLinePoints();

	gridwidth = calcolateGrid();
	auto GridsMap = createGridsNode();

	on_pbtnKmeans_clicked();

	_CoveredAreaVec.clear();
	ObstacleVec1.clear();
	isAddNoFly = false;
	isDelNoFly = false;
	isPushStartPoint = false;
	if (isAddKeyArea)
	{
		finishaddKeyAreaPoint();
		isAddKeyArea = false;
	}

	isDelKeyArea = false;
	if (isGetUp)
	{
		ui->pbtnGetArea->setChecked(true);
	}
	else
	{
		ui->pbtnGetArea->setChecked(false);
	}

	if (isPushStartPoint)
	{
		ui->pbtnTakeOff->setChecked(true);
	}
	else
	{
		ui->pbtnTakeOff->setChecked(false);
	}
	ui->pbtnDeletenoflyArea->tr("DeletenoflyArea");
	ui->pbtnAddnoFlyArea->tr("AddnoFlyArea");
	ui->pbtnAddImpArea->tr("AddImpArea");
	ui->pbtnDelImpArea->tr("DelImpArea");

	if (ui->ledtCurposLon->text().isEmpty() || ui->ledtCurposLat->text().isEmpty() || ui->ledtUavCruiseHeight->text().isEmpty()
		|| ui->ledtspeed->text().isEmpty() || ui->ledtflyTime->text().isEmpty() || ui->ledtFocalDis->text().isEmpty()
		|| ui->ledtLeftUpLon->text().isEmpty() || ui->ledtLeftUpLat->text().isEmpty()||
		ui->ledtRightDownLon->text().isEmpty() || ui->ledtRightDownLat->text().isEmpty()|| ui->letdCoverRate->text().isEmpty())
	{
		return;
	}
    double lon_curpos = QString(ui->ledtCurposLon->text()).toDouble();
    double lat_curpos = QString(ui->ledtCurposLat->text()).toDouble();

    cruiseHeight = QString(ui->ledtUavCruiseHeight->text()).toDouble();
	cruiseSpeed = QString(ui->ledtspeed->text()).toDouble();
	angularSpeed = QString(ui->ledtAngularSpeed->text()).toDouble();
	flyTime = QString(ui->ledtflyTime->text()).toDouble();
	safeDistance = QString(ui->ledtsafeDis->text()).toDouble();
	uavNum = QString(ui->ledtuavNum->text()).toDouble();

	pic_Length = QString(ui->ledtPicLength->text()).toDouble();
	pic_width = QString(ui->ledtPicWidth->text()).toDouble();
	focal_dis = QString(ui->ledtFocalDis->text()).toDouble();
	heading_overlap = QString(ui->ledtHeadingOverlap->text()).toDouble();
	side_overlap = QString(ui->ledtSideOverlap->text()).toDouble();

	coverRate = QString(ui->letdCoverRate->text()).toDouble();

	double lon_leftup, lat_leftup, lon_rightdown, lat_rightdown;
	if (targetLeftUp.x() == 0 && targetLeftUp.y() == 0)
	{
		lon_leftup = QString(ui->ledtLeftUpLon->text()).toDouble();
		lat_leftup = QString(ui->ledtLeftUpLat->text()).toDouble();
		lon_rightdown = QString(ui->ledtRightDownLon->text()).toDouble();
		lat_rightdown = QString(ui->ledtRightDownLat->text()).toDouble();
	}
	else
	{
		lon_leftup = targetLeftUp.x();
		lat_leftup = targetLeftUp.y();
		lon_rightdown = targetRightDown.x();
		lat_rightdown = targetRightDown.y();
	}

	realCoverRate = 0;
	remainFlyTime = 0;
	stagingPoint = casic_vector3d(0, 0, 0);
	currTakeOffPoint = casic_vector3d(0, 0, 0);

	curpos = casic_vector3d(lon_curpos, lat_curpos, cruiseHeight);
	leftup = casic_vector3d(lon_leftup, lat_leftup, cruiseHeight);
	rightdowm = casic_vector3d(lon_rightdown, lat_rightdown, cruiseHeight);
	//int planway = ui->ledtuavNum->text().toInt();
	int planway = 0;
	if (minAngle != maxAngle)
	{
		AreaSearchAlgorithm m_areasearch1;
		m_areasearch1.SpySideAreaSerachPlan(leftup, rightdowm, curpos, cruiseHeight, gridwidth, cruiseSpeed, angularSpeed, flyTime, coverRate, safeDistance,
			minAngle, maxAngle, uavNum, 5, AreaSearchPointMap);
	}
	else
	{
		if (kmeansLabels.size() != 0)
		{
			AreaSearchAlgorithm m_areasearch;
			m_areasearch.allGridsMaps.resize(clusterNum + 1);
			m_areasearch.allObstacleArea.resize(clusterNum + 1);
			AreaSearchPointMap.clear();
			searchStartReturnPointMap.clear();

			for (int i = 0; i < kmeansLabels.size(); i++)
			{
				double lon = kmeansSamples[i][0];
				double lat = kmeansSamples[i][1];
				for (int xIndex = 0; xIndex < GridPointMap.size(); xIndex++)
				{
					for (int yIndex = 0; yIndex < GridPointMap[xIndex].size(); yIndex++)
					{
						auto nowNodeItor = GridPointMap[xIndex][yIndex];
						if (nowNodeItor->coordinateX() == lon && nowNodeItor->coordinateY() == lat)
						{
							nowNodeItor->setAreaAttribute(kmeansLabels[i]);
							xIndex = GridPointMap.size();
							break;
						}
					}
				}
			}

			for (int i = 0; i < clusterNum; i++)
			{
				std::map<int, std::vector<casic_vector3d>> tmpMap;
				int pointsNum = 0;
				for (auto labelPoints : kmeansLabels)
				{
					if (labelPoints == i)
					{
						pointsNum++;
					}
				}

				std::map<int, std::vector<casic_vector3d>> startReturnPointMap;

				m_areasearch.PolygonAreaSerachPlan1(curpos, cruiseHeight, gridwidth, cruiseSpeed, angularSpeed,
					upSpeed, flyTime, coverRate, safeDistance, i, pointsNum, tmpMap, planway, startReturnPointMap);
				_CoveredAreaVec.insert(_CoveredAreaVec.end(), m_areasearch._coverAreaVec.begin(), m_areasearch._coverAreaVec.end());

				double planeIndex = AreaSearchPointMap.size();
				for (auto pointMap : tmpMap)
				{
					std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(planeIndex + pointMap.first, pointMap.second);
					AreaSearchPointMap.insert(mpair);
				}
				for (auto pm : startReturnPointMap) {
					std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(planeIndex + pm.first, pm.second);
					searchStartReturnPointMap.insert(mpair);
				}
			}
			realCoverRate = realCoverRate / (GridsMap.size()*GridsMap[0].size());
			if (realCoverRate > 1)
			{
				realCoverRate = 1;
			}

		}
		else
		{

			AreaSearchPointMap.clear();
			AreaSearchAlgorithm m_areasearch1;
			m_areasearch1.AreaSerachPlan(leftup, rightdowm, curpos, cruiseHeight, gridwidth, cruiseSpeed, angularSpeed, flyTime, coverRate, safeDistance, AreaSearchPointMap);
			_CoveredAreaVec = m_areasearch1._coverAreaVec;

		}
	}

	ui->ledtAreaLength->setText(QString::number(areaLength));
	ui->ledtAreaWidth->setText(QString::number(areaWidth));
	ui->ledtgridWidth->setText(QString::number(cameraGridWidth));
	ui->ledtRealCoverRate->setText(QString::number(realCoverRate));
	ui->ledtFlyTimeRemain->setText(QString::number(remainFlyTime));

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DPathLine(AreaSearchPointMap, QColor(0, 229, 238), 2);

	if (minAngle == maxAngle) {
		sparsePath(AreaSearchPointMap);
	}

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DNoFlyArea();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->draw3DObstacleArea();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->drawKeyArea3D();

	if (!m_pAutoPlanning) {
		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("区域搜索规划-航线规划")));
	}
}

void RoutePlanningDialog::draw3DFireLine(double dLon, double dLat, double dHeight, double heading, double speed)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	if (FireLinetrack != nullptr)
	{
		pEarth->GetGraphics()->RemoveObject(FireLinetrack);
	}
	FireLinetrack = nullptr;
	IUserMesh2 *pUM = nullptr;
	if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
	qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLon, dLat, dHeight, 0, dHeight, 0, ccipdata_80mmlist);

	double x = 0, y = 0, z = 0;

	const unsigned long long ccipdata_80mmlistSize = ccipdata_80mmlist.size();
	if (pUM != nullptr && ccipdata_80mmlistSize > 0 && ccipdata_80mmlistSize < 10000)
	{
		const auto &ccipdata_80mmlistLastData = ccipdata_80mmlist.back();
		x = ccipdata_80mmlistLastData.y;
		y = ccipdata_80mmlistLastData.x;
		z = dHeight - std::fabs(ccipdata_80mmlistLastData.zd);

		const double dAlt = std::fabs(ccipdata_80mmlistLastData.zd);

		int nI = 0;

		const int ccipdata_80mmlistSizeSumVLine = 5;
		const int ccipdata_80mmlistSizeStep = (ccipdata_80mmlistSize / ccipdata_80mmlistSizeSumVLine + 1);

		std::vector<VERTEX> pVerter;

		{
			for (const auto &data : ccipdata_80mmlist)
			{

				if (nI % ccipdata_80mmlistSizeStep == 0)
				{
					pVerter.push_back(VERTEX{ 0, 0, 0 });
					pVerter[nI].x = 0;
					pVerter[nI].y = data.x;
					pVerter[nI].z = data.zd;
					nI++;

					pVerter.push_back(VERTEX{ 0, 0, 0 });
					pVerter[nI].x = 0;
					pVerter[nI].y = data.x;
					pVerter[nI].z = -dAlt;
					nI++;
				}

				pVerter.push_back(VERTEX{ 0, 0, 0 });
				pVerter[nI].x = 0;
				pVerter[nI].y = data.x;
				pVerter[nI].z = data.zd;
				nI++;
			}

			if (nI > 0)
			{
				pUM->SetPoints(nI, pVerter.data(), IUserMesh::USERMESH_LINES);
			}
			pVerter.clear();

			pUM->SetState(IUserMesh::POINT, IUserMesh::USERMESH_LINE);
			pUM->SetState(IUserMesh::LINEWIDTH, 2);
			pUM->SetState(IUserMesh::LINESTIPPLE, 5);
			FireLinetrack = pUM;
		}
	}
	if (FireLinetrack != nullptr)
	{
		QColor colorTemp2(0, 0, 255, 200);
		FireLinetrack->SetPosition(dLon, dLat,
			dHeight);
		FireLinetrack->SetYawPitchRoll(heading, 0, 0);
		FireLinetrack->SetColor(colorTemp2.rgba());
		FireLinetrack->SetVisible(true);
	}

}

std::vector<casic_vector3d> RoutePlanningDialog::generateLinePoints(double dLon, double dLat, double dHeight, double heading, double speed)
{
	std::vector<casic_vector3d>tempVec;
	QGeoCoordinate currPos(dLat, dLon);
	double H = dHeight;

	return tempVec;
}

