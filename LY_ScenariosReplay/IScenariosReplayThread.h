#ifndef ISCENARIOSREPLAYTHREAD_H
#define ISCENARIOSREPLAYTHREAD_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>

struct LogDataInfo
{
	int recordTime;
	int recvSend;
	int ucType;
	int subType;
	int nodeType;
	int nodeID;
	int length;
	QString fileName;
	QByteArray data;

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

	LogDataInfo()
	{
		recordTime = 0;
		recvSend = 0;
	    ucType = 0;
		subType = 0;
		nodeType = 0;
		nodeID = 0;
		length = 0;
		fileName = "";
	}

   bool operator ==(const LogDataInfo &r)const
	{
	   if (this->nodeID == r.nodeID && this->recvSend == r.recvSend&& this->ucType == r.ucType &&this->subType == r.subType
		   &&this->nodeType == r.nodeType && this->length == r.length && this->recordTime== r.recordTime ) {
		   return true;
	   }
	   return false;
	}

   bool operator <(const LogDataInfo &r) const
   {
	   if (recordTime < r.recordTime)
	   {
		   return true;
	   }
	   return false;
   }

};

class IScenariosReplayThread : public QThread
{
	Q_OBJECT
public:
    IScenariosReplayThread(QObject *parent);
    ~IScenariosReplayThread();

	void initUdp();

    void run();

    void startThread();

    void stopThread();

    void pauseReplay(bool pause);

    void setSpeed(int speed);

    void setRate(int rate);

    bool isFinished();

signals:
    void flyReplayThreadrun();

    void percentChanged(int value);

public:
	QUdpSocket*					       m_udpSocket;
	QHostAddress					   udp_IP;
	qint16                             udp_Port;

public:
    const unsigned long int				_time;
    volatile bool						_running;
    volatile bool						_stop;
    volatile bool						_pause;
    unsigned int						_speed;
    volatile bool						_fixedFre;
    unsigned int						_rate;

    int									m_nPointNoMax;
    double                              m_percent;

	int									last_timpstamp = 0;

    std::vector<std::pair<QString, QByteArray>>          m_ReplayDataVec;
	std::vector<LogDataInfo> m_ReplayDataInfoVec;

	LogDataInfo startDatainfo;
	LogDataInfo endDatainfo;

	QString startDate;
	QString endDate;

	QString startTime;
	QString endTime;
};

#endif
