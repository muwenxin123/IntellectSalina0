#include <QSettings>
#include <QCString.h>
#include <QVector>
#include <QSharedPointer>
#include <QDebug>
#include <QSqlError>

#include "AgriFarmDatabaseHandler.h"

AgriFarmDatabaseHandler::AgriFarmDatabaseHandler(QObject *parent /*= nullptr*/)
	:isOpen(false)
{
	InitDataBase();
}

AgriFarmDatabaseHandler::~AgriFarmDatabaseHandler()
{

}

void AgriFarmDatabaseHandler::InitDataBase()
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = "Agri"; // TODO: 配置项
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
}

bool AgriFarmDatabaseHandler::executeQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
		return false;
	}
	return true;
}

QSqlQuery AgriFarmDatabaseHandler::executeSelectQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
	}
	return query;
}

bool AgriFarmDatabaseHandler::beginTransaction() {
	return db.transaction();
}

bool AgriFarmDatabaseHandler::commitTransaction() {
	return db.commit();
}

bool AgriFarmDatabaseHandler::rollbackTransaction() {
	return db.rollback();
}

QSharedPointer<QSqlQuery> AgriFarmDatabaseHandler::getQueryOperator()
{
	return QSharedPointer<QSqlQuery>(new QSqlQuery(db));
}

AgriFarmInfoVec AgriFarmDatabaseHandler::GetFarmInfoList() const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriFarmInfoVec	farmInfoPtrVec;

	QString strSql = "SELECT `id`,  `name`, `category`, `description`, `lon`, `lat`";
	QString strGeneralSql = strSql + " FROM " + AGRI_FARM_TABLE_NAME;

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

			AgriFarmInfo pInfo;
			pInfo.id = str1;
			pInfo.name = str2;
			pInfo.category = str3;
			pInfo.description = str4;
			pInfo.lon = str5.toDouble();
			pInfo.lat = str6.toDouble();

			farmInfoPtrVec.push_back(pInfo);
		}
	}

	return farmInfoPtrVec;
}

bool AgriFarmDatabaseHandler::AddFarmInfo(const AgriFarmInfo& farmInfo)
{
	QString strSql = "INSERT INTO " + QString(AGRI_FARM_TABLE_NAME) + " (`id`, `name`, `category`, `description`, `lon`, `lat`) VALUES (";
	strSql += "'" + farmInfo.id + "',";
	strSql += "'" + farmInfo.name + "',";
	strSql += "'" + farmInfo.category + "',";
	strSql += "'" + farmInfo.description+ "',";
	strSql += "'" + QString::number(farmInfo.lon, 'f', 7) + "',";
	strSql += "'" + QString::number(farmInfo.lat, 'f', 7) + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriFarmDatabaseHandler::DeleteFarmInfo(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_FARM_TABLE_NAME) + " where id = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

AgriFieldInfoVec AgriFarmDatabaseHandler::GetFieldInfoList() const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriFieldInfoVec fieldInfoPtrVec;

	QString strSql = "SELECT `id`, `farm_id`, `area_id`, `name`, `category`, `description`, `crop`, `crop_variety`, `plant_standard`, `sow_model`";
	QString strGeneralSql = strSql + " FROM " + AGRI_FARM_FIELD_TABLE_NAME;

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

			AgriFieldInfo pInfo;
			pInfo.id = str1;
			pInfo.farm_id = str2;
			pInfo.area_id = str3;
			pInfo.name = str4;
			pInfo.category = str5;
			pInfo.description = str6;
			pInfo.crop = str7.toInt();
			pInfo.crop_variety = str8.toInt();
			pInfo.plant_standard = str9.toInt();
			pInfo.sow_model = str10.toInt();

			fieldInfoPtrVec.push_back(pInfo);
		}
	}

	return fieldInfoPtrVec;
}

