#include "videothread.h"

#include <QDebug>
#include <QDateTime>
#include <QElapsedTimer>
#include <QTimer>
#include <QFuture>
#include <QtConcurrent>
#include "QCoreApplication.h"
#include <QHostInfo>
#include <QProcess>
#include <QUrl>
#include <memory>
#include <thread>
#pragma execution_character_set("utf-8")

// 初始化静态成员
std::atomic<bool> VideoThread::globalInit = false;


static const int MAX_RECONNECT_DELAY = 60000; // 最大重连等待时间60秒
static const int INITIAL_RECONNECT_DELAY = 2000; // 初始重连等待2秒
static const int READ_TIMEOUT = 10000; // 读取超时时间10秒

void delayedTask(int ms) {
	QEventLoop loop;
	QTimer::singleShot(ms, &loop, &QEventLoop::quit);
	loop.exec(); // 非阻塞等待

				 // 延迟后执行的代码
	qDebug() << "Background task done";
}

VideoThread::VideoThread(QObject *parent)
	: QThread(parent)
	, modelProcessor(new ModelProcessor(this))
{
	setPriority(QThread::Priority::LowPriority);
	// 确保全局初始化只执行一次
	if (!globalInit.exchange(true)) {
		avformat_network_init();
		avdevice_register_all();
	}

	lastPacketTime = QDateTime::currentMSecsSinceEpoch();
	reconnectDelay = INITIAL_RECONNECT_DELAY;
	// 初始化其他变量
	formatCtx = nullptr;
	codecCtx = nullptr;
	frame = nullptr;
	videoStream = -1;
	running = false;
	paused = false;
	frameRate = 0.0;
	frameInterval = 0;

	segmentTimer.start();
}

VideoThread::~VideoThread()
{
	stop();
	delete modelProcessor;
}

void VideoThread::startPlay(VideoInfoData &videoInfo)
{
	stop();
	setVideoInfo(videoInfo);

	bool isSuc = openStream(videoInfo);
	if (!isSuc) {
		qWarning() << "无法开始播放:" << videoInfo.url;
		emit playbackError(tr("无法打开视频流"));

		// 设置重连标志
		running = true;

		QTimer::singleShot(3000, this, [this]() {
			if(running){
			start();
		}
	});

		// 通知需要重连
		handleReadError(AVERROR(EIO));
	}
	else {
		qInfo() << "开始播放:" << videoInfo.url;
		running = true;
		start();
	}
}

