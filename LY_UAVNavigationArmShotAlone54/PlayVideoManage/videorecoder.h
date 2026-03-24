#ifndef VIDEORECODER_H
#define VIDEORECODER_H

#include <QObject>
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
//#include "darknet/include/yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <atomic>



class VideoRecoder : public QObject
{
    Q_OBJECT
public:
    explicit VideoRecoder(QObject *parent = nullptr);

	bool init(int width, int height, double frameRate, const std::string& filename);
	void writeFrame(const cv::Mat& frame);
	void stop();

private:
	void encoderThreadFunc();

	struct FrameData {
		int64_t pts;
		std::shared_ptr<cv::Mat> frame;
	};

	// 智能指针类型
	using AVFormatContextPtr = std::unique_ptr<AVFormatContext, void(*)(AVFormatContext*)>;
	using AVCodecContextPtr = std::unique_ptr<AVCodecContext, void(*)(AVCodecContext*)>;
	using SwsContextPtr = std::unique_ptr<SwsContext, void(*)(SwsContext*)>;

	// 编码线程状态
	std::atomic<bool> running_{ false };
	std::thread encoderThread_;

	// 帧队列和同步
	std::queue<FrameData> frameQueue_;
	mutable std::mutex queueMutex_;
	std::condition_variable queueCV_;
	const size_t MAX_QUEUE_SIZE = 15; // 最大队列长度
	std::atomic<int> droppedFrames_{ 0 };

	// 编码器状态
	AVFormatContextPtr outFormatCtx_{ nullptr, [](AVFormatContext* ctx) {
		if (ctx) {
			if (ctx->pb) avio_closep(&ctx->pb);
			avformat_free_context(ctx);
		}
	} };
	AVCodecContextPtr outCodecCtx_{ nullptr, [](AVCodecContext* ctx) {
		if (ctx) avcodec_free_context(&ctx);
	} };
	AVStream* outStream_ = nullptr;
	SwsContextPtr swsCtx_{ nullptr, [](SwsContext* ctx) {
		if (ctx) sws_freeContext(ctx);
	} };

	// 视频参数
	int width_ = 0;
	int height_ = 0;
	double frameRate_ = 0;
	std::atomic<int64_t> frameCounter_{ 0 };

	// 日志FFmpeg错误
	void logFFmpegError(const char* context, int errnum);
signals:

public slots:
};

#endif // VIDEORECODER_H