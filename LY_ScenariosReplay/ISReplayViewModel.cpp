#include "ISReplayViewModel.h"
#include "QCString.h"
#include "lY_ScenariosReplay.h"
#include "IScenariosReplayThread.h"
#include <QTimer>
#include "QSettings"
#include<QDebug>
#include <QString>
#include <QHeaderView>
#include <QFileDialog>
#include "FileTransProtocol.h"
#include "QtConcurrent/QtConcurrent"
#include <QUdpSocket>
#include "netheader.h"
#include <QByteArray>
#include <QDateTime>
#include <QMessageBox>
#include "Protocol/XygsTelegraph/TelegrpahHead.h"

ISReplayViewModel::ISReplayViewModel(QObject *parent)
	: QObject(parent)
{

	m_ReplayDataInfoVec = LY_ScenariosReplay::Get().m_pIScenariosReplayThread->m_ReplayDataInfoVec;
	m_SeatInfoMap.insert(std::make_pair(SEAT_COMMAND, SEAT_COMMAND_NAME));
	m_SeatInfoMap.insert(std::make_pair(SEAT_BATTLE, SEAT_BATTLE_NAME));
	m_SeatInfoMap.insert(std::make_pair(SEAT_INTELLIGENCE, SEAT_INTELLIGENCE_NAME));
	m_SeatInfoMap.insert(std::make_pair(SEAT_COORDINATION, SEAT_COORDINATION_NAME));

}

ISReplayViewModel::~ISReplayViewModel()
{
}

bool ISReplayViewModel::InitDataBase()
{
	QSettings iniFile("../data/conf/ly.ini", QSettings::IniFormat);
	iniFile.beginGroup("Database");
	QString strDbName = iniFile.value("LogDbName", "Agri_LOG").toString();
	QString strDbIP = iniFile.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = iniFile.value("User", "root").toString();
	QString strDbPass = iniFile.value("Password", "123456").toString();
	QString strDbDriver = iniFile.value("Driver", "QMYSQL").toString();

	log_db = QSqlDatabase::addDatabase(strDbDriver, QString("LogDbName"));
	log_db.setHostName(strDbIP);
	log_db.setPort(3306);
	log_db.setDatabaseName(strDbName);
	log_db.setUserName(strDbUser);
	log_db.setPassword(strDbPass);
	bool bresult = log_db.open();
	return bresult;
}

