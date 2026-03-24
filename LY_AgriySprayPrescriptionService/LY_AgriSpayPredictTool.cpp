#include "ly_agrispaypredicttool.h"
#include <QString>
#include <QDebug>
#include "qmath.h"
#include <QColor>
#include "CreateShape.h"
#include "LY_AgriySprayPrescriptionService.h"
#include "LY_CvsParserTool.h"


LY_AgriSpayPredictTool::LY_AgriSpayPredictTool(QObject *parent) :
	QObject(parent)
{
	if (m_playerTimer == nullptr)
	{
		m_playerTimer = new QTimer(this);
	}
	connect(m_playerTimer, &QTimer::timeout, this, &LY_AgriSpayPredictTool::on_time_out);

	//TODO:测试数据
	//CsvTable table = CsvParser::parseFile("E:\\InteAgri\\filtered_trajectory.csv");

	//// 输出表头信息
	//qDebug() << "CSV Table Headers: " << table.headers;
	//qDebug() << "Total rows: " << table.rowCount();
	//qDebug() << "Total columns: " << table.columnCount();

	//// 遍历数据
	//for (int i = 0; i < table.rowCount(); i++) {
	//	const CsvRow& row = table.rowAt(i);

	//	// 通过列名访问数据
	//	qDebug() << "Row" << i << ":";
	//	qDebug() << "  Name:" << row.get("Name");
	//	qDebug() << "  Age:" << row.get("Age").toInt();       // 自动类型转换
	//	qDebug() << "  Description:" << row.get("Description");

	//	// 也可以遍历所有字段
	//	for (const QString& header : table.headers) {
	//		qDebug() << "  " << header << ":" << row.get(header);
	//	}
	//}
}

