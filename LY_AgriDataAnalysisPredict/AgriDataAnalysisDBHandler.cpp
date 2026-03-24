#include <QSettings>
#include <QCString.h>
#include <QVector>
#include <QSharedPointer>
#include <QDebug>
#include <QSqlError>

#include "AgriDataAnalysisDBHandler.h"

AgriDataAnalysisDBHandler::AgriDataAnalysisDBHandler(QObject *parent /*= nullptr*/)
	:isOpen(false)
{
	InitDataBase();
}

AgriDataAnalysisDBHandler::~AgriDataAnalysisDBHandler()
{

}
#include <QThread>
void AgriDataAnalysisDBHandler::InitDataBase()
{
	//Qt::HANDLE threadId = QThread::currentThreadId();
	//QString threadIdStr = QString::number(reinterpret_cast<quintptr>(threadId), 16);
	//connectionName = "connection_" + threadIdStr;

	//if (QSqlDatabase::contains(connectionName)) {
	//	return;
	//}

	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = "Agri"; // TODO: 配置项
	QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = ini.value("User", "root").toString();
	QString strDbPass = ini.value("Password", "123456").toString();
	QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

	db = QSqlDatabase::addDatabase(strDbDriver/*, connectionName*/);
	db.setHostName(strDbIP);
	db.setPort(3306);
	db.setDatabaseName(strDbName);
	db.setUserName(strDbUser);
	db.setPassword(strDbPass);
	isOpen = db.open();
}

void AgriDataAnalysisDBHandler::CloseDatabase()
{
	QSqlDatabase db = QSqlDatabase::database(connectionName);
	if (db.isOpen()) {
		db.close();
		qDebug() << "Database connection closed for " << connectionName;
	}
	else {
		qDebug() << "Database connection not open for " << connectionName;
	}

	// 删除连接
	QSqlDatabase::removeDatabase(connectionName);
}

bool AgriDataAnalysisDBHandler::executeQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
		return false;
	}
	return true;
}

QSqlQuery AgriDataAnalysisDBHandler::executeSelectQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
	}
	return query;
}

bool AgriDataAnalysisDBHandler::beginTransaction() {
	return db.transaction();
}

bool AgriDataAnalysisDBHandler::commitTransaction() {
	return db.commit();
}

bool AgriDataAnalysisDBHandler::rollbackTransaction() {
	return db.rollback();
}

QSharedPointer<QSqlQuery> AgriDataAnalysisDBHandler::getQueryOperator()
{
	return QSharedPointer<QSqlQuery>(new QSqlQuery(db));
}

AgriPestDatasetPtrVec AgriDataAnalysisDBHandler::GetPestDatasetList(const QStringList& fieldList) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriPestDatasetPtrVec pestDatasetPtrVec;

	QString strSql = "SELECT `guid`, `name`, `field_id`, `crop_type`, `collect_type`, `begin_time`, `end_time`, `pest_disease`, `pest_category`";
	QString strGeneralSql = strSql + " FROM " + AGRI_PEST_DATASET_TABLE_NAME;

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
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();
			QString str8 = query.value(7).toString();
			QString str9 = query.value(8).toString();

			AgriPestDatasetPtr pInfo = std::make_shared<AgriPestDataset>();
			pInfo->guid = str1;
			pInfo->name = str2;
			pInfo->field_id = str3;
			pInfo->crop_type = str4.toInt();
			pInfo->collect_type = str5.toInt();
			pInfo->begin_time = str6;
			pInfo->end_time = str7;
			pInfo->pest_disease = str8.toInt();
			pInfo->pest_category = str9.toInt();

			pestDatasetPtrVec.push_back(pInfo);
		}
	}

	return pestDatasetPtrVec;
}

