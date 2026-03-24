#include "ly_agrifertilizepescviewmodel.h"
#include <QSettings>
#include <QDebug>
#include "LY_AgriFertilizerPrescription.h"
LY_AgriFertilizePescViewModel::LY_AgriFertilizePescViewModel(QObject *parent) : QObject(parent)
{
	InitDataBase();
}

void LY_AgriFertilizePescViewModel::InitDataBase() 
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = ini.value("DbName", "LY").toString();
	QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = ini.value("User", "root").toString();
	QString strDbPass = ini.value("Password", "123456").toString();
	QString strDbDriver = ini.value("Driver", "QMYSQL").toString();


	db = QSqlDatabase::addDatabase(strDbDriver);
	db.setHostName(strDbIP);
	db.setPort(3306);
	db.setDatabaseName(strDbName);
	db.setUserName(strDbUser);
	db.setPassword(strDbPass);
	isOpen = db.open();

	if (m_map3D != nullptr)
	{
		m_map3D->cleanMap();
	}

}
//添加处方图
bool LY_AgriFertilizePescViewModel::AddFertilizePrescription(FertilizerPrescription &model)
{
	/*if (!isOpen)
	{
		return false;
	}*/
	QMutexLocker locker(&sMutex);
	QString strSql("INSERT INTO agri_fertilizer_prescription (`guid`,`mapInfo_id`, `field_id`, `pescription_name`, `data_name`, `dis_map_name`, `nutrient_ele`,`datamap_id`,`dataset_id`, `pyfk`) VALUES ("); 
	strSql += "'" + model.guid + "',";
	strSql += "'" + model.mapInfo_id + "',";
	strSql += "'" + model.field_id + "',";
	strSql += "'" + model.pescription_name + "',";
	strSql += "'" + model.data_name + "',";
	strSql += "'" + model.dis_map_name + "',";
	strSql += "'" + model.nutrient_ele + "',";
	strSql += "'" + model.datamap_id + "',";
	strSql += "'" + model.dataset_id + "',";
	strSql += "'" + QString::number(model.pyfk) + "'";
	strSql += ")"; 
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	return success;
}

//获取处方图
FertilizerPrescriptionVec LY_AgriFertilizePescViewModel::GetFertilizePrescription(const QString field_id)
{
	FertilizerPrescriptionVec tempVec;
	//if (!isOpen)
	//{
	//	return tempVec;
	//}
	QMutexLocker locker(&sMutex);

	m_fertiliPresVec.clear();
	tempVec.clear();
	QSqlQuery query;
	QString strSql = "SELECT `guid`, `field_id`, `pescription_name`, `data_name`, `dis_map_name`, `nutrient_ele`, `mapInfo_id`,`datamap_id`,`dataset_id`, `pyfk`";
	QString strGeneralSql = strSql + " FROM " + "agri_fertilizer_prescription" + " WHERE field_id = \"" + field_id + "\";";

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();
			QString str8 = query.value(7).toString();
			QString str9 = query.value(8).toString();
			QString str10 = query.value(9).toString();

			FertilizerPrescription pInfo;
			pInfo.guid = str1;
			pInfo.field_id = str2;
			pInfo.pescription_name = str3;
			pInfo.data_name = str4;
			pInfo.dis_map_name = str5;
			pInfo.nutrient_ele = str6;
			pInfo.mapInfo_id = str7;
			pInfo.datamap_id = str8;
			pInfo.dataset_id = str9;
			pInfo.pyfk = str10.toInt();
			tempVec.push_back(pInfo);
		}
	}
	m_fertiliPresVec = tempVec;
	return tempVec;
}

//修改处方图
bool LY_AgriFertilizePescViewModel::UpdateFertilizePrescription(FertilizerPrescription &model)
{
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	QString strSql("UPDATE agri_fertilizer_prescription set ");
	strSql += "`field_id` = '" + model.field_id + "',";
	strSql += "`pescription_name` = '" + model.pescription_name + "',";
	strSql += "`data_name` = '" + model.data_name + "',";
	strSql += "`dis_map_name` = '" + model.dis_map_name + "',";
	strSql += "`nutrient_ele` = '" + model.nutrient_ele + "',";
	strSql += "`datamap_id` = '" + model.datamap_id + "',";
	strSql += "`dataset_id` = '" + model.dataset_id + "',";
	strSql += "`pyfk` = '" + QString::number(model.pyfk) + "'";
	strSql += "where `guid` = '" + model.guid + "'";

	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	return success;
}

