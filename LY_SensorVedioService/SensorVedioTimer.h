#ifndef __SENSORVEDIOTIMER_H
#define __SENSORVEDIOTIMER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMutexLocker>
#include <QSharedMemory>
#include <QSystemSemaphore>

#include "YoloImageMemoryManage.h"
#include "LY_SensorVedioService_global.h"

class SensorVedioTimer :public QObject {
	Q_OBJECT
public:
	SensorVedioTimer();
	virtual ~SensorVedioTimer();

	void timerEvent(QTimerEvent* event)override;

private:
	int m_timeOutCnt = 0;
	int m_timerID = -1;
	YoloImageClientMemoryManageSPtr m_pMemory;
};

#endif
