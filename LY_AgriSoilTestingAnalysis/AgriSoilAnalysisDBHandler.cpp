#include <QSettings>
#include <QCString.h>
#include <QVector>
#include <QSharedPointer>
#include <QDebug>
#include <QSqlError>

#include "AgriSoilAnalysisDBHandler.h"

AgriSoilAnalysisDBHandler::AgriSoilAnalysisDBHandler(QObject *parent /*= nullptr*/)
	:isOpen(false)
{
	InitDataBase();
}

AgriSoilAnalysisDBHandler::~AgriSoilAnalysisDBHandler()
{

}

void AgriSoilAnalysisDBHandler::InitDataBase()
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

bool AgriSoilAnalysisDBHandler::executeQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
		return false;
	}
	return true;
}

QSqlQuery AgriSoilAnalysisDBHandler::executeSelectQuery(const QString &queryStr) {
	QSqlQuery query;
	if (!query.exec(queryStr)) {
		qDebug() << "SQL Error: " << query.lastError().text();
	}
	return query;
}

bool AgriSoilAnalysisDBHandler::beginTransaction() {
	return db.transaction();
}

bool AgriSoilAnalysisDBHandler::commitTransaction() {
	return db.commit();
}

bool AgriSoilAnalysisDBHandler::rollbackTransaction() {
	return db.rollback();
}

QSharedPointer<QSqlQuery> AgriSoilAnalysisDBHandler::getQueryOperator()
{
	return QSharedPointer<QSqlQuery>(new QSqlQuery(db));
}

AgriSoilTestingFertilizer::AgriSoilTestingDatasetPtr AgriSoilAnalysisDBHandler::GetSoilTestingDataset(const QString& strId) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSoilTestingDatasetPtr soilTestingDatasetPtr = std::make_shared<AgriSoilTestingDataset>();

	QString strSql = "SELECT `guid`, `name`, `field_id`, `collect_type`, `soil_type`, `soil_color`, `soil_texture`, `n_type`, `p_type`, `k_type`, `sample_type`, `sample_mode`, `create_time`, `update_time`, `grid_size`, `grid_rotation`, `valid_identify`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SOIL_TESTING_DATASET_TABLE_NAME;
	strGeneralSql += QString(" WHERE `guid` = %1;").arg(QString("\"" + strId + "\""));

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		if (query.next())
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
			QString str17 = query.value(16).toString();

			
			soilTestingDatasetPtr->guid = str1;
			soilTestingDatasetPtr->name = str2;
			soilTestingDatasetPtr->field_id = str3;
			soilTestingDatasetPtr->collect_type = str4.toInt();
			soilTestingDatasetPtr->soil_type = str5.toInt();
			soilTestingDatasetPtr->soil_color = str6.toInt();
			soilTestingDatasetPtr->soil_texture = str7.toInt();
			soilTestingDatasetPtr->n_type = str8.toInt();
			soilTestingDatasetPtr->p_type = str9.toInt();
			soilTestingDatasetPtr->k_type = str10.toInt();
			soilTestingDatasetPtr->sample_type = str11.toInt();
			soilTestingDatasetPtr->sample_mode = str12.toInt();
			soilTestingDatasetPtr->create_time = str13;
			soilTestingDatasetPtr->update_time = str14;
			soilTestingDatasetPtr->grid_size = str15.toDouble();
			soilTestingDatasetPtr->grid_rotation = str16.toInt();
			soilTestingDatasetPtr->valid_identify = str17.toUInt();
		}
	}

	return soilTestingDatasetPtr;
}

AgriSoilTestingDatasetPtrVec AgriSoilAnalysisDBHandler::GetSoilTestingDatasetList(const QStringList& fieldList) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSoilTestingDatasetPtrVec soilTestingDatasetPtrVec;

	QString strSql = "SELECT `guid`, `name`, `field_id`, `collect_type`, `soil_type`, `soil_color`, `soil_texture`, `n_type`, `p_type`, `k_type`, `sample_type`, `sample_mode`, `create_time`, `update_time`, `grid_size`, `grid_rotation`, `valid_identify`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SOIL_TESTING_DATASET_TABLE_NAME;

	if (fieldList.size())
	{
		QString idString = fieldList.join(",\"");

		// 执行 WHERE 语句
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
			QString str10 = query.value(9).toString();
			QString str11 = query.value(10).toString();
			QString str12 = query.value(11).toString();
			QString str13 = query.value(12).toString();
			QString str14 = query.value(13).toString();
			QString str15 = query.value(14).toString();
			QString str16 = query.value(15).toString();
			QString str17 = query.value(16).toString();

			AgriSoilTestingDatasetPtr pInfo = std::make_shared<AgriSoilTestingDataset>();
			pInfo->guid = str1;
			pInfo->name = str2;
			pInfo->field_id = str3;
			pInfo->collect_type = str4.toInt();
			pInfo->soil_type = str5.toInt();
			pInfo->soil_color = str6.toInt();
			pInfo->soil_texture = str7.toInt();
			pInfo->n_type = str8.toInt();
			pInfo->p_type = str9.toInt();
			pInfo->k_type = str10.toInt();
			pInfo->sample_type = str11.toInt();
			pInfo->sample_mode = str12.toInt();
			pInfo->create_time = str13;
			pInfo->update_time = str14;
			pInfo->grid_size = str15.toDouble();
			pInfo->grid_rotation = str16.toInt();
			pInfo->valid_identify = str17.toUInt();

			soilTestingDatasetPtrVec.push_back(pInfo);
		}
	}

	return soilTestingDatasetPtrVec;
}

