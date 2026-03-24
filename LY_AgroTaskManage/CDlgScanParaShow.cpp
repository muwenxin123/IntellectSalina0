#include "CDlgScanParaShow.h"
#include "ui_CDlgScanParaShow.h"
#include "QCString.h"
#include "IAgroTaskManagerActivator.h"
#include "LyMessageBox.h"
#include "QGeoCoordinate"
#include "gridbasesweepplanner.h"
#include "LY_AgroTaskManage.h"
#include "QPen"
#include "QColor"
#include "LY_AgroTaskManageInterface.h"

using namespace qnzkna::AgroTask;
using namespace qnzkna::ZoneManage;

CDlgScanParaShow::CDlgScanParaShow(QWidget *parent) :
	LyDialog(tr2("规则内容"), parent),
	ui(new Ui::CDlgScanParaShow)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(800, 900);

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();
	m_pIZoneManageService = IAgroTaskManagerActivator::Get().getIZoneManageService();

	connect(ui->pushButton, &QPushButton::clicked, this, &CDlgScanParaShow::on_pushButton_clicked);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &CDlgScanParaShow::on_pushButton_2_clicked);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &CDlgScanParaShow::on_pushButton_3_clicked);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &CDlgScanParaShow::on_pushButton_4_clicked);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

CDlgScanParaShow::~CDlgScanParaShow()
{
    delete ui;
}

void CDlgScanParaShow::set_name(const QString &name)
{
	ui->label_3->setText(name);
}

void CDlgScanParaShow::set_explanation(const QString &explanation)
{
	ui->textEdit->setText(explanation);
}

void CDlgScanParaShow::on_pushButton_clicked()
{
	isShow_path = !isShow_path;
}

void CDlgScanParaShow::on_pushButton_2_clicked()
{
	path_plan(m_zoneName);
}

void CDlgScanParaShow::on_pushButton_3_clicked()
{
	if (m_pIZoneManageService == nullptr) {
		return;
	}

	CAirTrafficLine stLine;
	stLine.nID = m_pIZoneManageService->GetAirTrafficLineMaxID();
	QString strTmp = tr2("HX") + QString::number(stLine.nID);
	strncpy(stLine.strName, strTmp.toLocal8Bit(), MAX_AIR_LEN_1);
	QString strUAVID = "110303";
	strncpy(stLine.strUAVID, strUAVID.toLocal8Bit(), MAX_AIR_LEN_1);

	int nHeight = ui->spinBox->value();
	int nSpeed = ui->spinBox_2->value();
	int nPontID = m_pIZoneManageService->GetAirTrafficLinePointMaxID();

	int nCount = 0;
	int poiSize = std::min(path_poix.size(), path_poiy.size());
	for (int i = 0; i < poiSize; i++) {
		CAirTrafficLinePoint stPoint;
		stPoint.nNo = nPontID + nCount;
		stPoint.nLineID = stLine.nID;
		stPoint.dLon = path_poix[i];
		stPoint.dLat = path_poiy[i];
		stPoint.dAlt = nHeight;
		stPoint.dSpeed = nSpeed;
		stPoint.dHoverTime = 0;

		stLine.LinePointVec.push_back(stPoint);
		nCount++;
	}

	m_pIZoneManageService->AddAirTrafficLine(stLine);

	LY_AgroTaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);

}

void CDlgScanParaShow::on_pushButton_4_clicked()
{
	this->close();
}