bool ISReplayViewModel::LoadReplayDataBase(const QString &dbName)
{
	QMutexLocker locker(&sMutex);
	m_ReplayDataInfoVec.clear();
	filterVec.clear();
	m_tempVec.clear();
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->m_ReplayDataVec.clear();
	NodeAarray.clear();

    QDir dir("D:\\20240107");
	QStringList  filter;
	filter << "*.264";
	dir.setNameFilters(filter);
	QFileInfoList fileInfoList = dir.entryInfoList(filter);

	vector<QString> tempVec;
	for (int i = 0; i < fileInfoList.count(); i++)
	{
		QFileInfo fileInfo = fileInfoList.at(i);
		QString filePath;
		filePath.append(fileInfo.path());
		filePath += "\\" + fileInfo.fileName();

		QString fileNname = fileInfo.fileName();
		QStringList fileNameAarray = fileNname.split("_");
		if (fileNameAarray.size() > 0)
		{
			int fileTimeHour = fileNameAarray.at(0).mid(0, 2).toInt();
			int fileTimeMin = fileNameAarray.at(0).mid(2, 2).toInt();
			int fileTimeSec = fileNameAarray.at(0).mid(4, 2).toInt();
			int fileTime = fileTimeHour * 3600 + fileTimeMin * 60 + fileTimeSec;
			QStringList list1 = fileNameAarray.at(1).split(".");
			if (list1.size() > 0)
			{

				QString b = list1.at(0);
				int allNumber;

				if (b.size() > 4)
				{
					allNumber = b.mid(0, 2).toInt() + b.mid(4, 1).toInt();
				}
				else {
					allNumber = b.mid(0, 1).toInt() + b.mid(3, 1).toInt();
				}

				if (b.contains("8000", Qt::CaseInsensitive))
				{
					LogDataInfo videoInfo;
					videoInfo.recordTime = fileTime * 1000 + allNumber;
					videoInfo.fileName = filePath;
					videoInfo.recvSend = 100;
					videoInfo.ucType = 43;
					videoInfo.subType = 0;
					videoInfo.nodeType = 11;
					videoInfo.nodeID = 7;
					videoInfo.length = 80;
					m_ReplayDataInfoVec.push_back(videoInfo);
				}

				else if (b.contains("8001", Qt::CaseInsensitive))
				{
					LogDataInfo videoInfo;
					videoInfo.recordTime = fileTime * 1000 + allNumber;
					videoInfo.fileName = filePath;
					videoInfo.recvSend = 100;
					videoInfo.ucType = 998;
					videoInfo.subType = 0;
					videoInfo.nodeType = 9;
					videoInfo.nodeID = 3;
					videoInfo.length = 80;
					m_ReplayDataInfoVec.push_back(videoInfo);
				}
			}
		}
	}

	QSqlQuery query(log_db);
	bool success = query.exec("SELECT * FROM " + dbName);
	if (success)
	{
		while (query.next())
		{
			QString strTime = tr2(query.value(0).toString().toLocal8Bit().data());
			QByteArray strData = query.value(8).toByteArray();

			LogDataInfo datainfo;
			datainfo.recordTime = QString::fromLocal8Bit(query.value(0).toString().toLocal8Bit().data()).toInt();
			datainfo.recvSend = QString::fromLocal8Bit(query.value(1).toString().toLocal8Bit().data()).toInt();
			datainfo.ucType = QString::fromLocal8Bit(query.value(2).toString().toLocal8Bit().data()).toInt();
			datainfo.subType = QString::fromLocal8Bit(query.value(3).toString().toLocal8Bit().data()).toInt();
			datainfo.nodeType = QString::fromLocal8Bit(query.value(4).toString().toLocal8Bit().data()).toInt();
			datainfo.nodeID = QString::fromLocal8Bit(query.value(5).toString().toLocal8Bit().data()).toInt();
			datainfo.length = QString::fromLocal8Bit(query.value(6).toString().toLocal8Bit().data()).toInt();
			datainfo.data = query.value(8).toByteArray();

			m_ReplayDataInfoVec.push_back(datainfo);
		}

		if (LoadMessageSendListDB()) {
			LogDataInfo videoInfo;
			videoInfo.recordTime = m_ReplayDataInfoVec.back().recordTime + 1;
			videoInfo.recvSend = 100;
			videoInfo.ucType = 160;
			videoInfo.subType = 0;
			videoInfo.nodeType = 10;
			videoInfo.nodeID = 4;
			videoInfo.length = 80;
			m_ReplayDataInfoVec.push_back(videoInfo);
		}

		m_ReplayVec = m_ReplayDataInfoVec;
		m_tempAllData = m_ReplayDataInfoVec;

	}
	if (!m_ReplayDataInfoVec.empty())
	{
		this->beginTime = m_tempAllData.front().recordTime / 1000;
		this->endTime = m_tempAllData.back().recordTime / 1000;
		update_tableWidget_data(m_ReplayDataInfoVec);
		return true;
	}

	return false;
}