bool AgriSoilAnalysisDBHandler::AddSoilTestingDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset)
{
	if (!soilTestingDataset) return false;

	QString strSql = "INSERT INTO " + QString(AGRI_SOIL_TESTING_DATASET_TABLE_NAME) 
		+ " (`guid`, `name`, `field_id`, `collect_type`, `soil_type`, `soil_color`, `soil_texture`, `n_type`, `p_type`, `k_type`, `sample_type`, `sample_mode`, `create_time`, `update_time`, `grid_size`, `grid_rotation`, `valid_identify`) VALUES (";
	strSql += "'" + soilTestingDataset->guid + "',";
	strSql += "'" + soilTestingDataset->name + "',";
	strSql += "'" + soilTestingDataset->field_id + "',";
	strSql += "'" + QString::number(soilTestingDataset->collect_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->soil_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->soil_color) + "',";
	strSql += "'" + QString::number(soilTestingDataset->soil_texture) + "',";
	strSql += "'" + QString::number(soilTestingDataset->n_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->p_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->k_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->sample_type) + "',";
	strSql += "'" + QString::number(soilTestingDataset->sample_mode) + "',";
	strSql += "'" + soilTestingDataset->create_time + "',";
	strSql += "'" + soilTestingDataset->update_time + "',";
	strSql += "'" + QString::number(soilTestingDataset->grid_size) + "',";
	strSql += "'" + QString::number(soilTestingDataset->grid_rotation) + "',";
	strSql += "'" + QString::number(soilTestingDataset->valid_identify) + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriSoilAnalysisDBHandler::UpdateSoilTestingDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset)
{
	if (!soilTestingDataset) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_SOIL_TESTING_DATASET_TABLE_NAME) + " SET "
		"name = :name, "
		"field_id = :field_id, "
		"collect_type = :collect_type, "
		"soil_type = :soil_type, "
		"soil_color = :soil_color, "
		"soil_texture = :soil_texture, "
		"n_type = :n_type, "
		"p_type = :p_type, "
		"k_type = :k_type, "
		"sample_type = :sample_type, "
		"sample_mode = :sample_mode, "
		"create_time = :create_time, "
		"update_time = :update_time, "
		"grid_size = :grid_size, "
		"grid_rotation = :grid_rotation, "
		"valid_identify = :valid_identify "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", soilTestingDataset->name);
	query.bindValue(":field_id", soilTestingDataset->field_id);
	query.bindValue(":collect_type", soilTestingDataset->collect_type);
	query.bindValue(":soil_type", soilTestingDataset->soil_type);
	query.bindValue(":soil_color", soilTestingDataset->soil_color);
	query.bindValue(":soil_texture", soilTestingDataset->soil_texture);
	query.bindValue(":n_type", soilTestingDataset->n_type);
	query.bindValue(":p_type", soilTestingDataset->p_type);
	query.bindValue(":k_type", soilTestingDataset->k_type);
	query.bindValue(":sample_type", soilTestingDataset->sample_type);
	query.bindValue(":sample_mode", soilTestingDataset->sample_mode);
	query.bindValue(":create_time", soilTestingDataset->create_time);
	query.bindValue(":update_time", soilTestingDataset->update_time);
	query.bindValue(":grid_size", soilTestingDataset->grid_size);
	query.bindValue(":grid_rotation", soilTestingDataset->grid_rotation);
	query.bindValue(":valid_identify", soilTestingDataset->valid_identify);
	query.bindValue(":guid", soilTestingDataset->guid);

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError();
		return false;
	}
}

bool AgriSoilAnalysisDBHandler::DeleteSoilTestingDataset(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SOIL_TESTING_DATASET_TABLE_NAME) + " where guid = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

AgriSoilTestingCollectionPtrVec AgriSoilAnalysisDBHandler::GetSoilTestingCollectionList(const QString& sDatasetId) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSoilTestingCollectionPtrVec soilTestingDatasetPtrVec;

	QString strSql = "SELECT `guid`, `name`, `dataset_id`, `sample_type`, `sample_mode`, `sample_time`, `lon`, `lat`, `alt`, `ele_nutrient_N`, `ele_nutrient_P`, `ele_nutrient_K`, ";
	strSql += "`ele_nutrient_N`, `ele_nutrient_P`, `ele_nutrient_K`, ";
	strSql += "`ele_minor_Ca`, `ele_minor_Mg`, `ele_minor_Cu`, `ele_minor_Zn`, `ele_minor_Fe`, `ele_minor_S`, `ele_minor_Mn`, `ele_minor_Mo`, `ele_minor_B`, `ele_minor_Cl`, `ele_minor_Si`, ";
	strSql += "`ele_heavy_metal_Pb`, `ele_heavy_metal_Cr`, `ele_heavy_metal_Cd`, `ele_heavy_metal_Hg`, ";
	strSql += "`organic_matter`, ";
	strSql += "`mois_Temp`, `mois_Humid`, `mois_PH`, `mois_EC`, `mois_Water`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME + " WHERE dataset_id = \"" + sDatasetId + "\";";

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QStringList strList;
			for (int i = 0; i < 36; i++){
				strList << query.value(i).toString();
			}

			AgriSoilTestingCollectionPtr pInfo = std::make_shared<AgriSoilTestingCollection>();
			pInfo->guid = strList[0].toStdString();
			pInfo->name = strList[1].toStdString();
			pInfo->dataset_id = strList[2].toStdString();
			pInfo->sample_type = strList[3].toInt();
			pInfo->sample_mode = strList[4].toInt();
			pInfo->sample_time = strList[5].toStdString();
			pInfo->lon = strList[6].toDouble();
			pInfo->lat = strList[7].toDouble();
			pInfo->alt = strList[8].toDouble();
			pInfo->ele_nutrient_N = strList[9].toDouble();
			pInfo->ele_nutrient_P = strList[10].toDouble();
			pInfo->ele_nutrient_K = strList[11].toDouble();

			soilTestingDatasetPtrVec.push_back(pInfo);
		}
	}

	return soilTestingDatasetPtrVec;
}

