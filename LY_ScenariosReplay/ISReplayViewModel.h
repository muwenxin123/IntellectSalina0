#pragma once

#include "IScenariosReplayActivator.h"
#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include "QtSql\QSqlDatabase"
#include "QtSql\QSqlQuery"
#include <QMutex>
#include <QMenu>
#include "IScenariosReplayThread.h"
#include <iostream>
#include <string>
#include  <vector>
#include "RangeSlider.h"
#include <set>
#include  <unordered_map>
#include <QTimer>
#include  "qthread.h"

using namespace std;

typedef struct __SendMessageInfo
{
	int					nNo;
	int					nTime;
	QStringList			strSendAddressList;
	int					nType;
	QString				strDetail;
	int					nStates;
	int					nChildType1;
	int					nChildType2;
	QString				strMissionID;
	QStringList			strTargetIDList;

	__SendMessageInfo()
	{
		nNo = 0;
		nTime = 0;
		strSendAddressList.clear();
		nType = 0;
		strDetail = "";
		nStates = 0;
		nChildType1 = 0;
		nChildType2 = 0;
		strMissionID = "";
		strTargetIDList.clear();
	}

}SendMessageInfo;

#define SEAT_COMMAND				33331
#define SEAT_BATTLE					33332
#define SEAT_INTELLIGENCE			33333
#define SEAT_COORDINATION			33334

#define SEAT_COMMAND_NAME			tr2("指挥席")
#define SEAT_BATTLE_NAME			tr2("作战席")
#define SEAT_INTELLIGENCE_NAME		tr2("情报席")
#define SEAT_COORDINATION_NAME		tr2("协同席")

class ISReplayViewModel : public QObject
{
	Q_OBJECT

public:
	ISReplayViewModel(QObject *parent);
	~ISReplayViewModel();

	void initTableWidget(QTableWidget *tableWidget);

	vector<QString> getDataList();

	bool InitDataBase();

	bool LoadReplayDataBase(const QString &dbName);
	bool LoadMessageSendListDB();
	QStringList GetEachData(const QString& strDate);
	QString GetTime(const int& nTime);

	set<LogDataInfo> getFileVec();
	vector<LogDataInfo> m_ReplayDataInfoVec;
	vector<LogDataInfo> m_ReplayVec;
	vector<LogDataInfo> m_tempAllData;

	void update_tableWidget_data(vector<LogDataInfo> &vec);

	void setCurrentData(QString &data);

	void fiterDateWithTypeVec(vector<int> &TypeSelectVec);

	void showSendDate(int state);
	void showRecvDate(int state);

	void setBeginTime(int value);

	void setEndTime(int value);
	void fileData(std::vector<int> &TypeSelectVec,const int &recvSend);

	void setListVec(vector<LogDataInfo> vec);

	void  sendPic(LogDataInfo &info);
	void  sendMessageInfo();

signals:

	void selectedVideo(bool isSelected);
private slots:

    void selectBox(int r, int c);
	void sendMessageData();
private:

	IScenariosReplayActivator*  m_pActivator;

	QTableWidget                        *tableWidget;
	QMutex									sMutex;
	QSqlDatabase							log_db;
	QString                                  m_currentData;
	set<LogDataInfo>                 filterVec;
	set<LogDataInfo>                 listFilterVec;

	vector<pair<pair<int, bool>, vector<LogDataInfo>>>  m_tempVec;

	vector<SendMessageInfo>          sendMessageVec;
    map<int, QString>			            m_SeatInfoMap;
	string											m_strLocalNodeID;
	int                                            m_sendMessageindex = 0;
	QString                                  m_strSnedIP;
	QString                                   m_nSendPort;
	QTimer                                     timer;
	int                                            m_timer_index = 0;

	QString updAddress = "";
	int port = 58888;

	RangeSlider*							m_RsHsing;

	int                                           beginTime = 0;
	int                                           endTime = 0;

	vector<int>                           typeSelectVec;
	int                                          recv_send = 3;
	int                                          is_send = 1;
	int                                          is_recv = 1;

	QStringList                             m_tablesList;

	vector<pair<int, vector<LogDataInfo>>> NodeAarray;

private:

	QString getDataTypeWithType(  int &type);
	QString getNodeWithNodeType(const  int &type);

	int getStartTime(int &type);
	int getEndTime(int &type);

};
