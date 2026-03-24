#include "SensorPlayThread.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include "ISensorVedioServiceImpl.h"
#include "YoloImageMemoryManage.h"

SensorPlayThread::SensorPlayThread(QObject* pParent)
	:QThread(pParent)
{
	m_pMemory = new yoloImage::YoloImageClientMemoryManage(QString::number(QCoreApplication::applicationPid()));
	start();
}

SensorPlayThread::~SensorPlayThread()
{
	m_quitFlag.store(true);
	if (this->isRunning())
	{
		this->quit();
		this->wait();
	}

	if (nullptr != m_pMemory)
	{
		delete m_pMemory;
		m_pMemory = nullptr;
	}
}

void SensorPlayThread::run()
{
	int cnt = 0;
	//auto s_last = std::chrono::system_clock::now();
	//auto s_now = std::chrono::system_clock::now();
	//uint s_maxCustom = 0;
	//uint s_maxIntervel = 0;

	while (!m_quitFlag.load())
	{
		//s_now = std::chrono::system_clock::now();
		//uint intervelTime = (std::chrono::duration_cast<std::chrono::milliseconds>(s_now - s_last).count());
		//s_maxIntervel = qMax(intervelTime, s_maxIntervel);

		if (++cnt %4 )
		{
			QList<yoloImage::QComponentInfo> list;
			m_pMemory->GetAllComponentInfo(list);
			for (auto& data : list)
			{
				long long id = data.identity.toLongLong();
				qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().AddPixMap(id, data.imageByte);
			}
			list.clear();
		}
		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().PopPixMap();
		QThread::msleep(20);

		//s_last = std::chrono::system_clock::now();
		//uint curstm = (std::chrono::duration_cast<std::chrono::microseconds>(s_last - s_now).count());
		//s_maxCustom = qMax(curstm, s_maxCustom);
	}
}