AgriSoilTestingCollectionPtrVec AgriSoilAnalysisDBHandler::GetSoilTestingCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSoilTestingCollectionPtrVec soilTestingDatasetPtrVec;

	int offset = (nPage - 1) * nPageSize; // 计算偏移量
	QString queryStr = QString(
		"SELECT guid, name, dataset_id, sample_type, sample_mode, sample_time, lon, lat, alt, "
		"`ele_nutrient_N`, `ele_nutrient_P`, `ele_nutrient_K`, "
		"`ele_minor_Ca`, `ele_minor_Mg`, `ele_minor_Cu`, `ele_minor_Zn`, `ele_minor_Fe`, `ele_minor_S`, `ele_minor_Mn`, `ele_minor_Mo`, `ele_minor_B`, `ele_minor_Cl`, `ele_minor_Si`, "
		"`ele_heavy_metal_Pb`, `ele_heavy_metal_Cr`, `ele_heavy_metal_Cd`, `ele_heavy_metal_Hg`, "
		"`organic_matter`, "
		"`mois_Temp`, `mois_Humid`, `mois_PH`, `mois_EC`, `mois_Water`"
		" FROM " + QString(AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME) +
		" WHERE dataset_id = \"" + sDatasetId + "\""
		" ORDER BY id ASC"
		" LIMIT %1 OFFSET %2").arg(nPageSize).arg(offset);

	if (!query.exec(queryStr)) {
		qWarning() << "Query Failed:" << query.lastError().text();
		return soilTestingDatasetPtrVec;
	}

	while (query.next()) {
		AgriSoilTestingCollectionPtr item = std::make_shared<AgriSoilTestingCollection>();
		item->guid = query.value("guid").toString().toStdString();
		item->name = query.value("name").toString().toStdString();
		item->dataset_id = query.value("dataset_id").toString().toStdString();
		item->sample_type = query.value("sample_type").toInt();
		item->sample_mode = query.value("sample_mode").toInt();
		item->sample_time = query.value("sample_time").toString().toStdString();
		item->lon = query.value("lon").toDouble();
		item->lat = query.value("lat").toDouble();
		item->alt = query.value("alt").toDouble();
		item->ele_nutrient_N = query.value("ele_nutrient_N").toDouble();
		item->ele_nutrient_P = query.value("ele_nutrient_P").toDouble();
		item->ele_nutrient_K = query.value("ele_nutrient_K").toDouble();
		item->ele_minor_Ca = query.value("ele_minor_Ca").toDouble();
		item->ele_minor_Mg = query.value("ele_minor_Mg").toDouble();
		item->ele_minor_Cu = query.value("ele_minor_Cu").toDouble();
		item->ele_minor_Zn = query.value("ele_minor_Zn").toDouble();
		item->ele_minor_Fe = query.value("ele_minor_Fe").toDouble();
		item->ele_minor_S = query.value("ele_minor_S").toDouble();
		item->ele_minor_Mn = query.value("ele_minor_Mn").toDouble();
		item->ele_minor_Mo = query.value("ele_minor_Mo").toDouble();
		item->ele_minor_B = query.value("ele_minor_B").toDouble();
		item->ele_minor_Cl = query.value("ele_minor_Cl").toDouble();
		item->ele_minor_Si = query.value("ele_minor_Si").toDouble();
		item->ele_heavy_metal_Pb = query.value("ele_heavy_metal_Pb").toDouble();
		item->ele_heavy_metal_Cr = query.value("ele_heavy_metal_Cr").toDouble();
		item->ele_heavy_metal_Cd = query.value("ele_heavy_metal_Cd").toDouble();
		item->ele_heavy_metal_Hg = query.value("ele_heavy_metal_Hg").toDouble();
		item->organic_matter = query.value("organic_matter").toFloat();
		item->mois_Temp = query.value("mois_Temp").toFloat();
		item->mois_Humid = query.value("mois_Humid").toFloat();
		item->mois_PH = query.value("mois_PH").toFloat();
		item->mois_EC = query.value("mois_EC").toFloat();
		item->mois_Water = query.value("mois_Water").toFloat();

		soilTestingDatasetPtrVec.push_back(item);
	}

	return soilTestingDatasetPtrVec;
}