bool ISReplayViewModel::LoadMessageSendListDB()
{
	QSqlQuery query;

	bool success = query.exec("SELECT `No`,`nTime`,`SendTo`,`Type`,`Detail`,`States`,`ChildType1`,`ChildType2`,`MissionID`, `TargetID` FROM `ZKLY`.`messageSendList`");
	if (success)
	{
		sendMessageVec.clear();
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());
			QString str7 = tr2(query.value(6).toString().toLatin1().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());

			SendMessageInfo stInfo;
			stInfo.nNo = str1.toInt();
			stInfo.nTime = str2.toInt();
			stInfo.strSendAddressList = GetEachData(str3);
			stInfo.nType = str4.toInt();
			stInfo.strDetail = str5;
			stInfo.nStates = str6.toInt();
			stInfo.nChildType1 = str7.toInt();
			stInfo.nChildType2 = str8.toInt();
			stInfo.strMissionID = str9;
			stInfo.strTargetIDList = GetEachData(str10);
			sendMessageVec.push_back(stInfo);
		}
	}
	if (!sendMessageVec.empty())
	{
		return true;
	}

	return false;
}
QStringList ISReplayViewModel::GetEachData(const QString& strDate)
{
	QStringList strList;
	QString strTmp = strDate;
	int nIndex = strTmp.indexOf(";");
	while (nIndex >= 0)
	{
		strList.push_back(strTmp.left(nIndex));
		strTmp.remove(0, nIndex + 1);
		nIndex = strTmp.indexOf(";");
	}
	return strList;
}
void ISReplayViewModel::update_tableWidget_data(vector<LogDataInfo> &vec)
{

	int nNo = 0;

	this->tableWidget->clearContents();
	this->tableWidget->setRowCount(vec.size());
	NodeAarray.clear();
	m_tempVec.clear();

	if (vec.size() == 0)
		return;

	vector<LogDataInfo> Node00;
	vector<LogDataInfo> Node01;
	vector<LogDataInfo> Node02;
	vector<LogDataInfo> Node03;
	vector<LogDataInfo> Node04;
	vector<LogDataInfo> Node05;
	vector<LogDataInfo> Node06;
	vector<LogDataInfo> Node07;
	vector<LogDataInfo> Node08;
	vector<LogDataInfo> Node09;
	vector<LogDataInfo> Node10;
	vector<LogDataInfo> Node11;

	for (auto data : vec)
	{
		if (data.nodeType == 0) Node00.push_back(data);
		if (data.nodeType == 1) Node01.push_back(data);
		if (data.nodeType == 2) Node02.push_back(data);
		if (data.nodeType == 3) Node03.push_back(data);
		if (data.nodeType == 4) Node04.push_back(data);
		if (data.nodeType == 5) Node05.push_back(data);
		if (data.nodeType == 6) Node06.push_back(data);
		if (data.nodeType == 7) Node07.push_back(data);
		if (data.nodeType == 8) Node08.push_back(data);
		if (data.nodeType == 9) Node09.push_back(data);
		if (data.nodeType == 10) Node10.push_back(data);
		if (data.nodeType == 11) Node11.push_back(data);
	}
	NodeAarray.push_back(make_pair(0, Node00));
	NodeAarray.push_back(make_pair(1, Node01));
	NodeAarray.push_back(make_pair(2, Node02));
	NodeAarray.push_back(make_pair(3, Node03));
	NodeAarray.push_back(make_pair(4, Node04));
	NodeAarray.push_back(make_pair(5, Node05));
	NodeAarray.push_back(make_pair(6, Node06));
	NodeAarray.push_back(make_pair(7, Node07));
	NodeAarray.push_back(make_pair(8, Node08));
	NodeAarray.push_back(make_pair(9, Node09));
    NodeAarray.push_back(make_pair(10, Node10));
	NodeAarray.push_back(make_pair(11, Node11));

	for (auto itr: NodeAarray)
	{
		if (itr.second.size() > 0)
		{
			setListVec(itr.second);
		}
	}
	for (auto m_pair : m_tempVec)
	{
		auto start = m_pair.second.begin();
		auto end = m_pair.second.rbegin();
		QString strNo, nodeType, nodeName, dataType, startTime;
		strNo = QString::number(nNo + 1);
		nodeType = getNodeWithNodeType(start->nodeType);
		dataType = getDataTypeWithType(start->ucType);
		nodeName = QString::number(start->nodeID);
		int starT = start->recordTime;
		int replayTime = starT / 1000;
		int hours = replayTime / 3600;
		int minutes = (replayTime % 3600) / 60;
		int secs = replayTime % 60;

		startTime = m_currentData + " " + QString::number(hours) + ":" + QString::number(minutes) + ":" + QString::number(secs);

		int endT = end->recordTime;
		int endReplayTime = endT / 1000;
		int endHours = endReplayTime / 3600;
		int endMinutes = (endReplayTime % 3600) / 60;
		int endSecs = endReplayTime % 60;
		QString endTime = m_currentData + " " + QString::number(endHours) + ":" + QString::number(endMinutes) + ":" + QString::number(endSecs);
		int totalNumber = m_pair.second.size();

		bool bShow = m_pair.first.second;

		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(tr(""));
		if (bShow == true)
		{
			item1->setCheckState(Qt::Checked);
		}
		else
		{
			item1->setCheckState(Qt::Unchecked);
		}
		QTableWidgetItem* item4;
		QTableWidgetItem* item5;
		QTableWidgetItem* item6;
		QTableWidgetItem* item7;
		QTableWidgetItem* item2 = new QTableWidgetItem(nodeType);
		QTableWidgetItem* item3 = new QTableWidgetItem(nodeName);
		if (start->nodeID == 4)
		{
			QString endT = GetTime(this->sendMessageVec.front().nTime);
			QString startT = GetTime(this->sendMessageVec.back().nTime);
			item4 = new QTableWidgetItem(tr2("指挥文电"));
			item5 = new QTableWidgetItem(startT);
			item6 = new QTableWidgetItem(endT);
			item7 = new QTableWidgetItem(QString::number(this->sendMessageVec.size()));
		}
		else {
			item4 = new QTableWidgetItem(dataType);
			item5 = new QTableWidgetItem(startTime);
			item6 = new QTableWidgetItem(endTime);
			item7 = new QTableWidgetItem(QString::number(totalNumber));
		}

		this->tableWidget->setItem(nNo, 0, item0);
		this->tableWidget->setItem(nNo, 1, item1);
		this->tableWidget->setItem(nNo, 2, item2);
		this->tableWidget->setItem(nNo, 3, item3);
		this->tableWidget->setItem(nNo, 4, item4);
		this->tableWidget->setItem(nNo, 5, item5);
		this->tableWidget->setItem(nNo, 6, item6);
		this->tableWidget->setItem(nNo, 7, item7);
		nNo++;
	}

}
QString ISReplayViewModel::GetTime(const int& nTime)
{
	return QDateTime::fromSecsSinceEpoch(nTime).toString("yyyyMMdd HH:mm:ss");
}

