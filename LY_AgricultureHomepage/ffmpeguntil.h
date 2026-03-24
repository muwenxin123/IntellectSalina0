#ifndef FFMPEGUNTIL_H
#define FFMPEGUNTIL_H

#define OPENCV
#include "darknet/yolo_v2_class.hpp"
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QCloseEvent>
#include <QTimer>

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


using namespace std;

class FFmpegUntil : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegUntil(QObject *parent = nullptr);
	~FFmpegUntil();

	void MyFFmpegSetUrl(QString rtspUrl);
	void MyFFmpegInfo();

	int MyFFmpegInit();
	void MyFFmpegDestroy();
	int MyFFmpepReadFrame();

	bool isLocalVideo = true;

	void startPlay(); //开始
	void stopPlay(); //停止

	volatile bool   m_stopped;				// 停止播放标识，为true时停止播放，退出播放循环
	QString          m_rtspUrl = nullptr;



protected:

	void closeEvent(QCloseEvent *e);

private:
	int              m_videoIndex;
	
	AVPicture        m_AVPicture;
	AVCodec         *m_AVCodec = nullptr;
	std::atomic<AVFormatContext*> m_AVFormatContext = nullptr;
	AVCodecContext  *m_AVCodecContext =nullptr;
	AVFrame         *m_AVFrame = nullptr;
	AVFrame         *m_AVFrameRGB = nullptr;
	AVPacket        *m_AVPacket = nullptr;
	SwsContext      *m_SwsContext = nullptr;
	uint8_t         *m_OutBuffer = nullptr;

	QTimer timer;

	std::mutex m_operationMutex; // 用于保护相关操作
 signals:
	void sendOneFrame(QImage img);

public slots:
	void doDealingMessage();

	void timerTimeout();

};

#endif // FFMPEGUNTIL_H