int AgriSoilAnalysisDBHandler::GetSoilTestingCollectionCount(const QString& sDatasetId) const
{
	QString strSql = "SELECT COUNT(*)";
	QString strGeneralSql = strSql + " FROM " + AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME + " WHERE dataset_id = :dataset_id";

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

bool AgriSoilAnalysisDBHandler::AddSoilTestingCollection(const AgriSoilTestingCollectionPtr& soilTestingCollection)
{
	if (!soilTestingCollection) return false;

	QSqlQuery query;
	query.prepare(R"(
        INSERT INTO agri_soil_testing_collection
        (guid, name, dataset_id, sample_type, sample_mode, sample_time, lon, lat, alt, 
         ele_nutrient_N, ele_nutrient_P, ele_nutrient_K,
		 ele_minor_Ca, ele_minor_Mg, ele_minor_Cu, ele_minor_Zn, ele_minor_Fe, ele_minor_S, ele_minor_Mn, ele_minor_Mo, ele_minor_B, ele_minor_Cl, ele_minor_Si, 
		 ele_heavy_metal_Pb, ele_heavy_metal_Cr, ele_heavy_metal_Cd, ele_heavy_metal_Hg, 
		 organic_matter, 
		 mois_Temp, mois_Humid, mois_PH, ois_EC, mois_Water)
        VALUES 
        (:guid, :name, :dataset_id, :sample_type, :sample_mode, :sample_time, :lon, :lat, :alt, 
         :ele_nutrient_N, :ele_nutrient_P, :ele_nutrient_K,
		 :ele_minor_Ca, :ele_minor_Mg, :ele_minor_Cu, :ele_minor_Zn, :ele_minor_Fe, :ele_minor_S, :ele_minor_Mn, :ele_minor_Mo, :ele_minor_B, :ele_minor_Cl, :ele_minor_Si, 
		 :ele_heavy_metal_Pb, :ele_heavy_metal_Cr, :ele_heavy_metal_Cd, :ele_heavy_metal_Hg, 
		 :organic_matter, 
		 :mois_Temp, :mois_Humid, :mois_PH, :ois_EC, :mois_Water)
    )");

	query.bindValue(":guid", QString::fromStdString(soilTestingCollection->guid));
	query.bindValue(":name", QString::fromStdString(soilTestingCollection->name));
	query.bindValue(":dataset_id", QString::fromStdString(soilTestingCollection->dataset_id));
	query.bindValue(":sample_type", soilTestingCollection->sample_type);
	query.bindValue(":sample_mode", soilTestingCollection->sample_mode);
	query.bindValue(":sample_time", QString::fromStdString(soilTestingCollection->sample_time));
	query.bindValue(":lon", soilTestingCollection->lon);
	query.bindValue(":lat", soilTestingCollection->lat);
	query.bindValue(":alt", soilTestingCollection->alt);
	query.bindValue(":ele_nutrient_N", soilTestingCollection->ele_nutrient_N);
	query.bindValue(":ele_nutrient_P", soilTestingCollection->ele_nutrient_P);
	query.bindValue(":ele_nutrient_K", soilTestingCollection->ele_nutrient_K);
	query.bindValue(":ele_minor_Ca", soilTestingCollection->ele_minor_Ca);
	query.bindValue(":ele_minor_Mg", soilTestingCollection->ele_minor_Mg);
	query.bindValue(":ele_minor_Cu", soilTestingCollection->ele_minor_Cu);
	query.bindValue(":ele_minor_Zn", soilTestingCollection->ele_minor_Zn);
	query.bindValue(":ele_minor_Fe", soilTestingCollection->ele_minor_Fe);
	query.bindValue(":ele_minor_S", soilTestingCollection->ele_minor_S);
	query.bindValue(":ele_minor_Mn", soilTestingCollection->ele_minor_Mn);
	query.bindValue(":ele_minor_Mo", soilTestingCollection->ele_minor_Mo);
	query.bindValue(":ele_minor_B", soilTestingCollection->ele_minor_B);
	query.bindValue(":ele_minor_Cl", soilTestingCollection->ele_minor_Cl);
	query.bindValue(":ele_minor_Si", soilTestingCollection->ele_minor_Si);
	query.bindValue(":ele_heavy_metal_Pb", soilTestingCollection->ele_heavy_metal_Pb);
	query.bindValue(":ele_heavy_metal_Cr", soilTestingCollection->ele_heavy_metal_Cr);
	query.bindValue(":ele_heavy_metal_Cd", soilTestingCollection->ele_heavy_metal_Cd);
	query.bindValue(":ele_heavy_metal_Hg", soilTestingCollection->ele_heavy_metal_Hg);
	query.bindValue(":organic_matter", soilTestingCollection->organic_matter);
	query.bindValue(":mois_Temp", soilTestingCollection->mois_Temp);
	query.bindValue(":mois_Humid", soilTestingCollection->mois_Humid);
	query.bindValue(":mois_PH", soilTestingCollection->mois_PH);
	query.bindValue(":mois_EC", soilTestingCollection->mois_EC);
	query.bindValue(":mois_Water", soilTestingCollection->mois_Water);

	if (!query.exec()) {
		qDebug() << "Insert failed:" << query.lastError().text();
		return false;
	}
	qDebug() << "Insert successful!";
	return true;
}

bool AgriSoilAnalysisDBHandler::UpdateSoilTestingCollection(const AgriSoilTestingCollectionPtr& soilTestingCollection)
{
	if (!soilTestingCollection) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME) + " SET "
		"name = :name, "
		"dataset_id = :dataset_id, "
		"sample_type = :sample_type, "
		"sample_mode = :sample_mode, "
		"sample_time = :sample_time, "
		"lon = :lon, "
		"lat = :lat, "
		"alt = :alt, "
		"ele_nutrient_N = :ele_nutrient_N, "
		"ele_nutrient_P = :ele_nutrient_P, "
		"ele_nutrient_K = :ele_nutrient_K "
		"ele_minor_Ca = :ele_minor_Ca "
		"ele_minor_Mg = :ele_minor_Mg "
		"ele_minor_Cu = :ele_minor_Cu "
		"ele_minor_Zn = :ele_minor_Zn "
		"ele_minor_Fe = :ele_minor_Fe "
		"ele_minor_S = :ele_minor_S "
		"ele_minor_Mn = :ele_minor_Mn "
		"ele_minor_Mo = :ele_minor_Mo "
		"ele_minor_B = :ele_minor_B "
		"ele_minor_Cl = :ele_minor_Cl "
		"ele_minor_Si = :ele_minor_Si "
		"ele_heavy_metal_Pb = :ele_heavy_metal_Pb "
		"ele_heavy_metal_Cr = :ele_heavy_metal_Cr "
		"ele_heavy_metal_Cd = :ele_heavy_metal_Cd "
		"ele_heavy_metal_Hg = :ele_heavy_metal_Hg "
		"organic_matter = :organic_matter "
		"mois_Temp = :mois_Temp "
		"mois_Humid = :mois_Humid "
		"mois_PH = :mois_PH "
		"mois_EC = :mois_EC "
		"mois_Water = :mois_Water "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", QString::fromStdString(soilTestingCollection->name));
	query.bindValue(":dataset_id", QString::fromStdString(soilTestingCollection->dataset_id));
	query.bindValue(":sample_type", soilTestingCollection->sample_type);
	query.bindValue(":sample_mode", soilTestingCollection->sample_mode);
	query.bindValue(":sample_time", QString::fromStdString(soilTestingCollection->sample_time));
	query.bindValue(":lon", soilTestingCollection->lon);
	query.bindValue(":lat", soilTestingCollection->lat);
	query.bindValue(":alt", soilTestingCollection->alt);
	query.bindValue(":ele_nutrient_N", soilTestingCollection->ele_nutrient_N);
	query.bindValue(":ele_nutrient_P", soilTestingCollection->ele_nutrient_P);
	query.bindValue(":ele_nutrient_K", soilTestingCollection->ele_nutrient_K);
	query.bindValue(":ele_minor_Ca", soilTestingCollection->ele_minor_Ca);
	query.bindValue(":ele_minor_Mg", soilTestingCollection->ele_minor_Mg);
	query.bindValue(":ele_minor_Cu", soilTestingCollection->ele_minor_Cu);
	query.bindValue(":ele_minor_Zn", soilTestingCollection->ele_minor_Zn);
	query.bindValue(":ele_minor_Fe", soilTestingCollection->ele_minor_Fe);
	query.bindValue(":ele_minor_S", soilTestingCollection->ele_minor_S);
	query.bindValue(":ele_minor_Mn", soilTestingCollection->ele_minor_Mn);
	query.bindValue(":ele_minor_Mo", soilTestingCollection->ele_minor_Mo);
	query.bindValue(":ele_minor_B", soilTestingCollection->ele_minor_B);
	query.bindValue(":ele_minor_Cl", soilTestingCollection->ele_minor_Cl);
	query.bindValue(":ele_minor_Si", soilTestingCollection->ele_minor_Si);
	query.bindValue(":ele_heavy_metal_Pb", soilTestingCollection->ele_heavy_metal_Pb);
	query.bindValue(":ele_heavy_metal_Cr", soilTestingCollection->ele_heavy_metal_Cr);
	query.bindValue(":ele_heavy_metal_Cd", soilTestingCollection->ele_heavy_metal_Cd);
	query.bindValue(":ele_heavy_metal_Hg", soilTestingCollection->ele_heavy_metal_Hg);
	query.bindValue(":organic_matter", soilTestingCollection->organic_matter);
	query.bindValue(":mois_Temp", soilTestingCollection->mois_Temp);
	query.bindValue(":mois_Humid", soilTestingCollection->mois_Humid);
	query.bindValue(":mois_PH", soilTestingCollection->mois_PH);
	query.bindValue(":mois_EC", soilTestingCollection->mois_EC);
	query.bindValue(":mois_Water", soilTestingCollection->mois_Water);
	query.bindValue(":guid", QString::fromStdString(soilTestingCollection->guid));

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError() << query.executedQuery();
		return false;
	}
}