void  ISReplayViewModel::setListVec(vector<LogDataInfo> vec)
{
	for (auto data : vec)
	{
		int ID = data.nodeID;
		bool isfind = false;
		for (auto &temp : m_tempVec)
		{
			if (temp.first.first == ID)
			{
				temp.second.push_back(data);
				isfind = true;
				break;
			}
		}

		if (isfind == false)
		{
			vector<LogDataInfo>tm;
			tm.push_back(data);
			m_tempVec.push_back(make_pair(make_pair(ID, false), tm));
		}
	}

	for (auto mp : m_tempVec)
	{
		int id = mp.first.first;
		vector<LogDataInfo>tempL = mp.second;
	}

}
void ISReplayViewModel::fiterDateWithTypeVec(std::vector<int> &TypeSelectVec)
{
	if (this->typeSelectVec.size() != TypeSelectVec.size())
	{
		for (auto data: m_tempVec)
		{
			data.first.second = false;
			listFilterVec.clear();
		}
	}
	this->typeSelectVec = TypeSelectVec;

	fileData(TypeSelectVec, recv_send);
}
void ISReplayViewModel::setBeginTime(int value)
{
	if (m_tempAllData.size() == 0)
	{
		return;
	}
	this->beginTime = value;
	set<LogDataInfo> temSet;
	vector<LogDataInfo> array;
	if (this->endTime == 0)
	{
		this->endTime = m_tempAllData.back().recordTime / 1000;
	}
	for (auto data : m_tempAllData)
	{
		if (data.recordTime > this->beginTime * 1000 && data.recordTime < this->endTime * 1000 )
		{
			array.push_back(data);
		}
	}
	for (auto itr: array)
	{
		temSet.insert(itr);
	}
	vector<LogDataInfo> vc;
	vc.assign(array.begin(), array.end());
	update_tableWidget_data(vc);

}
void ISReplayViewModel::setEndTime(int value)
{
	this->endTime = value;
	set<LogDataInfo> temSet;

	for (auto data : m_tempAllData)
	{
		if (data.recordTime > this->beginTime * 1000 && data.recordTime < this->endTime * 1000)
		{
			temSet.insert(data);
		}
	}
	filterVec = temSet;

	vector<LogDataInfo> vc;
	vc.assign(filterVec.begin(), filterVec.end());
	update_tableWidget_data(vc);

}
void ISReplayViewModel::showSendDate(int state)
{
	is_send = state;

	if (state)
	{
		recv_send = (is_recv == true ? 3 : 1);
	}
	else {

		recv_send = (is_recv == true ? 0 : 4);
	}
	fileData(this->typeSelectVec, recv_send);
}
void ISReplayViewModel::showRecvDate(int state)
{
	is_recv = state;

	if (state)
	{
		recv_send = (is_send == true ? 3 : 0);
	}
	else
	{
		recv_send = (is_send == true ? 1 : 4);
	}
	fileData(this->typeSelectVec, recv_send);

}