bool AgriFarmDatabaseHandler::AddFieldInfo(const AgriFieldInfo& fieldInfo)
{
	QString strSql = "INSERT INTO " + QString(AGRI_FARM_FIELD_TABLE_NAME) + " (`id`, `farm_id`, `area_id`, `name`, `category`, `description`, `crop_variety`, `plant_standard`, `sow_model`) VALUES (";
	strSql += "'" + fieldInfo.id + "',";
	strSql += "'" + fieldInfo.farm_id + "',";
	strSql += "'" + fieldInfo.area_id + "',";
	strSql += "'" + fieldInfo.name + "',";
	strSql += "'" + fieldInfo.category + "',";
	strSql += "'" + fieldInfo.description + "',";
	strSql += "'" + QString::number(fieldInfo.crop) + "',";
	strSql += "'" + QString::number(fieldInfo.crop_variety) + "',";
	strSql += "'" + QString::number(fieldInfo.plant_standard) + "',";
	strSql += "'" + QString::number(fieldInfo.sow_model) + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriFarmDatabaseHandler::UpdateFieldInfo(const AgriFieldInfo& fieldInfo)
{
	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_FARM_FIELD_TABLE_NAME) +" SET "
		"farm_id = :farm_id, "
		"area_id = :area_id, "
		"name = :name, "
		"category = :category, "
		"description = :description, "
		"crop = :crop, "
		"crop_variety = :crop_variety, "
		"plant_standard = :plant_standard, "
		"sow_model = :sow_model "
		"WHERE id = :id";

	query.prepare(updateQuery);

	query.bindValue(":farm_id", fieldInfo.farm_id);
	query.bindValue(":area_id", fieldInfo.area_id);
	query.bindValue(":name", fieldInfo.name);
	query.bindValue(":category", fieldInfo.category);
	query.bindValue(":description", fieldInfo.description);
	query.bindValue(":crop", fieldInfo.crop);
	query.bindValue(":crop_variety", fieldInfo.crop_variety);
	query.bindValue(":plant_standard", fieldInfo.plant_standard);
	query.bindValue(":sow_model", fieldInfo.sow_model);
	query.bindValue(":id", fieldInfo.id);

	// 打印 SQL 查询语句，用于调试
	// qDebug() << "SQL Query: " << query.lastQuery();

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError();
		return false;
	}
}

bool AgriFarmDatabaseHandler::BatchInsertField(const AgriFieldInfoVec& fieldInfoVec)
{
	// 开启事务
	QSqlQuery query;
	if (!query.exec("START TRANSACTION")) {
		qDebug() << "Failed to start transaction: " << query.lastError().text();
		return false;
	}

	// 插入数据
	query.prepare("INSERT INTO agri_farm_field (id, farm_id, area_id, name, category, description, crop_variety, plant_standard, sow_model) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

	for(const auto& field : fieldInfoVec)
	{
		query.addBindValue(field.id);
		query.addBindValue(field.farm_id);
		query.addBindValue(field.area_id);
		query.addBindValue(field.name);
		query.addBindValue(field.category);
		query.addBindValue(field.description);
		query.addBindValue(field.crop_variety);
		query.addBindValue(field.plant_standard);
		query.addBindValue(field.sow_model);

		if (!query.exec()) {
			qDebug() << "Insert failed: " << query.lastError().text();
			query.exec("ROLLBACK"); // 如果插入失败，回滚事务
			return false;
		}
	}

	// 提交事务
	if (!query.exec("COMMIT")) {
		qDebug() << "Failed to commit transaction: " << query.lastError().text();
		return false;
	}

	return true;
}

bool AgriFarmDatabaseHandler::DeleteFieldInfo(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_FARM_FIELD_TABLE_NAME) + " where id = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

AgriMonitorInfoPtrVec AgriFarmDatabaseHandler::GetMonitorInfoList()
{
	QMutexLocker locker(&sMutex);
	QSqlQuery query;
	QString strSql = "SELECT `id`, `mointName`, `mointID`, `mointType`, `mointModenlName`, `localHeight`, `longitude`, `latitude`, `url`";
	QString strGeneralSql = strSql + " FROM `agri_monitor_sub_list`";

	m_vecMonitorInfo.clear();

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

			AgriMonitorInfoPtr m = std::make_shared<AgriMonitorInfo>();
			m->nID = str1.toInt();
			m->mointName = str2;
			m->mointID = str3.toInt();
			m->mointType = str4;
			m->mointModenlName = str5;
			m->localHeight = str6.toDouble();
			m->longitude = str7.toDouble();
			m->latitude = str8.toDouble();
			m->requsetUrl = str9;

			m_vecMonitorInfo.push_back(m);
		}
	}
	else
	{
		qDebug() << "Failed: " << query.lastError().text();
	}

	return m_vecMonitorInfo;
}

