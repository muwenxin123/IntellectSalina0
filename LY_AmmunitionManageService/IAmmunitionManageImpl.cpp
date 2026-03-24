#include "IAmmunitionManageImpl.h"
#include "QSettings"
#include "QCString.h"
#include "QVector"
#include "QSqlRecord"

qnzkna::AmmunitionManage::IAmmunitionManageImpl::IAmmunitionManageImpl(void)
{
	InitDataBase();
	LoadDataBase();
	LoadAmmunitionDB();
	LoadAmmoConntionMarkDB();

	LoadAmmoOnPlatDB();

}

qnzkna::AmmunitionManage::IAmmunitionManageImpl::~IAmmunitionManageImpl(void)
{

}

void qnzkna::AmmunitionManage::IAmmunitionManageImpl::InitDataBase()
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
	bool bresult = db.open();
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadDataBase()
{
	QSqlQuery query;
    bool success = query.exec("SELECT `DYID`, `DYLX`, `DYXH`, `DYMC`, `DYJC`, `SSWQID` FROM `RWSJ_DYZB`");
	if (success)
	{
		m_AmmunitionVec.clear();

		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());

			CNeubulaAmmunition stAmmunition;
			stAmmunition.SetID(str1.toInt());
			stAmmunition.SetType(str2.toInt());
			stAmmunition.SetMode(str3.toInt());
			stAmmunition.SetName(str4.toStdString());
			stAmmunition.SetAbbreviation(str5.toStdString());
			stAmmunition.SetWeaponID(str6.toInt());

			m_AmmunitionVec.push_back(stAmmunition);
		}
	}
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::EditDBData(const CNeubulaAmmunition& data)
{

	if (!IsIDExisted(data.GetID()))
	{
		return false;
	}

	QString strSql("UPDATE RWSJ_DYZB set ");
	strSql += "`DYLX` = " + QString::number(data.GetType()) + ",";
	strSql += "`DYXH` = " + QString::number(data.GetMode()) + ",";
	strSql += "`DYMC` = '" + QString::fromStdString(data.GetName()) + "',";
	strSql += "`DYJC` = '" + QString::fromStdString(data.GetAbbreviation()) + "',";
	strSql += "`SSWQID` = " + QString::number(data.GetWeaponID()) + " ";
	strSql += "where `DYID` = " + QString::number(data.GetID());
	QSqlQuery query;
	bool success = query.exec(strSql);
    return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddDBData(const CNeubulaAmmunition& data)
{

	if (IsIDExisted(data.GetID()))
	{
		return false;
	}

	QString strSql("INSERT INTO RWSJ_DYZB (`DYID`, `DYLX`, `DYXH`, `DYMC`, `DYJC`, `SSWQID`) VALUES (");
	strSql +=		QString::number(data.GetID()) + ",";
	strSql +=		QString::number(data.GetType()) + ",";
	strSql +=		QString::number(data.GetMode()) + ",";
	strSql += "'" + QString::fromStdString(data.GetName()) + "',";
	strSql += "'" + QString::fromStdString(data.GetAbbreviation()) + "',";
	strSql +=		QString::number(data.GetWeaponID());
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
    return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveDBData(int nID)
{

	if (!IsIDExisted(nID))
	{
		return false;
	}

	QString strSql("DELETE FROM RWSJ_DYZB where DYID = ");
	strSql += QString::number(nID);
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::IsIDExisted(int nID)
{
    QString strSql("SELECT  * FROM `RWSJ_DYZB` WHERE `DYID` = ");
	strSql += QString::number(nID);
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetMaxID()
{
	QSqlQuery query;
	int nNoMax = 0;
	bool success = query.exec("SELECT MAX(`DYID`) from RWSJ_DYZB");
	if (success)
	{
		while (query.next())
		{
			QString str01 = tr2(query.value(0).toString().toLatin1().data());
			nNoMax = str01.toInt();
		}
	}

	nNoMax = nNoMax + 1;
	return nNoMax;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmunitionDB()
{
	QSqlQuery query;
    bool success = query.exec("SELECT `DYID`, `DYLX`, `DYXH`, `SFGZ`, `XNDY`, `BZ` FROM `ZBGL_DY`");
	if (success)
	{
		m_AmmoVec.clear();
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

			CNeubulaAmmunitionInfo stAmmunition;
			std::string strID = tr3(str1);
			stAmmunition.SetID(strID);
			std::string strType = tr3(str2);
			stAmmunition.SetAmmoType(strType);
			std::string strMode = tr3(str3);
			stAmmunition.SetAmmoMode(strMode);
			if (str4.toStdString() == "1")
			{
				stAmmunition.SetisLoaded(true);
			}
			else
			{
				stAmmunition.SetisLoaded(false);
			}
			if (str5.toStdString() == "1")
			{
				stAmmunition.SetisVirtual(true);
			}
			else
			{
				stAmmunition.SetisVirtual(false);
			}
			stAmmunition.SetRemark(str6.toStdString());

			m_AmmoVec.push_back(stAmmunition);
		}
	}
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::EditAmmunitionDB(const CNeubulaAmmunitionInfo& data)
{

	if (!IsAmmunitionDBExisted(data.GetID()))
	{
		return false;
	}

	QString strSql("UPDATE ZBGL_DY set ");
	strSql += "`DYLX` = '";
	strSql += tr2(data.GetAmmoType().c_str()) + "',";
	strSql += "`DYXH` = '" + tr2(data.GetAmmoMode().c_str()) + "',";
	strSql += "`SFGZ` = '" + QString::number(data.GetisLoaded() == false? 0:1) + "',";
	strSql += "`XNDY` = '" + QString::number(data.GetisVirtual() == false ? 0 : 1) + "',";
	strSql += "`BZ` = '" + tr2(data.GetRemark().c_str()) + "'";
	strSql += "where `DYID` = '" + tr2(data.GetID().c_str()) + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmunitionDB(const CNeubulaAmmunitionInfo & data)
{
	QString strSql("INSERT INTO ZBGL_DY (`DYID`, `DYLX`, `DYXH`, `SFGZ`, `XNDY`, `BZ`) VALUES (");
	strSql += "'" + tr2(data.GetID().c_str()) + "',";
	strSql += "'" + tr2(data.GetAmmoType().c_str()) + "',";
	strSql += "'" + tr2(data.GetAmmoMode().c_str()) + "',";
	strSql += "'" + QString::number(data.GetisLoaded() == false ? 0 : 1) + "',";
	strSql += "'" + QString::number(data.GetisVirtual() == false ? 0 : 1) + "',";
	strSql += "'" + tr2(data.GetRemark().c_str()) + "'";
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmunitionDB(std::string nID)
{

	if (!IsAmmunitionDBExisted(nID))
	{
		return false;
	}

	QString strSql("DELETE FROM ZBGL_DY where DYID = ");
	strSql += "'" + QString::fromStdString(nID) + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::IsAmmunitionDBExisted(std::string nID)
{
    QString strSql("SELECT  * FROM `ZBGL_DY` WHERE `DYID` = ");
	strSql += "'" + QString::fromStdString(nID) + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmoOnPlatDB()
{
	QSqlQuery query;
    bool success = query.exec("SELECT `PTBH`,`DYXH`, `DYBH`, `GZFS`, `RWBH`, `GLBH` FROM `RWSJ_DJPP`");
	if (success)
	{
		m_AmmoConntion.clear();
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());

			CNeubulaConntionAmmo stAmmunition;

			stAmmunition.SetPlatID(str1.toStdString());
			stAmmunition.SetType(str2.toStdString());
			stAmmunition.SetAmmoID(str3.toStdString());
			stAmmunition.SetLoadMethod(str4.toInt());
			stAmmunition.SetTaskID(str5.toStdString());
			stAmmunition.SetRelationID(str6.toInt());
			m_AmmoConntion.push_back(stAmmunition);
		}
	}
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::SaveAmmoOnPlatDB(CNeubulaConntionAmmoVec data)
{
	if (data.empty())
		return false;
	std::string strTaskID = data[0].GetTaskID().c_str();
	QString str1("DELETE FROM RWSJ_DJPP where `RWBH` = ");
	str1 += "'";
	str1 += strTaskID.c_str();
	str1 += "'";
	QSqlQuery query;
	bool succ = true;
	if (!query.exec(str1))
		succ = false;
	for (auto info : data)
	{
		QString strSql("INSERT INTO RWSJ_DJPP (`PTBH`,`DYXH`, `DYBH`, `GZFS`, `RWBH`, `GLBH`) VALUES (");
		strSql += "'" + QString::fromStdString(info.GetPlatID()) + "',";
		strSql += "'" + QString::fromStdString(info.GetType()) + "',";
		strSql += "'" + QString::fromStdString(info.GetAmmoID()) + "',";
		strSql += "'" + QString::number(info.GetLoadMethod()) + "',";
		strSql += "'" + QString::fromStdString(info.GetTaskID()) + "',";
		strSql += "'" + QString::number(info.GetRelationID()) + "'";
		strSql += ")";
		QSqlQuery query;
		bool success = query.exec(strSql);
		if (!success)
			succ = false;
	}
	return succ;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmoOnPlatDB(std::string nTaskID)
{
	QString str1("DELETE FROM RWSJ_DJPP where `RWBH` = ");
	str1 += "'";
	str1 += nTaskID.c_str();
	str1 += "'";
	QSqlQuery query;
	bool succ = true;
	if (!query.exec(str1))
		succ = false;

	return succ;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmoConntionMarkDB()
{
	QSqlQuery query;
    bool success = query.exec("SELECT `MBBH`, `DYBH`, `RWBH` FROM `RWSJ_DMPP`");
	if (success)
	{
		m_AmmoConntionMarkVec.clear();
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());

			CNeubulaAmmoConntionMark stAmmunitionMark;

			stAmmunitionMark.SetMarkID(str1.toStdString());
			stAmmunitionMark.SetAmmoMode(str2.toStdString());
			stAmmunitionMark.SetTaskID(str3.toStdString());
			m_AmmoConntionMarkVec.push_back(stAmmunitionMark);
		}
	}
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmoConntionMarkDB(const CNeubulaAmmoConntionMark& data)
{
	QString strSql("INSERT INTO RWSJ_DMPP (`MBBH`, `DYBH`, `RWBH`) VALUES (");
	strSql += "'" + QString::fromStdString(data.GetMarkID().c_str()) + "',";
	strSql += "'" + QString::fromStdString(data.GetAmmoMode().c_str()) + "',";
	strSql += "'" + QString::fromStdString(data.GetTaskID().c_str()) + "'";
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmoConntionMarkDB(std::string sTaskID)
{

	if (!IsAmmoConntionMarkExisted(sTaskID))
	{
		return false;
	}

	QString strSql("DELETE FROM RWSJ_DMPP where RWBH = ");
	strSql += "'" + QString::fromStdString(sTaskID) + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::IsAmmoConntionMarkExisted(std::string sTaskID)
{
    QString strSql("SELECT  * FROM `RWSJ_DMPP` WHERE `RWBH` = ");
	strSql +="'"+ QString::fromStdString(sTaskID) + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

CNeubulaConntionAmmoVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAmmoOnPlatVec()
{
	QMutexLocker locker(&rAllMutex);

	return m_AmmoConntion;
}

CNeubulaConntionAmmoVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAmmunitionConnByTaskID(const std::string nTaskID)
{
	QMutexLocker locker(&rAllMutex);

	CNeubulaConntionAmmoVec newVec;
	for (auto data : m_AmmoConntion)
	{
		if (data.GetTaskID() == nTaskID)
		{
			newVec.push_back(data);
		}
	}
	return newVec;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmunitionConnDB(CNeubulaConntionAmmoVec data)
{
	QMutexLocker locker(&rAllMutex);
	bool succ = SaveAmmoOnPlatDB(data);
	LoadAmmoOnPlatDB();
	return succ;
}
void qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmoPlatDB()
{
	QMutexLocker locker(&rAllMutex);
	LoadAmmoOnPlatDB();
}

void qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmoMarkDB()
{
	QMutexLocker locker(&rAmmoMarkMutex);
	LoadAmmoConntionMarkDB();
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmoConntionMark(const CNeubulaAmmoConntionMark& data)
{
	QMutexLocker locker(&rAmmoMarkMutex);

	if (AddAmmoConntionMarkDB(data) == false)
	{
		return false;
	}

	m_AmmoConntionMarkVec.push_back(data);
	return true;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmoConntionMark(std::string sTaskID)
{
	QMutexLocker locker(&rAmmoMarkMutex);

	if (IsAmmoConntionMarkExisted(sTaskID) == false)
	{
		return false;
	}

	return RemoveAmmoConntionMarkDB(sTaskID);
}

CNeubulaAmmoConntionMarkVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAllAmmoConntionMarkVec()
{
	QMutexLocker locker(&rAmmoMarkMutex);

	return m_AmmoConntionMarkVec;
}

CNeubulaAmmoConntionMarkVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetTaskIDByInfo(std::string sTaskID)
{
	QMutexLocker locker(&rAmmoMarkMutex);

	CNeubulaAmmoConntionMarkVec stAmmoConntionMarkVec;
	for (auto data : m_AmmoConntionMarkVec)
	{
		if (data.GetTaskID() == sTaskID)
		{
			stAmmoConntionMarkVec.push_back(data);
		}
	}
	return stAmmoConntionMarkVec;
}

std::vector<std::string> qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetMarkIDByAmmoMode(const CNeubulaAmmoConntionMarkVec data, const std::string sMarkID)
{
	QMutexLocker locker(&rAmmoMarkMutex);

	std::vector<std::string> newVec;
	for (auto temp:data)
	{
		if (temp.GetMarkID()==sMarkID)
		{
			newVec.push_back(temp.GetAmmoMode());
		}
	}
	return newVec;
}

void qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadDataFromDB()
{
	QMutexLocker locker(&sMutex);
	LoadDataBase();
}

CNeubulaAmmunitionVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAllAmmunitionVec()
{
	QMutexLocker locker(&sMutex);

	return m_AmmunitionVec;
}

CNeubulaAmmunition qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAmmunition(const int nID)
{
	QMutexLocker locker(&sMutex);

	CNeubulaAmmunition stAmmunition;
	for (auto itr : m_AmmunitionVec)
	{
		if (itr.GetID() == nID)
		{
			stAmmunition = itr;
			return stAmmunition;
		}
	}
	return stAmmunition;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmunition(const CNeubulaAmmunition data)
{
	QMutexLocker locker(&sMutex);

	if (IsAmmunitionIDExisted(data.GetID()) == true)
	{
		return false;
	}

	if (AddDBData(data) == false)
	{
		return false;
	}

	m_AmmunitionVec.push_back(data);
    return true;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::EditAmmunition(const CNeubulaAmmunition data)
{
	QMutexLocker locker(&sMutex);

	if (IsAmmunitionIDExisted(data.GetID()) == false)
	{
		return false;
	}

	if (EditDBData(data) == false)
	{
		return false;
	}

	for (auto& itr:m_AmmunitionVec)
	{
		if (itr.GetID() == data.GetID())
		{
			itr = data;
			return true;
		}
	}
	return false;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmunition(const int nID)
{
	QMutexLocker locker(&sMutex);

	if (IsAmmunitionIDExisted(nID) == false)
	{
		return false;
	}

	return RemoveDBData(nID);
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::IsAmmunitionIDExisted(int nID)
{
	for (auto itr : m_AmmunitionVec)
	{
		if (itr.GetID() == nID)
		{
			return true;
		}
	}
	return false;
}

int qnzkna::AmmunitionManage::IAmmunitionManageImpl::AutoAmmunitionID()
{
	return GetMaxID();
}

void qnzkna::AmmunitionManage::IAmmunitionManageImpl::LoadAmmoDB()
{
	QMutexLocker locker(&rMutex);
	LoadAmmunitionDB();
}

CNeubulaAmmunitionInfoVec qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAllAmmoVec()
{
	QMutexLocker locker(&rMutex);

	return m_AmmoVec;
}

CNeubulaAmmunitionInfo qnzkna::AmmunitionManage::IAmmunitionManageImpl::GetAmmoByID(const std::string nID)
{
	QMutexLocker locker(&rMutex);

	CNeubulaAmmunitionInfo stAmmunition;
	for (auto itr : m_AmmoVec)
	{
		if (itr.GetID() == nID)
		{
			stAmmunition = itr;
			return stAmmunition;
		}
	}
	return stAmmunition;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::AddAmmo(const CNeubulaAmmunitionInfo data)
{
	QMutexLocker locker(&rMutex);

	if (IsAmmoIDExisted(data.GetID()) == true)
	{
		return false;
	}

	if (AddAmmunitionDB(data) == false)
	{
		return false;
	}

	m_AmmoVec.push_back(data);
	return true;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::EditAmmo(const CNeubulaAmmunitionInfo data)
{
	QMutexLocker locker(&rMutex);

	if (IsAmmoIDExisted(data.GetID()) == false)
	{
		return false;
	}

	if (EditAmmunitionDB(data) == false)
	{
		return false;
	}

	for (auto& itr : m_AmmoVec)
	{
		if (itr.GetID() == data.GetID())
		{
			itr = data;
			return true;
		}
	}
	return false;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAmmo(const std::string nID)
{
	QMutexLocker locker(&rMutex);

	if (IsAmmoIDExisted(nID) == false)
	{
		return false;
	}

	return RemoveAmmunitionDB(nID);
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::RemoveAllAmmo()
{
	QMutexLocker locker(&rAllMutex);
	QString strSql("DELETE FROM ZBGL_DY");
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success)
		m_AmmoVec.clear();
	return success;
}

bool qnzkna::AmmunitionManage::IAmmunitionManageImpl::IsAmmoIDExisted(std::string nID)
{
	for (auto itr : m_AmmoVec)
	{
		if (itr.GetID() == nID)
		{
			return true;
		}
	}
	return false;
}