set<LogDataInfo> ISReplayViewModel::getFileVec()
{
	return filterVec;
}
void ISReplayViewModel::fileData(std::vector<int> &TypeSelectVec, const int &recvSend)
{

	this->tableWidget->clearContents();
	this->tableWidget->setRowCount(filterVec.size());

	switch (recv_send)
	{
	case 0:
	{
		if (TypeSelectVec.size() > 0)
		{
			set<LogDataInfo> temSet;
			for (auto data : m_tempAllData)
			{
				for (auto teml : TypeSelectVec)
				{
					if (data.ucType == teml)
					{
						temSet.insert(data);
						if (data.recvSend == 0)
						{
							temSet.insert(data);
						}
					}
				}
			}
			filterVec = temSet;
		}
		else
		{
			set<LogDataInfo> temSet;
			for (auto data : m_tempAllData)
			{
				if (data.recvSend == 0)
				{
					temSet.insert(data);
				}
			}
			filterVec = temSet;
		}

		break;
	}
	case 1:
	{
		if (TypeSelectVec.size() > 0)
		{
			set<LogDataInfo> temSet;
			for (auto data : m_tempAllData)
			{
				for (auto teml : TypeSelectVec)
				{
					if (data.ucType == teml)
					{
						temSet.insert(data);
						if (data.recvSend == 1)
						{
							temSet.insert(data);
						}
					}
				}
			}
			filterVec = temSet;
		}
		else
		{
			set<LogDataInfo> temSet;
			for (auto data : m_tempAllData)
			{
				if (data.recvSend == 1)
				{
					temSet.insert(data);
				}
			}
			filterVec = temSet;
		}

		break;
	}
	case 3:
	{
		set<LogDataInfo> temSet;
		if (TypeSelectVec.size() > 0)
		{
			for (auto data : m_tempAllData)
			{
				for (auto teml : TypeSelectVec)
				{
					if (data.ucType == teml)
					{
						temSet.insert(data);
					}
				}
			}
		}
		filterVec = temSet;

		break;
	}
	case 4:
	{

		break;
	}

	default:
		break;
	}

	if (filterVec.size() > 0)
	{
		vector<LogDataInfo> vc;
		vc.assign(filterVec.begin(), filterVec.end());
		update_tableWidget_data(vc);
	}
	else {
		filterVec.clear();

		update_tableWidget_data(m_tempAllData);
		LY_ScenariosReplay::Get().m_pIScenariosReplayThread->m_ReplayDataInfoVec = m_tempAllData;
	}
	if (recv_send == 4)
	{
		filterVec.clear();
		m_ReplayDataInfoVec.clear();
		this->tableWidget->clearContents();
		this->tableWidget->setRowCount(filterVec.size());
		update_tableWidget_data(m_ReplayDataInfoVec);
		LY_ScenariosReplay::Get().m_pIScenariosReplayThread->m_ReplayDataInfoVec = m_ReplayDataInfoVec;
	}
}