//删除处方图
bool LY_AgriFertilizePescViewModel::DeleteFertilizePrescription(QString &guid)
{
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	QString strSql("DELETE FROM agri_fertilizer_prescription where guid = '");
	strSql += guid + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success)
	{
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
		
	}
	return success;
}

//添加处方图info
bool LY_AgriFertilizePescViewModel::AddFertilizePrescriptionInfo(FertilizerPrescriptionGridInfo &model)
{
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	QString strSql("INSERT INTO agri_fertilizer_prescription_info (`guid`, `color_name`, `color_dis`, `grid_num`, `grid_size`, `fert_dosage`, `fert_name`) VALUES (");
	strSql += "'" + model.guid + "',";
	strSql += "'" + model.color_name + "',";
	strSql += "'" + model.color_dis + "',";
	strSql += "'" + QString::number(model.grid_num) + "',";
	strSql += "'" + QString::number(model.grid_size) + "',";
	strSql += "'" + model.fert_dosage + "',";
	strSql += "'" + model.fert_name + "'";
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	return success;
}

//获取处方图info
FertilizerPrescriptionGridInfoVec LY_AgriFertilizePescViewModel::GetFertilizePrescriptionInfo(const QString map_id)
{

	QMutexLocker locker(&sMutex);

	QSqlQuery query;
	QString strSql = "SELECT `guid`, `color_name`, `color_dis`, `grid_num`, `grid_size`, `fert_dosage`, `fert_name`";
	QString strGeneralSql = strSql + " FROM " + "agri_fertilizer_prescription_info" + " WHERE guid = \"" + map_id + "\";";
	FertilizerPrescriptionGridInfoVec tempVec;
	tempVec.clear();
	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();

			FertilizerPrescriptionGridInfo pInfo;
			pInfo.guid = str1;
			pInfo.color_name = str2;
			pInfo.color_dis = str3;
			pInfo.grid_num = str4.toInt();
			pInfo.grid_size = str5.toInt();
			pInfo.fert_dosage = str6;
			pInfo.fert_name = str7;
			tempVec.push_back(pInfo);
		}
	}
	return tempVec;
}

//修改处方图
bool LY_AgriFertilizePescViewModel::UpdateFertilizePrescriptionInfo(FertilizerPrescriptionGridInfo &model)
{
	QMutexLocker locker(&sMutex);
	QString strSql("UPDATE agri_fertilizer_prescription_info set ");
	strSql += "`color_name` = '" + model.color_name + "',";
	strSql += "`color_dis` = '" + model.color_dis + "',";
	strSql += "`grid_num` = '" + QString::number(model.grid_num) + "',";
	strSql += "`grid_size` = '" + QString::number(model.grid_num) + "',";
	strSql += "`fert_dosage` = '" + model.fert_dosage + "'";
	strSql += "where `guid` = '" + model.guid + "'";

	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	return success;
}