bool AgriDataAnalysisDBHandler::AddPestDataset(const AgriPestDatasetPtr& pestDataset)
{
	if (!pestDataset) return false;

	QString strSql = "INSERT INTO " + QString(AGRI_PEST_DATASET_TABLE_NAME) + " (`guid`, `name`, `field_id`, `crop_type`, `collect_type`, `begin_time`, `end_time`, `pest_disease`, `pest_category`) VALUES (";
	strSql += "'" + pestDataset->guid + "',";
	strSql += "'" + pestDataset->name + "',";
	strSql += "'" + pestDataset->field_id + "',";
	strSql += "'" + QString::number(pestDataset->crop_type) + "',";
	strSql += "'" + QString::number(pestDataset->collect_type) + "',";
	strSql += "'" + pestDataset->begin_time + "',";
	strSql += "'" + pestDataset->end_time + "',";
	strSql += "'" + QString::number(pestDataset->pest_disease) + "',";
	strSql += "'" + QString::number(pestDataset->pest_category) + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriDataAnalysisDBHandler::UpdatePestDataset(const AgriPestDatasetPtr& pestDataset)
{
	if (!pestDataset) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_PEST_DATASET_TABLE_NAME) + " SET "
		"name = :name, "
		"field_id = :field_id, "
		"crop_type = :crop_type, "
		"collect_type = :collect_type, "
		"begin_time = :begin_time, "
		"end_time = :end_time, "
		"pest_disease = :pest_disease, "
		"pest_category = :pest_category "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", pestDataset->name);
	query.bindValue(":field_id", pestDataset->field_id);
	query.bindValue(":crop_type", pestDataset->crop_type);
	query.bindValue(":collect_type", pestDataset->collect_type);
	query.bindValue(":begin_time", pestDataset->begin_time);
	query.bindValue(":end_time", pestDataset->end_time);
	query.bindValue(":pest_disease", pestDataset->pest_disease);
	query.bindValue(":pest_category", pestDataset->pest_category);
	query.bindValue(":guid", pestDataset->guid);

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError();
		return false;
	}
}

bool AgriDataAnalysisDBHandler::DeletePestDataset(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_PEST_DATASET_TABLE_NAME) + " where guid = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

AgriPestCollectionPtrVec AgriDataAnalysisDBHandler::GetPestCollectionList(const QString& sDatasetId) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriPestCollectionPtrVec pestDatasetPtrVec;

	QString strSql = "SELECT `guid`, `name`, `dataset_id`, `sensor_id`, `collect_type`, `collect_time`, `lon`, `lat`, `alt`, `category`, `stage`, `count`, `range`, `level`, `recognize_image`";
	QString strGeneralSql = strSql + " FROM " + AGRI_PEST_COLLECTION_TABLE_NAME + " WHERE dataset_id = \"" + sDatasetId + "\";";

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
			QByteArray str15 = query.value(14).toByteArray();

			AgriPestCollectionPtr pInfo = std::make_shared<AgriPestCollection>();
			pInfo->guid = str1;
			pInfo->name = str2;
			pInfo->dataset_id = str3;
			pInfo->sensor_id = str4;
			pInfo->collect_type = str5.toInt();
			pInfo->collect_time = str6;
			pInfo->lon = str7.toDouble();
			pInfo->lat = str8.toDouble();
			pInfo->alt = str9.toDouble();
			pInfo->category = str10.toInt();
			pInfo->stage = str11.toInt();
			pInfo->count = str12.toInt();
			pInfo->range = str13.toDouble();
			pInfo->level = str14.toInt();
			pInfo->recognize_image = str15;

			pestDatasetPtrVec.push_back(pInfo);
		}
	}
	else {
		qDebug() << "Error: " << query.lastError() << query.executedQuery();
	}

	return pestDatasetPtrVec;
}

AgriPestCollectionPtrVec AgriDataAnalysisDBHandler::GetPestCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriPestCollectionPtrVec pestDatasetPtrVec;

	int offset = (nPage - 1) * nPageSize; // 计算偏移量
	QString queryStr = QString(
		"SELECT guid, name, dataset_id, sensor_id, collect_type, collect_time, lon, lat, alt, "
		"category, stage, `count`, `range`, `level`, recognize_image"
		" FROM " + QString(AGRI_PEST_COLLECTION_TABLE_NAME) +
		" WHERE dataset_id = \"" + sDatasetId + "\""
		" ORDER BY id ASC"
		" LIMIT %1 OFFSET %2").arg(nPageSize).arg(offset);

	if (!query.exec(queryStr)) {
		qWarning() << "Query Failed:" << query.lastError().text();
		return pestDatasetPtrVec;
	}

	while (query.next()) {
		AgriPestCollectionPtr item = std::make_shared<AgriPestCollection>();
		item->guid = query.value("guid").toString();
		item->name = query.value("name").toString();
		item->dataset_id = query.value("dataset_id").toString();
		item->sensor_id = query.value("sensor_id").toString();
		item->collect_type = query.value("collect_type").toInt();
		item->collect_time = query.value("collect_time").toString();
		item->lon = query.value("lon").toDouble();
		item->lat = query.value("lat").toDouble();
		item->alt = query.value("alt").toDouble();
		item->category = query.value("category").toInt();
		item->stage = query.value("stage").toInt();
		item->count = query.value("count").toInt();
		item->range = query.value("range").toDouble();
		item->level = query.value("level").toInt();
		item->recognize_image = query.value("recognize_image").toByteArray();

		pestDatasetPtrVec.push_back(item);
	}

	return pestDatasetPtrVec;
}