void ISReplayViewModel::setCurrentData(QString &data)
{
	QString matchingNum = "";
	QRegExp numberRegex("\\d+");
	int pos = 0;
	while ((pos = numberRegex.indexIn(data, pos)) != -1)
	{
		matchingNum.append(numberRegex.cap(0));
		pos += numberRegex.matchedLength();
	}
	m_currentData = matchingNum;
}
vector<QString> ISReplayViewModel::getDataList()
{
	vector<QString> itemArr;
	bool isconnect = InitDataBase();
	if (isconnect)
	{
		m_tablesList = log_db.tables();
		for (auto item : m_tablesList)
		{
			if (item.contains("net20"))
			{
				itemArr.push_back(item);
			}
		}
	}
	return itemArr;
}

void ISReplayViewModel::initTableWidget(QTableWidget *tableWidget)
{
	this->tableWidget = tableWidget;
	connect(this->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectBox(int, int)));

	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("播放标识") << tr2("节点类型") << tr2("节点名称") << tr2("数据类型") << tr2("开始时间") << tr2("结束时间") << tr2("数据计数");

	tableWidget->setColumnCount(headerlabels.size());
	tableWidget->setHorizontalHeaderLabels(headerlabels);
	tableWidget->verticalHeader()->setHidden(true);
	tableWidget->verticalHeader()->setHidden(true);
	tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget->setAlternatingRowColors(true);
	tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	tableWidget->setSortingEnabled(false);
	tableWidget->setDragEnabled(false);
}
void ISReplayViewModel::selectBox(int r, int c)
{
	if (c != 1)
	{
		return;
	}

	vector<LogDataInfo> tem_vec;
	if (this->tableWidget->item(r, 1) != 0)
	{
		if (this->tableWidget->item(r, 1)->checkState() == Qt::Checked)
		{
			auto itr = m_tempVec.begin();
			if (itr + r == m_tempVec.end())
			{
				return;
			}
			itr += r;

			if (itr->first.second == false)
			{
				itr->first.second = true;
				for (auto data : itr->second)
				{
					listFilterVec.insert(data);
				}
				filterVec = listFilterVec;
			}
		}
		else
		{
			auto itr = m_tempVec.begin();
			if (itr + r == m_tempVec.end())
			{
				return;
			}
			itr += r;
			if (itr->first.second == true)
			{
				itr->first.second = false;

				for (auto data : itr->second)
				{
					auto itor = listFilterVec.find(data);
					if (itor != listFilterVec.end())
					{
						listFilterVec.erase(itor);
					}
				}
				filterVec = listFilterVec;
			}
		}
	}
}
void  ISReplayViewModel::sendMessageInfo()
{
	m_strLocalNodeID = IScenariosReplayActivator::Get().getConfigProjectControlService()->systemSeatInfo().SystemNodeId();
	if (m_strLocalNodeID.empty())
	{
		return;
	}

	connect(&timer, SIGNAL(timeout()), this, SLOT(sendMessageData()));
	timer.start(1000);

	QSettings iniFile("../data/conf/ly.ini", QSettings::IniFormat);
	iniFile.beginGroup("Multicast");
	m_strSnedIP = iniFile.value("GroupIP", "225.1.1.1").toString();
	m_nSendPort = iniFile.value("PeerPort", "8889").toString();

}
void ISReplayViewModel::sendMessageData()
{
	m_timer_index++;
	if (m_timer_index == 16)
	{
		m_timer_index = 0;
	}
	QUdpSocket cli;
	if (m_timer_index != 0)
	{
		return;
	}
	while (m_sendMessageindex < this->sendMessageVec.size())
	{
		auto stInfo = this->sendMessageVec.at(m_sendMessageindex);
		if (stInfo.nType == 0 || stInfo.nType == 1 || stInfo.nType == 2)
		{
			for (auto itrSendAddress : stInfo.strSendAddressList)
			{
				char pBuf[512] = { 0 };
				int nSendLen = sizeof(NetHeader) + sizeof(TeleMsg_FB_30);
				NetHeader* pHeader = (NetHeader*)pBuf;
				TeleMsg_FB_30* pMsgTele = (TeleMsg_FB_30*)(pBuf + sizeof(NetHeader));
				if (m_SeatInfoMap.size() == 0)
				{
					pHeader->ucType = NETHEADER_UCTYPE_TELEGRAPH_CMD;
				}

				auto itr = m_SeatInfoMap.find(itrSendAddress.toInt());
				if (itr != m_SeatInfoMap.end())
				{
					pHeader->ucType = NETHEADER_UCTYPE_TRACK_COMMON;
				}
				else
				{
					pHeader->ucType = NETHEADER_UCTYPE_TELEGRAPH_CMD;
				}

				pHeader->ucSubType = NETHEADER_UCTYPE_TELEGRAPH_CMD_UP_TO_SERVER;
				pHeader->unLength = nSendLen;
				pHeader->unNodeID = itrSendAddress.toInt();
				pHeader->unNodeType = 3;

				pMsgTele->head.MainCode = 0xFB;
				pMsgTele->head.SubCode = 0x30;
				pMsgTele->head.MsgVersion = 1;
				pMsgTele->head.RecvId = itrSendAddress.toInt();
				pMsgTele->head.SendId = QString::fromStdString(m_strLocalNodeID).toInt();;
				pMsgTele->head.PassId = 0x00;
				pMsgTele->head.MsgIndex = stInfo.nNo;
				pMsgTele->head.MsgTime = stInfo.nTime;

				pMsgTele->nType = stInfo.nType;

				strcpy_s(pMsgTele->strDetails, sizeof(pMsgTele->strDetails), stInfo.strDetail.toLocal8Bit().data());
				cli.writeDatagram((const char*)&pBuf, nSendLen, QHostAddress(m_strSnedIP), m_nSendPort.toInt());
			}
		}

		m_sendMessageindex++;

		return;
	}

	if (m_sendMessageindex == this->sendMessageVec.size())
	{
		timer.stop();
		timer.interval();
	}

}
void  ISReplayViewModel::sendPic(LogDataInfo &info)
{

	updAddress = "255.255.255.255";
	if (updAddress.isEmpty() || port == 0) {
		return;
	}

	int rowIndex = 0;
	QString currDir = QString("../src/ly/qrc/pic/");
	QString path = currDir + info.fileName;
	QString sizeT = info.length;
	QDateTime  current_data = QDateTime::currentDateTime();
	QString timeT = current_data.toString("yyyy.MM.dd hh:mm:ss");
	QString pno = "3";

	QFile file(path);
	file.open(QIODevice::ReadOnly);
	QString fileName(path);
	QByteArray bty;
	bty = file.readAll();
	qDebug() << "pic_size:" << bty.size();

	QtConcurrent::run([=]() {

		Net_File_Package_Cmd fileSendbag;
		fileSendbag.netHead.ucType = NETHEADER_UCTYPE_FILETRANS_CMD;
		fileSendbag.netHead.ucSubType = 0x00;
		fileSendbag.netHead.unLength = sizeof(Net_File_Package_Cmd);
		fileSendbag.cmdHead.nMainCmd = 1;
		fileSendbag.cmdHead.nSubCmd = 0;
		fileSendbag.cmdHead.nfileSize = bty.size();
		if (fileName.contains(".bmp")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_BMP;
		}
		else if (fileName.contains(".png")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_PNG;
		}
		else if (fileName.contains(".jpg")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_JPEG;
		}
		else if (fileName.contains(".excel")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_XLS;
		}
		else if (fileName.contains(".xlsx")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_XLSX;
		}
		else if (fileName.contains(".doc")) {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_DOC;
		}
		else {
			fileSendbag.cmdHead.nFileType = FILE_TYPE_UNKNOWN;
		}
		fileSendbag.cmdHead.nFileBN = pno.toInt();
		fileSendbag.packageTotal = fileSendbag.cmdHead.nfileSize / MAX_PACK_SIZE;
		fileSendbag.packageTotal += fileSendbag.cmdHead.nfileSize % MAX_PACK_SIZE > 0 ? 1 : 0;
		int num = 1;
		int count = fileSendbag.packageTotal;
		qDebug() << QString::fromLocal8Bit("当前图片分包数量") << count;
		QUdpSocket cli;

		for (int i = 0; i < count; i++) {
			fileSendbag.packageCurIdx = num;
			if (fileSendbag.cmdHead.nfileSize - i * MAX_PACK_SIZE >= MAX_PACK_SIZE) {
				fileSendbag.packageSize = MAX_PACK_SIZE;
				memcpy(fileSendbag.buf, bty.data() + i * MAX_PACK_SIZE, MAX_PACK_SIZE);
			}
			else {
				fileSendbag.packageSize = fileSendbag.cmdHead.nfileSize - i * MAX_PACK_SIZE;
				memcpy(fileSendbag.buf, bty.data() + i * MAX_PACK_SIZE, fileSendbag.packageSize);
			}
			cli.writeDatagram((const char*)&fileSendbag, fileSendbag.netHead.unLength, QHostAddress(updAddress), port);
			num++;
		}

	});
}