bool AgriSoilAnalysisDBHandler::DeleteSoilTestingCollection(const QString& strId)
{
	if (strId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME) + " where guid = ";
	strSql += "'" + strId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}

bool AgriSoilAnalysisDBHandler::BatchInsertSoilTestingCollection(const AgriSoilTestingCollectionPtrVec& collectInfoVec)
{
	// 开启事务
	QSqlQuery query;
	if (!query.exec("START TRANSACTION")) {
		qDebug() << "Failed to start transaction: " << query.lastError().text();
		return false;
	}

	// 插入数据
	query.prepare("INSERT INTO agri_soil_testing_collection (`guid`, `name`, `dataset_id`, `sample_type`, `sample_mode`, `sample_time`, `lon`, `lat`, `alt`, "
		"`ele_nutrient_N`, `ele_nutrient_P`, `ele_nutrient_K`, "
		"`ele_minor_Ca`, `ele_minor_Mg`, `ele_minor_Cu`, `ele_minor_Zn`, `ele_minor_Fe`, `ele_minor_S`, `ele_minor_Mn`, `ele_minor_Mo`, `ele_minor_B`, `ele_minor_Cl`, `ele_minor_Si`, "
		"`ele_heavy_metal_Pb`, `ele_heavy_metal_Cr`, `ele_heavy_metal_Cd`, `ele_heavy_metal_Hg`, "
		"`organic_matter`, "
		"`mois_Temp`, `mois_Humid`, `mois_PH`, `mois_EC`, `mois_Water`) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

	for (const auto& collect : collectInfoVec)
	{
		query.addBindValue(QString::fromStdString(collect->guid));
		query.addBindValue(QString::fromStdString(collect->name));
		query.addBindValue(QString::fromStdString(collect->dataset_id));
		query.addBindValue(collect->sample_type);
		query.addBindValue(collect->sample_mode);
		query.addBindValue(QString::fromStdString(collect->sample_time));
		query.addBindValue(collect->lon);
		query.addBindValue(collect->lat);
		query.addBindValue(collect->alt);
		query.addBindValue(collect->ele_nutrient_N);
		query.addBindValue(collect->ele_nutrient_P);
		query.addBindValue(collect->ele_nutrient_K);
		query.addBindValue(collect->ele_minor_Ca);
		query.addBindValue(collect->ele_minor_Mg);
		query.addBindValue(collect->ele_minor_Cu);
		query.addBindValue(collect->ele_minor_Zn);
		query.addBindValue(collect->ele_minor_Fe);
		query.addBindValue(collect->ele_minor_S);
		query.addBindValue(collect->ele_minor_Mn);
		query.addBindValue(collect->ele_minor_Mo);
		query.addBindValue(collect->ele_minor_B);
		query.addBindValue(collect->ele_minor_Cl);
		query.addBindValue(collect->ele_minor_Si);
		query.addBindValue(collect->ele_heavy_metal_Pb);
		query.addBindValue(collect->ele_heavy_metal_Cr);
		query.addBindValue(collect->ele_heavy_metal_Cd);
		query.addBindValue(collect->ele_heavy_metal_Hg);
		query.addBindValue(collect->organic_matter);
		query.addBindValue(collect->mois_Temp);
		query.addBindValue(collect->mois_Humid);
		query.addBindValue(collect->mois_PH);
		query.addBindValue(collect->mois_EC);
		query.addBindValue(collect->mois_Water);

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

bool AgriSoilAnalysisDBHandler::BatchDeleteSoilTestingCollection(const QStringList &guids)
{
	if (guids.isEmpty()) {
		qDebug() << "ID 列表为空，跳过删除";
		return false;
	}

	QString sql = QString("DELETE FROM agri_soil_testing_collection WHERE id IN (%1)").arg(guids.join(","));

	QSqlQuery query(db);
	if (!query.exec(sql)) {
		qDebug() << "删除失败:" << query.lastError().text();
		return false;
	}

	qDebug() << "删除成功, 影响行数:" << query.numRowsAffected();
	return true;
}

bool AgriSoilAnalysisDBHandler::BatchDeleteSoilTestingCollection(const QString sDatasetId)
{
	if (sDatasetId.isEmpty())
	{
		return false;
	}

	int count = GetSoilTestingCollectionCount(sDatasetId);
	if (0 == count)
	{
		// 无待删除数据
		return true;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SOIL_TESTING_COLLECTION_TABLE_NAME) + " where dataset_id = ";
	strSql += "'" + sDatasetId + "'";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Delete failed: " << query.lastError().text();
		return false;
	}

	if (query.next() && query.value(0).toInt() == 0) {
		// 表为空时，重置 AUTO_INCREMENT
		if (!query.exec("ALTER TABLE agri_soil_testing_collection AUTO_INCREMENT = 1")) {
			qDebug() << "重置 ID 失败:" << query.lastError().text(); 
		}
	}

	// 释放已删除数据的空间
	if (!query.exec("OPTIMIZE TABLE agri_soil_testing_collection")) {
		qDebug() << "优化表失败:" << query.lastError().text();
	}

	return true;
}

AgriSoilFertilityDistributeMapPtrVec AgriSoilAnalysisDBHandler::GetSoilTestingDistributeMap(const QString& sDatasetId) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	AgriSoilFertilityDistributeMapPtrVec pestDistributeMapPtrVec;

	QString strSql = "SELECT `guid`, `name`, `dataset_id`, `crop_type`, `growth_stage`, `field_area`, `sample_type`, `sample_mode`, `collect_count`, `grid_unit`, `diffuse_layer`, `distribute_statistic`, `map_type`, `valid_identify`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SOIL_TESTING_DISTRIBUTE_MAP_TABLE_NAME + " WHERE dataset_id = \"" + sDatasetId + "\";";

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

			AgriSoilFertilityDistributeMapPtr pInfo = std::make_shared<AgriSoilFertilityDistributeMap>();
			pInfo->guid = str1;
			pInfo->name = str2;
			pInfo->dataset_id = str3;
			pInfo->crop_type = str4.toInt();
			pInfo->growth_stage = str5.toInt();
			pInfo->field_area = str6.toDouble();
			pInfo->sample_type = str7.toInt();
			pInfo->sample_mode = str8.toInt();
			pInfo->collect_count = str9.toInt();
			pInfo->grid_unit = str10.toInt();
			pInfo->diffuse_layer = str11.toInt();
			pInfo->distribute_statistic = str12;
			pInfo->map_type = str13.toInt();
			pInfo->valid_identify = str14.toInt();

			pestDistributeMapPtrVec.push_back(pInfo);
		}
	}

	return pestDistributeMapPtrVec;
}

