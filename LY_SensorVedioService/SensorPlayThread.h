#ifndef __SENSORPLAYTHREAD_H
#define __SENSORPLAYTHREAD_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QThread>
#include <QMutexLocker>
#include <QSharedMemory>
#include <QSystemSemaphore>

namespace yoloImage {
	class YoloImageClientMemoryManage;
};
class SensorPlayThread :public QThread {
public:
	SensorPlayThread(QObject* pParent);
	virtual ~SensorPlayThread();

	void run()override;

private:
	std::atomic_bool m_quitFlag = false;
	yoloImage::YoloImageClientMemoryManage* m_pMemory = nullptr;
};

#endif