LY_AgriSpayPredictTool::~LY_AgriSpayPredictTool()
{
	m_playerTimer = nullptr;
}
void LY_AgriSpayPredictTool::startFlying(double flyHeight, bool isLitering){

	m_isLitering = isLitering;
	m_flyHeight = flyHeight;
	//if (!isLitering)
	//{
	//	m_flyHeight = 3.0;
	//}
	m_playerTimer->setInterval(1000 / 2);
    m_playerTimer->start();

}
void LY_AgriSpayPredictTool::on_time_out() {
	startFlying("M1", 4.0, m_flyHeight, m_isPlayOrderfilght);
}
void LY_AgriSpayPredictTool::setFlyDisance(double disance)
{
	//allFlyLineditance = disance;
}
void LY_AgriSpayPredictTool::startInstructionFlying(bool isPlayOrderfilght)
{
	m_isPlayOrderfilght = isPlayOrderfilght;
}
void LY_AgriSpayPredictTool::setOriginPoints(const std::vector<Poi> &points)
{
	m_origin_points.clear();
	for (const auto& p  : points)
	{
		m_origin_points.push_back(p);
	}
	allFlyLineditance = 0.0;
	flyingDiance = 0.0;
	for (int i = 0; i < m_origin_points.size() - 1; i++)
	{
		QGeoCoordinate start = QGeoCoordinate(m_origin_points[i].lat, m_origin_points[i].lon);
		QGeoCoordinate end = QGeoCoordinate(m_origin_points[i + 1].lat, m_origin_points[i + 1].lon);
		double di = start.distanceTo(end);
		allFlyLineditance += di;
	}
}
void LY_AgriSpayPredictTool::startFlying(QString flyID1, double flySpeed1, double flyHeight1, bool isPlayOrderfilght)
{
	UavStatus uavStatus;
	UAVinformation uavInfo;
	if (currentIndex >= new_poiPaths.size()) {
		if (!isPlayOrderfilght && m_isFerili)
		{
			LY_AgriySprayPrescriptionService::Get().GetAgriFertiliResultWidget()->show();
			LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->hide();
		}
		qDebug() << "所有POI处理完成，停止定时器";
		m_playerTimer->stop();
		//interpolateSegments(4.0, m_poiPaths);
		currentIndex = 0;
		m_isend = false;
		m_first = false;
		uavStatus.stage = "landed";
		uavStatus.task_stage = "finished";
		m_usvStatus = uavStatus;
		uavInfo.speed = "0";
		//if (LY_AgriySprayPrescriptionService::Get().GetCameraWidget1())
		//{
		//	LY_AgriySprayPrescriptionService::Get().GetCameraWidget1()->pause(true);
		//}
		//if (LY_AgriySprayPrescriptionService::Get().GetCameraWidget2())
		//{
		//	LY_AgriySprayPrescriptionService::Get().GetCameraWidget2()->pause(true);
		//}
		return;
	}
	
	// 取出当前Poi并处理
	const Poi &currentPoi = new_poiPaths[currentIndex];

	
	uavStatus.longitude = currentPoi.lon;
	uavStatus.latitude = currentPoi.lat;
	uavStatus.altitude = flyHeight1;
	uavStatus.speed = flySpeed1;
	if (!m_origin_points.empty())
	{
		if (currentPoi.lon == m_origin_points.front().lon)
		{
			m_first = true;
		}
		else if (currentPoi.lon == m_origin_points.back().lon)
		{
			uavStatus.task_stage = "finished";
			m_isend = true;
		}
	}
	if (!m_first && !m_isend)
	{
		uavStatus.task_stage = "takeoff";
		uavStatus.stage = "takeoff";
	}
	else if (m_first && !m_isend)
	{
		uavStatus.task_stage = "progress";
		uavStatus.stage = "spraying";
	
		m_time++;
		if (m_time % 5 == 0)
		{
			temp_temp++;
		}
	}
	else if (m_first && m_isend)
	{
		uavStatus.task_stage = "finished";
		uavStatus.stage = "returning";
	}
	m_usvStatus = uavStatus;


	QGeoCoordinate endP = QGeoCoordinate(currentPoi.lat, currentPoi.lon);
	double angle;
	 if (m_first)
	{
	   double di = startPoint.distanceTo(endP);
	   angle = endP.azimuthTo(startPoint);
	   flyingDiance += di;
	   double process = QString::number(flyingDiance, 'f', 2).toDouble() / QString::number(allFlyLineditance, 'f', 2).toDouble();
	   if (m_first && !m_isend)
	   {
		   workProcess = QString::number(process, 'f', 4).toDouble() * 100;
	   }
	   else if(m_isend)
	   {
		   workProcess = 100.00;
	   }
	}

	 TaskProgress progreess;
	 progreess.progress = QString::number(workProcess, 'f', 2);
	 progreess.time = QString::number(temp_temp, 'f', 2);
	
	uavInfo.lon = QString::number(currentPoi.lon, 'f', 7);
	uavInfo.lat = QString::number(currentPoi.lat, 'f', 7);
	uavInfo.controlmodel = "AUTO";
	uavInfo.flightphase = uavStatus.stage;
	if (LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo())
	{
		LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo()->SetTaskProgress(progreess);
		LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo()->SetUAVinformation(uavInfo);
	}
	 

	double elevation = LY_AgriSPrayPrescriptionUntil::getElevation(currentPoi.lon, currentPoi.lat);

	double h = flyHeight1 * 1.0;

	draw3DModel(currentPoi.lon, currentPoi.lat, elevation + h, angle);

	QString ID = tr2("编号:") + flyID1;
	QString fly = tr2("类型:") + tr2("无人机");
	QString status = tr2("状态:") + tr2("飞行中");
	QString flySpeed = tr2("飞行速度:") + QString::number(flySpeed1, 'f', 1);
	QString flyHeight = tr2("飞行高度:") + QString::number(flyHeight1, 'f', 1);
	QString str2 = m_isLitering ? tr2("巡飞") : tr2("喷药");
	QString workType = tr2("作业类型:") + str2;
	QString workProcessStr = tr2("作业进度") + QString::number(workProcess) + "%";

	// taskWidget->setTaskTypeName(tr2("喷药"));
	//taskWidget->setTaskProcess(QString::number(workProcess));

	

	QString str = ID + "\n"
		+ fly + "\n" + status + "\n"
		+ flySpeed + "\n" + flyHeight + "\n"
		+ workType + "\n"
		+ workProcessStr;

	draw3DFlyLabel(currentPoi.lon, currentPoi.lat, elevation + h, str);

	casic_vector3d curPos(currentPoi.lon, currentPoi.lat, elevation + h);

	auto colorleve = [&](int level) {
		switch (level)
		{
		case 1:
			return "#ff8080";
		case 2:
			return "#ff4d4d";
		case 3:
			return "#ff0000";
		case 4:
			return "#cc0000";
		case 5:
			return "#800000";
		}
	};

	auto uavImageLevel = [&](int level) {
		switch (level)
		{
		case 0:
			return "Temp_Line/fly_1.png";
		case 1:
			return "Temp_Line/fly_1.png";
		case 2:
			return "Temp_Line/fly_2.png";
		case 3:
			return "Temp_Line/fly_3.png";
		case 4:
			return "Temp_Line/fly_4.png";
		case 5:
			return "Temp_Line/fly_5.png";
		}
	};

	if (isPlayOrderfilght)
	{
		//draw3DImage(currentPoi.lon, currentPoi.lat, elevation + h, uavImageLevel(currentPoi.alt));
		if (currentPoi.alt > 0)
		{
			QColor color(colorleve(currentPoi.alt));
			double spsj = 30 * qTan(20 / 2 * M_PI / 180) * 2;
			double czsj = 30 * qTan(30 / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, 55, -90.0, 0, spsj, czsj, flyHeight1, color);
			

		}
		else
		{
			clearn3DCone();
		}
	}
	else
	{
	
		/*if (m_first)
		{*/
		clearn3DCone();
		if (m_isLitering)//巡航
		{
			double spsj = 30 * qTan(20 / 2 * M_PI / 180) * 2;
			double czsj = 30 * qTan(30 / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, 55, -90.0, 0, spsj, czsj, 35, Qt::red);
		}
		else {
			double spsj = 40 * qTan(20 / 2 * M_PI / 180) * 2;
			double czsj = 40 * qTan(30 / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, 55, -90.0, 0, spsj, czsj, flyHeight1, Qt::red);
		}

		/*}
		if (m_isend)
		{
			clearn3DCone();
		}*/

		//draw3DImage(currentPoi.lon, currentPoi.lat, elevation + h, uavImageLevel(0));
	}


	++currentIndex; // 移动到下一个元素
	startPoint = endP;

}
void LY_AgriSpayPredictTool::setupFlylineStatus(bool isShow)
{

	if (Path3DLines.empty() && Path3DLines_plat.empty()) return;
	if (m_playerTimer)
	{
		isShow ? m_playerTimer->start() : m_playerTimer->stop();
	}
	for (const auto &userMesh: Path3DLines) {
		userMesh->SetVisible(isShow);
	}
	for (const auto &renderableObject : Path3DLines_plat) {
		renderableObject->SetVisible(isShow);
	}
	for (const auto &Itext : m_map3DTextVec) {
		Itext->SetVisible(isShow);
	}
	for (const auto &uav : m_uav3DVec) {
		uav.pSightingLine->SetVisible(isShow);
		uav.pSightingLine2->SetVisible(isShow);
	}
	for (const auto &disZone : discoverZone) {
		disZone->SetVisible(isShow);
	}
	for (const auto &line : m_allLines) {
		line->setNodeMask(isShow ? 0xFFFFFFFF : 0x0);
	}
	if (m_modelTransform)
	{
		m_modelTransform->setNodeMask(isShow ? 0xFFFFFFFF : 0x0);
	}
	if (m_line)
	{
		m_line->setNodeMask(isShow ? 0xFFFFFFFF : 0x0);
	}
	if (m_routeGeode) {
		m_routeGeode->setNodeMask(isShow ? 0xFFFFFFFF : 0x0);
	}

}
//绘制飞机标牌
void LY_AgriSpayPredictTool::draw3DFlyLabel(double lon, double lat, double alt, QString str)
{

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode) return;

	// 删除所有线
	for (auto& line : m_allLines) {
		mapNode->removeChild(line);
	}
	m_allLines.clear();

	double distance = 10.0;
	double azimuth = 135.0;
	double elevation = 30.0;

	double endLat, endLon, endHeight;
	LY_AgriSPrayPrescriptionUntil::calculateNewPosition(lat, lon, alt, distance, azimuth, elevation, endLat, endLon, endHeight);

	const osgEarth::SpatialReference* wgs84 = mapNode->getMapSRS()->getGeographicSRS();
	osgEarth::GeoPoint startPoint(wgs84, lon, lat, alt, osgEarth::ALTMODE_ABSOLUTE);
	osgEarth::GeoPoint endPoint(wgs84, endLon, endLat, endHeight + 10, osgEarth::ALTMODE_ABSOLUTE);
	m_line = createDirectLine(startPoint, endPoint);

	// 创建组节点并添加线段
	m_lineGroup = new osg::Group;
	m_lineGroup->addChild(m_line);

	mapNode->addChild(m_lineGroup);
	m_allLines.push_back(m_lineGroup);

	auto itr2 = m_map3DTextVec.begin();
	for (; itr2 != m_map3DTextVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_map3DTextVec.clear();

	if (pEarth->GetGraphics() == nullptr || pEarth == nullptr) return;

	//QString strPath = "D:/InteAgri/src/FireProt/qrc/images/AgriFarm/fly_line_bg.png";
	//ILabel * label = pEarth->GetGraphics()->CreateLabel(str.toLocal8Bit(), strPath.toStdString().c_str(), endLon, endLat, endHeight + 10, 400 * 0.5, 300 * 0.5, 0, NULL, 22, TRGB(128, 255, 255, 255), 100);

	IText* mshpText = pEarth->GetGraphics()->CreateText(str.toLocal8Bit(), endLon, endLat, endHeight + 10, NULL, 22,
		TRGB(255, 255, 255, 255), TRGB(11, 25, 60, 128), TRGB(91, 219, 64, 255), 4);

	m_map3DTextVec.emplace_back(mshpText);
}