int AgriDataAnalysisDBHandler::GetPestCollectionCount(const QString& sDatasetId) const
{
	QString strSql = "SELECT COUNT(*)";
	QString strGeneralSql = strSql + " FROM " + AGRI_PEST_COLLECTION_TABLE_NAME + " WHERE dataset_id = :dataset_id";

	QSqlQuery query;
	query.prepare(strGeneralSql);
	query.bindValue(":dataset_id", sDatasetId);

	int count = 0;
	if (!query.exec()) {
		qDebug() << "查询失败：" << query.lastError().text();
	}

	if (query.next()) {
		count = query.value(0).toInt();
	}
	return count;
}

bool AgriDataAnalysisDBHandler::AddPestCollection(const AgriPestCollectionPtr& pestCollection)
{
	if (!pestCollection) return false;

	QSqlQuery query;
	query.prepare(R"(
        INSERT INTO agri_pest_collection 
        (guid, name, dataset_id, sensor_id, collect_type, collect_time, lon, lat, alt, 
         category, stage, `count`, `range`, `level`, recognize_image)
        VALUES 
        (:guid, :name, :dataset_id, :sensor_id, :collect_type, :collect_time, :lon, :lat, :alt, 
         :category, :stage, :count, :range_value, :level, :recognize_image)
    )");

	query.bindValue(":guid", pestCollection->guid);
	query.bindValue(":name", pestCollection->name);
	query.bindValue(":dataset_id", pestCollection->dataset_id);
	query.bindValue(":sensor_id", pestCollection->sensor_id);
	query.bindValue(":collect_type", pestCollection->collect_type);
	query.bindValue(":collect_time", pestCollection->collect_time);
	query.bindValue(":lon", pestCollection->lon);
	query.bindValue(":lat", pestCollection->lat);
	query.bindValue(":alt", pestCollection->alt);
	query.bindValue(":category", pestCollection->category);
	query.bindValue(":stage", pestCollection->stage);
	query.bindValue(":count", pestCollection->count);
	query.bindValue(":range_value", pestCollection->range);
	query.bindValue(":level", pestCollection->level);
	query.bindValue(":recognize_image", pestCollection->recognize_image);

	if (!query.exec()) {
		qDebug() << "Insert failed:" << query.lastError().text();
		return false;
	}
	qDebug() << "Insert successful!";
	return true;
}

bool AgriDataAnalysisDBHandler::UpdatePestCollection(const AgriPestCollectionPtr& pestCollection)
{
	if (!pestCollection) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_PEST_COLLECTION_TABLE_NAME) + " SET "
		"name = :name, "
		"dataset_id = :dataset_id, "
		"sensor_id = :sensor_id, "
		"collect_type = :collect_type, "
		"collect_time = :collect_time, "
		"lon = :lon, "
		"lat = :lat, "
		"alt = :alt, "
		"category = :category, "
		"stage = :stage, "
		"`count` = :count, "
		"`range` = :range, "
		"`level` = :level, "
		"recognize_image = :recognize_image "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", pestCollection->name);
	query.bindValue(":dataset_id", pestCollection->dataset_id);
	query.bindValue(":sensor_id", pestCollection->sensor_id);
	query.bindValue(":collect_type", pestCollection->collect_type);
	query.bindValue(":collect_time", pestCollection->collect_time);
	query.bindValue(":lon", pestCollection->lon);
	query.bindValue(":lat", pestCollection->lat);
	query.bindValue(":alt", pestCollection->alt);
	query.bindValue(":category", pestCollection->category);
	query.bindValue(":stage", pestCollection->stage);
	query.bindValue(":count", pestCollection->count);
	query.bindValue(":range", pestCollection->range);
	query.bindValue(":level", pestCollection->level); 
	query.bindValue(":recognize_image", pestCollection->recognize_image);
	query.bindValue(":guid", pestCollection->guid);

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError() << query.executedQuery();
		return false;
	}
}

