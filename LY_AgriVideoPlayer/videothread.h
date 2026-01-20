#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QImage>
#include <QWaitCondition>
#include <QThread>
#include <QElapsedTimer>
#include "modelprocessor.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include "libavutil/imgutils.h"
#include <libavutil/time.h>
}
#define OPENCV
//#include "darknet/include/yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <direct.h>
#include <io.h>
//#include "darknet/PutTextZH.h"
#include <iosfwd>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "videoconfig.h"
#include "videorecoder.h"

#include <QTimer>
#include <QReadWriteLock>



#include "ISelectYoloModelDialog.h"

using namespace std;


class VideoThread : public QThread
{
	Q_OBJECT

public:
	explicit VideoThread(QObject *parent = nullptr);
	virtual ~VideoThread();

    void setUavID(QString uavID);

	// 视频控制接口
	void startPlay(VideoInfoData &videoInfo);
	void stop();
	void pause(bool state);
	void startRecording(const int &videoID);
	void stopRecording(const int &videoID);
	void setSegmentDuration(qint64 durationMinutes);

	// 模型管理
	void setVideoInfo(VideoInfoData &videoInfo);

signals:
	// 发送解码后的视频帧
	void sendOneFrame(QImage image);
	// 播放错误信号
	void playbackError(const QString &message);

protected:
	void run() override;


private:
    //////////////////////////////////////////////////////////////////////////
    QString CurrUavID;
    QString m_yoloPath = "./Data/Model/";
    QTimer* m_timer = nullptr;
    mutable QReadWriteLock m_lock;
    std::map<QString, YoloModelInfo> m_YoloModelInfoMap;
    void onGetDataTimeout();
    void startLoadYoloModel();
    std::map<QString, ModelProcessor*> m_modelProcessMap;
   

	qint64 lastPacketTime; // 上次成功读到数据包的时间
	int reconnectDelay = 500;   // 当前重连等待时间（毫秒）
	
	void writeVideoFrame(const cv::Mat& bgrImage);
	QString getNewRecordingFilename(const int &videoID);
	QString getCurrentDateFolder() ;
	//void cleanupVideoWriter();

	std::unique_ptr<VideoRecoder> recorder_;


	QString recordingPath = "../data/Record/recordings"; // 默认录制路径

	QElapsedTimer segmentTimer;      // 当前分段计时器
	qint64 maxSegmentDuration = 0;   // 每个分段的最大时长(毫秒)
	int segmentCounter = 0;          // 当前分段计数器

private:
	// 视频流操作
	bool openStream(VideoInfoData &videoInfo);
	void cleanup();
	void logError(const char* context, int errnum);
	void handleReadError(int ret);

	double frameRate;
	double frameInterval;
	bool isLocalFile;

	QString rtspUrl = "";

	// 模型操作
	bool switchModel(const QString &cfgPath, const QString &weightsPath);
	void safeSwitchModel();

	// 帧处理
	void processFrame(AVFrame *frame);

	// FFmpeg 资源
	AVFormatContext* formatCtx = nullptr;
	AVCodecContext* codecCtx = nullptr;
	AVFrame* frame = nullptr;          // 解码帧
	//SwsContext* swsCtx = nullptr;      // 图像转换上下文
	int videoStream = -1;               // 视频流索引

	// 状态控制
	std::atomic<bool> running{ false };   // 运行标志
	std::atomic<bool> paused{ false };    // 暂停标志
	mutable QMutex mutex;               // 状态互斥锁
	QWaitCondition condition;           // 暂停条件变量

	// 模型处理
	ModelProcessor* modelProcessor;     // 模型处理器
	QMutex modelMutex;                  // 模型互斥锁
	QString requestedCfg;               // 请求的模型配置文件
	QString requestedWeights;           // 请求的模型权重文件

	// 视频信息
	VideoInfoData m_ModelVideoInfo;     // 当前模型配置
	VideoInfoData m_videoInfo;          // 当前视频源信息

	// 转换上下文状态
	int currentWidth = 0;               // 当前帧宽度
	int currentHeight = 0;              // 当前帧高度
	int currentPixFmt = 0;              // 当前像素格式

	// 全局初始化标志
	static std::atomic<bool> globalInit;
};


#endif // VIDEOTHREAD_H