osg::Geometry* LY_AgriSpayPredictTool::createDirectLine(
	osgEarth::GeoPoint& start,
	osgEarth::GeoPoint& end)
{

	//osg::AutoTransform* at = new osg::AutoTransform;
	//at->setAutoScaleToScreen(true);  // 关键：启用屏幕空间自适应缩放

	osg::Billboard* billboard = new osg::Billboard;
	billboard->setMode(osg::Billboard::POINT_ROT_EYE);  // 始终朝向相机

														// 转换为世界坐标
	osg::Vec3d startWorld, endWorld;
	start.toWorld(startWorld);
	end.toWorld(endWorld);

	// 创建几何体
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(startWorld);
	vertices->push_back(endWorld);
	geom->setVertexArray(vertices);

	// 设置颜色
	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(0.357f, 0.859f, 0.965f, 1.0f));
	geom->setColorArray(colors, osg::Array::BIND_OVERALL);

	// 设置线宽
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(1.5f);
	geom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	// 添加图元
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

	billboard->addDrawable(geom);

	return geom.release();
}
void LY_AgriSpayPredictTool::convert3DMap(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float angle_l, float angle_w, float dis_m, QColor zoneColor)
{
	if (heading > 360)
		heading -= 360;

	clearn3DCone();

	discoverZone = LY_AgriSPrayPrescriptionUntil::createDetectionZone(angle_l, angle_w, dis_m);

	Uav3D uav3D;
	if (discoverZone.size() == 2)
	{
		uav3D.pSightingLine = discoverZone.at(0);
		uav3D.pSightingLine2 = discoverZone.at(1);
	}
	if (uav3D.pSightingLine != nullptr && uav3D.pSightingLine2 != nullptr)
	{
		uav3D.pSightingLine->SetPosition(curPos.x, curPos.y,
			curPos.z);
		uav3D.pSightingLine->SetYawPitchRoll(heading, fuyang, pianzhuan);
		uav3D.pSightingLine2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		uav3D.pSightingLine2->SetYawPitchRoll(heading, fuyang, pianzhuan);

		QColor colorTrack(zoneColor);
		colorTrack.setAlpha(255);
		QColor colorTemp;
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

		colorTrack.setAlpha(40);
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
		uav3D.pSightingLine->SetColor(colorTemp.rgba());
		colorTrack.setAlpha(80);
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
		uav3D.pSightingLine2->SetColor(colorTemp.rgba());
	}

	m_uav3DVec.push_back(uav3D);
}
void LY_AgriSpayPredictTool::clearn3DModel()
{

	IEarth* pEarth = IEarth::GetEarth();
	if (!pEarth) return;
	osgEarth::MapNode* mapNode = static_cast<osgEarth::MapNode*>(
		pEarth->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode || !m_modelTransform) return;

	mapNode->removeChild(m_modelTransform);
	m_modelTransform = nullptr;
	m_model = nullptr;

	if (m_routeGeode && m_routeGeode->getNumParents() > 0) {
		// 遍历所有父节点并移除
		while (m_routeGeode->getNumParents() > 0) {
			osg::Group* parent = m_routeGeode->getParent(0);
			parent->removeChild(m_routeGeode);
		}
	}

	m_flightPath.clear();
	if (m_routeVertices) m_routeVertices->clear();
	if (m_routeGeometry) {
		m_routeGeometry->removePrimitiveSet(0, m_routeGeometry->getNumPrimitiveSets());
	}
	m_routeGeode = nullptr;
	m_routeGeometry = nullptr;
	m_routeVertices = nullptr;
	m_routeDrawArrays = nullptr;

	auto itr2 = m_map3DTextVec.begin();
	for (; itr2 != m_map3DTextVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_map3DTextVec.clear();


	if (!m_line || !m_lineGroup) return;
	while (m_line->getNumParents() > 0) {
		osg::Group* parent = dynamic_cast<osg::Group*>(m_line->getParent(0));
		if (parent) parent->removeChild(m_line);
	}
	m_line = nullptr;
	m_lineGroup = nullptr;
}
void LY_AgriSpayPredictTool::clearn3DCone()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	if (discoverZone.size() > 0)
	{
		auto itr2 = discoverZone.begin();
		for (; itr2 != discoverZone.end(); itr2++)
		{
			pEarth->GetGraphics()->RemoveObject(*itr2);
			*itr2 = nullptr;
			delete *itr2;
		}
		m_uav3DVec.clear();
		discoverZone.clear();
	}
}
void LY_AgriSpayPredictTool::draw3DPathLine(std::vector<Poi> points, QColor pathColor, int lineWidth, double flyHeight)
{
	delete3DflyPathLine();

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();
	for (int i = 0; i <points.size(); i++)
	{
		Poi point = points.at(i);
		pdLon.push_back(point.lon);
		pdLat.push_back(point.lat);
		double elevation = LY_AgriSPrayPrescriptionUntil::getElevation(point.lon, point.lat) + flyHeight * 1.0;
		pdHeight.push_back(elevation);

		double pdLon1[2] = { point.lon ,point.lon };
		double pdLat1[2] = { point.lat, point.lat };
		double pdHeight1[2] = { elevation ,0 };
		IUserMesh1	*pUM1 = nullptr;
		CCreateShape cs1;
		SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
		if (pShp1 != nullptr)
		{
			pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			if (pUM1 != nullptr)
			{
				QColor col(255, 255, 255, 100);
				pUM1->SetColor(col.rgba());
				pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
				pUM1->SetState(IUserMesh::LINEWIDTH, 4);
				pUM1->SetState(GL_DEPTH_TEST, 1);
				Path3DLines.push_back(pUM1);
			}
		}
	}

	IUserMesh1 *pUM = nullptr;
	SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdHeight.size(),
		100, false, nullptr, pdHeight.data());
	if (pShp != nullptr || pEarth->GetGraphics() != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(pathColor.rgb());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
			pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);
			Path3DLines_plat.push_back(pUM);
		}
	}
}
void LY_AgriSpayPredictTool::clear3DLineAndModel()
{
	if (m_isFerili && LY_AgriySprayPrescriptionService::Get().GetAgriFertiliResultWidget())
	{
		LY_AgriySprayPrescriptionService::Get().GetAgriFertiliResultWidget()->hide();
	}
	workProcess = 0.0;
	flyingDiance = 0.0;
	allFlyLineditance = 0.0;
	m_time = 0;
	temp_temp = 0;
	m_usvStatus = UavStatus();
	delete3DflyPathLine();
	clearn3DCone();
	clearn3DModel();
}
void LY_AgriSpayPredictTool::delete3DflyPathLine()
{
	if (m_playerTimer != nullptr)
	{
		m_playerTimer->stop();
	}
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < Path3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines[index]);
	}
	for (int index = 0; index < Path3DLines_plat.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines_plat[index]);
	}
	Path3DLines.clear();
}
void LY_AgriSpayPredictTool::interpolateSegments(double flySpeed, std::vector<Poi> m_origin_poiPaths)
{
	m_poiPaths.clear();
	m_poiPaths = m_origin_poiPaths;
	int nCalculatePoint = 1;
	new_poiPaths.clear();

	

	QGeoCoordinate QPoint1, QPoint2;
	auto itrPoint = m_origin_poiPaths.begin();
	for (; itrPoint != m_origin_poiPaths.end(); itrPoint++)
	{
		if ((itrPoint + 1) != m_origin_poiPaths.end())
		{
			double dLongStart(0), dLatStart(0), dLongEnd(0), dLatEnd(0), dHeightEnd(0), dHeight(0);
			dLongStart = itrPoint->lon;
			dLatStart = itrPoint->lat;
			dLongEnd = (itrPoint + 1)->lon;
			dLatEnd = (itrPoint + 1)->lat;
			QPoint1.setLongitude(dLongStart);
			QPoint1.setLatitude(dLatStart);
			QPoint2.setLongitude(dLongEnd);
			QPoint2.setLatitude(dLatEnd);

			double localdistance = QPoint1.distanceTo(QPoint2);

			double dbearing = QPoint1.azimuthTo(QPoint2);
			//步长
			double dDisOnce = flySpeed;
			if (dDisOnce <= 0)
			{
				dDisOnce = 0.5;
			}
			double dNewLong(0), dNewLat(0);

			Poi stTelemetryInformation;
			memset(&stTelemetryInformation, 0x00, sizeof(stTelemetryInformation));
			stTelemetryInformation.lon = dLongStart;
			stTelemetryInformation.lat = dLatStart;
			stTelemetryInformation.alt = itrPoint->alt;

			new_poiPaths.push_back(stTelemetryInformation);

			nCalculatePoint++;

			double dlocaldistanceOnce = localdistance;
			double dLastDistance = localdistance;
			while (abs(localdistance - dDisOnce) > dDisOnce)
			{

				QPoint1 = QPoint1.atDistanceAndAzimuth(dDisOnce, dbearing);

				dLastDistance = localdistance;
				localdistance = QPoint1.distanceTo(QPoint2);

				dbearing = QPoint1.azimuthTo(QPoint2);

				Poi stTelemetryInformationTmp;
				memset(&stTelemetryInformationTmp, 0x00, sizeof(stTelemetryInformationTmp));
				stTelemetryInformationTmp.lon = QPoint1.longitude();
				stTelemetryInformationTmp.lat = QPoint1.latitude();
				//stTelemetryInformationTmp.alt = 0;
				new_poiPaths.push_back(stTelemetryInformationTmp);

				nCalculatePoint++;
				if (dLastDistance < localdistance)
				{
					break;
				}
			}
		}
		else
		{

			double dLongStart(0), dLatStart(0), dHeight(0);
			dLongStart = itrPoint->lon;
			dLatStart = itrPoint->lat;

			Poi stTelemetryInformation;
			memset(&stTelemetryInformation, 0x00, sizeof(stTelemetryInformation));
			stTelemetryInformation.lon = dLongStart;
			stTelemetryInformation.lat = dLatStart;
			stTelemetryInformation.alt = itrPoint->alt;
			new_poiPaths.push_back(stTelemetryInformation);
			nCalculatePoint++;

		}
	}
}

