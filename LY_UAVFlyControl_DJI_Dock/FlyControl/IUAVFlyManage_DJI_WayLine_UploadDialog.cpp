#include "IUAVFlyManage_DJI_WayLine_UploadDialog.h"
#include "ui_IUAVFlyManage_DJI_WayLine_UploadDialog.h"
#include "QCString.h"
#include "LyMessageBox.h"
#include "UAVFlyManage_DJI_DockDialog.h"
#include "IUAVFlyManage_DJI_SendTask_Dialog.h"
#include <QtMath>
IUAVFlyManage_DJI_WayLine_UploadDialog::IUAVFlyManage_DJI_WayLine_UploadDialog(QWidget *parent) :
	LyDialog(tr2("航线编辑"),parent),
    ui(new Ui::IUAVFlyManage_DJI_WayLine_UploadDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	resize(1730, 800);

	connect(ui->pushButton_VehicleUpDownMissionWayPointAltitude, &QPushButton::clicked, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::on_pushButton_VehicleUpDownMissionWayPointAltitude);
	connect(ui->pushButton_VehicleUpDownMissionWayPointSpeed, &QPushButton::clicked, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::on_doubleSpinBox_VehicleUpDownMissionWayPointSpeed);
	connect(ui->tableWidget, &QTableWidget::currentItemChanged, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::on_tableWidget_currentItemChanged);

	connect(ui->tableWidget, &IUAVFlyMessage_DJI_WayLine_Up::signal_uploadRoute, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::slots_uploadRoute);
	connect(ui->tableWidget, &IUAVFlyMessage_DJI_WayLine_Up::signal_downloadRoute, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::slots_downloadRoute);
}

IUAVFlyManage_DJI_WayLine_UploadDialog::~IUAVFlyManage_DJI_WayLine_UploadDialog()
{
    delete ui;
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::Init()
{
	if (m_droneEnum2Type == nullptr)
	{
		m_droneEnum2Type = new DroneEnum2Type;
	}
	if (m_payloadEnum2Type == nullptr)
	{
		m_payloadEnum2Type = new PayloadEnum2Type;
	}

	ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(1200));

	if (m_payloadEnum2Type != nullptr)
	{
		QStringList payloadStrs;
		std::map<std::string, long long> PayloadTypes = m_payloadEnum2Type->getNames();
		auto payloadTypeItor = PayloadTypes.begin();
		while (payloadTypeItor != PayloadTypes.end())
		{
			payloadStrs << QString::fromStdString(payloadTypeItor->first);
			payloadTypeItor++;
		}
		ui->comboBoxCamera->clear();
		ui->comboBoxCamera->addItems(payloadStrs);

		if (payloadStrs.contains("Matrice 3TD Camera")) {
			ui->comboBoxCamera->setCurrentText("Matrice 3TD Camera");
		}
	}

	if (m_droneEnum2Type != nullptr)
	{
		QStringList droneStrs;
		std::map<std::string, long long> droneTypes = m_droneEnum2Type->getNames();
		auto droneTypeItor = droneTypes.begin();
		while (droneTypeItor != droneTypes.end())
		{
			droneStrs << QString::fromStdString(droneTypeItor->first);
			droneTypeItor++;
		}
		ui->comboBoxUAVType->clear();
		ui->comboBoxUAVType->addItems(droneStrs);

		if (droneStrs.contains("Matrice 3TD")) {
			ui->comboBoxUAVType->setCurrentText("Matrice 3TD");
		}
	}

	ui->label_success->setText("");
	ui->label_success->setFixedHeight(2);
	ui->label_success->setStyleSheet("background-color: red;");

	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString timestamp = currentDateTime.toString("yyyyMMddHHmmss");
	QString fileName = "DJ_" + timestamp;
	ui->lineEdit->setText(fileName);
	ui->lineEdit_2->setText(fileName);



	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &IUAVFlyManage_DJI_WayLine_UploadDialog::onTimerTimeout);


}