bool AgriDataAnalysisDBHandler::DeletePestCollection(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_PEST_COLLECTION_TABLE_NAME) + " where guid = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

AgriPestPhotoIndeficationPtrVec AgriDataAnalysisDBHandler::GetPestPhotoIndeficationList(const QString& sFieldId) const
{
	AgriPestPhotoIndeficationPtrVec vecIndeficationList;

	QString strSql = "SELECT ap.id, photo_id, mointId, recode_time, localHeight, longitude, latitude, pest_type, pest_stage, pest_number, recode_range, pest_grade, original_image, recognize_image";
	strSql += " FROM " + QString(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME) + " arf ";
	strSql += " JOIN " + QString(AGRI_PEST_PHOTO_INDEFICATION_TABLE_NAME) + " ap ON arf.sensor_id = ap.mointId";
	strSql += " WHERE arf.field_id = :field_id"; //  AND ap.sensor_type = :sensor_type

	//strSql = "SELECT moni.id, moni.mointName, moni.mointID, moni.mointType, moni.mointModenlName, moni.localHeight, moni.longitude, moni.latitude, moni.url";
	//strGeneralSql = QString(strSql + " FROM %1 moni JOIN %2 rel ON moni.mointID = rel.sensor_id WHERE rel.field_id = \"%3\"")
	//	.arg(AGRI_MONITOR_SUB_LIST_TABLE_NAME)
	//	.arg(AGRI_RELATION_FIELD_SENSOR_TABLE_NAME)
	//	.arg(fieldId);

	QSqlQuery query;
	query.prepare(strSql);
	query.bindValue(":field_id", sFieldId);
	//query.bindValue(":sensor_type", AGRI_MONITOR_PHOTO);

	if (!query.exec()) {
		qDebug() << "Query execution failed:" << query.lastError().text();
	}

	while (query.next()) {
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
		QByteArray str13 = query.value(12).toByteArray();
		QByteArray str14 = query.value(13).toByteArray();

		AgriPestPhotoIndeficationPtr pInfo = std::make_shared<AgriPestPhotoIndefication>();
		pInfo->id = str1.toInt();
		pInfo->photo_id = str2;
		pInfo->mointId = str3.toInt();
		pInfo->recode_time = str4.toInt();
		pInfo->localHeight = str5.toDouble();
		pInfo->longitude = str6.toDouble();
		pInfo->latitude = str7.toDouble();
		pInfo->pest_type = str8;
		pInfo->pest_stage = str9;
		pInfo->pest_number = str10.toInt();
		pInfo->recode_range = str11.toInt();
		pInfo->pest_grade = str12;
		pInfo->original_image = str13;
		pInfo->recognize_image = str14;

		vecIndeficationList.push_back(pInfo);
	}

	return vecIndeficationList;
}

AgriPestDistributeMapPtrVec AgriDataAnalysisDBHandler::GetPestDistributeMap(const QString& sDatasetId) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriPestDistributeMapPtrVec pestDistributeMapPtrVec;

	QString strSql = "SELECT `guid`, `name`, `dataset_id`, `crop_type`, `growth_stage`, `field_area`, `collect_count`, `grid_unit`, `diffuse_layer`, `distribute_statistic`";
	QString strGeneralSql = strSql + " FROM " + AGRI_PEST_DISTRIBUTE_MAP_TABLE_NAME + " WHERE dataset_id = \"" + sDatasetId + "\";";

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

			AgriPestDistributeMapPtr pInfo = std::make_shared<AgriPestDistributeMap>();
			pInfo->guid = str1;
			pInfo->name = str2;
			pInfo->dataset_id = str3;
			pInfo->crop_type = str4.toInt();
			pInfo->growth_stage = str5.toInt();
			pInfo->field_area = str6.toDouble();
			pInfo->collect_count = str7.toInt();
			pInfo->grid_unit = str8.toInt();
			pInfo->diffuse_layer = str9.toInt();
			pInfo->distribute_statistic = str10;

			pestDistributeMapPtrVec.push_back(pInfo);
		}
	}

	return pestDistributeMapPtrVec;
}

