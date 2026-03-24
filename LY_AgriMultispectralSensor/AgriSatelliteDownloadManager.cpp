#include <QFile>
#include <QJsonDocument>

#include "AgriSatelliteDownloadManager.h"


std::unique_ptr<AgriSatelliteDownloadManager> AgriSatelliteDownloadManager::instancePtr;
std::once_flag AgriSatelliteDownloadManager::initFlag;

AgriSatelliteDownloadManager& AgriSatelliteDownloadManager::instance() 
{
	std::call_once(initFlag, []() {
		instancePtr.reset(new AgriSatelliteDownloadManager());
		std::atexit(&AgriSatelliteDownloadManager::destroy);
	});
	return *instancePtr;
}

AgriSatelliteDownloadManager::AgriSatelliteDownloadManager() 
{
	// 自动恢复任务状态信息
	//loadTasksFromFile();
}

AgriSatelliteDownloadManager::~AgriSatelliteDownloadManager() 
{
	clearAllWorkers();
}

void AgriSatelliteDownloadManager::destroy() 
{
	instancePtr.reset();
}

void AgriSatelliteDownloadManager::addTask(const DownloadTask& task) 
{
	QMutexLocker locker(&mutex);

	if (workers.contains(task.taskId)) return;  // 已存在

	pendingTasks.enqueue(task);
	tryStartNextTask();

	// 持久化任务
	//saveTasksToFile();
}

void AgriSatelliteDownloadManager::tryStartNextTask() 
{
	//QMutexLocker locker(&mutex);

	if (workers.size() >= maxConcurrentTasks || pendingTasks.isEmpty()) return;

	DownloadTask task = pendingTasks.dequeue();
	auto* worker = new AgriSatelliteDownloadWorker(task);
	workers.insert(task.taskId, worker);

	connect(worker, &AgriSatelliteDownloadWorker::progress, this, [this, task](const QString& msg) {
		emit taskProgress(task.taskId, msg);
	});
	connect(worker, &AgriSatelliteDownloadWorker::taskFinished, this, [this](const QString& id) {
		handleWorkerFinish(id);
	});
	connect(worker, &QThread::finished, worker, &QObject::deleteLater);
	worker->start();
}

void AgriSatelliteDownloadManager::handleWorkerFinish(const QString& taskId) 
{
	QMutexLocker locker(&mutex);

	if (!workers.contains(taskId)) return;

	workers.remove(taskId);
	emit taskFinished(taskId);

	//updateTaskStatus(taskId, TaskStatus::Finished); // 更新任务完成状态

	tryStartNextTask();  // 调度下一个
}

QList<QString> AgriSatelliteDownloadManager::getWorkerIds() 
{
	QMutexLocker locker(&mutex);

	return workers.keys();
}

bool AgriSatelliteDownloadManager::isWorkerRunning(const QString& id) 
{
	QMutexLocker locker(&mutex);

	return workers.contains(id) && workers[id]->isRunning();
}

bool AgriSatelliteDownloadManager::stopTask(const QString& id)
{
	QMutexLocker locker(&mutex);

	if (workers.contains(id)){
		auto &worker = workers[id];
		worker->quit();
		worker->wait();
		worker->deleteLater();

		workers.remove(id);
	}

	return true;
}

void AgriSatelliteDownloadManager::clearAllWorkers()
{
	QMutexLocker locker(&mutex);

	for (auto* worker : workers) {
		worker->quit();
		worker->wait();
		worker->deleteLater();
	}
	workers.clear();
}

void AgriSatelliteDownloadManager::loadTasksFromFile() 
{
	QFile file(taskFilePath);
	if (!file.open(QIODevice::ReadOnly)) return;

	QByteArray data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isArray()) return;

	QMutexLocker locker(&mutex);

	QJsonArray array = doc.array();
	for (const QJsonValue& val : array) {
		DownloadTask task = DownloadTask::fromJson(val.toObject());
		if (task.status != TaskStatus::Finished) {
			task.status = TaskStatus::Pending; // 重新排队
			pendingTasks.enqueue(task);
		}
	}
}

void AgriSatelliteDownloadManager::saveTasksToFile() 
{
	//QMutexLocker locker(&mutex);

	QJsonArray array;
	for (const auto& worker : workers) {
		DownloadTask task;
		task.taskId = worker->getId();
		task.status = TaskStatus::Running;
		array.append(task.toJson());
	}

	for (const auto& task : pendingTasks) {
		array.append(task.toJson());
	}

	QFile file(taskFilePath);
	if (file.open(QIODevice::WriteOnly)) {
		QJsonDocument doc(array);
		file.write(doc.toJson());
	}
}

void AgriSatelliteDownloadManager::updateTaskStatus(const QString& taskId, TaskStatus status) 
{
	QFile file(taskFilePath);
	if (!file.open(QIODevice::ReadOnly)) return;

	QByteArray data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isArray()) return;

	QJsonArray array = doc.array();
	for (QJsonValueRef val : array) {
		QJsonObject obj = val.toObject();
		if (obj["taskId"].toString() == taskId) {
			obj["status"] = DownloadTask::statusToString(status);
			val = obj;
			break;
		}
	}

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(QJsonDocument(array).toJson());
	}
}