bool IUAVFlyManage_DJI_WayLine_UploadDialog::getWidgetEnable()
{
	return ui->tableWidget->bIsEnable();
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::on_pushButton_VehicleUpDownMissionWayPointAltitude()
{
	if (ui->tableWidget->rowCount() <= 0) {
		return;
	}
	double altitude = ui->doubleSpinBox_VehicleUpDownMissionWayPointAltitude->value();
	if (altitude < 2) {
		return;
	}
	ui->tableWidget->setAltitudeAll(altitude);
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::on_doubleSpinBox_VehicleUpDownMissionWayPointSpeed()
{
	if (ui->tableWidget->rowCount() <= 0) {
		return;
	}
	double speed = ui->doubleSpinBox_VehicleUpDownMissionWayPointSpeed->value();
	if (speed < 2) {
		return;
	}
	ui->tableWidget->setSpeed(speed);
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::on_tableWidget_currentItemChanged(QTableWidgetItem* item)
{
	//计算航点间距
	if (item == nullptr) {
		return;
	}
	int currentRow = item->row();
	double distance = ui->tableWidget->setCurrentItem_distance(currentRow);
	ui->label_VehicleUpDownMissionInfo_Count->setText(tr2("航点：") + QString::number(currentRow));
	ui->label_VehicleUpDownMissionInfo_Dist->setText(tr2("距离：") + QString::number(distance) + "m");
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::slots_downloadRoute()
{

}

void IUAVFlyManage_DJI_WayLine_UploadDialog::onTimerTimeout()
{
	d_phase += 0.05; 
	if (d_phase > 2 * M_PI) {
		d_phase -= 2 * M_PI;
	}
	double intensity = (sin(d_phase) + 1.0) / 2.0; // 0.0 ~ 1.0
	if (b_timer_success)
	{
		int green = 50 + static_cast<int>(intensity * (255 - 50));
		QString style = QString(
			"QLabel {"
			"   background-color: rgb(0, %1, 0); "  
			"   color: white;                      " 
			"   border-radius: 10px;               " 
			"   font-weight: bold;                  "
			"}"
		).arg(green);

		ui->label_success->setStyleSheet(style);
	}
	else 
	{
		int red = 50 + static_cast<int>(intensity * (255 - 50)); 
		QString style = QString(
			"QLabel {"
			"   background-color: rgb(%1, 0, 0); "  
			"   color: white;"
			"   border-radius: 10px;"
			"   font-weight: bold;"
			"}"
		).arg(red);
	}

	if (intensity < 0.01) {
		m_timer->stop();          // 停止定时器
	}
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::DrawRouteLine(QPainter & painter, IProj * pProj)
{
	QVector<QPair<int, QPointF>> routepointv = ui->tableWidget->getRoutePointVector();
	if (routepointv.size() <= 0 || ui->checkBox_VehicleUpDownMissionHide->isChecked())
		return;
	if (routepointv.size() == 1)
	{
		QPoint ptPoint1 = pProj->Jwd2Screen(routepointv[0].second.x(), routepointv[0].second.y());
		QColor greeb1 = 0x0000ff;
		QPen pen1(Qt::yellow);
		pen1.setWidth(3);
		painter.setPen(pen1);
		painter.setBrush(greeb1);
		painter.drawPoint(ptPoint1);

		QPen pen(Qt::green, 10);
		painter.setPen(pen);
		painter.drawPoint(ptPoint1);
		painter.drawText(ptPoint1, QString::fromLocal8Bit("1"));
		return;
	}

	for (int i = 0;i < routepointv.size();++i)
	{
		QPoint ptPoint1 = pProj->Jwd2Screen(routepointv[i].second.x(), routepointv[i].second.y());
		if(i + 1 >= routepointv.size())
			break;
		QPoint ptPoint2 = pProj->Jwd2Screen(routepointv[i + 1].second.x(), routepointv[i + 1].second.y());
		QColor greeb1 = 0x0000ff;
		QPen pen1(Qt::yellow);
		pen1.setWidth(3);
		painter.setPen(pen1);
		painter.setBrush(greeb1);
		painter.drawLine(ptPoint1, ptPoint2);

		QPen pen(Qt::green, 10);
		painter.setPen(pen);
		painter.drawPoint(ptPoint1);
		painter.drawText(ptPoint1, QString::number(i));
	}
	
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::AddRoutePoint(const double & dlon, const double & dlat)
{
	ui->tableWidget->addNewRow(dlon, dlat);
}

void IUAVFlyManage_DJI_WayLine_UploadDialog::slots_uploadRoute()
{
	if (!createWaylineStruct()) {
		return;
	}
	std::string idText_wpmzName = tr3(ui->lineEdit->text());
	std::string xml, md5;
	bool b_succ = LY_UAVFlyControl_DJI_Dock::Get().minIO_uploadfile(idText_wpmzName + ".kmz", savePath + idText_wpmzName + ".kmz", xml, md5);

	if (b_succ) {
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->setWaylineURLorFingerpoint(xml.c_str(), md5.c_str());
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->getListObjectsInBucket();
		//更新combo
	}
	else {
		ui->label_success->setStyleSheet("background-color: green;");
	}
	b_timer_success = b_succ;
	m_timer->start(50);
}

bool IUAVFlyManage_DJI_WayLine_UploadDialog::createWaylineStruct()
{
	if (ui->lineEdit->text().isEmpty())
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("航线编号不能为空！"), QMessageBox::Ok);
		return false;
	}
	if (ui->lineEdit_2->text().isEmpty())
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("航线名称不能为空！"), QMessageBox::Ok);
		return false;
	}
	std::vector<wcs::WPMLPlacemark> WPMLplcemark = ui->tableWidget->getWaypointInfoKmlPlacemark();
	if (WPMLplcemark.size() <= 0) {
		return false;
	}
	double saftTakeoff = ui->doubleSpinBox_SaftTakeoff_height->value();
	if (saftTakeoff > 1500 || saftTakeoff < 8)
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("安全起飞高度不符合区间[8-1500]！"), QMessageBox::Ok);
		return false;
	}
	double flytoStartSpeed = ui->doubleSpinBox_flytoStart_speed->value();
	if (flytoStartSpeed > 15 || flytoStartSpeed < 1)
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("全局航线过渡速度不符合区间[1-15]！"), QMessageBox::Ok);
		return false;
	}

	wcs::WPMZData wpmzData;
	wpmzData.templateKML.author = "admin";
	wpmzData.templateKML.createTime = QDateTime::currentMSecsSinceEpoch();
	wpmzData.templateKML.updateTime = QDateTime::currentMSecsSinceEpoch();

	//任务信息
	wpmzData.templateKML.missionConfig.flyToWaylineMode = wce::FlyToWaylineMode::safely;
	wpmzData.templateKML.missionConfig.finishAction = wce::FinishAction::goHome;
	wpmzData.templateKML.missionConfig.exitOnRCLost = wce::ExitOnRCLost::executeLostAction;		//RC断联执行失控动作
	int domain, type, sub_type;
	if (m_droneEnum2Type->getTypeIDFromDroneName(ui->comboBoxUAVType->currentText().toStdString(), domain, type, sub_type))
	{
		wpmzData.templateKML.missionConfig.droneInfo.droneEnumValue = type;
		wpmzData.templateKML.missionConfig.droneInfo.droneSubEnumValue = sub_type;
	}
	else
	{
		wpmzData.templateKML.missionConfig.droneInfo.droneEnumValue = 91;
		wpmzData.templateKML.missionConfig.droneInfo.droneSubEnumValue = 1;
	}

	wpmzData.templateKML.missionConfig.takeOffSecurityHeight = saftTakeoff;		//安全起飞高度 
	wpmzData.templateKML.missionConfig.globalTransitionalSpeed = flytoStartSpeed;	/*[1,15] 注：飞行器飞往每条航线首航点的速度。航线任务中断时，飞行器从当前位置恢复至断点的速度。*/
	
	double RTHHeight = LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->getReturnRouteHeight();

	wpmzData.templateKML.missionConfig.globalRTHHeight = RTHHeight;			//全局返航高度
	wpmzData.templateKML.missionConfig.executeRCLostAction = wce::ExecuteRCLostAction::goBack;
	int ptype, psub_type, pGimbalindex;
	if (m_payloadEnum2Type->getTypeIDFromPayloadName(ui->comboBoxCamera->currentText().toStdString(), ptype, psub_type, pGimbalindex))
	{
		wpmzData.templateKML.missionConfig.payloadInfo.payloadEnumValue = ptype;
		wpmzData.templateKML.missionConfig.payloadInfo.payloadPositionIndex = pGimbalindex;
	}
	else
	{
		wpmzData.templateKML.missionConfig.payloadInfo.payloadEnumValue = 81;
		wpmzData.templateKML.missionConfig.payloadInfo.payloadPositionIndex = 0;
	}
	wpmzData.templateKML.missionConfig.autoRerouteInfo.missionAutoRerouteMode = 0;
	wpmzData.templateKML.missionConfig.autoRerouteInfo.missionAutoRerouteMode = 0;

	//模板信息
	wcs::WaypointFlightKMLFolder tempWayPointFolder;
	tempWayPointFolder.templateType = wce::TemplateType::waypoint;
	tempWayPointFolder.templateId = 0;
	tempWayPointFolder.autoFlightSpeed = 8;
	//wpmzData.templateKML.folder->waylineCoordinateSysParam			//坐标系参数

	wcs::WaylineCoordinateSysParam waylineCoordinateSysParam;
	waylineCoordinateSysParam.coordinateMode = wce::CoordinateMode::WGS84;
	waylineCoordinateSysParam.heightMode = wce::HeightMode::relativeToStartPoint;
	waylineCoordinateSysParam.positioningType = wce::PositioningType::GPS;
	waylineCoordinateSysParam.globalShootHeight = 50;
	waylineCoordinateSysParam.surfaceFollowModeEnable = 0;

	tempWayPointFolder.waylineCoordinateSysParam = waylineCoordinateSysParam;
	//
	//负载设置
	wcs::PayloadParam plParam;
	{
		plParam.payloadPositionIndex = pGimbalindex;
		plParam.focusMode = wce::FocusMode::firstPoint;
		plParam.meteringMode = wce::MeteringMode::spot;
		plParam.dewarpingEnable = 1;
		plParam.returnMode = wce::ReturnMode::singleReturnStrongest;
		plParam.samplingRate = 60000;
		plParam.scanningMode = wce::ScanningMode::nonRepetitive;
		plParam.modelColoringEnable = 0;
		plParam.imageFormat.clear();
		plParam.imageFormat.push_back(wce::ImageFormat::ir);
		plParam.imageFormat.push_back(wce::ImageFormat::visable);
		plParam.imageFormat.push_back(wce::ImageFormat::zoom);
	}
	tempWayPointFolder.payloadParam = plParam;
	tempWayPointFolder.globalWaypointTurnMode = wce::WaypointTurnMode::toPointAndPassWithContinuityCurvature;
	tempWayPointFolder.globalUseStraightLine = 1;
	tempWayPointFolder.gimbalPitchMode = wce::GimbalPitchMode::fixed;
	tempWayPointFolder.globalHeight = wpmzData.templateKML.missionConfig.takeOffSecurityHeight;
	tempWayPointFolder.globalWaypointHeadingParam.waypointHeadingMode = wce::WaypointHeadingMode::followWayline;
	tempWayPointFolder.globalWaypointHeadingParam.waypointHeadingAngle = 0;
	tempWayPointFolder.globalWaypointHeadingParam.waypointHeadingPathMode = wce::WaypointHeadingPathMode::clockwise;
	wcs::KPoint point;
	point.lon = 0;
	point.lat = 0;
	point.alt = 0;
	tempWayPointFolder.globalWaypointHeadingParam.waypointPoiPoint = point;

	tempWayPointFolder.placemark.clear();

	for (int PointIndex = 0; PointIndex < WPMLplcemark.size(); PointIndex++)
	{
		wcs::WaypointInfoKMLPlacemark placemark;
		placemark.isRisky = 0;
		placemark.point.lon = WPMLplcemark[PointIndex].point.lon;
		placemark.point.lat = WPMLplcemark[PointIndex].point.lat;
		placemark.index = PointIndex;
		placemark.useGlobalHeight = 0;
		placemark.useGlobalSpeed = 0;	//是否使用全局飞行速度，需要设置参数
		placemark.useGlobalHeadingParam = 1;
		placemark.useGlobalTurnParam = 1;
		placemark.useStraightLine = 1;		//该航段是否贴合直线
		placemark.ellipsoidHeight = WPMLplcemark[PointIndex].executeHeight;			//椭球体高度，可不用
		placemark.height = WPMLplcemark[PointIndex].executeHeight;
		placemark.waypointSpeed = WPMLplcemark[PointIndex].waypointSpeed;//航点飞行速度
		placemark.gimbalPitchAngle = 0.0;
		placemark.waypointHeadingParam.waypointHeadingMode = wce::WaypointHeadingMode::followWayline;
		placemark.waypointHeadingParam.waypointHeadingPathMode = wce::WaypointHeadingPathMode::followBadArc;
		placemark.waypointTurnParam.waypointTurnMode = wce::WaypointTurnMode::toPointAndStopWithDiscontinuityCurvature;

		placemark.waypointTurnParam.waypointTurnDampingDist = 1;

		tempWayPointFolder.placemark.push_back(placemark);
	}

	wpmzData.templateKML.folder = std::make_shared<wcs::WaypointFlightKMLFolder>(tempWayPointFolder);

	//WPML文件
	wpmzData.waylinesWPML.missionConfig.flyToWaylineMode = wpmzData.templateKML.missionConfig.flyToWaylineMode;
	wpmzData.waylinesWPML.missionConfig.finishAction = wpmzData.templateKML.missionConfig.finishAction;
	wpmzData.waylinesWPML.missionConfig.exitOnRCLost = wpmzData.templateKML.missionConfig.exitOnRCLost;
	wpmzData.waylinesWPML.missionConfig.droneInfo = wpmzData.templateKML.missionConfig.droneInfo;
	wpmzData.waylinesWPML.missionConfig.executeRCLostAction = wpmzData.templateKML.missionConfig.executeRCLostAction;
	wpmzData.waylinesWPML.missionConfig.takeOffSecurityHeight = wpmzData.templateKML.missionConfig.takeOffSecurityHeight;
	wpmzData.waylinesWPML.missionConfig.globalTransitionalSpeed = wpmzData.templateKML.missionConfig.globalTransitionalSpeed;
	wpmzData.waylinesWPML.missionConfig.globalRTHHeight = wpmzData.templateKML.missionConfig.globalRTHHeight;
	wpmzData.waylinesWPML.missionConfig.payloadInfo = wpmzData.templateKML.missionConfig.payloadInfo;
	wpmzData.waylinesWPML.missionConfig.autoRerouteInfo = wpmzData.templateKML.missionConfig.autoRerouteInfo;

	//航线信息
	wpmzData.waylinesWPML.folder.clear();
	wcs::WPMLFolder lineFolder;
	lineFolder.templateId = wpmzData.templateKML.folder->templateId;
	lineFolder.waylineId = 0;
	lineFolder.autoFlightSpeed = wpmzData.templateKML.folder->autoFlightSpeed;
	lineFolder.executeHeightMode = wce::ExecuteHeightMode::relativeToStartPoint;
	lineFolder.placemark.clear();
	for (int PointIndex = 0; PointIndex < WPMLplcemark.size(); PointIndex++)
	{
		wcs::WPMLPlacemark placemark;
		placemark.isRisky = 0;
		placemark.point.lon = WPMLplcemark[PointIndex].point.lon;
		placemark.point.lat = WPMLplcemark[PointIndex].point.lat;
		placemark.index = PointIndex;
		placemark.useStraightLine = 1;
		placemark.executeHeight = WPMLplcemark[PointIndex].executeHeight;
		placemark.waypointSpeed = WPMLplcemark[PointIndex].waypointSpeed;
		placemark.waypointHeadingParam.waypointHeadingMode = wce::WaypointHeadingMode::followWayline;
		placemark.waypointHeadingParam.waypointHeadingPathMode = wce::WaypointHeadingPathMode::followBadArc;
		placemark.waypointTurnParam.waypointTurnMode = wce::WaypointTurnMode::toPointAndStopWithDiscontinuityCurvature;
		placemark.waypointTurnParam.waypointTurnDampingDist = 1;
		placemark.startActionGroup = WPMLplcemark[PointIndex].startActionGroup;

		lineFolder.placemark.push_back(placemark);
	}
	wpmzData.waylinesWPML.folder.push_back(lineFolder);
	m_wpmzData = wpmzData;

	std::string idText_wpmzName = tr3(ui->lineEdit->text());
	bool saveSucc = wcc::createWPMZ(wpmzData, savePath + idText_wpmzName + ".kmz");

	return saveSucc;
}