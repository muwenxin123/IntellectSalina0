#include "IAgroTaskImpl.h"
#include "LY_AgroTaskManageService.h"
#include "QSqlQuery"
#include "QVariant"
#include "LyMessageBox.h"

#define PI			3.1415926
#define NPrecision	10000
using namespace qnzkna::AgroTask;


IAgroTaskImpl::IAgroTaskImpl(void)
{
	loadDataFromDB_CAgroRefDataInfoVec();
	loadDataFromDB_CFarmlandAttributeInfoVec();
	loadDataFromDB_CFarmEvevtInfoVec();

	initSeedStateVec();
}

IAgroTaskImpl::~IAgroTaskImpl(void)
{

}

void qnzkna::AgroTask::IAgroTaskImpl::DrawDynamic(QPainter *painter)
{
	
}

void qnzkna::AgroTask::IAgroTaskImpl::loadDataFromDB_CAgroRefDataInfoVec()
{
	m_AgroRefDataInfoVec.clear();

	QSqlQuery query1;
	QString strSQL = "select BH,ZZBH,ZZZT,ZZZZT,ZZZZZT,SJJD,CZZZT,CZZZZT,CZZZZZT from `ZHNY_CKSJ`";
	bool success = query1.exec(strSQL);
	if (success)
	{
		while (query1.next())
		{
			AgroRefDataInfo info;
			info.nNo = query1.value(0).toInt();
			strncpy(info.cSeedNo, query1.value(1).toString().toLocal8Bit(), MAX_CHAR_LEN);
			info.nPlanState = query1.value(2).toInt();
			info.nPlanSubState = query1.value(3).toInt();
			info.nPlanSsubState = query1.value(4).toInt();
			info.nDataTime = query1.value(5).toInt();
			strncpy(info.cPlanState, query1.value(6).toString().toLocal8Bit(), MAX_CHAR_LEN);
			strncpy(info.cPlanSubState, query1.value(7).toString().toLocal8Bit(), MAX_CHAR_LEN);
			strncpy(info.cPlanSsubState, query1.value(8).toString().toLocal8Bit(), MAX_CHAR_LEN);

			m_AgroRefDataInfoVec.push_back(info);
		}
	}

	for (auto &data_info : m_AgroRefDataInfoVec) {
		QSqlQuery query2;
		strSQL = "select BH,CKSJBH,LX,SJCS,CFTJ,CSMC,CSNRMAX,CSNRMIN,CLBF from `ZHNY_NYCS` ";
		strSQL += "where CKSJBH=" + QString::number(data_info.nNo);
		success = query2.exec(strSQL);
		if (success)
		{
			while (query2.next())
			{
				AgroRefDataSubInfo info;
				info.nDataType = query2.value(2).toInt();
				info.nTimeParam = query2.value(3).toInt();
				strncpy(info.cTriggerCondition, query2.value(4).toString().toLocal8Bit(), MAX_CHAR_LEN);
				strncpy(info.cParamName, query2.value(5).toString().toLocal8Bit(), MAX_CHAR_LEN);
				info.dMaxParam = query2.value(6).toDouble();
				info.dMinParam = query2.value(7).toDouble();
				strncpy(info.cHandMethod, query2.value(8).toString().toLocal8Bit(), MAX_CHAR_LEN);

				data_info.vCAgroRefDataSubInfoVec.push_back(info);
			}
		}
	}

}

void qnzkna::AgroTask::IAgroTaskImpl::loadDataFromDB_CFarmlandAttributeInfoVec()
{
	m_FarmlandAttributeInfoVec.clear();

	QSqlQuery query3;
	QString strSQL = "select NTBH,NTMC,ZZBH,ZZZT,ZZZZT,BZSJ,SB,BZ from `ZHNY_NTSXB`";
	bool success = query3.exec(strSQL);
	if (success)
	{
		while (query3.next())
		{
			FarmlandAttributeInfo info;
			info.nNo = query3.value(0).toInt();
			strncpy(info.cName, query3.value(1).toString().toLocal8Bit(), MAX_CHAR_LEN);
			strncpy(info.cSeedNo, query3.value(2).toString().toLocal8Bit(), MAX_CHAR_LEN);
			info.nPlanState = query3.value(3).toInt();
			info.nPlanSubState = query3.value(4).toInt();
			info.llPlanDate = query3.value(5).toDouble();
			strncpy(info.cEquipment, query3.value(6).toString().toLocal8Bit(), MAX_CHAR_LEN);
			strncpy(info.remarks, query3.value(7).toString().toLocal8Bit(), MAX_CHAR_LEN);

			m_FarmlandAttributeInfoVec.push_back(info);
		}
	}
}

void qnzkna::AgroTask::IAgroTaskImpl::loadDataFromDB_CFarmEvevtInfoVec()
{
	m_FarmEvevtInfoVec.clear();

	QSqlQuery query4;
	QString strSQL = "select NTBH,SJ,NR,BZ from `ZHNY_NTSJB`";
	bool success = query4.exec(strSQL);
	if (success)
	{
		while (query4.next())
		{
			FarmEvevtInfo info;
			info.nNo = query4.value(0).toInt();
			info.nOperDate = query4.value(1).toDouble();
			strncpy(info.cOperContent, query4.value(2).toString().toLocal8Bit(), MAX_CHAR_LEN);
			strncpy(info.cRemarks, query4.value(3).toString().toLocal8Bit(), MAX_CHAR_LEN);

			m_FarmEvevtInfoVec.push_back(info);
		}
	}
}

