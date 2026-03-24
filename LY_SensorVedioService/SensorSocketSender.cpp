#include "SensorSocketSender.h"
#include <QDb.h>
#include <QCString.h>
#include <QGeoCoordinate>

#include <time.h>
#include <QDebug>
#include <QSettings>
#include <QUdpSocket>
#include <QNetworkAddressEntry>

#include "LY_SensorVedioService_global.h"
#include "LY_SensorVedioService.h"

SensorSocketSender::SensorSocketSender(QObject* pObj)
	: QThread(pObj)
	, m_flag(new std::atomic_flag ATOMIC_FLAG_INIT)
{
	QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);
	settings.beginGroup("SensorVedioMultiCast");
	m_hostAddress = settings.value("GroupIP", "234.1.1.8").toString();
	m_port = settings.value("Port", 40000).toInt();
	settings.endGroup();
	m_uuID = LY_SensorVedioService::Get().GetUUID();
}

SensorSocketSender::~SensorSocketSender(void)
{
	if (this->isRunning())
	{
		this->TerminateIt();
		this->quit();
		this->wait();
	}

	{
		SpinLocker locker(m_flag);
		onDeleteValue();
	}
}

void SensorSocketSender::TerminateIt()
{
	m_terminateFlag.store(true);
}

void SensorSocketSender::AppendData(const qnzkna::SensorVedio::EComponentRectUpateType& type, char* data, size_t length)
{
	SpinLocker locker(m_flag);
	m_queueList.push_back({ type, {data, (int)length} });
}

void SensorSocketSender::AppendData(const qnzkna::SensorVedio::EComponentRectUpateType& type, QByteArray& array)
{
	SpinLocker locker(m_flag);
	m_queueList.push_back({ type, array });
}

void SensorSocketSender::run()
{
	QUdpSocket socket;
	InitSocket(socket);
	QHostAddress host(m_hostAddress);
	
	size_t headerSize = sizeof(sensorSpace::SensorHeader);
	QByteArray headerByte;
	headerByte.resize((int)headerSize);

	sensorSpace::SensorHeader header;
	memcpy(&(header.senderUUID), m_uuID.toLocal8Bit().data(), qMin(m_uuID.toLocal8Bit().size(), 64));

	unsigned sortIndex = 0;
	while (true)
	{
		std::list<QPair<int, QByteArray>> queueList;
		{
			SpinLocker locker(m_flag);
			if (m_terminateFlag.load())
			{
				m_terminateFlag.store(false);
				onDeleteValue();
				break;
			}
			else
			{
				queueList.swap(m_queueList);
				m_queueList.clear();
			}
		}

		if (queueList.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			continue;
		}
		QByteArray sendData;
		for (auto& data : queueList)
		{
			// 头部数据压入
			header.type = (unsigned)data.first;
			header.length = data.second.size();
			header.timeStep = QDateTime::currentDateTime().currentMSecsSinceEpoch();
			memcpy(headerByte.data(), &header, headerSize);

			// 数据整合
			sendData.append(headerByte);
			sendData.append(data.second);

			// 数据发送
			socket.writeDatagram(sendData, QHostAddress(m_hostAddress), m_port);
			socket.waitForBytesWritten();

			// 数据清空
			sendData.clear();
			sendData.squeeze();

			// 包序号清0
			if (sortIndex & 0xFFFF) sortIndex = 0;
		}
	};
}

void SensorSocketSender::onDeleteValue()
{
	std::list<QPair<int, QByteArray>> queue;
	queue.swap(m_queueList);
	m_queueList.clear();

	for (auto& frame : queue)
	{
		frame.second.clear();
		frame.second.squeeze();
	}
	queue.clear();
}

void SensorSocketSender::InitSocket(QUdpSocket& socket)
{
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (auto& inface : interfaces)
	{
		if (!inface.isValid() || inface.flags().testFlag(QNetworkInterface::IsLoopBack)
			|| !inface.flags().testFlag(QNetworkInterface::IsUp))
			continue;

		QList<QNetworkAddressEntry> entries = inface.addressEntries();
		for (auto& entrie : entries)
		{
			if (entrie.ip().protocol() == QAbstractSocket::IPv4Protocol)
			{
				socket.setMulticastInterface(inface);
				return;
			}
		}
	}
}