int ISReplayViewModel::getStartTime(int &type)
{
	for (auto itr: NodeAarray)
	{
		if (itr.first == type)
		{
			if (itr.second.size() == 0)
			{
				continue;
			}
			LogDataInfo model = itr.second.front();
			return model.recordTime;
		}
	}
}

int ISReplayViewModel::getEndTime(int &type)
{

	for (auto itr : NodeAarray)
	{
		if (itr.first == type)
		{
			if (itr.second.size() == 0)
			{
				continue;
			}
			LogDataInfo model = itr.second.back();
			return model.recordTime;
		}
	}
}

QString ISReplayViewModel::getNodeWithNodeType(const  int &type)
{
	if (type == 0)
	{
		return tr2("未知");
	}
	else if (type == 1)
	{
		return tr2("指挥");
	}
	else if (type == 2)
	{
		return tr2("武器系统");
	}
	else if (type == 3)
	{
		return tr2("武器装备");
	}
	else if (type == 4)
	{
		return tr2("通信设备");
	}
	else if (type == 5)
	{
		return tr2("通信组");
	}
	else if (type == 6)
	{
		return tr2("传感器");
	}
	else if (type == 7)
	{
		return tr2("弹药");
	}
	else if (type == 8)
	{
		return tr2("图片");
	}
	else if (type == 9)
	{
		return tr2("视频");
	}
	else if (type == 10)
	{
		return tr2("文电");
	}
	else if (type == 11)
	{
		return tr2("红外瞄具");
	}
	return tr2("未知");
}