bool VideoThread::openStream(VideoInfoData &videoInfo)
{
	//	stop();
	QMutexLocker locker(&mutex);
	cleanup();

	// 更新视频信息
	m_videoInfo = videoInfo;
	rtspUrl = videoInfo.url.contains("rtsp")
		? videoInfo.url
		: QCoreApplication::applicationDirPath() + videoInfo.url;

	qInfo() << "打开视频源:" << rtspUrl;

	// 判断是否为本地文件
	isLocalFile = !rtspUrl.contains("rtsp") &&
		(rtspUrl.contains("file:") ||
			QFile::exists(rtspUrl) ||
			rtspUrl.contains("/") ||
			rtspUrl.contains("\\"));

	// 打开视频流
	AVDictionary* options = nullptr;
	if (rtspUrl.contains("rtsp")) {
		av_dict_set(&options, "rtsp_transport", "tcp", 0);
		av_dict_set(&options, "max_delay", "500000", 0); // 500ms
		av_dict_set(&options, "stimeout", "2000000", 0); // 2秒超时
	}

	// 打开视频流
	int ret = avformat_open_input(&formatCtx, rtspUrl.toUtf8().constData(), nullptr, &options);
	av_dict_free(&options);

	if (ret < 0) {
		logError("avformat_open_input", ret);
		cleanup();
		return false;
	}

	// 获取流信息
	if ((ret = avformat_find_stream_info(formatCtx, nullptr)) < 0) {
		logError("avformat_find_stream_info", ret);
		cleanup();
		return false;
	}

	// 查找视频流
	videoStream = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (videoStream < 0) {
		qCritical("未找到视频流");
		cleanup();
		return false;
	}

	// 获取解码器
	AVCodecParameters* codecPar = formatCtx->streams[videoStream]->codecpar;
	const AVCodec* codec = avcodec_find_decoder(codecPar->codec_id);
	if (!codec) {
		qCritical("找不到解码器");
		cleanup();
		return false;
	}

	// 创建解码上下文
	codecCtx = avcodec_alloc_context3(codec);
	if (!codecCtx) {
		qCritical("无法分配解码器上下文");
		cleanup();
		return false;
	}

	// 复制编解码器参数
	if ((ret = avcodec_parameters_to_context(codecCtx, codecPar)) < 0) {
		logError("avcodec_parameters_to_context", ret);
		cleanup();
		return false;
	}

	// 打开解码器
	if ((ret = avcodec_open2(codecCtx, codec, nullptr)) < 0) {
		logError("avcodec_open2", ret);
		cleanup();
		return false;
	}

	// 初始化帧和转换上下文
	frame = av_frame_alloc();
	if (!frame) {
		qCritical("无法分配帧");
		cleanup();
		return false;
	}

	// 获取视频帧率
	AVStream* stream = formatCtx->streams[videoStream];
	frameRate = av_q2d(av_guess_frame_rate(formatCtx, stream, nullptr));
	if (frameRate <= 0) {
		frameRate = 25.0; // 默认帧率
	}

	// 计算帧间隔时间（毫秒）
	frameInterval = static_cast<int>(1000.0 / frameRate);

	qDebug() << "视频帧率:" << frameRate << "fps, 帧间隔:" << frameInterval << "ms";

	return true;
}

void VideoThread::setVideoInfo(VideoInfoData &videoInfo)
{
	QMutexLocker locker(&mutex);
	m_ModelVideoInfo = videoInfo;

	QString cfg, weights;
	switch (getModelTypeWithValue(videoInfo.model_type)) {
	case ModelType_FIRE:
		cfg = "../3rdParty/bin/yolo/yolov4-tiny-forestfire.cfg";
		weights = "../3rdParty/bin/yolo/yolov4-tiny-forestfire.weights";
		break;
		//case ModelType_FLOWER:
		//	cfg = "../3rdParty/bin/yolo/yolo_repa_flower.cfg";
		//	weights = "../3rdParty/bin/yolo/yolo_repa_flower.weights";
		//	break;
		//case ModelType_PEST:
		//	cfg = "../3rdParty/bin/yolo/nongye.cfg";
		//	weights = "../3rdParty/bin/yolo/nongye.weights";
		//	break;
	case ModelType_fire:
		cfg = "../3rdParty/bin/yolo/yolov3-fire.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-fire_final.weights";
		break;
	case ModelType_front01:
		cfg = "../3rdParty/bin/yolo/yolov3-front01.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-front01_final.weights";
		break;
	case ModelType_Bridge:
		cfg = "../3rdParty/bin/yolo/yolov3-Bridge.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-Bridge_final.weights";
		break;
	case ModelType_CityManagement:
		cfg = "../3rdParty/bin/yolo/yolov3-ElectricWireFault.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-CityManagement_final.weights";
		break;
	case ModelType_ForestFireSmokeDetection:
		cfg = "../3rdParty/bin/yolo/yolov3-ForestFireSmokeDetection.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-ForestFireSmokeDetection_final.weights";
		break;
	case ModelType_InsPLAD:
		cfg = "../3rdParty/bin/yolo/yolov3-InsPLAD.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-InsPLAD_final.weights";
		break;
	case ModelType_RoadDisaster:
		cfg = "../3rdParty/bin/yolo/yolov3-RoadDisaster.cfg";
		weights = "../3rdParty/bin/yolo/yolov3-RoadDisaster_final.weights";
		break;
	default: // NONE 和 NORMAL
		cfg = "";
		weights = "";
	}

	switchModel(cfg, weights);
}