AgriMonitorInfoPtrVec AgriFarmDatabaseHandler::GetRelateSensorList(const QString& fieldId)
{
	QMutexLocker locker(&sMutex);
	QSqlQuery query;

	QString strSql, strGeneralSql;
	strSql = "SELECT moni.id, moni.mointName, moni.mointID, moni.mointType, moni.mointModenlName, moni.localHeight, moni.longitude, moni.latitude, moni.url";
	strGeneralSql = QString(strSql + " FROM %1 moni JOIN %2 rel ON moni.mointID = rel.sensor_id WHERE rel.field_id = \"%3\"")
		.arg(AGRI_MONITOR_SUB_LIST_TABLE_NAME)
		.arg(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME)
		.arg(fieldId);

	m_vecMonitorInfo.clear();

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

			AgriMonitorInfoPtr m = std::make_shared<AgriMonitorInfo>();
			m->nID = str1.toInt();
			m->mointName = str2;
			m->mointID = str3.toInt();
			m->mointType = str4;
			m->mointModenlName = str5;
			m->localHeight = str6.toDouble();
			m->longitude = str7.toDouble();
			m->latitude = str8.toDouble();
			m->requsetUrl = str9;

			m_vecMonitorInfo.push_back(m);
		}
	}

	return m_vecMonitorInfo;
}

bool AgriFarmDatabaseHandler::GetFieldSensorRelation(const QString& fieldId, std::vector<int>& sensorIds)
{
	QSqlQuery query;
	query.prepare("SELECT sensor_id FROM " + QString(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME) + " WHERE field_id = :field_id"); // 替换为实际的表名和列名
	query.bindValue(":field_id", fieldId);

	if (!query.exec()) {
		qDebug() << "Query failed: " << query.lastError().text();
		return false;
	}

	while (query.next()) {
		int sensorId = query.value(0).toInt();
		sensorIds.push_back(sensorId);
	}

	return true;
}

bool AgriFarmDatabaseHandler::UpdateFieldSensorRelation(const QString& fieldId, const std::vector<int>& delSensorIdVec, const std::vector<int>& addSensorIdVec)
{
	if (!isOpen) {
		qDebug() << "Failed to connect to database: " << db.lastError().text();
		return false;
	}

	// 使用事务开始
	QSqlQuery query;
	if (!query.exec("START TRANSACTION;")) {
		qDebug() << "Failed to start transaction: " << query.lastError().text();
		return false;
	}

	// 1. 删除原所有记录
	if (delSensorIdVec.size())
	{
		// 构建 ID 列表的字符串，用于 IN 子句
		QStringList idList;
		for (int id : delSensorIdVec) {
			idList << QString::number(id);
		}
		QString idString = idList.join(",");

		// 执行 DELETE 语句
		QString deleteQuery = QString("DELETE FROM %1 WHERE %2 IN (%3);")
			.arg(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME)
			.arg("sensor_id")
			.arg(idString);

		if (!executeQuery(deleteQuery)) {
			qDebug() << "Delete operation failed. Rolling back.";
			query.exec("ROLLBACK;");
			return false;
		}
	}


	// 2. 插入新记录
	if (addSensorIdVec.size())
	{
		QString insertQuery = "INSERT INTO " + QString(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME)
			+ "(field_id, sensor_id)  VALUES ";

		for (const auto& sensorId : addSensorIdVec)
		{
			insertQuery += "(\"" + fieldId + "\" ," + QString::number(sensorId) + "),";
		}

		// 删除多余","
		insertQuery.chop(1);

		if (!executeQuery(insertQuery)) {
			qDebug() << "Insert operation failed. Rolling back.";
			query.exec("ROLLBACK;");
			return false;
		}
	}

	// 如果一切成功，提交事务
	if (!query.exec("COMMIT;")) {
		qDebug() << "Failed to commit transaction: " << query.lastError().text();
		return false;
	}

	return true;
}