QString ISReplayViewModel::getDataTypeWithType(int &type)
{

	if (type == 0x00) {
		return tr2("作战时间");
	}
	else if (type > 0 && type < 21)
	{
		return tr2("系统内部使用");
	}
	else if (type == 0x22) {
		return tr2("火箭弹DY 领云 DY（弹药）");
	}
	else if (type == 0x21) {
		return tr2("翔仪恒昌 协议（包括单机、多机）");
	}
	else if (type == 0x23) {
		return tr2("消防弹");
	}
	else if (type == 0x24) {
		return tr2("40mm激光弹药 精密弹药");
	}
	else if (type == 0x25) {
		return tr2("54火箭弹");
	}
	else if (type == 0x26) {
		return tr2("侦察吊舱 高德吊舱  侦察（ZC）");
	}
	else if (type == 0x27) {
		return tr2("侦察吊舱 云汉吊舱NEO");
	}
	else if (type == 0x28) {
		return tr2("侦察吊舱 品灵吊舱");
	}
	else if (type == 0x29) {
		return tr2("侦察吊舱 XK400吊舱");
	}
	else if (type == 0x2A) {
		return tr2("侦察吊舱 54吊舱");
	}
	else if (type == 0x2B) {
		return tr2("红外瞄具 54红外瞄具");
	}
	else if (type == 0x2C) {
		return tr2("Sbg导航 车载导航");
	}
	else if (type == 0x2D) {
		return tr2("海格电台 海格电台");
	}
	else if (type == 0x2E) {
		return tr2("Mavlink Mavlink");
	}
	else if (type == 0x2F) {
		return tr2("DY60 60所 靶机 导弹 DY60 60所 靶机 导弹");
	}
	else if (type == 0x30) {
		return tr2("SwarmNanHang 蜂群 指控系统 地面站 南航 蜂群指控");
	}
	else {
		return tr2("作战时间");
	}
	return tr2("未知");
}

