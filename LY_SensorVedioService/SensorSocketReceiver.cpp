#include "SensorSocketReceiver.h"

#include <time.h>
#include <QDebug>
#include <QSettings>
#include <QUdpSocket>
#include <QNetworkInterface>

#include <QCString.h>
#include "LY_SensorVedioService.h"
#include "ISensorVedioServiceImpl.h"

SensorSocketReceiver::SensorSocketReceiver(QObject* pObject)
	:QThread(pObject)
	, m_processer(new ReceiveBufferProcesser(nullptr))
	, m_flag(new std::atomic_flag ATOMIC_FLAG_INIT)
{
	QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);
	settings.beginGroup("SensorVedioMultiCast");
	m_hostAddress = settings.value("GroupIP", "234.1.1.8").toString();
	m_port = settings.value("Port", 40000).toInt();

	settings.endGroup();
	m_uuID = LY_SensorVedioService::Get().GetUUID();

	m_processer->start();
}

SensorSocketReceiver::~SensorSocketReceiver(void)
{
	if (nullptr != m_processer)
	{
		delete m_processer;
		m_processer = nullptr;
	}

	if (this->isRunning())
	{
		this->TerminateIt();
		this->quit();
		this->wait();
	}
}

void SensorSocketReceiver::run()
{
	QUdpSocket socket;
	InitSocket(socket);
	while (true)
	{
		{
			SpinLocker locker(m_flag);
			if (m_terminateFlag.load())
			{
				m_terminateFlag.store(false);
				break;
			}
		}

		if (socket.pendingDatagramSize() <= 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			continue;
		}

		if (socket.pendingDatagramSize() > 0)
		{
			QByteArray datagram;
			datagram.resize(socket.pendingDatagramSize());

			QHostAddress sender;
			quint16 sendPort;
			socket.readDatagram(datagram.data(), datagram.size(), &sender, &sendPort);
			m_processer->AppendData(datagram);
		}
	}

	socket.close();
}

void SensorSocketReceiver::TerminateIt()
{
	m_terminateFlag.store(true);
}

void SensorSocketReceiver::InitSocket(QUdpSocket& socket)
{
	QHostAddress address(m_hostAddress);
	socket.bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress);
	socket.setSocketOption(QAbstractSocket::MulticastTtlOption, 2);
	//socket.joinMulticastGroup(address);

	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (auto& inface : interfaces)
	{
		if (!inface.isValid() || inface.type() == QNetworkInterface::Wifi
			|| inface.flags().testFlag(QNetworkInterface::IsLoopBack) 
			||!inface.flags().testFlag(QNetworkInterface::IsUp))
			continue;

		QList<QNetworkAddressEntry> entries = inface.addressEntries();
		for (auto& entrie : entries)
		{
			if (entrie.ip().protocol() == QAbstractSocket::IPv4Protocol)
			{
				socket.joinMulticastGroup(address, inface);
				return;
			}
		}
	}
}

ReceiveBufferProcesser::ReceiveBufferProcesser(QObject* pObject)
	: QThread(pObject)
	, m_flag(new std::atomic_flag ATOMIC_FLAG_INIT)
{
	m_uuID = LY_SensorVedioService::Get().GetUUID();
}

ReceiveBufferProcesser::~ReceiveBufferProcesser()
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

void ReceiveBufferProcesser::run()
{
	while (true)
	{
		std::list<QByteArray> queueList;
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
				queueList.swap(m_byteArrayList);
				m_byteArrayList.clear();
			}
		}

		if (queueList.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			continue;
		}

		QList<QPair<sensorSpace::SensorHeader, QByteArray> > arrayList;
		prevParsePackage(queueList, arrayList);
		for (auto& data : arrayList)
		{
			parsePackage(data.first, data.second);
		}
		queueList.clear();
		arrayList.clear();
	};
}

void ReceiveBufferProcesser::TerminateIt()
{
	m_terminateFlag.store(true);
}

void ReceiveBufferProcesser::AppendData(QByteArray& array)
{
	SpinLocker locker(m_flag);
	m_byteArrayList.push_back(std::move(array));
}

void ReceiveBufferProcesser::onDeleteValue()
{
	std::list<QByteArray> queue;
	queue.swap(m_byteArrayList);
	m_byteArrayList.clear();

	for (auto& frame : queue)
	{
		frame.clear();
		frame.squeeze();
	}
	queue.clear();
}

void ReceiveBufferProcesser::parsePackage(sensorSpace::SensorHeader& header, QByteArray& array)
{
	// 数据校验失败
	if (!header.checkVal == 0x00FF)
		return;

	// 发送者ID和接收者ID一致
	//if (m_uuID == header.senderUUID)
	//	return;

	switch (header.type)
	{
	case qnzkna::SensorVedio::UpdateTargets:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvUpdateComponentTargets(array.data() , header.length);
	}
	break;
	case qnzkna::SensorVedio::UpdateTargetData:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvUpdateComponentTargetData(array.data(), header.length);
	}
	break;
	case qnzkna::SensorVedio::ClearTarget:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvClearComponentTargets(array.data(), header.length);
	}
	break;
	case  qnzkna::SensorVedio::UpdateFriendData:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvUpdateComponentFriendData(array.data(), header.length);
	} break;
	case  qnzkna::SensorVedio::UpdateFriends:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvUpdateComponentFriends(array.data(), header.length);
	} break;
	case  qnzkna::SensorVedio::ClearFriends:
	{
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().
			RecvClearComponentFriend(array.data(), header.length);
	} break;
	default:
		break;
	}
}

void ReceiveBufferProcesser::prevParsePackage(std::list<QByteArray>& array, 
	QList<QPair<sensorSpace::SensorHeader, QByteArray> >& arrayList)
{
	size_t headerSize = sizeof(sensorSpace::SensorHeader);
	sensorSpace::SensorHeader header;

	arrayList.clear();

	int index = 0;
	for (auto& data : array)
	{
		if (data.size() < headerSize)
			return;

		memcpy(&header, data.data(), headerSize);

		QByteArray array;
		array.clear();
		array.squeeze();
		array.resize(0);
		if (header.length > 0 && data.size() >= headerSize + header.length)
			array.append(data.data() + headerSize, header.length);

		arrayList.append({ header, array });
	}

	std::sort(arrayList.begin(), arrayList.end(), [](const QPair<sensorSpace::SensorHeader, QByteArray>& prev,
		const QPair<sensorSpace::SensorHeader, QByteArray>& next) -> bool{
		return prev.first.timeStep < next.first.timeStep;
	});
}
