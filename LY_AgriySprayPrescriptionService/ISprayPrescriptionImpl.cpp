#include "LY_AgriSprayPrescriptionActivator.h"
#include "ISprayPrescriptionImpl.h"
#include "LY_AgriySprayPrescriptionService.h"
#include "LY_ShowTaskDataInfo.h"
#include "QSqlQuery"
#include <QVariant>
#include <memory>
#include <QSettings>
#include <QSqlError>
#include "QSqlQuery"
#include <QDebug>
#include <QTimer>
#include <QGeoCoordinate>
#include "CreateShape.h"
#include "qmath.h"


using namespace qnzkna::AgriSprayPrescription;
using namespace  AgriSoilTestingFertilizer;

ISprayPrescriptionImpl::ISprayPrescriptionImpl()
	:m_flyTool(new LY_AgriSpayPredictTool)
{
	InitDataBase();
}

void ISprayPrescriptionImpl::InitDataBase() const
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = ini.value("DbName", "LY").toString();
	QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = ini.value("User", "root").toString();
	QString strDbPass = ini.value("Password", "123456").toString();
	QString strDbDriver = ini.value("Driver", "QMYSQL").toString();
	

	//db = QSqlDatabase::addDatabase(strDbDriver);
	//db.setHostName(strDbIP);
	//db.setPort(3306);
	//db.setDatabaseName(strDbName);
	//db.setUserName(strDbUser);
	//db.setPassword(strDbPass);
	//isOpen = db.open();
}
 QString ISprayPrescriptionImpl::GetFarmName(const QString& str) const
{
	 if (str.isEmpty())
	 {
		 return false;
	 }
	 QString strSql = "SELECT `name`";
	 QString strGeneralSql = strSql + " FROM `agri_farm`" + " where id = ";
	 strGeneralSql += "'" + str + "'";


	 QSqlQuery query;
	 bool success = query.exec(strGeneralSql);
	 QString str1;
	 if (success)
	 {
		 while (query.next())
		 {
			  str1 = query.value(0).toString();
		 }
	 }
	 return str1;

}

 AgriPestCollectionVec ISprayPrescriptionImpl::GetPestCollectionList(const QString& sDatasetId) const
 {
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

	 AgriPestCollectionVec pestDatasetPtrVec;

	 QString strSql = "SELECT `guid`, `name`, `dataset_id`, `collect_type`, `lon`, `lat`, `alt`, `category`, `stage`, `count`, `range`, `level`";
	 QString strGeneralSql = strSql + " FROM " + "agri_pest_collection" + " WHERE dataset_id = \"" + sDatasetId + "\";";

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
			 QString str11 = query.value(10).toString();
			 QString str12 = query.value(11).toString();

			 AgriPestCollection pInfo;
			 pInfo.guid = str1;
			 pInfo.name = str2;
			 pInfo.dataset_id = str3;
			 pInfo.collect_type = str4.toInt();
			 pInfo.lon = str5.toDouble();
			 pInfo.lat = str6.toDouble();
			 pInfo.alt = str7.toDouble();
			 pInfo.category = str8.toInt();
			 pInfo.stage = str9.toInt();
			 pInfo.count = str10.toInt();
			 pInfo.range = str11.toDouble();
			 pInfo.level = str12.toInt();

			 pestDatasetPtrVec.push_back(pInfo);
		 }
	 }

	 return pestDatasetPtrVec;
 }
 //获取生长阶段
 AgriGrowthStageVec ISprayPrescriptionImpl::GetGrowthStageList() const
 {
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

	 AgriGrowthStageVec tempVec;

	 QString strSql = "SELECT `id`, `sub_phase`, `sub_phase_name`";
	 QString strGeneralSql = strSql + " FROM " + "agri_maize_growth_stage";

	 bool success = query.exec(strGeneralSql);
	 if (success)
	 {
		 while (query.next())
		 {
			 QString str1 = query.value(0).toString();
			 QString str2 = query.value(1).toString();
			 QString str3 = query.value(2).toString();
			 //QString str4 = query.value(3).toString();

			 AgriGrowthStage pInfo;
			 pInfo.id = str1.toInt();
			 pInfo.sub_phase = str2;
			 pInfo.sub_phase_name = str3;
			// pInfo.sub_phase_name = str4.toInt();

			 tempVec.push_back(pInfo);
		 }
	 }
	 return tempVec;
 }
 AgriPestMapInfoModel ISprayPrescriptionImpl::GetPestInfoMap(const QString& sDatasetId) const
 {
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

 /*diffuse_layer: disCount
	 equalCount : 0

	 grid_unit : spacing*/
	 QString strSql = "SELECT `guid`, `name`, `dataset_id`, `crop_type`, `growth_stage`, `field_area`, `collect_count`, `grid_unit`, `diffuse_layer`, `distribute_statistic`";
	 QString strGeneralSql = strSql + " FROM " + "agri_pest_distribute_map" + " WHERE dataset_id = \"" + sDatasetId + "\";";

	 bool success = query.exec(strGeneralSql);
	 AgriPestMapInfoModel pInfo;
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

			 
			 pInfo.guid = str1;
			 pInfo.name = str2;
			 pInfo.dataset_id = str3;
			 pInfo.crop_type = str4.toInt();
			 pInfo.growth_stage = str5.toInt();
			 pInfo.field_area = str6.toDouble();
			 pInfo.collect_count = str7.toInt();
			 pInfo.grid_unit = str8.toInt();
			 pInfo.diffuse_layer = str9.toInt();
			 pInfo.distribute_statistic = str10;

			 return pInfo;
		 }
	 }
	 return pInfo;
	
 }
 //添加处方图
 bool ISprayPrescriptionImpl::AddSprayPrescription(AgriSprayPrescriptionModel &model) const
 {
	 QMutexLocker locker(&sMutex);
	 
	 QString strSql("INSERT INTO agri_prescription_map (`guid`, `work_type`, `map_name`, `data_source_id`, `field_id`, `crop_type`, `pest_category`, `pest_disease`, `compute_type`, `map_level`, `leve_arr`, `leve_value`, `pyfk`, `pygd`) VALUES (");
	 //strSql += "'" + QString::number(listModel.nID) + "',";
	 strSql += "'" + model.guid + "',";
	 strSql += "'" + QString::number(model.work_type) + "',";
	 strSql += "'" + model.map_name + "',";
	 strSql += "'" + model.data_source_id + "',";
	 strSql += "'" + model.field_id + "',";
	 strSql += "'" + QString::number(model.crop_type) + "',";
	 strSql += "'" + QString::number(model.pest_category) + "',";
	 strSql += "'" + QString::number(model.pest_disease) + "',";
	 strSql += "'" + QString::number(model.compute_type) + "',";
	 strSql += "'" + QString::number(model.map_level) + "',";
	 strSql += "'" + LY_AgriSPrayPrescriptionUntil::vector2DToString(model.leve_arr) + "',";
	 strSql += "'" + LY_AgriSPrayPrescriptionUntil::vectorDoubleToString(model.leve_value) + "',";
	 strSql += "'" + QString::number(model.pyfk) + "',";
	 strSql += "'" + QString::number(model.pygd) + "'";
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
 AgriSprayPrescriptionVec ISprayPrescriptionImpl::GetSprayPrescription(const QString field_id) const
 {
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

	 AgriSprayPrescriptionVec tempVec;

	 QString strSql = "SELECT `guid`, `work_type`, `map_name`, `data_source_id`, `field_id`, `crop_type`, `pest_category`, `pest_disease`, `compute_type`, `map_level`, `leve_arr`, `leve_value`, `pyfk`, `pygd`";
	 QString strGeneralSql = strSql + " FROM " + "agri_prescription_map" + " WHERE field_id = \"" + field_id + "\";";

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
			 QString str11 = query.value(10).toString();
			 QString str12 = query.value(11).toString();
			 QString str13 = query.value(12).toString();
			 QString str14 = query.value(13).toString();

			 AgriSprayPrescriptionModel pInfo;
			 pInfo.guid = str1;
			 pInfo.work_type = str2.toInt();
			 pInfo.map_name = str3;
			 pInfo.data_source_id = str4;
			 pInfo.field_id = str5;
			 pInfo.crop_type = str6.toInt();
			 pInfo.pest_category = str7.toInt();
			 pInfo.pest_disease = str8.toInt();
			 pInfo.compute_type = str9.toInt();
			 pInfo.map_level = str10.toInt();
			 pInfo.leve_arr = LY_AgriSPrayPrescriptionUntil::stringToVector2D(str11);
			 pInfo.leve_value = LY_AgriSPrayPrescriptionUntil::stringToVectorDouble(str12);
			 pInfo.pyfk = str13.toInt();
			 pInfo.pygd = str14.toInt();
			 // pInfo.sub_phase_name = str4.toInt();

			 tempVec.push_back(pInfo);
		 }
	 }
	 return tempVec;
 }

 //修改处方图
 bool ISprayPrescriptionImpl::UpdateSprayPrescription(AgriSprayPrescriptionModel &model) const
 {

	 QMutexLocker locker(&sMutex);
	 QString strSql("UPDATE agri_prescription_map set ");
	 strSql += "`work_type` = '" + QString::number(model.work_type) + "',";
	strSql += "`map_name` = '" + model.map_name + "',";
	 strSql += "`data_source_id` = '" + model.data_source_id + "',";
	 strSql += "`field_id` = '" + model.field_id + "',";
	 strSql += "`crop_type` = '" + QString::number(model.crop_type) + "',";
	 strSql += "`pest_category` = '" + QString::number(model.pest_category) + "',";
	 strSql += "`pest_disease` = '" + QString::number(model.pest_disease) + "',";
	 strSql += "`compute_type` = '" + QString::number(model.compute_type) + "',";
	 strSql += "`map_level` = '" + QString::number(model.map_level) + "',";
	 strSql += "`leve_arr` = '" + LY_AgriSPrayPrescriptionUntil::vector2DToString(model.leve_arr) + "',";
	 strSql += "`leve_value` = '" + LY_AgriSPrayPrescriptionUntil::vectorDoubleToString(model.leve_value) + "',";
	 strSql += "`pyfk` = '" + QString::number(model.pyfk) + "',";
	 strSql += "`pygd` = '" + QString::number(model.pygd) + "'";
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
 bool ISprayPrescriptionImpl::DeleteSprayPrescription(QString &guid) const
 {

	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_prescription_map where guid = '");
	 strSql += guid + "'";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 return success;
 }
AgriPestDatasetVec ISprayPrescriptionImpl::GetPestList(const QStringList& fieldList) const
{

	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriPestDatasetVec pestDatasetPtrVec;


	QString strSql = "SELECT `guid`, `name`, `field_id`, `crop_type`,  `pest_category`";
	QString strGeneralSql = strSql + " FROM " + "agri_pest_dataset";

	if (fieldList.size())
	{
		QString idString = fieldList.join(",\"");

		// 执行 DELETE 语句
		strGeneralSql += QString(" WHERE %1 IN (%2);")
			.arg("field_id")
			.arg(QString("\"" + idString + "\""));
	}

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

			AgriPestDataset pInfo;
			pInfo.guid = str1;
			pInfo.name = str2;
			pInfo.field_id = str3;
			pInfo.crop_type = str4.toInt();
			//pInfo.collect_type = str5.toInt();
			//pInfo.begin_time = str6;
			//pInfo.end_time = str7;
			pInfo.pest_disease = str5.toInt();
			//pInfo.pest_category = str9.toInt();
			
			pestDatasetPtrVec.push_back(pInfo);
		}
	}

	return pestDatasetPtrVec;
}
 //获取任务指令
 bool  ISprayPrescriptionImpl::GetTaskInstruction() const
 {
	 return true;
 }
 //添加任务指令
 bool  ISprayPrescriptionImpl::AddTaskInstruction(AgriTaskInstruction &Instruction) const
 {
	 QMutexLocker locker(&sMutex);

	 QString strSql("INSERT INTO agri_task_Instruction_spay (`spray_guid`, `lat`, `lon`, `pygd`, `pyfk`, `pysb`, `pzlx`, `pyfs`, `pykg`, `rote_speed`, `flow_speed`,`droplet_size`, `penzui_size`, `point_status`) VALUES (");
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
 bool  ISprayPrescriptionImpl::DeleteTaskInstruction() const
 {
	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_task_Instruction_spay");
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 return success;
 }


 void ISprayPrescriptionImpl::generateSprayPrescriptionDiagram(std::vector<QPointF> m_boundPoings,
	 QString pest_id, double pyfk, int map_level, bool isLoitering)
 {
	 //QMutexLocker locker(&sMutex);

	// auto pWidget = LY_AgriySprayPrescriptionService::Get().GetDomainWidget();
	// QMetaObject::invokeMethod(pWidget, [=]() {
		//生成处方图
		std::unordered_map<int, QList<QPair<QString, std::string>>>				m_mDiffusionLayerHexColor;
		std::unordered_map<int, std::vector<osgEarth::Symbology::Color>>			m_mDiffusionLayerOsgColor;
		//QString color1 = isLoitering ? "1e6527" : "#3EA044";
		//QString color2 = isLoitering ? "1e6527" : "#3EA044";
		//QString color3 = isLoitering ? "1e6527" : "#3EA044";
		//QString color4 = isLoitering ? "1e6527" : "#3EA044";
		//QString color5 = isLoitering ? "1e6527" : "#C62828";
		// 一级
		QList<QPair<QString, std::string>> legendData1 = {
			{ isLoitering ? "#1e6527" : "#3EA044", "1" }
		};
		// 二级
		QList<QPair<QString, std::string>> legendData2 = {
			{ isLoitering ? "#1e6527" : "#3EA044", "1" },{ isLoitering ? "#1e6527" : "#8BC34A", "2" }
		};
		// 三级
		QList<QPair<QString, std::string>> legendData3 = {
			{ isLoitering ? "#1e6527" : isLoitering ? "1e6527" : "#3EA044", "1" },{ isLoitering ? "#1e6527" : "#8BC34A", "2" },{ isLoitering ? "#1e6527" : "#FFC107", "3" }
		};
		// 四级
		QList<QPair<QString, std::string>> legendData4 = {
			{ isLoitering ? "#1e6527" : "#3EA044", "1" },{ isLoitering ? "#1e6527" : "#8BC34A", "2" },{ isLoitering ? "#1e6527" : "#FFC107", "3" },{ isLoitering ? "#1e6527" : "#FF5722", "4" }
		};
		// 五级
		QList<QPair<QString, std::string>> legendData5 = {
			{ isLoitering ? "#1e6527" : "#3EA044", "1" },{ isLoitering ? "#1e6527" : "#8BC34A", "2" },{ isLoitering ? "#1e6527" : "#FFC107", "3" },{ isLoitering ? "#1e6527" : "#FF5722", "4" },{ isLoitering ? "#1e6527" : "#C62828", "5" }
		};

		m_mDiffusionLayerHexColor.insert(std::make_pair(1, legendData1));
		m_mDiffusionLayerHexColor.insert(std::make_pair(2, legendData2));
		m_mDiffusionLayerHexColor.insert(std::make_pair(3, legendData3));
		m_mDiffusionLayerHexColor.insert(std::make_pair(4, legendData4));
		m_mDiffusionLayerHexColor.insert(std::make_pair(5, legendData5));

		// 十六进制的 RGB 颜色值osgEarth::Symbology::Color 对象进行存储
		for (const auto& pair : m_mDiffusionLayerHexColor)
		{
			std::vector<osgEarth::Symbology::Color> osgColor;
			for (const auto& legend : pair.second)
			{
				osgEarth::Symbology::Color c = LY_AgriSPrayPrescriptionUntil::hexToColor(legend.first.toStdString());
				osgColor.emplace_back(c);
			}
			m_mDiffusionLayerOsgColor.insert(std::make_pair(pair.first, osgColor));
		}

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		HeatMap3D map3D(mapNode);
		m_map3D.cleanMap();

		if (m_boundPoings.size() > 0)
		{
			map3D.setSpace(pyfk);
			map3D.setBoundPoints(m_boundPoings);

			AgriPestMapInfoModel pestMapInfo = GetPestInfoMap(pest_id);

			auto tempVec = GetPestCollectionList(pest_id);

			std::vector<double> _lons;
			std::vector<double> _lats;
			std::vector<double> _levels;

			for (auto m : tempVec)
			{
				_lons.push_back(m.lon);
				_lats.push_back(m.lat);
				_levels.push_back(static_cast<double>(m.level));
			}

			std::vector<Poi> difPoints;
			AgriDataAnalysisPredictAlgorithm::createGridPointDiffuse(m_boundPoings, _lons, _lats, _levels, 1.0 * (1.0 / 110000.0), pestMapInfo.diffuse_layer, difPoints);

			std::vector<double> _newLons;
			std::vector<double> _newlats;
			std::vector<double> _newvalues;
			for (const auto &poi : difPoints)
			{
				_newLons.push_back(poi.lon);
				_newlats.push_back(poi.lat);
				_newvalues.push_back(poi.alt);
			}

			map3D.setLonLatValue(_newLons, _newlats, _newvalues);

			auto colorLayer = m_mDiffusionLayerOsgColor.at(map_level);
			// 插入底色
			colorLayer.insert(colorLayer.begin(), LY_AgriSPrayPrescriptionUntil::hexToColor("#1e6527", 0.95f));
			map3D.updateColors(colorLayer);

			double angle = osg::DegreesToRadians(10.0);
			map3D.startCreateHeatMap_forDisPosition(angle, isLoitering);

			m_map3D = map3D;
		}
	 //}, Qt::QueuedConnection);
 }

 void ISprayPrescriptionImpl::clearSprayDiagramData()
 {
	 QMutexLocker locker(&sMutex);

	 m_map3D.cleanMap();
 }
 void ISprayPrescriptionImpl::showSprayDiagramData(bool isShow)
 {
	 m_map3D.setImageShowOrHide_forDisPosition(isShow);
 }
 void ISprayPrescriptionImpl::generateSprayDiagramFlyLine(std::vector<QPointF> m_boundPoings, 
	 QString spray_id, 
	 double pyfk, 
	 double flyHeight,
	 bool isLoitering,
	 std::vector<Poi> &flyPoints,
	 double &flyDisance )
 {
	 m_origin_poiPaths.clear();

	 GetPestCollectionList(spray_id);

	 std::vector<double> _lons;
	 std::vector<double> _lats;
	 std::vector<double> _levels;
	 std::vector<double> _values;
	 std::vector<Poi> points;

	 //TODO: 空管航线规划算法
	 //if (compute_type == 0)
	 //{

	 //	if (prescriptionModel.data_source_id.length() > 0)
	 //	{

	 //		std::vector<std::vector<QPointF>>foxy;

	 //		std::vector<double> rx;
	 //		std::vector<double> ry;
	 //		std::vector<Poi> stZonePointVec;
	 //		for (const auto &p: boundPoings)
	 //		{
	 //			_lons.push_back(p.x());
	 //			_lats.push_back(p.y());
	 //			points.push_back(Poi(p.x(), p.y()));
	 //		}
	 //		QPointF areaFarPos(_lons.at(0), _lats.at(0));
	 //		double spaceing = 4.0 *  (1.0 / 110000.0);
	 //		AgriDataAnalysisPredictAlgorithm::planning(_lons, _lats, foxy, points, areaFarPos, spaceing, rx, ry, planWay);

	 //		//std::vector<Poi> poiPaths;

	 //		for (int i = 0; i < rx.size(); i++)
	 //		{
	 //			Poi p;
	 //			p.lon = rx.at(i);
	 //			p.lat = ry.at(i);
	 //			m_origin_poiPaths.push_back(p);
	 //		}
	 //		QColor col(255, 254, 169, 100);
	 //		draw3DPathLine(m_origin_poiPaths, col, 4);
	 //	}
	 //}
	 //else
	 //{

	 if (spray_id.length() > 0)
	 {
		 std::vector<QPoint> pointPath;
		 for (const auto &m : m_map3D.gridVec) {

			 _lons.push_back(m.lon);
			 _lats.push_back(m.lat);
			 _levels.push_back(m.alt);
			 _values.push_back(LY_AgriSPrayPrescriptionUntil::getElevation(m.lon, m.lat));
			 points.push_back(Poi(m.lon, m.lat));
			 //pointPath.push_back(QPoint(m.lon, m.lat));
		 }
		 double spaceing = pyfk * 1.0 *  (1.0 / 110000.0);

		 //std::map<QPoint, Poi, QPointFCompare> pointMap;
		 m_pointMap.clear();
		 bool isSucee = AgriDataAnalysisPredictAlgorithm::createGridGeometryMap(m_boundPoings, _lons, _lats, _levels, spaceing, pointPath, m_pointMap);

		 if (isSucee)
		 {
			 //isGeneratingCourse = true;

			 m_paths.clear();
			 std::vector<QPoint> uploadPath;
			 AgriDataAnalysisPredictAlgorithm::generatePath(pointPath, m_paths, uploadPath);


			 tempPoints.clear();
			 m_flyPointMap.clear();
			 for (const auto &path : m_paths) {

				 auto it = m_pointMap.find(path);
				 if (it != m_pointMap.end()) {
					 Poi p = it->second;
					 m_origin_poiPaths.push_back(p);
					 if (isLoitering)
					 {
						 tempPoints.push_back(path);
						 m_flyPointMap[path] = p;
					 }
					 else {
						 if (p.alt > 0)
						 {
						     tempPoints.push_back(path);
						     m_flyPointMap[path] = p;
						  }
					 }
				
				 }
			 }

			 // 航点
			 std::vector<Poi> m_points;
			 for (const auto &path : uploadPath) {
				 auto it = m_pointMap.find(path);
				 if (it != m_pointMap.end()) {
					 m_points.push_back(it->second);
					
				 }
			 }
			 flyPoints = m_points;
			m_flyTool->setOriginPoints(m_points);

			 //m_origin_poiPaths.emplace(m_origin_poiPaths.begin(), Poi(104.0901304, 30.9468978));
			 //m_origin_poiPaths.emplace(m_origin_poiPaths.end(), Poi(104.0901304, 30.9468978));

			 flyDisance = 0;
			 for (int i = 0; i < m_points.size() - 1; i++)
			 {
				 QGeoCoordinate start = QGeoCoordinate(m_points[i].lat, m_points[i].lon);
				 QGeoCoordinate end = QGeoCoordinate(m_points[i + 1].lat, m_points[i + 1].lon);
				 double di = start.distanceTo(end);
				 flyDisance += di;
			 }

			 m_flyTool->setFlyDisance(flyDisance);
			 QColor col(255, 254, 169, 100);
			 m_flyTool->draw3DPathLine(flyPoints, col, 2, flyHeight);

			 m_flyTool->interpolateSegments(4.0, m_origin_poiPaths);

			 //LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->setIsSpray(true);
			 if (!isLoitering)
			 {
				 m_flyTool->m_isFerili = false;
			 }

			 TaskTextInfo textInfo;
			 textInfo.text11 = tr2("飞行高度");
			 textInfo.text12 = tr2("3m");
			 textInfo.text13 = tr2("飞行速度");
			 textInfo.text14 = tr2("5m/s");
			 textInfo.text21 = tr2("喷洒幅宽");
			 textInfo.text22 = tr2("4m");
			 textInfo.text23 = tr2("飞行距离");
			 textInfo.text24 = tr2("1450m");
			 textInfo.text31 = tr2("任务面积");
			 textInfo.text32 = tr2("7.7亩");
			 textInfo.text33 = tr2("雾滴颗粒");
			 textInfo.text34 = tr2("60um");
			 if (LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo())
			 {
				 LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo()->SetTaskTextInfo(textInfo);
			 }
		 }
	 }
 }
 void ISprayPrescriptionImpl::setFlylineStatus(bool isShow)
 {
	 m_flyTool->setupFlylineStatus(isShow);
 }
 void ISprayPrescriptionImpl::create2DSprayDiagram(QPainter& painter, IProj *pProj)
 {
	 m_map3D.create2DGridDrawing(painter, pProj);
 }
 void ISprayPrescriptionImpl::generateFertilizerDiagramFlyLine(std::vector<QPointF> m_boundPoings,
	 std::vector<AgriSoilTestingFertilityPoint> gridVec,
	 double pyfk,
	 double flyHeight,
	 std::vector<AgriSoilTestingFertilityPoint> &flyPoints,
	 double &flyDisancee)
 {
	 std::vector<AgriSoilTestingFertilityPoint> m_Fertility_poiPaths;
	 m_Fertility_poiPaths.clear();

	 std::vector<double> _lons;
	 std::vector<double> _lats;
	 std::vector<double> _levels;
	 std::vector<double> _values;
	 std::vector<QPoint> pointPath;

	 for (AgriSoilTestingFertilityPoint &ptr :gridVec) {
		 //if (ptr.fertility > 1)
		 //{
		 _lons.push_back(ptr.lon);
		 _lats.push_back(ptr.lat);
		 _levels.push_back(ptr.fertility);
		 //_values.push_back(getElevation(pt.lon, ptr->lat));
		 //}
	 }
	 

	 std::map<QPoint, Poi, QPointFCompare> pointMap;
	 m_pointMap.clear();

	 double spaceing = pyfk * 1.0 *  (1.0 / 110000.0);
	 bool isSucee = AgriDataAnalysisPredictAlgorithm::createFertilityGridGeometryMap(m_boundPoings, _lons, _lats, _levels, spaceing, pointPath, feripointMap);

	 if (isSucee)
	 {
		// isGeneratingCourse = true;

		 m_paths.clear();
		 std::vector<QPoint> uploadPath;
		 AgriDataAnalysisPredictAlgorithm::generatePath(pointPath, m_paths, uploadPath);


		 tempPoints.clear();
		 m_flyFeriliPointMap.clear();
		 for (const auto &path : m_paths) {

			 auto it = feripointMap.find(path);
			 if (it != feripointMap.end()) {
				 AgriSoilTestingFertilityPoint p = it->second;
				 m_Fertility_poiPaths.push_back(p);
				 Poi p1;
				 p1.lon = p.lon;
				 p1.lat = p.lat;
				 p1.alt = p.fertility;
				 
				 if (p.fertility > 0)
				 {
					 tempPoints.push_back(path);
					 m_flyFeriliPointMap[path] = p;
					 m_flyPointMap[path] = p1;
				 }
			 }
		 }

		 // 航点
		 std::vector<AgriSoilTestingFertilityPoint> m_points;
		 for (const auto &path : uploadPath) {
			 auto it = feripointMap.find(path);
			 if (it != feripointMap.end()) {
				 m_points.push_back(it->second);
			 }
		 }

		 //ui->flight_point->setText(QString::number(m_points.size()));

		 double ditance = 0.0;
		 for (int i = 0; i < m_points.size() - 1; i++)
		 {
			 QGeoCoordinate start = QGeoCoordinate(m_points[i].lat, m_points[i].lon);
			 QGeoCoordinate end = QGeoCoordinate(m_points[i + 1].lat, m_points[i + 1].lon);
			 double di = start.distanceTo(end);
			 ditance += di;
		 }
		// ui->flight_distance->setText(QString::number(ditance, 'f', 2) + "m");
		 flyPoints = m_points;
		 QColor col(255, 254, 169, 100);

		 std::vector<Poi> tempPoint;
		 tempPoint.clear();
		 for (const auto &p : flyPoints) {
			 tempPoint.push_back(Poi(p.lon, p.lat, p.fertility));
		 }



		 m_flyTool->setOriginPoints(tempPoint);

		 m_flyTool->draw3DPathLine(tempPoint, col, 2, flyHeight);


		 std::vector<Poi> tempoiPaths;
		 tempoiPaths.clear();
		 for (const auto &p : m_Fertility_poiPaths) {
			 tempoiPaths.push_back(Poi(p.lon, p.lat, p.fertility));
		 }



		 //tempoiPaths.emplace(tempoiPaths.begin(), Poi(104.0901304, 30.9468978));
		 //tempoiPaths.emplace(tempoiPaths.end(), Poi(104.0901304, 30.9468978));

		 m_flyTool->interpolateSegments(4.0, tempoiPaths);
		 if (LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget())
		 {
			 LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->setIsSpray(false);
		 }

		 m_flyTool->m_isFerili = true;

		 TaskTextInfo textInfo;
		textInfo.text11 = tr2("飞行高度");
		textInfo.text12 = tr2("3m");
		textInfo.text13 = tr2("飞行速度");
		textInfo.text14 = tr2("5m/s");
		textInfo.text21 = tr2("喷洒幅宽");
		textInfo.text22 = tr2("4m");
		textInfo.text23 = tr2("飞行距离");
		textInfo.text24 = tr2("1450m");
		textInfo.text31 = tr2("肥料用量");
		textInfo.text32 = tr2("49.2L");
		textInfo.text33 = tr2("任务面积");
		textInfo.text34 = tr2("7.7亩");
		if (LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo())
		{
			LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo()->SetTaskTextInfo(textInfo);
		}
		
	 }
 }

 void  ISprayPrescriptionImpl::flyRouteSimulPlayback(double flyHeight, bool isLoitering)
 {
	 if (!isLoitering)
	 {
		 std::vector< AirInstructionPoi>  tempVec = generateInstructionPoint(4.0);
		 if (LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget())
		 {
			 LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->show();
		 }
		 if (LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget())
		 {
			 LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->setData(tempVec);
		 }
		
	 }
	 //if (LY_AgriySprayPrescriptionService::Get().GetCameraWidget1())
	 //{
		// LY_AgriySprayPrescriptionService::Get().GetCameraWidget1()->pause(false);
	 //}
	
	 //if (LY_AgriySprayPrescriptionService::Get().GetCameraWidget2())
	 //{
		// LY_AgriySprayPrescriptionService::Get().GetCameraWidget2()->pause(false);
	 //}


	 m_flyTool->startFlying(flyHeight, isLoitering);
	// SetTaskTextInfo(const TaskTextInfo& data);
	 TaskTextInfo textInfo;
	 if (isLoitering)
	 {
		 textInfo.text11 = tr2("载荷类型");
		 textInfo.text12 = tr2("多光谱相机");
		 textInfo.text13 = tr2("波段数量");
		 textInfo.text14 = tr2("6");
		 textInfo.text21 = tr2("航向重叠度");
		 textInfo.text22 = tr2("75%");
		 textInfo.text23 = tr2("旁向重叠度");
		 textInfo.text24 = tr2("78%");
		 textInfo.text31 = tr2("飞行高度");
		 textInfo.text32 = tr2("30");
		 textInfo.text33 = tr2("高度容差");
		 textInfo.text34 = tr2("10");
		 if (LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo())
		 {
			 LY_AgriySprayPrescriptionService::Get().GetShowTaskDataInfo()->SetTaskTextInfo(textInfo);
		 }
		
	 }

 }

  void ISprayPrescriptionImpl::clearn3DflyData()
 {
	  if (LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget())
	  {
		  LY_AgriySprayPrescriptionService::Get().GetAgriSprayCommandWidget()->hide();
	  }
	
	  {
		  m_flyTool->clear3DLineAndModel();
	  }
	 
 }
  void ISprayPrescriptionImpl::playInstructionFlying(bool isPlayOrderfilght )
  {
	  m_flyTool->startInstructionFlying(isPlayOrderfilght);
  }
  UavStatus ISprayPrescriptionImpl::getUavStatus()
  {
	  QMutexLocker locker(&sMutex);

	  return m_flyTool->getFlyStatus();
  }

  std::vector< AirInstructionPoi> ISprayPrescriptionImpl::generateInstructionPoint(double pyfk)
  {
	  if (tempPoints.size() <= 0 || m_flyPointMap.size() <= 0)
	  {
		  return std::vector< AirInstructionPoi>();
	  }
	  m_airInstructionPoints.clear();
	  double spaceing = pyfk * 1.0 *  (1.0 / 110000.0);
	  //key：方格的y  vector:方格x的数组
	  std::map<int, std::vector<int>> tempMap;
	  for (int i = 0; i < tempPoints.size(); i++)
	  {
		  auto it = tempMap.find(tempPoints[i].ry());
		  if (it != tempMap.end())
		  {
			  it->second.push_back(tempPoints[i].rx());
		  }
		  else
		  {
			  std::vector<int> temp;
			  temp.push_back(tempPoints[i].rx());
			  tempMap.insert(std::make_pair(tempPoints[i].ry(), temp));
		  }
	  }
	  //存储方格的方向  y: 方向  
	  std::map<int, bool> dirctionMap;
	  bool isLeft = true; //从左往右
	  int y = 0;
	  for (int i = 0; i < tempPoints.size(); i++)
	  {
		  if (y != 0)
		  {
			  if (y != tempPoints[i].ry()) {
				  isLeft = !isLeft;
				  y = tempPoints[i].ry();
			  }
		  }
		  else
		  {
			  y = tempPoints[i].ry();
		  }
		  dirctionMap[tempPoints[i].ry()] = isLeft;
	  }

	  auto calculatedSpray = [&](int value) {
		  static const std::unordered_map<int, double> ratioMap = {
			  { 1, 0.2 },
			  { 2, 0.4 },
			  { 3, 0.6 },
			  { 4, 0.8 },
			  { 5, 1.0 }
		  };

		  // 查找输入数字对应的比例值
		  auto it = ratioMap.find(value);
		  if (it != ratioMap.end()) {
			  return it->second;
		  }
		  return 0.0;
	  };
	  double gridAngle = osg::DegreesToRadians(10.0);
	  double topBearing = gridAngle;               // 正方向(55°)
	  double rightBearing = gridAngle - osg::PI_2; // 55° - 90° = -35°
	  double bottomBearing = gridAngle + osg::PI;  // 55° + 180° = 235°
	  double leftBearing = gridAngle + osg::PI_2;  // 55° + 90° = 145°
	  for (const auto &map : tempMap)
	  {
		  auto dirction = dirctionMap.find(map.first);
		  if (dirction != dirctionMap.end())
		  {
			  std::vector<std::vector<int>> groups = m_map3D.groupConsecutiveNumbers(map.second, dirction->second);

			 // std::vector<AirInstructionPoi> InstructionPoiVec;
			  //InstructionPoiVec.clear();
			  bool isLeftDirection = dirction->second;
			  for (const auto& group : groups) {

				  double centerLat;
				  double endLon;
				  double mindleLon;

				  int index = 0;
				  Poi tempP;// 记录当前方格的等级
				  tempP.alt = 0.0;


				  for (const auto& subGroup : group)
				  {

					  QPoint startPath(subGroup, map.first);
					  Poi p;
					  auto it = m_flyPointMap.find(startPath);
					  if (it != m_flyPointMap.end()) {
						   p = it->second;
					  }
					  centerLat = p.lat;

					  AirInstructionPoi airPoi;
					  airPoi.centerLat = centerLat;

					  double lon;
					  double lat;
					  if (dirction->second)
					  {
						  lon = p.lon - spaceing / 2;
						  //double leftLatR, leftLonR;
						  //osgEarth::GeoMath::destination(
							 // osg::DegreesToRadians(centerLat),
							 // osg::DegreesToRadians(p.lon),
							 // leftBearing, spaceing,
							 // leftLatR, leftLonR);
						 //lon = osg::RadiansToDegrees(leftLonR);
						// lat = osg::RadiansToDegrees(leftLatR);
					  }
					  else
					  {
						  lon = p.lon + spaceing / 2;
					  }
					  if (index == 0)
					  {
						  airPoi.startLon = lon;
						  airPoi.height = p.alt;
						 // airPoi.centerLat = lat;
						  airPoi.openValue = calculatedSpray(p.alt);
						  airPoi.point_status = POINT_STATUS_START;
						  airPoi.isOpen = true;
					  }
					  else
					  {
						  airPoi.point_status = POINT_STATUS_MIDDLE;
						  airPoi.isOpen = true;

						  if (tempP.alt != p.alt) //不同等级  调整飞机喷洒农药的量
						  {
							  airPoi.openValue = calculatedSpray(p.alt);
							  airPoi.middleLon = lon;
							 // airPoi.centerLat = lat;
							  airPoi.height = p.alt;
						  }
						  else
						  {
							  continue;
						  }

					  }
					  tempP = p;
					  index++;
					  m_airInstructionPoints.push_back(airPoi);
					 // InstructionPoiVec.push_back(airPoi);
				  }
				  QPoint endPtah(group.back(), map.first);

				  Poi p;
				  auto it = m_flyPointMap.find(endPtah);
				  if (it != m_flyPointMap.end()) {
					  p = it->second;
				  }
				  AirInstructionPoi airPoi;
				  airPoi.point_status = POINT_STATUS_END;
				  airPoi.isOpen = false;
				  if (dirction->second)
				  {
					  airPoi.endLon = p.lon + spaceing / 2;
				  }
				  else
				  {
					  airPoi.endLon = p.lon - spaceing / 2;
				  }
				  airPoi.centerLat = centerLat;
				  m_airInstructionPoints.push_back(airPoi);
				  //InstructionPoiVec.push_back(airPoi);
			  }
		  }
	  }
	  return m_airInstructionPoints;
  }