void CDlgScanParaShow::path_plan(const QString &zoneName)
{
	m_zoneName = zoneName;
	CZoneBase curZone;
	bool success = m_pIZoneManageService->GetZoneBase(zoneName, curZone);
	if (!success) {
		LyMessageBox::warning(this, tr2("提示"), tr2("获取盐田区域失败！"));
		return;
	}

	std::vector<double> _lons;
	std::vector<double> _lats;
	std::vector<std::vector<QPointF>>foxy;//空
	std::vector<PoiScan> points;

	for (const auto &poi : curZone.stZonePointVec) {
		_lons.push_back(poi.dLon);
		_lats.push_back(poi.dLat);
		points.push_back(PoiScan(poi.dLon, poi.dLat));
	}

	QPointF areaFarPos(_lons.at(0), _lats.at(0));
	double spaceing = ui->lineEdit_3->text().toDouble() *  (1.0 / 110000.0);

	std::vector<double> rx;
	std::vector<double> ry;
	int planWay = ui->comboBox->currentIndex();
	planning(_lons, _lats, foxy, points, areaFarPos, spaceing, rx, ry, planWay);

	//抽稀路线点集
	std::vector<double> newx;
	std::vector<double> newy;
	int poiSize = std::min(rx.size(), ry.size());
	QGeoCoordinate prePoi;
	QGeoCoordinate curPoi;
	double preAngle = 0;
	double curAngle = 0;
	if (poiSize > 1) {
		prePoi = QGeoCoordinate(ry[0], rx[0]);
		curPoi = QGeoCoordinate(ry[1], rx[1]);
		preAngle = curPoi.azimuthTo(prePoi);
		newx.push_back(rx[0]);
		newy.push_back(ry[0]);
		newx.push_back(rx[1]);
		newy.push_back(ry[1]);
		prePoi = curPoi;
	}
	for (int i = 2; i < poiSize; i++) {
		curPoi = QGeoCoordinate(ry[i], rx[i]);
		curAngle = curPoi.azimuthTo(prePoi);
		if (std::fabs(curAngle - preAngle) > 1e-3) {
			int curSize = newx.size();
			if (curSize - 1 >= 0) {
				QGeoCoordinate p1(newy[curSize - 1], newx[curSize - 1]);
				if (p1.distanceTo(prePoi) > 1e-2) {
					newx.push_back(prePoi.longitude());
					newy.push_back(prePoi.latitude());
				}
			}
			newx.push_back(rx[i]);
			newy.push_back(ry[i]);
		}
		prePoi = curPoi;
		preAngle = curAngle;
	}

	int curSize = newx.size();
	if (poiSize - 1 >= 0 && curSize - 1 >= 0) {
		QGeoCoordinate p1(newy[curSize - 1], newx[curSize - 1]);
		QGeoCoordinate p2(ry[poiSize - 1], rx[poiSize - 1]);
		if (p1.distanceTo(p2) > 1e-2) {
			newx.push_back(rx[poiSize - 1]);
			newy.push_back(ry[poiSize - 1]);
		}
	}

	init_widget(newx, newy);

	path_poix = newx;
	path_poiy = newy;
}

std::vector<QPointF> CDlgScanParaShow::getPathPlanPoiVec(const QString & zoneName, double spacing, int planWay)
{
	std::vector<QPointF> poiVec;

	CZoneBase curZone;
	bool success = m_pIZoneManageService->GetZoneBase(zoneName, curZone);
	if (!success) {
		LyMessageBox::warning(this, tr2("提示"), tr2("获取盐田区域失败！"));
		return poiVec;
	}

	std::vector<double> _lons;
	std::vector<double> _lats;
	std::vector<std::vector<QPointF>>foxy;//空
	std::vector<PoiScan> points;

	for (const auto &poi : curZone.stZonePointVec) {
		_lons.push_back(poi.dLon);
		_lats.push_back(poi.dLat);
		points.push_back(PoiScan(poi.dLon, poi.dLat));
	}

	QPointF areaFarPos(_lons.at(0), _lats.at(0));

	std::vector<double> rx;
	std::vector<double> ry;
	spacing = spacing *  (1.0 / 110000.0);
	planning(_lons, _lats, foxy, points, areaFarPos, spacing, rx, ry, planWay);

	//抽稀路线点集
	int poiSize = std::min(rx.size(), ry.size());
	QGeoCoordinate prePoi;
	QGeoCoordinate curPoi;
	double preAngle = 0;
	double curAngle = 0;
	if (poiSize > 1) {
		prePoi = QGeoCoordinate(ry[0], rx[0]);
		curPoi = QGeoCoordinate(ry[1], rx[1]);
		preAngle = curPoi.azimuthTo(prePoi);

		poiVec.push_back(QPointF(rx[0], ry[0]));
		poiVec.push_back(QPointF(rx[1], ry[1]));

		prePoi = curPoi;
	}
	for (int i = 2; i < poiSize; i++) {
		curPoi = QGeoCoordinate(ry[i], rx[i]);
		curAngle = curPoi.azimuthTo(prePoi);
		if (std::fabs(curAngle - preAngle) > 1e-3) {
			int curSize = poiVec.size();
			if (curSize - 1 >= 0) {
				QGeoCoordinate p1(poiVec[curSize - 1].y(), poiVec[curSize - 1].x());
				if (p1.distanceTo(prePoi) > 1e-2) {
					poiVec.push_back(QPointF(prePoi.longitude(), prePoi.latitude()));
				}
			}
			poiVec.push_back(QPointF(rx[i], ry[i]));
		}
		prePoi = curPoi;
		preAngle = curAngle;
	}

	int curSize = poiVec.size();
	if (poiSize - 1 >= 0 && curSize - 1 >= 0) {
		QGeoCoordinate p1(poiVec[curSize - 1].y(), poiVec[curSize - 1].x());
		QGeoCoordinate p2(ry[poiSize - 1], rx[poiSize - 1]);
		if (p1.distanceTo(p2) > 1e-2) {
			poiVec.push_back(QPointF(rx[poiSize - 1], ry[poiSize - 1]));
		}
	}

	return poiVec;

}