CAgroRefDataInfoVec qnzkna::AgroTask::IAgroTaskImpl::getAgroRefDataInfoVec()
{
	return m_AgroRefDataInfoVec;
}

CAgroRefDataInfoVec qnzkna::AgroTask::IAgroTaskImpl::getAgroRefDataInfo(const QString &strSeedNo)
{
	CAgroRefDataInfoVec resVec;
	for (const auto &info : m_AgroRefDataInfoVec) 
	{
		if (QString::fromLocal8Bit(info.cSeedNo) == strSeedNo) 
		{
			resVec.push_back(info);
		}
	}
	return resVec;
}

bool qnzkna::AgroTask::IAgroTaskImpl::addFarmlandAttributeInfo(FarmlandAttributeInfo info)
{
	//写入数据库
	QString strSql("INSERT INTO ZHNY_NTSXB (`NTBH`,`NTMC`,`ZZBH`,`ZZZT`,`ZZZZT`,`BZSJ`,`SB`,`BZ`) VALUES (");
	strSql += "'" + QString::number(info.nNo) + "'" + ",";
	strSql += "'" + QString::fromLocal8Bit(info.cName) + "'" + ",";
	strSql += "'" + QString::fromLocal8Bit(info.cSeedNo) + "'" + ",";
	strSql += "'" + QString::number(info.nPlanState) + "'" + ",";
	strSql += "'" + QString::number(info.nPlanSubState) + "'" + ",";
	strSql += "'" + QString::number(info.llPlanDate) + "'" + ",";
	strSql += "'" + QString::fromLocal8Bit(info.cEquipment) + "'";
	strSql += "'" + QString::fromLocal8Bit(info.remarks) + "'";
	strSql += ")";

	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("创建失败！"));
	}
	else {
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("创建成功！"));
		m_FarmlandAttributeInfoVec.push_back(info);
	}

	return success;
}

bool qnzkna::AgroTask::IAgroTaskImpl::deleteFarmlandAttributeInfo(int no)
{
	QString strSql = "DELETE FROM ZHNY_NTSXB WHERE NTBH=" + QString::number(no);
	QSqlQuery queryUser;
	bool success = queryUser.exec(strSql);
	if (!success) {
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("删除失败！"));
		return false;
	}

	LyMessageBox::warning(nullptr, tr2("提示"), tr2("删除成功！"));

	for (int i = 0; i < m_FarmlandAttributeInfoVec.size(); i++) {
		if (m_FarmlandAttributeInfoVec[i].nNo == no) {
			m_FarmlandAttributeInfoVec.erase(m_FarmlandAttributeInfoVec.begin() + i);
			break;
		}
	}

	return true;
}

CFarmlandAttributeInfoVec qnzkna::AgroTask::IAgroTaskImpl::getFarmlandAttributeInfoVec()
{
	return m_FarmlandAttributeInfoVec;
}

FarmlandAttributeInfo qnzkna::AgroTask::IAgroTaskImpl::getFarmlandAttributeInfo(int no)
{
	for (const auto &info : m_FarmlandAttributeInfoVec) {
		if (info.nNo == no) {
			return info;
		}
	}
	FarmlandAttributeInfo infoo;
	return infoo;
}

CFarmEvevtInfoVec qnzkna::AgroTask::IAgroTaskImpl::getFarmEvevtInfoVec()
{
	return m_FarmEvevtInfoVec;
}

FarmEvevtInfo qnzkna::AgroTask::IAgroTaskImpl::getFarmEvevtInfo(int no)
{
	for (const auto &info : m_FarmEvevtInfoVec) {
		if (info.nNo == no) {
			return info;
		}
	}
	FarmEvevtInfo infoo;
	return infoo;
}

QString qnzkna::AgroTask::IAgroTaskImpl::getSeedStateRefer(int seedState, int seedSubState /*= -1*/)
{
	int vSize = seedStateVec.size();
	if (seedState < 0 || seedState >= vSize) {
		return "";
	}

	if (seedSubState == -1) {
		return seedStateVec[seedState];
	}
	else {
		auto subVec = seedSubStateVec[seedState];
		int ssize = subVec.size();
		if (seedSubState >= 0 && seedSubState < ssize) {
			return subVec[seedSubState];
		}
	}

	return "";
}

void qnzkna::AgroTask::IAgroTaskImpl::initSeedStateVec()
{
	seedStateVec.push_back(tr2("准备任务阶段"));
	seedStateVec.push_back(tr2("耕地"));
	seedStateVec.push_back(tr2("播种"));
	seedStateVec.push_back(tr2("管理"));
	seedStateVec.push_back(tr2("收获"));

	seedSubStateVec.resize(5);
	seedSubStateVec[1].push_back(tr2("土地设置"));
	seedSubStateVec[1].push_back(tr2("耕地设置"));
	seedSubStateVec[3].push_back(tr2("苗期"));
	seedSubStateVec[3].push_back(tr2("穗期"));
	seedSubStateVec[3].push_back(tr2("花粒期"));

}