//绘制飞机模型
void LY_AgriSpayPredictTool::draw3DModel(double lon, double lat, double alt, double jiaodu)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr) return;

	osgEarth::MapNode* mapNode = static_cast<osgEarth::MapNode*>(
		IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode) return;

	// ==== 模型初始化（首次调用时执行）====
	if (!m_modelTransform)
	{
		std::string modelNameStr = "newFlyModel";
		m_model = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/" + modelNameStr + ".ive");
		if (!m_model) {
			qDebug() << "Failed to load model!";
			return;
		}

		// 动画设置
		osgAnimation::BasicAnimationManager* animManager =
			dynamic_cast<osgAnimation::BasicAnimationManager*>(m_model->getUpdateCallback());
		if (animManager && !animManager->getAnimationList().empty()) {
			animManager->playAnimation(animManager->getAnimationList()[0], 1.0, 0.0);
		}

		// 创建变换节点
		m_modelTransform = new osg::MatrixTransform;
		m_modelTransform->setName("UAVTransform");
		m_modelTransform->addChild(m_model);
		createFlightRouteSystem(mapNode);

		// ==== 关键设置：确保模型显示在顶层 ====
		osg::StateSet* ss = m_modelTransform->getOrCreateStateSet();
		// 1. 设置渲染顺序为最高优先级（值越大越后渲染）
		ss->setRenderBinDetails(100, "RenderBin");
		// 2. 禁用深度测试（避免被地形遮挡）
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		//ss->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0, 1, false));
		ss->setAttribute(new osg::PolygonOffset(1.0f, 1.0f));  // 深度偏移参数
		ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL));

		mapNode->addChild(m_modelTransform);
	}

	// ==== 实时更新位置 ====
	// 1. 计算模型中心偏移（仅需计算一次）
	static osg::Vec3d modelCenterOffset = [&]() {
		osg::ComputeBoundsVisitor cbv;
		m_model->accept(cbv);
		return cbv.getBoundingBox().center();
	}();

	// 2. 计算世界坐标变换
	osg::Matrixd posMatrix;
	osg::EllipsoidModel* em = static_cast<osg::EllipsoidModel*>(
		IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	em->computeLocalToWorldTransformFromLatLongHeight(
		osg::inDegrees(lat), osg::inDegrees(lon), alt, posMatrix);

	osg::Vec3d currentPosition = posMatrix.getTrans();

	// 3. 构建变换矩阵
	//const double scale = 2.0 * 2 * 0.001; 
	const double scale = 6.0;
	osg::Matrixd matrix =
		osg::Matrixd::translate(-modelCenterOffset) *   // 中心点校正
		osg::Matrixd::rotate(osg::inDegrees(jiaodu), osg::Z_AXIS) * // 旋转
		osg::Matrixd::scale(scale, scale, scale) *      // 缩放
		posMatrix;                                      // 地理定位

	// 4. 更新变换节点
	m_modelTransform->setMatrix(matrix);
	if (m_first && !m_isend)
	{
		updateFlightRoute(currentPosition);
	}
	
}
// 创建航线绘制系统
void LY_AgriSpayPredictTool::createFlightRouteSystem(osgEarth::MapNode* mapNode)
{
	// 创建航线几何体
	m_routeGeometry = new osg::Geometry;
	m_routeVertices = new osg::Vec3dArray;
	m_routeGeometry->setVertexArray(m_routeVertices);

	// 设置颜色（RGBA：红色带半透明）
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.7f)); // 主航线颜色
	m_routeGeometry->setColorArray(colors, osg::Array::BIND_OVERALL);

	// 设置绘制方式为连续线段
	m_routeDrawArrays = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 0);
	m_routeGeometry->addPrimitiveSet(m_routeDrawArrays);

	// 启用VBO以获得最佳性能
	m_routeGeometry->setUseDisplayList(false);
	m_routeGeometry->setUseVertexBufferObjects(true);

	// 创建航线节点
	m_routeGeode = new osg::Geode;
	m_routeGeode->addDrawable(m_routeGeometry);

	// 设置线宽
	osg::LineWidth* lineWidth = new osg::LineWidth(3.0f);

	// 设置航线渲染状态
	osg::StateSet* ss = m_routeGeode->getOrCreateStateSet();
	ss->setRenderBinDetails(101, "RenderBin"); // 在模型下方一层
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	// 启用混合（透明效果）
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	//设置深度偏移（确保在飞机上方）
	ss->setAttribute(new osg::PolygonOffset(-1.0f, -1.0f));
	//禁用深度写入（确保航线不会相互遮挡）
	ss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
	// 禁用深度测试（确保航线始终可见）
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(lineWidth);
	// 添加到场景
	mapNode->addChild(m_routeGeode);
}
// 更新飞行航线
void LY_AgriSpayPredictTool::updateFlightRoute(const osg::Vec3d& newPosition)
{

	// 1. 添加新位置
	m_flightPath.push_back(newPosition);

	// 2. 限制路径长度
	//if (m_flightPath.size() > MAX_POINTS) {
	//	m_flightPath.pop_front(); // 使用deque更高效
	//}

	// 3. 直接更新顶点数组（避免清除重建）
	m_routeVertices->resize(m_flightPath.size());
	for (size_t i = 0; i < m_flightPath.size(); ++i) {
		(*m_routeVertices)[i] = m_flightPath[i];
	}

	// 4. 标记顶点数组已修改
	m_routeVertices->dirty();

	// 5. 更新PrimitiveSet（如果顶点数量变化）
	if (m_routeDrawArrays && m_routeDrawArrays->getCount() != m_routeVertices->size()) {
		m_routeGeometry->removePrimitiveSet(0);
		m_routeDrawArrays = new osg::DrawArrays(
			osg::PrimitiveSet::LINE_STRIP, 0, m_routeVertices->size());
		m_routeGeometry->addPrimitiveSet(m_routeDrawArrays);
	}

	// 6. 更新包围盒
	m_routeGeometry->dirtyBound();

	// 7. 对于VBO，通知需要更新
	m_routeGeometry->dirtyDisplayList();
}
UavStatus LY_AgriSpayPredictTool::getFlyStatus()
{
	return m_usvStatus;
}