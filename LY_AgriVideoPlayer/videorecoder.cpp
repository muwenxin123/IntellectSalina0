#include "videorecoder.h"
#include <QDebug>
#include <QThread>

VideoRecoder::VideoRecoder(QObject *parent) : QObject(parent)
{

}

bool VideoRecoder::init(int width, int height, double frameRate, const std::string& filename) {
	stop(); // 确保停止现有录制

	width_ = width;
	height_ = height;
	frameRate_ = (frameRate > 0) ? frameRate : 25.0;
	frameCounter_.store(0, std::memory_order_relaxed);
	droppedFrames_.store(0, std::memory_order_relaxed);

	// 1. 初始化输出格式上下文
	AVFormatContext* fmtCtx = nullptr;
	if (avformat_alloc_output_context2(&fmtCtx, nullptr, nullptr, filename.c_str()) < 0) {
		//qCritical() << "无法创建输出格式上下文";
		return false;
	}
	outFormatCtx_.reset(fmtCtx);

	// 2. 查找编码器
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) {
		qDebug() << "找不到H.264编码器";
		return false;
	}

	// 3. 创建输出流
	outStream_ = avformat_new_stream(outFormatCtx_.get(), codec);
	if (!outStream_) {
		qDebug() << "无法创建输出流";
		return false;
	}

	// 4. 配置编码器上下文
	AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
	if (!codecCtx) {
		qDebug() << "无法分配编码器上下文";
		return false;
	}
	outCodecCtx_.reset(codecCtx);

	outCodecCtx_->width = width_;
	outCodecCtx_->height = height_;
	outCodecCtx_->pix_fmt = AV_PIX_FMT_YUV420P;
	outCodecCtx_->time_base = av_make_q(1, static_cast<int>(frameRate_));
	//outCodecCtx_->bit_rate = 4000000; // 4 Mbps
	//outCodecCtx_->rc_max_rate = 6000000; // 峰值码率
	//outCodecCtx_->rc_buffer_size = 2000000; // 码控缓冲区
	outCodecCtx_->gop_size = static_cast<int>(frameRate_); // 1秒关键帧间隔
	outCodecCtx_->max_b_frames = 0; // 禁用B帧
	outCodecCtx_->profile = FF_PROFILE_H264_BASELINE; // 兼容性最好的profile

	// 设置编码器选项
	av_opt_set(outCodecCtx_->priv_data, "preset", "ultrafast", 0);
	av_opt_set(outCodecCtx_->priv_data, "tune", "zerolatency", 0);

	// 5. 打开编码器
	if (avcodec_open2(outCodecCtx_.get(), codec, nullptr) < 0) {
		qDebug() << "无法打开编码器";
		return false;
	}

	// 6. 复制参数到流
	if (avcodec_parameters_from_context(outStream_->codecpar, outCodecCtx_.get()) < 0) {
		qDebug() << "无法复制编码器参数";
		return false;
	}

	// 7. 打开输出文件
	if (!(outFormatCtx_->oformat->flags & AVFMT_NOFILE)) {
		if (avio_open(&outFormatCtx_->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
			qDebug() << "无法打开输出文件:" << filename.c_str();
			return false;
		}
	}

	// 8. 写入文件头
	if (avformat_write_header(outFormatCtx_.get(), nullptr) < 0) {
		qDebug() << "无法写入文件头";
		return false;
	}

	// 9. 启动编码线程
	running_ = true;
	encoderThread_ = std::thread(&VideoRecoder::encoderThreadFunc, this);

	return true;
}

void VideoRecoder::writeFrame(const cv::Mat& frame) {
	if (!running_ || frame.empty()) return;

	// 使用共享指针避免数据复制
	auto frameCopy = std::make_shared<cv::Mat>();
	frame.copyTo(*frameCopy);

	FrameData data;
	data.pts = frameCounter_.fetch_add(1, std::memory_order_relaxed);
	data.frame = frameCopy;

	{
		std::unique_lock<std::mutex> lock(queueMutex_);

		// 限制队列大小防止内存爆炸
		if (frameQueue_.size() >= MAX_QUEUE_SIZE) {
			// 丢弃旧帧，保留新帧
			frameQueue_.pop();
			droppedFrames_.fetch_add(1, std::memory_order_relaxed);
		}

		frameQueue_.push(std::move(data));
	}

	queueCV_.notify_one();
}

