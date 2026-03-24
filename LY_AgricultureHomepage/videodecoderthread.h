#ifndef VIDEODECODERTHREAD_H
#define VIDEODECODERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QBasicTimer>
#include <atomic>
#include <QTimerEvent>
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class VideoDecoderThread : public QThread
{
	Q_OBJECT
public:
	explicit VideoDecoderThread(QObject *parent = nullptr);
	~VideoDecoderThread();

	void switchStream(const QString &url);
	void stop();

	void setCheckInterval(int seconds);      // 设置检测间隔（秒）
	void setRetryPolicy(int maxRetries, int retryInterval); // 设置重试策略

signals:
	void frameReady(QImage frame);           // 帧就绪信号
	void connectionStatusChanged(bool connected); // 连接状态变化信号
	void errorOccurred(const QString &message); // 错误发生信号

protected:
	void run() override;
	void timerEvent(QTimerEvent *event) override;

private:
	struct StreamContext {
		AVFormatContext *formatCtx = nullptr;
		AVCodecContext *codecCtx = nullptr;
		SwsContext *swsCtx = nullptr;
		int videoStream = -1;
		qint64 lastFrameTime = 0;
	};

	// 线程控制
	std::atomic_bool m_running{ false };
	std::atomic_bool m_needSwitch{ false };
	QMutex m_mutex;

	// 流信息
	QString m_currentUrl;
	StreamContext* m_ctx = nullptr;

	// 连接检测
	QBasicTimer m_checkTimer;
	int m_checkInterval = 5;      // 默认检测间隔（秒）
	int m_maxRetries = 3;         // 最大重试次数
	int m_retryInterval = 2;      // 重试间隔（秒）
	int m_currentRetries = 0;

	// FFmpeg 方法
	bool initFFmpegContext(StreamContext* ctx);
	void freeCurrentContext();
	void safeEmitFrame(const QImage &frame);
	bool checkConnectionValidity();
	
	void handleConnectionError();
	void handleReadError(int errorCode);
	void processPacket(AVPacket &packet);

	// 网络操作
	void setupInterruptCallback(StreamContext* ctx);
	static int interruptCallback(void* ctx);
};

#endif // VIDEODECODERTHREAD_H