//删除处方图
bool LY_AgriFertilizePescViewModel::DeleteFertilizePrescriptionInfo(QString &guid)
{
	QMutexLocker locker(&sMutex);
	QString strSql("DELETE FROM agri_fertilizer_prescription_info where guid = '");
	strSql += guid + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

//添加任务指令
bool  LY_AgriFertilizePescViewModel::AddTaskInstruction(AgriTaskInstruction &Instruction) const
{
	QMutexLocker locker(&sMutex);

	QString strSql("INSERT INTO agri_task_Instruction_fertilize (`spray_guid`, `lat`, `lon`, `pygd`, `pyfk`, `pysb`, `pzlx`, `pyfs`, `pykg`, `rote_speed`, `flow_speed`,`droplet_size`, `penzui_size`, `point_status`) VALUES (");
	//strSql += "'" + QString::number(listModel.nID) + "',";
	strSql += "'" + Instruction.spray_guid + "',";
	strSql += "'" + QString::number(Instruction.lat, 'f', '7') + "',";
	strSql += "'" + QString::number(Instruction.lon, 'f', '7') + "',";
	strSql += "'" + QString::number(Instruction.pygd, 'f', '1') + "',";
	strSql += "'" + QString::number(Instruction.pyfk, 'f', '1') + "',";
	strSql += "'" + Instruction.pysb + "',";
	strSql += "'" + Instruction.pzlx + "',";
	strSql += "'" + Instruction.pyfs + "',";
	strSql += "'" + Instruction.pykg + "',";
	strSql += "'" + QString::number(Instruction.rote_speed, 'f', '2') + "',";
	strSql += "'" + QString::number(Instruction.flow_speed, 'f', '2') + "',";
	strSql += "'" + QString::number(Instruction.droplet_size, 'f', '2') + "',";
	strSql += "'" + QString::number(Instruction.penzui_size) + "',";
	strSql += "'" + QString::number(Instruction.point_status) + "'";
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	return success;
}
//删除任务指令
bool  LY_AgriFertilizePescViewModel::DeleteTaskInstruction() const
{
	QMutexLocker locker(&sMutex);
	QString strSql("DELETE FROM agri_task_Instruction_fertilize");
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}
QString LY_AgriFertilizePescViewModel::getFertilizeName(int &valid_identify)
{
	return parseValidBit(valid_identify);
}
void LY_AgriFertilizePescViewModel::setFieldId(QString field_id, QString field_name)
{
	m_fieldName = field_name;
	m_currentFieldID = field_id;
}

void LY_AgriFertilizePescViewModel::draw3DGridMap_Update(double space_width, double degress, QString area_id)
{
	QMutexLocker locker(&sMutex);
	if (m_map3D != nullptr)
	{
		m_map3D->cleanMap();
	}

	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

	m_map3D = new HeatMap3D(mapNode);

	m_boundPoings.clear();
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = LY_AgriFertilizerPrescriptionActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	m_fieldName = m_fieldName.isEmpty() ? area_id : m_fieldName;
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_fieldName.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			double curLon = itrZone.stRoundParameter.dLon;
			double curLat = itrZone.stRoundParameter.dLat;

			if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
				itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
				if (itrZone.stZonePointVec.size() > 0) {

					for (auto zone : itrZone.stZonePointVec)
					{
						QPointF pointF;
						pointF.setX(zone.dLon);
						pointF.setY(zone.dLat);
						m_boundPoings.push_back(pointF);
					}
				}
			}
		}
	}
	if (m_boundPoings.size() <= 0)
	{
		return;
	}
	m_space_width = space_width;
	m_map3D->setSpace(space_width);
	m_map3D->setBoundPoints(m_boundPoings);
	m_map3D->startCreateHeatMap_forDisPosition(osg::DegreesToRadians(degress));
}

void LY_AgriFertilizePescViewModel::draw3DGridMap(AgriSoilTestingFertilityPointPtrVec soilFertilityVec, double space_width, QString area_id /*= ""*/)
{
	QMutexLocker locker(&sMutex);

	//auto pWidget = LY_AgriFertilizerPrescription::Get().GetDomainWidget();
	//QMetaObject::invokeMethod(pWidget, [&]() {
		if (m_map3D != nullptr)
		{
			m_map3D->cleanMap();
		}

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		m_map3D = new HeatMap3D(mapNode);

		m_boundPoings.clear();
		qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = LY_AgriFertilizerPrescriptionActivator::Get().getPIZoneManageService()->GetAllZoneBase();
		m_fieldName = m_fieldName.isEmpty() ? area_id : m_fieldName;
		for (const auto &itrZone : zoneBaseVec)
		{
			if (0 == m_fieldName.compare(QString::fromLocal8Bit(itrZone.strID)))
			{
				double curLon = itrZone.stRoundParameter.dLon;
				double curLat = itrZone.stRoundParameter.dLat;

				if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
					itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
					if (itrZone.stZonePointVec.size() > 0) {

						for (auto zone : itrZone.stZonePointVec)
						{
							QPointF pointF;
							pointF.setX(zone.dLon);
							pointF.setY(zone.dLat);
							m_boundPoings.push_back(pointF);
						}
					}
				}
			}
		}
		if (m_boundPoings.size() <= 0)
		{
			return;
		}
		m_space_width = space_width;
		m_map3D->setSpace(space_width);
		m_map3D->setBoundPoints(m_boundPoings);
		//m_map3D->startCreateHeatMap_forDisPosition(osg::DegreesToRadians(20.0));