void VideoRecoder::encoderThreadFunc() {
	// 设置线程优先级
	QThread::currentThread()->setPriority(QThread::LowPriority);

	// 性能监控
	auto lastStatTime = std::chrono::steady_clock::now();
	int framesProcessed = 0;
	int totalDropped = 0;

	while (running_) {
		FrameData data;
		bool hasFrame = false;

		{
			std::unique_lock<std::mutex> lock(queueMutex_);

			// 使用超时避免永久阻塞
			if (queueCV_.wait_for(lock, std::chrono::milliseconds(5), [this] {
				return !frameQueue_.empty() || !running_;
			})) {
				if (!frameQueue_.empty()) {
					data = std::move(frameQueue_.front());
					frameQueue_.pop();
					hasFrame = true;
				}
			}
		}

		if (!running_) break;
		if (!hasFrame) continue;

		// 检查图像有效性
		if (!data.frame || data.frame->empty() ||
			data.frame->cols != width_ ||
			data.frame->rows != height_ ||
			data.frame->type() != CV_8UC3)
		{
			qWarning() << "丢弃无效帧";
			continue;
		}

		// 创建YUV帧
		AVFrame* yuvFrame = av_frame_alloc();
		if (!yuvFrame) {
			qCritical() << "无法分配YUV帧";
			continue;
		}

		yuvFrame->format = AV_PIX_FMT_YUV420P;
		yuvFrame->width = width_;
		yuvFrame->height = height_;
		yuvFrame->pts = data.pts;

		// 分配缓冲区
		if (av_frame_get_buffer(yuvFrame, 32) < 0) {
			qCritical() << "无法分配帧缓冲区";
			av_frame_free(&yuvFrame);
			continue;
		}

		// 创建/更新转换上下文
		if (!swsCtx_) {
			swsCtx_.reset(sws_getContext(
				width_, height_, AV_PIX_FMT_BGR24,
				width_, height_, AV_PIX_FMT_YUV420P,
				SWS_BICUBIC, nullptr, nullptr, nullptr));

			if (!swsCtx_) {
				qCritical() << "无法创建SWS上下文";
				av_frame_free(&yuvFrame);
				continue;
			}
		}

		// 转换BGR到YUV
		const uint8_t* srcData[] = { data.frame->data };
		int srcLinesize[] = { static_cast<int>(data.frame->step) };

		sws_scale(swsCtx_.get(), srcData, srcLinesize, 0,
			height_, yuvFrame->data, yuvFrame->linesize);

		// 发送到编码器
		int ret = avcodec_send_frame(outCodecCtx_.get(), yuvFrame);
		av_frame_free(&yuvFrame);

		if (ret < 0) {
			logFFmpegError("avcodec_send_frame", ret);
			continue;
		}

		// 接收并写入数据包
		AVPacket* pkt = av_packet_alloc();
		if (!pkt) continue;

		while (ret >= 0) {
			ret = avcodec_receive_packet(outCodecCtx_.get(), pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				break;
			}
			if (ret < 0) {
				logFFmpegError("avcodec_receive_packet", ret);
				break;
			}

			// 设置流索引和时间戳
			pkt->stream_index = outStream_->index;
			av_packet_rescale_ts(pkt, outCodecCtx_->time_base, outStream_->time_base);

			// 写入数据包
			if (av_interleaved_write_frame(outFormatCtx_.get(), pkt) < 0) {
				qWarning() << "写入数据包失败";
			}

			av_packet_unref(pkt);
		}

		av_packet_free(&pkt);

		// 性能统计
		framesProcessed++;
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastStatTime);

		if (elapsed.count() >= 5) {
			int currentDropped = droppedFrames_.load();
			int newDropped = currentDropped - totalDropped;
			totalDropped = currentDropped;

			qDebug() << "编码性能: "
				<< "队列大小: " << frameQueue_.size()
				<< " | 处理帧率: " << framesProcessed / elapsed.count() << " fps"
				<< " | 丢弃帧: " << newDropped;

			lastStatTime = now;
			framesProcessed = 0;
		}
	}

	// 刷新编码器
	avcodec_send_frame(outCodecCtx_.get(), nullptr);
	AVPacket* pkt = av_packet_alloc();
	if (pkt) {
		while (avcodec_receive_packet(outCodecCtx_.get(), pkt) >= 0) {
			av_interleaved_write_frame(outFormatCtx_.get(), pkt);
			av_packet_unref(pkt);
		}
		av_packet_free(&pkt);
	}

	// 写入文件尾
	av_write_trailer(outFormatCtx_.get());
}

void VideoRecoder::stop() {
	if (!running_) return;

	running_ = false;
	queueCV_.notify_all();

	if (encoderThread_.joinable()) {
		encoderThread_.join();
	}

	// 清理资源
	swsCtx_.reset();
	outCodecCtx_.reset();

	if (outFormatCtx_) {
		if (outFormatCtx_->pb) {
			avio_closep(&outFormatCtx_->pb);
		}
	}
	outFormatCtx_.reset();

	// 清空队列
	std::queue<FrameData> empty;
	std::swap(frameQueue_, empty);

	qDebug() << "录制停止，总丢弃帧数: " << droppedFrames_.load();
}

void VideoRecoder::logFFmpegError(const char* context, int errnum) {
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errnum, errbuf, sizeof(errbuf));
	qWarning() << context << "错误: " << errbuf;
}