bool AgriSoilAnalysisDBHandler::AddSoilTestingDistributeMap(const AgriSoilFertilityDistributeMapPtr& disMap)
{
	if (!disMap) return false;

	QString strSql = "INSERT INTO " + QString(AGRI_SOIL_TESTING_DISTRIBUTE_MAP_TABLE_NAME) + " (`guid`, `name`, `dataset_id`, `crop_type`, `growth_stage`, `field_area`, `sample_type`, `sample_mode`, `collect_count`, `grid_unit`, `diffuse_layer`, `distribute_statistic`, `map_type`, `valid_identify`) VALUES (";
	strSql += "'" + disMap->guid + "',";
	strSql += "'" + disMap->name + "',";
	strSql += "'" + disMap->dataset_id + "',";
	strSql += "'" + QString::number(disMap->crop_type) + "',";
	strSql += "'" + QString::number(disMap->growth_stage) + "',";
	strSql += "'" + QString::number(disMap->field_area) + "',";
	strSql += "'" + QString::number(disMap->sample_type) + "',";
	strSql += "'" + QString::number(disMap->sample_mode) + "',";
	strSql += "'" + QString::number(disMap->collect_count) + "',";
	strSql += "'" + QString::number(disMap->grid_unit) + "',";
	strSql += "'" + QString::number(disMap->diffuse_layer) + "',";
	strSql += "'" + disMap->distribute_statistic + "',";
	strSql += "'" + QString::number(disMap->map_type) + "',";
	strSql += "'" + QString::number(disMap->valid_identify) + "'";
	strSql += ")";

	QSqlQuery query;
	if (!query.exec(strSql)) {
		qDebug() << "Insert failed. Error:" << query.lastError().text();
		return false;
	}
	return true;
}

bool AgriSoilAnalysisDBHandler::UpdateSoilTestingDistributeMap(const AgriSoilFertilityDistributeMapPtr& disMap)
{
	if (!disMap) return false;

	QSqlQuery query;

	QString updateQuery = "UPDATE " + QString(AGRI_SOIL_TESTING_DISTRIBUTE_MAP_TABLE_NAME) + " SET "
		"name = :name, "
		"dataset_id = :dataset_id, "
		"crop_type = :crop_type, "
		"growth_stage = :growth_stage, "
		"field_area = :field_area, "
		"sample_type = :sample_type, "
		"sample_mode = :sample_mode, "
		"collect_count = :collect_count, "
		"grid_unit = :grid_unit, "
		"diffuse_layer = :diffuse_layer, "
		"distribute_statistic = :distribute_statistic, "
		"map_type = :map_type, "
		"valid_identify = :valid_identify "
		"WHERE guid = :guid";

	query.prepare(updateQuery);

	query.bindValue(":name", disMap->name);
	query.bindValue(":dataset_id", disMap->dataset_id);
	query.bindValue(":crop_type", disMap->crop_type);
	query.bindValue(":growth_stage", disMap->growth_stage);
	query.bindValue(":field_area", disMap->field_area);
	query.bindValue(":sample_type", disMap->sample_type);
	query.bindValue(":sample_mode", disMap->sample_mode);
	query.bindValue(":collect_count", disMap->collect_count);
	query.bindValue(":grid_unit", disMap->grid_unit);
	query.bindValue(":diffuse_layer", disMap->diffuse_layer);
	query.bindValue(":distribute_statistic", disMap->distribute_statistic);
	query.bindValue(":map_type", disMap->map_type);
	query.bindValue(":valid_identify", disMap->valid_identify);
	query.bindValue(":guid", disMap->guid);

	if (query.exec()) {
		return true;
	}
	else {
		qDebug() << "Error: Unable to update record." << query.lastError() << query.executedQuery();
		return false;
	}
}