bool VideoThread::switchModel(const QString &cfgPath, const QString &weightsPath)
{
	QMutexLocker locker(&modelMutex);

	// 跳过相同的模型请求
	if (requestedCfg == cfgPath && requestedWeights == weightsPath) {
		return true;
	}

	requestedCfg = cfgPath;
	requestedWeights = weightsPath;
	return true;
}

void VideoThread::safeSwitchModel()
{
	QMutexLocker locker(&modelMutex);
	if (requestedCfg.isEmpty()) return;

	QString cfg = requestedCfg;
	QString weights = requestedWeights;
	requestedCfg.clear();
	requestedWeights.clear();

	if (modelProcessor->loadModel(cfg, weights)) {
		qInfo() << "已切换模型:" << weights;
	}
	else {
		qWarning() << "模型切换失败:" << weights;
	}
}

void VideoThread::run()
{
	AVPacket packet;
	av_init_packet(&packet);
	packet.data = nullptr;
	packet.size = 0;

	QElapsedTimer fpsTimer;
	fpsTimer.start();
	int frameCount = 0;

	// 用于控制本地视频播放速度的计时器
	QElapsedTimer frameTimer;
	frameTimer.start();

	// 主处理循环
	while (running) {
		// 检查是否超时（10秒无数据）
		if (QDateTime::currentMSecsSinceEpoch() - lastPacketTime > READ_TIMEOUT) {
			qWarning() << "视频流读取超时，触发重连";
			handleReadError(AVERROR(ETIMEDOUT));
			continue;
		}

		// 暂停处理
		if (paused) {
			QMutexLocker locker(&mutex);
			if (!condition.wait(&mutex, 100) && paused) {
				continue;
			}
		}

		// 每10帧检查模型切换
		if (frameCount % 10 == 0 && !requestedCfg.isEmpty()) {
			safeSwitchModel();
		}

		// 如果流未打开，尝试重新打开
		if (!formatCtx) {
			VideoInfoData info;
			{
				QMutexLocker locker(&mutex);
				info = m_videoInfo;
			}

			if (!openStream(info)) {
				handleReadError(AVERROR(EIO));
				continue;
			}
		}

		// 对于本地文件，控制播放速度
		if (isLocalFile && frameInterval > 0) {
			int elapsed = frameTimer.elapsed();
			if (elapsed < frameInterval) {
				QThread::msleep(frameInterval - elapsed);
			}
			frameTimer.restart();
		}

		// 读取数据包
		int ret = av_read_frame(formatCtx, &packet);
		if (ret < 0) {
			handleReadError(ret);
			av_packet_unref(&packet);
			continue;
		}

		// 更新最后收到数据包的时间
		lastPacketTime = QDateTime::currentMSecsSinceEpoch();

		// 仅处理视频流
		if (packet.stream_index != videoStream) {
			av_packet_unref(&packet);
			continue;
		}

		// 发送到解码器
		ret = avcodec_send_packet(codecCtx, &packet);
		av_packet_unref(&packet);

		if (ret < 0) {
			logError("avcodec_send_packet", ret);
			continue;
		}

		// 接收解码帧
		while (ret >= 0) {
			if (!running) break;

			ret = avcodec_receive_frame(codecCtx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				break;
			}
			if (ret < 0) {
				logError("avcodec_receive_frame", ret);
				break;
			}

			// 处理帧
			processFrame(frame);
			frameCount++;

			// 计算FPS
			if (fpsTimer.elapsed() >= 2000) {
				double fps = frameCount * 1000.0 / fpsTimer.elapsed();
				qDebug() << "FPS:" << QString::number(fps, 'f', 1);
				frameCount = 0;
				fpsTimer.restart();
			}
		}
	}

	// 清理资源
	av_packet_unref(&packet);
	cleanup();
}

