#include <QSettings>
#include <QCString.h>
#include <QVector>
#include <QSharedPointer>
#include <QDebug>
#include <QSqlError>

#include "AgriMultispectralSensorDBHandler.h"

AgriMultispectralSensorDBHandler::AgriMultispectralSensorDBHandler(QObject *parent /*= nullptr*/)
	:isOpen(false)
{
	InitDataBase();
}

AgriMultispectralSensorDBHandler::~AgriMultispectralSensorDBHandler()
{

}

void AgriMultispectralSensorDBHandler::InitDataBase()
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = "Agri"; // TODO: ≈‰÷√œÓ
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

bool AgriMultispectralSensorDBHandler::executeQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
		return false;
	}
	return true;
}

QSqlQuery AgriMultispectralSensorDBHandler::executeSelectQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
	}
	return query;
}

bool AgriMultispectralSensorDBHandler::beginTransaction() {
	return db.transaction();
}

bool AgriMultispectralSensorDBHandler::commitTransaction() {
	return db.commit();
}

bool AgriMultispectralSensorDBHandler::rollbackTransaction() {
	return db.rollback();
}

QSharedPointer<QSqlQuery> AgriMultispectralSensorDBHandler::getQueryOperator()
{
	return QSharedPointer<QSqlQuery>(new QSqlQuery(db));
}

AgriSentinelMetadataPtrVec AgriMultispectralSensorDBHandler::GetSentinelMetadataList() const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSentinelMetadataPtrVec pestDatasetPtrVec;

	QString strSql = "SELECT `guid`, `file_name`, `size`, `identifier`, `begin_time`, `end_time`, `ingestion_date`, `orbit_number`, `relative_orbit_number`, `product_type`, `cloud_cover_percentage`, `sensor_operational_mode`, `foot_print`, `download_url`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SENTINEL_METADATA_TABLE_NAME;
	
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
			QString str15 = query.value(14).toString();
			QString str16 = query.value(15).toString();

			AgriSentinelMetadataPtr pInfo = std::make_shared<AgriSentinelMetadata>();
			pInfo->guid = str1;
			pInfo->file_name = str2;
			pInfo->size = str3.toInt();
			pInfo->identifier = str4;
			pInfo->begin_time = str5;
			pInfo->end_time = str6;
			pInfo->ingestion_date = str7;
			pInfo->orbit_number = str8.toInt();
			pInfo->relative_orbit_number = str9.toInt();
			pInfo->platform_name = str10;
			pInfo->platform_identifier = str11;
			pInfo->sensor_operational_mode = str12;
			pInfo->product_type = str13;
			pInfo->cloud_cover_percentage = str14.toFloat();
			pInfo->foot_print = str15;
			pInfo->download_url = str16;

			pestDatasetPtrVec.push_back(pInfo);
		}
	}

	return pestDatasetPtrVec;
}

bool AgriMultispectralSensorDBHandler::AddSentinelMetadata(const AgriSentinelMetadataPtr& pestDataset)
{
	if (!pestDataset) return false;

	QString strSql = "INSERT INTO " + QString(AGRI_SENTINEL_METADATA_TABLE_NAME) + " (`guid`, `name`, `field_id`, `crop_type`, `collect_type`, `begin_time`, `end_time`, `pest_disease`, `pest_category`) VALUES (";
	
	/*
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
	*/

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriMultispectralSensorDBHandler::UpdateSentinelMetadata(const AgriSentinelMetadataPtr& pestDataset)
{
	if (!pestDataset) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_SENTINEL_METADATA_TABLE_NAME) + " SET "
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

	/*
	query.bindValue(":name", pestDataset->name);
	query.bindValue(":field_id", pestDataset->field_id);
	query.bindValue(":crop_type", pestDataset->crop_type);
	query.bindValue(":collect_type", pestDataset->collect_type);
	query.bindValue(":begin_time", pestDataset->begin_time);
	query.bindValue(":end_time", pestDataset->end_time);
	query.bindValue(":pest_disease", pestDataset->pest_disease);
	query.bindValue(":pest_category", pestDataset->pest_category);
	query.bindValue(":guid", pestDataset->guid);
	*/

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError();
		return false;
	}
}

bool AgriMultispectralSensorDBHandler::DeleteSentinelMetadata(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SENTINEL_METADATA_TABLE_NAME) + " where guid = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}