// 		std::vector<double> _lons;
// 		std::vector<double> _lats;
// 		std::vector<double> _values;
// 		std::vector<double> _origins;
// 
// 		for (AgriSoilTestingFertilityPointPtr &ptr : soilFertilityVec)
// 		{
// 			_lons.push_back(ptr->lon);
// 			_lats.push_back(ptr->lat);
// 			_values.push_back(ptr->fertility);
// 			_origins.push_back(ptr->origin);
// 		}
// 		m_map3D->setLonLatValue(_lons, _lats, _values);
// 
// 		// 一级扩散
// 		QList<QPair<QString, std::string>> legendData1 = {
// 			{ "#FF9800", "一级" }
// 		};
// 
// 		// 二级扩散
// 		QList<QPair<QString, std::string>> legendData2 = {
// 			{ "#4CAF50", "一级" },{ "#FF9800", "二级" }
// 		};
// 
// 		// 五级扩散
// 		QList<QPair<QString, std::string>> legendData3 = {
// 			{ "#4CAF50", "一级" },{ "#CDDC39", "二级" },{ "#FF9800", "三级" },{ "#E53935", "四级" },{ "#8F1D1D", "五级" }
// 		};
// 		m_colorVec.clear();
// 		for (const auto& pair : legendData3)
// 		{
// 			m_colorVec.push_back(pair.first);
// 		}
// 
// 
// 		// 十级扩散
// 		QList<QPair<QString, std::string>> legendData4 = {
// 			{ "#3EA044", "一级" },{ "#4CAF50", "二级" },{ "#8BC34A", "三级" },{ "#CDDC39", "四级" },{ "#FFC107", "五级" },
// 			{ "#FF9800", "六级" },{ "#FF5722", "七级" },{ "#E53935", "八级" },{ "#C62828", "九级" },{ "#8F1D1D", "十级" }
// 		};
// 
// 		m_mDiffusionLayerHexColor.insert(std::make_pair(1, legendData1));
// 		m_mDiffusionLayerHexColor.insert(std::make_pair(2, legendData2));
// 		m_mDiffusionLayerHexColor.insert(std::make_pair(3, legendData3));
// 		m_mDiffusionLayerHexColor.insert(std::make_pair(4, legendData4));
// 
// 		// 十六进制的 RGB 颜色值osgEarth::Symbology::Color 对象进行存储
// 		for (const auto& pair : m_mDiffusionLayerHexColor)
// 		{
// 			std::vector<osgEarth::Symbology::Color> osgColor;
// 			for (const auto& legend : pair.second)
// 			{
// 				osgEarth::Symbology::Color c = hexToColor(legend.first.toStdString());
// 				osgColor.emplace_back(c);
// 			}
// 			m_mDiffusionLayerOsgColor.insert(std::make_pair(pair.first, osgColor));
// 		}
// 		auto colorLayer = m_mDiffusionLayerOsgColor.at(3);
// 		std::reverse(colorLayer.begin(), colorLayer.end());
// 		// 插入底色
// 		colorLayer.insert(colorLayer.begin(), hexToColor("#30913C", 0.95f));
// 		m_map3D->updateColors(colorLayer);
// 
// 		double angle = osg::DegreesToRadians(10.0);
// 		m_map3D->startCreateHeatMap_forDisPosition(angle);
// 		QMap<double, int> keyCounts = m_map3D->keyCounts;
// 		for (AgriSoilTestingFertilityPointPtr ptr : soilFertilityVec)
// 		{
// 			auto it = keyCounts.find(ptr->fertility);
// 			if (it != keyCounts.end()) {
// 				int value = it.value(); // 获取值
// 				soilFertilityMap[ptr->fertility] = ptr;
// 			}
// 		}

	
}
HeatMap3D *LY_AgriFertilizePescViewModel::getHeatMap3D() 
{
	return m_map3D;
}
FertilizerPrescriptionGridInfoVec LY_AgriFertilizePescViewModel::getMapInfoData() const
{
	FertilizerPrescriptionGridInfoVec tempInfoVec;
	QMap<double, int> keyCounts = m_map3D->keyCounts;
	for (auto it = keyCounts.begin(); it != keyCounts.end(); ++it) {
		FertilizerPrescriptionGridInfo info;
		info.grid_num = it.value();
		info.grid_size = m_space_width;
		tempInfoVec.push_back(info);
	}
	FertilizerPrescriptionGridInfoVec tempInfoVec1;
	for (size_t i = 0; i < m_colorVec.size(); i++)
	{
		if (i < tempInfoVec.size())
		{
			FertilizerPrescriptionGridInfo info = tempInfoVec.at(i);
			info.color_name = m_colorVec.at(i);
			tempInfoVec1.push_back(info);
		}
	}
	FertilizerPrescriptionGridInfoVec tempInfoVec2;
	std::vector<QString> tempVec({ "150", "175", "225","275","300"});
	for (size_t i = 0; i < tempVec.size(); i++)
	{
		if (i < tempInfoVec1.size())
		{
			FertilizerPrescriptionGridInfo info = tempInfoVec1.at(i);
			info.color_dis = tempVec.at(i);
			tempInfoVec2.push_back(info);
		}
	}

	FertilizerPrescriptionGridInfoVec tempInfoVec3;
	int index = 0;
	for (auto it = soilFertilityMap.begin(); it != soilFertilityMap.end(); ++it) {

		if (index < tempInfoVec2.size())
		{
			FertilizerPrescriptionGridInfo info = tempInfoVec2.at(index);
			double value = info.color_dis.toDouble() - it->second->origin;
			info.fert_dosage = QString::number(value, 'f', 2);
			tempInfoVec3.push_back(info);
		}
		index++;
	}
	return tempInfoVec3;

}
void LY_AgriFertilizePescViewModel::draw2DGridMap(QPainter &painter, IProj *pProj)
{
	if (m_map3D != nullptr)
	{
		m_map3D->create2DGridDrawing(painter, pProj);
	}
}
void LY_AgriFertilizePescViewModel::clear3dMap()
{
	QMutexLocker locker(&sMutex);
	if (m_map3D != nullptr)
	{
		m_map3D->cleanMap();
	}
}
void LY_AgriFertilizePescViewModel::generateFertilizerDiagramFlyLine(double pyfk, double flyHeight)
{
	//QMutexLocker locker(&sMutex);

	double flyDisancee;
	m_flyPoints.clear();
	LY_AgriFertilizerPrescriptionActivator::Get().getPAgriSprayPrescriptionService()->generateFertilizerDiagramFlyLine(
		m_boundPoings,
		m_map3D->gridVec,
		pyfk,
		flyHeight,
		m_flyPoints,
		flyDisancee
	);
		//}, Qt::QueuedConnection);

}
std::vector< AirInstructionPoi> LY_AgriFertilizePescViewModel::generateInstructionPoint(double pyfk)
{
	return LY_AgriFertilizerPrescriptionActivator::Get().getPAgriSprayPrescriptionService()->generateInstructionPoint(4.0);
}
//清理航线数据
void LY_AgriFertilizePescViewModel::clearFlyLine()
{
	QMutexLocker locker(&sMutex);

	//auto pWidget = LY_AgriFertilizerPrescription::Get().GetDomainWidget();
	//QMetaObject::invokeMethod(pWidget, [&]() {
		LY_AgriFertilizerPrescriptionActivator::Get().getPAgriSprayPrescriptionService()->clearn3DflyData();
		//}, Qt::QueuedConnection);
	
}
//模拟飞行
void LY_AgriFertilizePescViewModel::staringFly(double flyHeight)
{
	//QMutexLocker locker(&sMutex);

	//auto pWidget = LY_AgriFertilizerPrescription::Get().GetDomainWidget();
	//QMetaObject::invokeMethod(pWidget, [&]() {
		LY_AgriFertilizerPrescriptionActivator::Get().getPAgriSprayPrescriptionService()->flyRouteSimulPlayback(flyHeight, false);
		//}, Qt::QueuedConnection);

}
void LY_AgriFertilizePescViewModel::staringInstructionFly(double isPlayInstruction)
{
	LY_AgriFertilizerPrescriptionActivator::Get().getPAgriSprayPrescriptionService()->playInstructionFlying(isPlayInstruction);
}
void LY_AgriFertilizePescViewModel::showAndHideMap(bool isShow)
{
	QMutexLocker locker(&sMutex);

	if (m_map3D != nullptr)
	{
		m_map3D->setImageShowOrHide_forDisPosition(isShow);
	}
	
}
osgEarth::Symbology::Color LY_AgriFertilizePescViewModel::hexToColor(const std::string& hexColor, float rgb_a/* = 1.0f*/)
{
	{
		unsigned int r, g, b;
		if (sscanf(hexColor.c_str(), "#%02X%02X%02X", &r, &g, &b) == 3)
		{
			return osgEarth::Symbology::Color(r / 255.0f, g / 255.0f, b / 255.0f, rgb_a); // 透明度默认为 1.0
		}
		return osgEarth::Symbology::Color(); // 失败返回默认颜色
	}
}
std::vector<std::vector<int>> LY_AgriFertilizePescViewModel::groupConsecutiveNumbers(const std::vector<int>& nums, bool isLeft) {
	std::vector<std::vector<int>> result;

	if (nums.empty()) return result;

	std::vector<int> currentGroup = { nums[0] };

	for (size_t i = 1; i < nums.size(); ++i) {
		if (isLeft)
		{
			if (nums[i] == nums[i - 1] + 1) {
				// 当前数字与前一个数字相邻，加入当前组
				currentGroup.push_back(nums[i]);
			}
			else {
				// 不相邻，将当前组加入结果，开始新组
				result.push_back(currentGroup);
				currentGroup = { nums[i] };
			}
		}
		else
		{
			if (nums[i] == nums[i - 1] - 1) {
				// 当前数字与前一个数字相邻，加入当前组
				currentGroup.push_back(nums[i]);
			}
			else {
				// 不相邻，将当前组加入结果，开始新组
				result.push_back(currentGroup);
				currentGroup = { nums[i] };
			}
		}

	}

	// 添加最后一组
	result.push_back(currentGroup);

	return result;
}
QString LY_AgriFertilizePescViewModel::parseValidBit(int valid_identify)
{
	if (!isValidIdentify(valid_identify)) {
		return "无效标识（必须恰好一位为1）";
	}

	switch (valid_identify) {
	case 1 << 0:  return tr2("氮");
	case 1 << 1:  return tr2("磷");
	case 1 << 2:  return tr2("钾");
	case 1 << 3:  return tr2("钙");
	case 1 << 4:  return tr2("镁");
	case 1 << 5:  return tr2("铜");
	case 1 << 6:  return tr2("锌");
	case 1 << 7:  return tr2("铁");
	case 1 << 8:  return tr2("硫");
	case 1 << 9:  return tr2("锰");
	case 1 << 10: return tr2("钼");
	case 1 << 11: return tr2("硼");
	case 1 << 12: return tr2("氯");
	case 1 << 13: return tr2("硅");
	case 1 << 14: return tr2("铅");
	case 1 << 15: return tr2("铬");
	case 1 << 16: return tr2("镉");
	case 1 << 17: return tr2("汞");
	case 1 << 18: return tr2("有机质");
	case 1 << 19: return tr2("温度");
	case 1 << 20: return tr2("湿度");
	case 1 << 21: return tr2("PH");
	case 1 << 22: return tr2("EC");
	case 1 << 23: return tr2("水势");
	default:      return "未知位";
	}
}