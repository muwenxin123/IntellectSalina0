#ifndef _SENSORSOCKETRECEIVER_H
#define _SENSORSOCKETRECEIVER_H

#include <atomic>

#include <QList>
#include <QThread>
#include <QByteArray>

#include "LY_SensorVedioService_global.h"
#include "DataManage/SensorVedioService/SensorVedioInfo.h"

class QUdpSocket;
class ReceiveBufferProcesser;
class SensorSocketReceiver : public QThread
{
public:
	SensorSocketReceiver(QObject* pObject);
    virtual ~SensorSocketReceiver(void);

	//运行
	void run()override;
	
	// 关闭标识
	void TerminateIt();

private:
	void InitSocket(QUdpSocket& socket);

private: 
	std::shared_ptr<std::atomic_flag>  m_flag;						// 标识
	std::atomic_bool m_terminateFlag = false;
	QString m_uuID;
	QString  m_hostAddress;
	int m_port = 0;
	ReceiveBufferProcesser* m_processer;
};

class ReceiveBufferProcesser : public QThread
{
public:
	ReceiveBufferProcesser(QObject* pObject);
	virtual ~ReceiveBufferProcesser();

	//运行
	void run()override;

	// 关闭标识
	void TerminateIt();

	// 压入数据
	void AppendData(QByteArray& array);

	void onDeleteValue();

private:
	void parsePackage(sensorSpace::SensorHeader& header, QByteArray& array);
	void prevParsePackage(std::list<QByteArray>& array, QList<QPair<sensorSpace::SensorHeader, QByteArray> >& arrayList);

private:
	std::shared_ptr<std::atomic_flag>  m_flag;						// 标识
	std::atomic_bool m_terminateFlag = false;
	std::list<QByteArray> m_byteArrayList;
	QString m_uuID;
};
#endif