AgriCropInfoPtrVec AgriFarmDatabaseHandler::GetCropInfoList() const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriCropInfoPtrVec cropInfoPtrVec;

	QString strSql = "SELECT `id`, `name`";
	QString strGeneralSql = strSql + " FROM " + AGRI_CROP_TABLE_NAME;

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();

			AgriCropInfoPtr pInfo = std::make_shared<AgriCropInfo>();
			pInfo->id = str1.toInt();
			pInfo->name = str2;

			cropInfoPtrVec.push_back(pInfo);
		}
	}

	return cropInfoPtrVec;
}

AgriCropVarietyInfoPtrVec AgriFarmDatabaseHandler::GetCropVarietyInfoList() const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriCropVarietyInfoPtrVec cropVarietyInfoPtrVec;

	QString strSql = "SELECT `id`, `crop_id`, `name`";
	QString strGeneralSql = strSql + " FROM " + AGRI_CROP_VARIETY_LIST_TABLE_NAME;

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();

			AgriCropVarietyInfoPtr pInfo = std::make_shared<AgriCropVarietyInfo>();
			pInfo->id = str1.toInt();
			pInfo->crop_id = str2.toInt();
			pInfo->name = str3;

			cropVarietyInfoPtrVec.push_back(pInfo);
		}
	}

	return cropVarietyInfoPtrVec;
}

AgriEnumInfoPtrVec AgriFarmDatabaseHandler::FetchCropPlantStandardEnumInfoList()
{
	if (m_CropPlantStandard.empty())
	{
		m_CropPlantStandard = FetchEnumInfoList(AGRI_ENUM_CROP_PLANT_STANDARD_TABLE_NAME);
	}
	return m_CropPlantStandard;
}

AgriEnumInfoPtrVec AgriFarmDatabaseHandler::FetchCropSowModelEnumInfoList()
{
	if (m_SowModel.empty())
	{
		m_SowModel = FetchEnumInfoList(AGRI_ENUM_CROP_SOW_MODEL_TABLE_NAME);
	}
	return m_SowModel;
}

QByteArray AgriFarmDatabaseHandler::GetAgriFarmImage(const std::string& strGuid)
{
	QSqlQuery query;
	query.prepare("SELECT image FROM " + QString(AGRI_FARM_IMAGE_TABLE_NAME) + " WHERE guid = :guid"); // 替换为实际的表名和列名
	query.bindValue(":guid", QString::fromStdString(strGuid));

	if (!query.exec()) {
		// TODO: 统一查询失败图片
		return QByteArray();
	}

	QByteArray imageData;
	if (query.next()) {
		imageData = query.value(0).toByteArray(); // 获取图片数据
	}
	return imageData;
}

AgriEnumInfoPtrVec AgriFarmDatabaseHandler::FetchEnumInfoList(const std::string& strTableName)
{
	AgriEnumInfoPtrVec result;
	QSqlQuery query(db);

	query.prepare("SELECT * FROM " + QString::fromStdString(strTableName));

	if (!query.exec())
	{
		return result;
	}

	while (query.next())
	{
		QString str1 = query.value(0).toString();
		QString str2 = query.value(1).toString();
		QString str3 = query.value(2).toString();

		AgriEnumInfoPtr pInfo = std::make_shared<AgriEnumInfo>();
		pInfo->id = str1.toInt();
		pInfo->description = str2.toStdString();
		pInfo->comment = str3.toStdString();

		result.emplace_back(pInfo);
	}

	return result;
}

