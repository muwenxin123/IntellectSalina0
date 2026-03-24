#ifndef AGRISATELLITEDOWNLOADMANAGER_H_
#define AGRISATELLITEDOWNLOADMANAGER_H_

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <memory>
#include <mutex>
#include <QJsonArray>

#include "AgriSatelliteDownloadWorker.h"


/* 多线程任务管理调度
** > 支持并发控制（可设置 maxConcurrentTasks）；
** > 支持任务排队；
** > 支持任务进度和完成信号；
** > 支持参数初始化；
** > 单例线程安全、自动析构。
*/
class AgriSatelliteDownloadManager : public QObject {
	Q_OBJECT

public:
	static AgriSatelliteDownloadManager& instance();

	void addTask(const DownloadTask& task);       // 支持调度队列
	QList<QString> getWorkerIds();
	bool isWorkerRunning(const QString& id);
	bool stopTask(const QString& id);
	void clearAllWorkers();

signals:
	void taskProgress(const QString& taskId, const QString& message);
	void taskFinished(const QString& taskId);

public:
	~AgriSatelliteDownloadManager();

private:
	AgriSatelliteDownloadManager();
	AgriSatelliteDownloadManager(const AgriSatelliteDownloadManager&) = delete;
	AgriSatelliteDownloadManager& operator=(const AgriSatelliteDownloadManager&) = delete;

	static void destroy();
	void tryStartNextTask();  // 调度
	void handleWorkerFinish(const QString& taskId);

private:
	/* 持久化任务状态
	 1.添加任务时保存任务信息；
	 2.任务完成/失败时更新状态；
	 3.程序启动时自动恢复未完成任务。
    */
	void loadTasksFromFile();
	void saveTasksToFile();
	void updateTaskStatus(const QString& taskId, TaskStatus status);
	const QString taskFilePath = "tasks.json";

private:
	static std::unique_ptr<AgriSatelliteDownloadManager>	instancePtr;
	static std::once_flag									initFlag;
	QMap<QString, AgriSatelliteDownloadWorker*>				workers;
	QQueue<DownloadTask>									pendingTasks;
	QMutex													mutex;
	int														maxConcurrentTasks = 5;  // 可配置并发上限
};

#endif  // AgriSatelliteDownloadManager_H