bool AgriSoilAnalysisDBHandler::DeleteSoilTestingDistributeMap(const QString& sDisMapId)
{
	if (sDisMapId.isEmpty())
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SOIL_TESTING_DISTRIBUTE_MAP_TABLE_NAME) + " where guid = ";
	strSql += "'" + sDisMapId + "'";
	QSqlQuery query;

	return query.exec(strSql);
}


AgriSoilTestingFertilizer::SaltFieldEventTypeInfoIDMap AgriSoilAnalysisDBHandler::GetSaltFieldEventTypeIDMap() const
{
	QMutexLocker locker(&sMutex);
	AgriSoilTestingFertilizer::SaltFieldEventTypeInfoIDMap dataMap;

	QSqlQuery query;
	QString strSql = "SELECT `id`, `name`, `code`, `eventTempLate`, `description`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SALT_FIELD_EVENT_TYPE_TABLE_NAME;

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		AgriSoilTestingFertilizer::SaltFieldEventTypeInfo info;
		while(query.next())
		{
			info.id = query.value(0).toInt();
			info.name = query.value(1).toString();
			info.code = query.value(2).toString();
			info.eventTemplate = query.value(3).toString();
			info.desc = query.value(4).toString();
			dataMap.insert(info.id, info);
		}
	}
	return dataMap;
}

AgriSoilTestingFertilizer::SaltFieldEventTypeInfoCodeMap AgriSoilAnalysisDBHandler::GetSaltFieldEventTypeCodeMap() const
{
	QMutexLocker locker(&sMutex);
	AgriSoilTestingFertilizer::SaltFieldEventTypeInfoCodeMap dataMap;

	QSqlQuery query;
	QString strSql = "SELECT `id`, `name`,  `code`, `eventTempLate`, `description`";
	QString strGeneralSql = strSql + " FROM " + AGRI_SALT_FIELD_EVENT_TYPE_TABLE_NAME;

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		AgriSoilTestingFertilizer::SaltFieldEventTypeInfo info;
		while(query.next())
		{
			info.id = query.value(0).toInt();
			info.name = query.value(1).toString();
			info.code = query.value(2).toString();
			info.eventTemplate = query.value(3).toString();
			info.desc = query.value(4).toString();
			dataMap.insert(info.code, info);
		}
	}
	return dataMap;
}

/*获取盐田的事件信息*/
AgriSoilTestingFertilizer::SaltFieldEventInfoPtr AgriSoilAnalysisDBHandler::GetSaltFieldEventDataset(int id) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;
	SaltFieldEventInfoPtr sPtr;
	QString strSql = "SELECT `id` ,`event_name`, `event_reporter_id`, `source_field_id`, `event_type`, `find_time`, `lon`, `lat`, `pic_data` , `description` ";
	QString strGeneralSql = strSql + " FROM " + AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME;
	strGeneralSql += QString(" WHERE `id` = %1;").arg(id);

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		if (query.next())
		{
			sPtr = std::make_shared<SaltFieldEventInfo>();
			sPtr->id = query.value(0).toInt();
			sPtr->name = query.value(1).toString();
			sPtr->reporterID = query.value(2).toString();
			sPtr->sourceFieldID = query.value(3).toString();
			sPtr->eventType = query.value(4).toInt();
			sPtr->findTime = query.value(5).toDateTime();
			sPtr->lon = query.value(6).toDouble();
			sPtr->lat = query.value(7).toDouble();
			sPtr->picData = query.value(8).toByteArray();
			sPtr->desc = query.value(9).toString();
		}
	}

	return sPtr;
}

/*获取盐田的事件信息*/
AgriSoilTestingFertilizer::SaltFieldEventInfoPtrVec AgriSoilAnalysisDBHandler::GetSaltFieldEventDatasetList(const QStringList& fieldList) const
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	SaltFieldEventInfoPtrVec ptrList;

	QString strSql = "SELECT `id` ,`event_name`, `event_reporter_id`, `source_field_id`, `event_type`, `find_time`, `lon`, `lat`, `pic_data` , `description` ";
	QString strGeneralSql = strSql + " FROM " + AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME;

	if (fieldList.size())
	{
		QString idString = fieldList.join(",\"");

		// 执行 WHERE 语句
		strGeneralSql += QString(" WHERE %1 IN (%2);")
			.arg("source_field_id")
			.arg(QString("\"" + idString + "\""));
	}

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			SaltFieldEventInfoPtr sPtr = std::make_shared<SaltFieldEventInfo>();
			sPtr->id = query.value(0).toInt();
			sPtr->name = query.value(1).toString();
			sPtr->reporterID = query.value(2).toString();
			sPtr->sourceFieldID = query.value(3).toString();
			sPtr->eventType = query.value(4).toInt();
			sPtr->findTime = query.value(5).toDateTime();
			sPtr->lon = query.value(6).toDouble();
			sPtr->lat = query.value(7).toDouble();
			sPtr->picData = query.value(8).toByteArray();
			sPtr->desc = query.value(9).toString();

			ptrList.push_back(sPtr);
		}
	}

	return ptrList;
}

