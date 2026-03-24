#ifndef AGRISATELLITEDOWNLOADWORKER_H_
#define AGRISATELLITEDOWNLOADWORKER_H_
 
#include <QThread>
#include <QString>
#include <atomic>
#include <QJsonObject>

enum class TaskStatus 
{
	Pending, 
	Running, 
	Finished, 
	Failed
};

struct DownloadTask {
	DownloadTask() {};
	DownloadTask(QString sTaskId, QString sUrl, QString sSavePath, TaskStatus sStatus = TaskStatus::Pending)
		: taskId(sTaskId)
		, url(sUrl)
		, savePath(sSavePath)
		, status(sStatus)
	{};

	QString taskId;
	QString url;
	QString savePath;
	TaskStatus status = TaskStatus::Pending;

	QJsonObject toJson() const;

	static DownloadTask fromJson(const QJsonObject& obj);

	static QString statusToString(TaskStatus s);

	static TaskStatus stringToStatus(const QString& s);
};

struct DownloadProgress {
	DownloadProgress() {};
	DownloadProgress(QString sTaskId, QString sSavePath, int fileSize, int downloadSize)
		: taskId(sTaskId)
		, savePath(sSavePath)
		, fileSize(0)
		, downloadSize(0)
	{};

	QString taskId;
	QString savePath;
	int fileSize; // bytes
	int downloadSize; // bytes

	QJsonObject toJson() const;

	static DownloadProgress fromJson(const QJsonObject& obj);
};


class AgriSatelliteDownloadWorker : public QThread {
	Q_OBJECT

public:
	QString getId() const { return task.taskId; }
	bool isRunning() const { return isRunningFlag.load(); }

signals:
	void progress(const QString& msg);
	void taskFinished(const QString& taskId);

protected:
	void run() override;

private:
	explicit AgriSatelliteDownloadWorker(const DownloadTask& task, QObject* parent = nullptr);
	~AgriSatelliteDownloadWorker();

private:
	DownloadTask task;
	std::atomic<bool> isRunningFlag;

private:
	friend class AgriSatelliteDownloadManager;
};

#endif  // AgriSatelliteDownloadWorker_H