bool AgriDataAnalysisDBHandler::AddPestDistributeMap(const AgriPestDistributeMapPtr& pestDisMap)
{
	if (!pestDisMap) return false;

	QString strSql = "INSERT INTO " + QString(AGRI_PEST_DISTRIBUTE_MAP_TABLE_NAME) + " (`guid`, `name`, `dataset_id`, `crop_type`, `growth_stage`, `field_area`, `collect_count`, `grid_unit`, `diffuse_layer`, `distribute_statistic`) VALUES (";
	strSql += "'" + pestDisMap->guid + "',";
	strSql += "'" + pestDisMap->name + "',";
	strSql += "'" + pestDisMap->dataset_id + "',";
	strSql += "'" + QString::number(pestDisMap->crop_type) + "',";
	strSql += "'" + QString::number(pestDisMap->growth_stage) + "',";
	strSql += "'" + QString::number(pestDisMap->field_area) + "',";
	strSql += "'" + QString::number(pestDisMap->collect_count) + "',";
	strSql += "'" + QString::number(pestDisMap->grid_unit) + "',";
	strSql += "'" + QString::number(pestDisMap->diffuse_layer) + "',";
	strSql += "'" + pestDisMap->distribute_statistic + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriDataAnalysisDBHandler::UpdatePestDistributeMap(const AgriPestDistributeMapPtr& pestDisMap)
{
	if (!pestDisMap) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_PEST_DISTRIBUTE_MAP_TABLE_NAME) + " SET "
		"name = :name, "
		"dataset_id = :dataset_id, "
		"crop_type = :crop_type, "
		"growth_stage = :growth_stage, "
		"field_area = :field_area, "
		"collect_count = :collect_count, "
		"grid_unit = :grid_unit, "
		"diffuse_layer = :diffuse_layer, "
		"distribute_statistic = :distribute_statistic "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", pestDisMap->name);
	query.bindValue(":dataset_id", pestDisMap->dataset_id);
	query.bindValue(":crop_type", pestDisMap->crop_type);
	query.bindValue(":growth_stage", pestDisMap->growth_stage);
	query.bindValue(":field_area", pestDisMap->field_area);
	query.bindValue(":collect_count", pestDisMap->collect_count);
	query.bindValue(":grid_unit", pestDisMap->grid_unit);
	query.bindValue(":diffuse_layer", pestDisMap->diffuse_layer);
	query.bindValue(":distribute_statistic", pestDisMap->distribute_statistic);
	query.bindValue(":guid", pestDisMap->guid);

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError() << query.executedQuery();
		return false;
	}
}

bool AgriDataAnalysisDBHandler::DeletePestDistributeMap(const QString& sDisMapId)
{
	if (sDisMapId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_PEST_DISTRIBUTE_MAP_TABLE_NAME) + " where guid = ";
	strSql += "'" + sDisMapId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

bool AgriDataAnalysisDBHandler::BatchInsertPestCollection(const AgriPestCollectionPtrVec& collectInfoVec)
{
	// 开启事务
	QSqlQuery query;
	if (!query.exec("START TRANSACTION")) {
		qDebug() << "Failed to start transaction: " << query.lastError().text();
		return false;
	}

	// 插入数据
	query.prepare("INSERT INTO agri_pest_collection (`guid`, `name`, `dataset_id`, `sensor_id`, `collect_type`, `collect_time`, `lon`, `lat`, `alt`, `category`, `stage`, `count`, `range`, `level`) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

	for (const auto& collect : collectInfoVec)
	{
		query.addBindValue(collect->guid);
		query.addBindValue(collect->name);
		query.addBindValue(collect->dataset_id);
		query.addBindValue(collect->sensor_id);
		query.addBindValue(collect->collect_type);
		query.addBindValue(collect->collect_time);
		query.addBindValue(collect->lon);
		query.addBindValue(collect->lat);
		query.addBindValue(collect->alt);
		query.addBindValue(collect->category);
		query.addBindValue(collect->stage);
		query.addBindValue(collect->count);
		query.addBindValue(collect->range);
		query.addBindValue(collect->level);

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
