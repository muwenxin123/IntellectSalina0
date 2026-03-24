#include "SensorVedioTimer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QThreadPool.h>
#include "ConvertTask.h"
#include "ISensorVedioServiceImpl.h"

SensorVedioTimer::SensorVedioTimer()
{
	m_pMemory = std::make_shared<yoloImage::YoloImageClientMemoryManage>(QString::number(QCoreApplication::applicationPid()));

	m_timerID = startTimer(80);
}

SensorVedioTimer::~SensorVedioTimer()
{
	killTimer(m_timerID);
}

void SensorVedioTimer::timerEvent(QTimerEvent* event)
{
	QThreadPool::globalInstance()->start(new ConvertTask([ pMemory = m_pMemory,timeOutCnt= m_timeOutCnt]() {
		if (pMemory->Size() > 0)
		{
			QList<yoloImage::QComponentInfo> list;
			pMemory->GetAllComponentInfo(list);
			for (auto& data : list)
			{
				long long id = data.identity.toLongLong();
				qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().AddPixMap(id, data.imageByte);
				data.imageByte.clear();
				data.imageByte.squeeze();
			}
			list.clear();
		}

		qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().PopPixMap();

		if (timeOutCnt % 10 == 0)
		{
			qnzkna::SensorVedio::ISensorVedioServiceImpl::Get().OnTimeOut();
		}
	}));
	m_timeOutCnt++;
	return QObject::timerEvent(event);
}