bool CDlgScanParaShow::DrawDynamic(QPainter *painter, IProj *pProj)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	
	double x, y;
	float  l, b;
	int ssize = std::min(path_poix.size(), path_poiy.size());
	QVector<QPointF> points;
	for (int i = 0; i < ssize; i++) {
		LY_AgroTaskManage::Get().L_X(path_poix[i], path_poiy[i], &x, &y);
		LY_AgroTaskManage::Get().X_S(x, y, &l, &b);

		points.push_back(QPointF(l, b));
	}

	QPen pen1;
	pen1.setColor(QColor(Qt::blue));
	pen1.setWidth(2);
	painter->setPen(pen1);

	painter->drawPolyline(points);

	painter->restore();

	return true;
}

void CDlgScanParaShow::init_widget(const std::vector<double>& rx, const std::vector<double>& ry)
{
	double alt = ui->spinBox->value();
	double speed = ui->spinBox_2->value();

	QGeoCoordinate prePoi;
	if (rx.size() > 0 && ry.size() > 0) {
		prePoi = QGeoCoordinate(ry[0], rx[0]);
	}

	int rowC = rx.size();
	ui->lineEdit->setText(QString::number(rowC));

	double pathLongth = 0;
	ui->tableWidget->setRowCount(0);
	ui->tableWidget->setRowCount(rowC);
	for (int i = 0; i < rowC; i++) {
		auto item0 = new QTableWidgetItem(QString::number(i + 1));
		item0->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(i, 0, item0);

		auto item1 = new QTableWidgetItem(QString::number(rx[i], 'f', 6));
		item1->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(i, 1, item1);

		if (i < ry.size()) {
			auto item2 = new QTableWidgetItem(QString::number(ry[i], 'f', 6));
			item2->setTextAlignment(Qt::AlignCenter);
			ui->tableWidget->setItem(i, 2, item2);
		}
		
		auto item3 = new QTableWidgetItem(QString::number(alt, 'f', 2));
		item3->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(i, 3, item3);

		auto item4 = new QTableWidgetItem(QString::number(speed, 'f', 2));
		item4->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(i, 4, item4);

		QGeoCoordinate curPoi(ry[i], rx[i]);
		double dis = curPoi.distanceTo(prePoi);
		auto item5 = new QTableWidgetItem(QString::number(dis, 'f', 2));
		item5->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(i, 5, item5);

		pathLongth += dis;
		prePoi = curPoi;
	}

	ui->lineEdit_2->setText(QString::number(pathLongth, 'f', 2));

}

void CDlgScanParaShow::planning(
	std::vector<double>& ox, 
	std::vector<double>& oy, 
	std::vector<std::vector<QPointF>>foxy, 
	std::vector<PoiScan> stZonePointVec,
	QPointF areaFarPos, 
	double resolution, 
	std::vector<double>& rx, 
	std::vector<double>& ry, 
	int planWay)
{
	QGeoCoordinate areaNearGeo;
	int xdis = INT_MAX;
	int idis = 0;
	for (int i = 0; i < stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate areaGeo(stZonePointVec[i].lat, stZonePointVec[i].lon);
		areaNearGeo = areaGeo;
		//for (auto point : minLineZone.stZonePointVec)
		//{
		//	QGeoCoordinate lineGeo(point.dLat, point.dLon);
		//	double tpDIs = lineGeo.distanceTo(areaGeo);
		//	if (tpDIs < xdis)
		//	{
		//		xdis = tpDIs;
		//		lineNearPos = QPointF(point.dLon, point.dLat);
		//		areaNearPos = QPointF(areaGeo.longitude(), areaGeo.latitude());
		//		areaNearGeo = areaGeo;
		//	}
		//}
	}

	QGeoCoordinate areaFarGeo(areaFarPos.y(), areaFarPos.x());
	int moveDirection1, moveDirection2;
	if (planWay == 0)
	{
		/*	double directi = areaNearGeo.azimuthTo(areaFarGeo);
		if (directi >= 0 && directi <= 90)
		{
		moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
		}
		else if (directi > 90 && directi <= 180)
		{
		moveDirection1 = (int)UAVPlanner::MovingDirection::LEFT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
		}
		else if (directi > 180 && directi <= 270)
		{*/
		moveDirection1 = -1;
		moveDirection2 = 1;
		//}
		//else if (directi > 270 && directi <= 360)
		//{
		//	moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		//	moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
		//}
	}
	else if (planWay == 1)
	{
		moveDirection1 = -1;
		moveDirection2 = -1;
	}
	else if (planWay == 2)
	{
		moveDirection1 = 1;
		moveDirection2 = 1;
	}
	else if (planWay == 3)
	{
		moveDirection1 = 1;
		moveDirection2 = -1;
	}
	else {
		moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
	}

	UAVPlanner::GridBaseSweepPlanner planner;
	planner.planning(ox, oy, foxy, resolution, moveDirection1, moveDirection2, rx, ry);
}
