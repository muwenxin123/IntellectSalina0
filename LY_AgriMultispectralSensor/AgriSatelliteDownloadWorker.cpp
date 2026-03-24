#include <QDebug>

#include "AgriSatelliteDownloadWorker.h"
#include "AgriSatellitePythonGenerator.h"


QJsonObject DownloadTask::toJson() const
{
	return{
		{ "taskId", taskId },
		{ "url", url },
		{ "savePath", savePath },
		{ "status", statusToString(status) }
	};
}

DownloadTask DownloadTask::fromJson(const QJsonObject& obj)
{
	DownloadTask task;
	task.taskId = obj["taskId"].toString();
	task.url = obj["url"].toString();
	task.savePath = obj["savePath"].toString();
	task.status = stringToStatus(obj["status"].toString());
	return task;
}

QString DownloadTask::statusToString(TaskStatus s)
{
	switch (s) {
	case TaskStatus::Pending: return "pending";
	case TaskStatus::Running: return "running";
	case TaskStatus::Finished: return "finished";
	case TaskStatus::Failed: return "failed";
	}
	return "unknown";
}

TaskStatus DownloadTask::stringToStatus(const QString& s)
{
	if (s == "pending") return TaskStatus::Pending;
	if (s == "running") return TaskStatus::Running;
	if (s == "finished") return TaskStatus::Finished;
	if (s == "failed") return TaskStatus::Failed;
	return TaskStatus::Pending;
}


QJsonObject DownloadProgress::toJson() const
{
	return{
		{ "taskId", taskId },
		{ "savePath", savePath },
		{ "fileSize", QString::number(fileSize) },
		{ "downloadSize", QString::number(downloadSize) }
	};
}

DownloadProgress DownloadProgress::fromJson(const QJsonObject& obj)
{
	DownloadProgress task;
	task.taskId = obj["taskId"].toString();
	task.fileSize = obj["fileSize"].toInt();
	task.savePath = obj["savePath"].toString();
	task.downloadSize = obj["downloadSize"].toInt();
	return task;
}


AgriSatelliteDownloadWorker::AgriSatelliteDownloadWorker(const DownloadTask& task, QObject* parent/* = nullptr*/)
	: QThread(parent), task(task), isRunningFlag(false)
{

}

AgriSatelliteDownloadWorker::~AgriSatelliteDownloadWorker()
{

}

void AgriSatelliteDownloadWorker::run()
{
	//{
	//	isRunningFlag.store(true);

	//	// 模拟下载过程
	//	for (int i = 1; i <= 100; ++i) {
	//		QThread::msleep(50);
	//		emit progress(i);
	//	}

	//	emit taskFinished(task.taskId);
	//	isRunningFlag.store(false);
	//}

	if (!task.savePath.isEmpty())
	{
		isRunningFlag.store(true);

		// 设置 dispatcher 给当前线程
		CallbackDispatcher* dispatcher = new CallbackDispatcher();
		dispatcher->moveToThread(this);
		CallbackWrapper::dispatcherPool.setLocalData(dispatcher);

		QObject::connect(dispatcher, &CallbackDispatcher::messageReceived, [&](const QString& msg) {
			//qDebug() << "[Worker thread]:" << QThread::currentThreadId() << " Received from Python:" << msg;
			emit progress(msg);

			// TODO: 释放dispatcher
			//if (xxx)
			//{
			//	CallbackWrapper::dispatcherPool.setLocalData(nullptr);
			//	delete dispatcher;
			//  isRunningFlag.store(false);
			//}
		});

		auto cb = CallbackWrapper::get_callback_capsule();
		AgriSatellitePythonGenerator::getInstance().download_sentinel_data(task.savePath.toStdString(), task.savePath.toStdString(), cb);

		CallbackWrapper::dispatcherPool.setLocalData(nullptr);
		if (dispatcher)
		{
			//delete dispatcher;
			emit taskFinished(task.taskId);
			isRunningFlag.store(false);
		}
	}
}