void VideoThread::processFrame(AVFrame *frame)
{
	// 转换为RGB32
	SwsContext *swsCtx = sws_getContext(
		frame->width, frame->height, (AVPixelFormat)frame->format,
		frame->width, frame->height, AV_PIX_FMT_BGR24,
		SWS_BILINEAR, NULL, NULL, NULL);

	if (!swsCtx) return;

	// 创建RGB帧
	AVFrame *rgbFrame = av_frame_alloc();
	uint8_t *buffer = new uint8_t[av_image_get_buffer_size(AV_PIX_FMT_BGR24, frame->width, frame->height, 1)];
	av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer,
		AV_PIX_FMT_BGR24, frame->width, frame->height, 1);

	// 转换格式
	sws_scale(swsCtx, frame->data, frame->linesize, 0,
		frame->height, rgbFrame->data, rgbFrame->linesize);

	cv::Mat matImage(cv::Size(frame->width, frame->height), CV_8UC3);
	matImage.data = (uchar*)rgbFrame->data[0];//注意不能写为：(uchar*)pFrameBGR->data

	if (m_ModelVideoInfo.isOpen &&  modelProcessor->isModelLoaded())
	{
		modelProcessor->process(matImage, m_ModelVideoInfo);
	}

	// 检查是否需要分段
	if (recorder_ && maxSegmentDuration > 0 && segmentTimer.elapsed() >= maxSegmentDuration) {
		// 开始新分段
		startRecording(m_videoInfo.id);
	}
	// 发送到录制器（线程安全）
	if (recorder_) {
		writeVideoFrame(matImage);
	}
	QImage qImage = QImage((const unsigned char*)(matImage.data),
		matImage.cols,
		matImage.rows,
		matImage.step,
		QImage::Format_RGB888);

	QImage image = qImage.rgbSwapped().copy();
	// 发送QImage
	emit sendOneFrame(image);

	// 清理资源
	sws_freeContext(swsCtx);
	swsCtx = nullptr;
	av_frame_free(&rgbFrame);
	delete[] buffer;
}

void VideoThread::cleanup()
{
	if (codecCtx) {
		avcodec_free_context(&codecCtx);
		codecCtx = nullptr;
	}

	if (formatCtx) {
		avformat_close_input(&formatCtx);
		formatCtx = nullptr;
	}

	if (frame) {
		av_frame_free(&frame);
		frame = nullptr;
	}

	stopRecording(m_videoInfo.id);

	videoStream = -1;
	frameRate = 0.0;
	frameInterval = 0;
}

void VideoThread::stop()
{
	if (!running) return;

	running = false;
	paused = false;
	condition.wakeAll();

	if (!wait(2000)) {
		qWarning("视频线程未正常退出，强制终止");
		terminate();
		wait(500);
	}
	stopRecording(m_videoInfo.id);
	cleanup();
}

void VideoThread::pause(bool state)
{
	paused = state;
	if (!state) {
		condition.wakeAll();
	}
}

void VideoThread::logError(const char* context, int errnum)
{
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errnum, errbuf, sizeof(errbuf));
	qWarning() << context << "错误: " << errbuf;
}