bool AgriSoilAnalysisDBHandler::AddSaltFieldEventDataset(SaltFieldEventInfoPtr& eventInfo)
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;
	QString strSqlPrev = QString("INSERT INTO %1 (").arg(AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME);
	QString strSqlMid = QString(") VALUES (");
	QString strSqlEnd = QString(")");
	QMap<QString, QVariant> identity2ValMap;

	if (eventInfo->id > 0)
	{
		strSqlPrev += "id ,";
		strSqlMid += ":id ,";
		identity2ValMap.insert(":id", eventInfo->id);
	}

	if (!eventInfo->name.isEmpty())
	{
		strSqlPrev += "event_name ,";
		strSqlMid += ":event_name ,";
		identity2ValMap.insert(":event_name", eventInfo->name);
	}

	if (!eventInfo->reporterID.isEmpty())
	{
		strSqlPrev += "event_reporter_id ,";
		strSqlMid += ":event_reporter_id ,";
		identity2ValMap.insert(":event_reporter_id", eventInfo->reporterID);
	}

	if (!eventInfo->sourceFieldID.isEmpty())
	{
		strSqlPrev += "source_field_id ,";
		strSqlMid += ":source_field_id ,";
		identity2ValMap.insert(":source_field_id", eventInfo->sourceFieldID);
	}


	if (eventInfo->eventType > 0)
	{
		strSqlPrev += "event_type ,";
		strSqlMid += ":event_type ,";
		identity2ValMap.insert(":event_type", eventInfo->eventType);
	}

	if (eventInfo->findTime.isValid())
	{
		strSqlPrev += "find_time ,";
		strSqlMid += ":find_time ,";
		identity2ValMap.insert(":find_time", eventInfo->findTime);
	}

	strSqlPrev += "lon ,";
	strSqlMid += ":lon ,";
	identity2ValMap.insert(":lon", eventInfo->lon);

	strSqlPrev += "lat ,";
	strSqlMid += ":lat ,";
	identity2ValMap.insert(":lat", eventInfo->lat);


	if (!eventInfo->picData.isNull())
	{
		strSqlPrev += "pic_data ,";
		strSqlMid += ":pic_data ,";
		identity2ValMap.insert(":pic_data", eventInfo->picData);
	}

	if (!eventInfo->desc.isEmpty())
	{
		strSqlPrev += "description ,";
		strSqlMid += ":description ,";
		identity2ValMap.insert(":description", eventInfo->desc);
	}

	if (strSqlPrev.endsWith(","))
		strSqlPrev.chop(1);

	if (strSqlMid.endsWith(","))
		strSqlMid.chop(1);

	QString strSql = strSqlPrev + strSqlMid + strSqlEnd;

	query.prepare(strSql);
	for (auto iter = identity2ValMap.begin(); iter != identity2ValMap.end(); iter++)
	{
		query.bindValue(iter.key(), iter.value());
	}
	bool success = query.exec();

	// 获取最后一次添加数据的ID
	if (eventInfo->id <= 0)
	{
		QString sql = QString("SELECT MAX(`ID`) from `%1` ").arg(AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME);
		if (query.exec(sql))
		{
			while (query.next())
			{
				eventInfo->id = query.value(0).toInt();
			}
		}
	}

	return success;
}

bool AgriSoilAnalysisDBHandler::UpdateSaltFieldEventDataset(const SaltFieldEventInfoPtr& eventInfo)
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;
	QString strSqlPrev = QString("UPDATE %1 ").arg(AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME);
	QString strSqlMid = QString("SET ");
	QString strSqlEnd = QString("");
	QMap<QString, QVariant> identity2ValMap;

	if (eventInfo->id > 0)
	{
		strSqlMid += "id=:id,";
		identity2ValMap.insert(":id", eventInfo->id);
	}

	if (!eventInfo->name.isEmpty())
	{
		strSqlMid += "event_name=:event_name,";
		identity2ValMap.insert(":event_name", eventInfo->name);
	}

	if (!eventInfo->reporterID.isEmpty())
	{
		strSqlMid += "event_reporter_id=:event_reporter_id,";
		identity2ValMap.insert(":event_reporter_id", eventInfo->reporterID);
	}

	if (!eventInfo->sourceFieldID.isEmpty())
	{
		strSqlMid += "source_field_id=:source_field_id,";
		identity2ValMap.insert(":source_field_id", eventInfo->sourceFieldID);
	}

	if (eventInfo->eventType > 0)
	{
		strSqlMid += "event_type=:event_type,";
		identity2ValMap.insert(":event_type", eventInfo->eventType);
	}

	if (eventInfo->findTime.isValid())
	{
		strSqlMid += "find_time=:find_time,";
		identity2ValMap.insert(":find_time", eventInfo->findTime);
	}

	strSqlMid += "lon=:lon,";
	identity2ValMap.insert(":lon", eventInfo->lon);

	strSqlMid += "lat=:lat,";
	identity2ValMap.insert(":lat", eventInfo->lat);


	if (!eventInfo->picData.isNull())
	{
		strSqlMid += "pic_data=:pic_data,";
		identity2ValMap.insert(":pic_data", eventInfo->picData);
	}

	if (!eventInfo->desc.isEmpty())
	{
		strSqlMid += "description=:description,";
		identity2ValMap.insert(":description", eventInfo->desc);
	}

	if (strSqlPrev.endsWith(","))
		strSqlPrev.chop(1);

	if (strSqlMid.endsWith(","))
		strSqlMid.chop(1);

	QString strSql = strSqlPrev + strSqlMid + strSqlEnd;
	query.prepare(strSql);
	for (auto iter = identity2ValMap.begin(); iter != identity2ValMap.end(); iter++)
	{
		query.bindValue(iter.key(), iter.value());
	}
	bool success = query.exec();
	QString text = query.lastError().text();
	return success;
}

bool AgriSoilAnalysisDBHandler::DeleteSaltFieldEventDataset(int id)
{
	if (id <= 0)
	{
		return false;
	}

	QString strSql = "DELETE FROM " + QString(AGRI_SALT_FIELD_EVENT_INFO_TABLE_NAME) + " where id = ";
	strSql += "'" +QString::number(id)  + "'";
	QSqlQuery query;

	return query.exec(strSql);
}
