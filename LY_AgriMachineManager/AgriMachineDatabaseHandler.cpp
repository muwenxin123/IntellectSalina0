#include <QSettings>
#include <QCString.h>
#include <QVector>
#include <QDebug>
#include <QSqlError>
#include <QSharedPointer>

#include "AgriMachineDatabaseHandler.h"

AgriMachineDatabaseHandler::AgriMachineDatabaseHandler(QObject *parent /*= nullptr*/)
	:isOpen(false)
{
	InitDataBase();
	LoadDirDataBase();
}

AgriMachineDatabaseHandler::~AgriMachineDatabaseHandler()
{

}

void AgriMachineDatabaseHandler::InitDataBase()
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

void AgriMachineDatabaseHandler::LoadDirDataBase()
{
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	QString strSql = "SELECT `id`, `code`, `name`, `english_name`, `parent_code`, `description`, `comment`";
	QString strGeneralSql = strSql + " FROM `agri_dir_machine`";

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		m_AgriDirMachineInfoPtrVec.clear();

		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();

			AgriDirMachineInfo *pInfo = new AgriDirMachineInfo;
			pInfo->nID = str1.toInt();
			pInfo->nCode = str2.toInt();
			pInfo->sName = str3;
			pInfo->sEnglishName = str4;
			pInfo->nParentCode = str5.toInt();
			pInfo->sDescription = str6;
			pInfo->sComment = str7;

			m_AgriDirMachineInfoPtrVec.push_back(pInfo);
		}
	}
}

AgriMachineBasicInfoPtr AgriMachineDatabaseHandler::MatchComponents(QString componentId)
{
	auto basicInfo = FetchMachineInfo<AgriMachineBasicInfo>(componentId.toStdString());
	return std::make_shared<AgriMachineBasicInfo>(basicInfo);

	//// 视图查询组件
	//QSqlQuery query;
	//query.prepare("SELECT * FROM combined_view WHERE guid = :guid");
	//query.bindValue(":guid", componentId);

	//query.first();
	//do {
	//	int leafNodeId = query.value("item").toInt();
	//	QString machineName = query.value("name").toString();

	//	AgriMachineBasicInfoPtr machineBasicPtr = std::make_shared<AgriMachineBasicInfo>();

	//} while (query.next());
}

QSharedPointer<QSqlQuery> AgriMachineDatabaseHandler::getQueryOperator()
{
	return QSharedPointer<QSqlQuery>(new QSqlQuery(db));
}

AgriDirMachineInfoPtrVec AgriMachineDatabaseHandler::GetDirMachineInfoList() const
{
	return m_AgriDirMachineInfoPtrVec;
}

AgriMachineInfoPtrVec AgriMachineDatabaseHandler::GetMachineInfoList()
{
	// 查询农机列表
	QSqlQuery query;

	QString strSql = "SELECT `id`, `guid`, `name`, `count`, `description`, `power_machine`, `work_machine`, `assembled`, `create_time`, `modify_time`";
	QString strGeneralSql = strSql + " FROM `agri_machine`";

	if (!query.exec(strGeneralSql)) {
		qDebug() << "Failed to query agri_machine:" << query.lastError().text();
	}

	AgriMachineInfoPtrVec machinePtrVec;

	while (query.next())
	{
		AgriMachineInfoPtr machinePtr = std::make_shared<AgriMachineInfo>();

		machinePtr->id = query.value("id").toInt();
		machinePtr->guid = query.value("guid").toString().toStdString();
		machinePtr->name = query.value("name").toString().toStdString();
		machinePtr->count = query.value("count").toInt();
		machinePtr->description = query.value("description").toString().toStdString();
		machinePtr->power_machine = MatchComponents(query.value("power_machine").toString());
		machinePtr->work_machine = MatchComponents(query.value("work_machine").toString());
		machinePtr->assembled = query.value("assembled").toBool();
		machinePtr->create_time = query.value("create_time").toString().toStdString();
		machinePtr->modify_time = query.value("modify_time").toString().toStdString();
	 
		machinePtrVec.emplace_back(machinePtr);
	};

	return machinePtrVec;
}

QByteArray AgriMachineDatabaseHandler::GetAgriMachineImage(const std::string& strGuid)
{
	QSqlQuery query;
	query.prepare("SELECT image FROM " + QString(AGRI_MACHINE_IMAGE_TABLE_NAME) + " WHERE guid = :guid");
	query.bindValue(":guid", QString::fromStdString(strGuid));

	if (!query.exec()) {
		// TODO: 同一查询失败图片
		return QByteArray();
	}

	QByteArray imageData;
	if (query.next()) {
		imageData = query.value(0).toByteArray(); // 获取图片数据
	}
	return imageData;
}

AgriEnumInfoVec AgriMachineDatabaseHandler::FetchMachineEnumInfoList(const std::string& strKey)
{
	QString strMachEnumTablename = AgriMachineManagerUtil::getMachineEnumTablenameByKey(QString::fromStdString(strKey));

	AgriEnumInfoVec result;
	QSqlQuery query(db);

	query.prepare("SELECT * FROM " + strMachEnumTablename);

	if (!query.exec())
	{
		return result;
	}

	while (query.next())
	{
		QString str1 = query.value(0).toString();
		QString str2 = query.value(1).toString();
		QString str3 = query.value(2).toString();

		AgriEnumInfo pInfo;
		pInfo.id = str1.toInt();
		pInfo.description = str2.toStdString();
		pInfo.comment = str3.toStdString();

		result.emplace_back(pInfo);
	}

	return result;
}