void VideoThread::handleReadError(int ret)
{
	if (ret == AVERROR_EOF) {
		qInfo("视频流结束");

		// 重置播放位置
		if (av_seek_frame(formatCtx, videoStream, 0, AVSEEK_FLAG_BACKWARD) >= 0) {
			avcodec_flush_buffers(codecCtx);
		}
	}
	else {
		char errbuf[AV_ERROR_MAX_STRING_SIZE];
		av_strerror(ret, errbuf, sizeof(errbuf));
		qWarning() << "视频流错误 (" << errbuf << ")，等待" << (reconnectDelay / 1000) << "秒后重连...";

		// 使用指数退避等待
		int remaining = reconnectDelay;
		while (remaining > 0 && running) {
			QThread::msleep(2);
			remaining -= 100;
		}
		if (!running) return;

		// 尝试重新连接
		VideoInfoData info = m_videoInfo;
		cleanup();
		if (openStream(info)) {
			qInfo("重新连接成功");
			reconnectDelay = INITIAL_RECONNECT_DELAY; // 成功则重置等待时间
			lastPacketTime = QDateTime::currentMSecsSinceEpoch(); // 重置时间戳
		}
		else {
			// 失败则增加等待时间（指数退避）
			reconnectDelay = qMin(reconnectDelay * 2, MAX_RECONNECT_DELAY);
			qWarning() << "重连失败，下次等待:" << (reconnectDelay / 1000) << "秒";
		}
	}
}

/**************录制视频*********************/
void VideoThread::startRecording(const int &videoID)
{
	QMutexLocker locker(&mutex);

	// 如果已有录制器且未超时，则继续使用当前录制
	if (recorder_ && segmentTimer.elapsed() < maxSegmentDuration) {
		return;
	}

	// 停止当前录制（如果存在）
	if (recorder_) {
		recorder_->stop();
		recorder_.reset();
	}

	// 获取视频参数
	int width = codecCtx->width;
	int height = codecCtx->height;
	double frameRate = av_q2d(av_guess_frame_rate(formatCtx,
		formatCtx->streams[videoStream],
		nullptr));

	if (frameRate <= 0) frameRate = 25.0;

	// 初始化录制器
	recorder_ = std::make_unique<VideoRecoder>();

	QString filename = getNewRecordingFilename(videoID);
	// 初始化录制器
	if (!recorder_->init(width, height, frameRate, filename.toStdString())) {
		qWarning() << "无法初始化视频录制器";
		recorder_.reset();
		return;
	}
	// 重置分段计时器
	segmentTimer.restart();

}

// 设置分段时长
void VideoThread::setSegmentDuration(qint64 durationMinutes)
{
	QMutexLocker locker(&mutex);
	// 将分钟转换为毫秒
	maxSegmentDuration = durationMinutes * 60 * 1000;
	qDebug() << "设置分段时长:" << durationMinutes << "分钟";
}
void VideoThread::stopRecording(const int &videoID)
{
	//QMutexLocker locker(&mutex);

	if (recorder_) {
		recorder_->stop();
		recorder_.reset();
	}
	// 重置分段计数器
	segmentCounter = 0;
}


// 添加视频帧写入函数
void VideoThread::writeVideoFrame(const cv::Mat& bgrImage)
{
	QMutexLocker locker(&mutex);

	if (recorder_) {
		recorder_->writeFrame(bgrImage);
	}
}


QString VideoThread::getCurrentDateFolder()
{
	QDate today = QDate::currentDate();
	QString dateStr = today.toString("yyyyMMdd");
	QString folderPath = QDir(recordingPath).filePath(dateStr);

	// 创建文件夹如果不存在
	QDir dir;
	if (!dir.exists(folderPath)) {
		dir.mkpath(folderPath);
	}

	return folderPath;

}
QString VideoThread::getNewRecordingFilename(const int & videoID)
{
	QDateTime now = QDateTime::currentDateTime();
	QString folderPath = getCurrentDateFolder();
	QString timeStr = now.toString("yyyyMMdd_HHmmss") + "_" + QString::number(videoID);

	// 重置分段计数器如果日期变化
	static QDate lastDate;
	if (lastDate != now.date()) {
		segmentCounter = 0;
		lastDate = now.date();
	}

	// 生成带序号的文件名
	segmentCounter++;
	return QDir(folderPath).filePath(
		QString("recording_%1_%2.mp4").arg(timeStr).arg(segmentCounter, 3, 10, QChar('0'))
	);
}