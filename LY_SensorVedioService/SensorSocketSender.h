#ifndef _SENDERSOCKETSENDER_H
#define _SENDERSOCKETSENDER_H

#include <atomic>

#include <QList>
#include <QThread>
#include <QByteArray>
#include <list>

#include "DataManage/SensorVedioService/SensorVedioInfo.h"

class QUdpSocket;
class SensorSocketSender : public QThread 
{
	Q_OBJECT
public:
	SensorSocketSender(QObject* pParent);
    ~SensorSocketSender(void);

	// 关闭标识
	void TerminateIt();

	// 压入数据
	void AppendData(const qnzkna::SensorVedio::EComponentRectUpateType& type, char* data, size_t length);

	// 压入数据
	void AppendData(const qnzkna::SensorVedio::EComponentRectUpateType& type,  QByteArray& array);

	//运行
	void run()override;

	void onDeleteValue();

	void InitSocket(QUdpSocket& socket);

private:
	std::shared_ptr<std::atomic_flag>  m_flag;						// 标识
	std::atomic_bool  m_terminateFlag = false;
	std::list<QPair<int, QByteArray>> m_queueList;
	QUdpSocket* m_pSocket = nullptr;
	QString m_uuID;
	QString  m_hostAddress;
	int m_port;
};

#endif
