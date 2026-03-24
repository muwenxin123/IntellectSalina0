#include "VideoDecoderThread.h"
#include <QDateTime>
#include <QDebug>

// 全局中断标记（用于超时检测）
static std::atomic_bool g_interruptFlag{ false };

VideoDecoderThread::VideoDecoderThread(QObject *parent)
	: QThread(parent)
{
	avformat_network_init();
}

VideoDecoderThread::~VideoDecoderThread()
{
	stop();
	wait(2000);
	freeCurrentContext();
	avformat_network_deinit();
}

void VideoDecoderThread::switchStream(const QString &url)
{
	QMutexLocker lock(&m_mutex);
	if (url != m_currentUrl) {
		m_currentUrl = url;
		m_needSwitch.store(true);
		m_currentRetries = 0;  // 重置重试计数
	}
}

void VideoDecoderThread::stop()
{
	m_running.store(false);
}

void VideoDecoderThread::setCheckInterval(int seconds)
{
	QMutexLocker lock(&m_mutex);
	m_checkInterval = qBound(1, seconds, 60);
	if (m_checkTimer.isActive()) {
		m_checkTimer.start(m_checkInterval * 1000, this);
	}
}

void VideoDecoderThread::setRetryPolicy(int maxRetries, int retryInterval)
{
	QMutexLocker lock(&m_mutex);
	m_maxRetries = qMax(1, maxRetries);
	m_retryInterval = qMax(1, retryInterval);
}

// 主线程循环
void VideoDecoderThread::run()
{
	m_running.store(true);
	m_checkTimer.start(m_checkInterval * 1000, this);

	while (m_running) {
		// 处理流切换请求
		if (m_needSwitch.exchange(false)) {
			freeCurrentContext();
			emit connectionStatusChanged(false);

			StreamContext* newCtx = new StreamContext();
			if (initFFmpegContext(newCtx)) {
				QMutexLocker lock(&m_mutex);
				m_ctx = newCtx;
				m_currentRetries = 0;
				emit connectionStatusChanged(true);
			}
			else {
				delete newCtx;
				handleConnectionError();
			}
		}

		// 解码处理
		if (m_ctx && m_ctx->formatCtx) {
			AVPacket packet;
			int ret = av_read_frame(m_ctx->formatCtx, &packet);

			if (ret >= 0) {
				processPacket(packet);
				av_packet_unref(&packet);
				m_ctx->lastFrameTime = QDateTime::currentSecsSinceEpoch();
			}
			else if (ret == AVERROR(EAGAIN)) {
				QThread::msleep(10);
			}
			else {
				handleReadError(ret);
			}
		}
		else {
			QThread::msleep(100);
		}
	}

	freeCurrentContext();
	m_checkTimer.stop();
}

// 定时检测事件
void VideoDecoderThread::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_checkTimer.timerId()) {
		if (!checkConnectionValidity()) {
			qWarning() << "Connection validation failed, triggering reconnect";
			QMutexLocker lock(&m_mutex);
			m_needSwitch.store(true);
		}
	}
	QThread::timerEvent(event);
}

// 初始化FFmpeg上下文
bool VideoDecoderThread::initFFmpegContext(StreamContext* ctx)
{
	// 网络参数设置
	AVDictionary* options = nullptr;
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	av_dict_set(&options, "stimeout", "5000000", 0); // 5秒超时

													 // 打开输入流
	g_interruptFlag.store(false);
	if (ctx == nullptr)
	{
		return false;
	}
	//setupInterruptCallback(ctx);

	int ret = avformat_open_input(&ctx->formatCtx, m_currentUrl.toUtf8().constData(), nullptr, &options);
	av_dict_free(&options);
	if (ret != 0) {
		emit errorOccurred(tr("Failed to open input stream"));
		return false;
	}

	// 查找流信息
	if ((ret = avformat_find_stream_info(ctx->formatCtx, nullptr)) < 0) {
		emit errorOccurred(tr("Failed to find stream info"));
		return false;
	}

	// 查找视频流
	ctx->videoStream = -1;
	for (unsigned int i = 0; i < ctx->formatCtx->nb_streams; i++) {
		if (ctx->formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			ctx->videoStream = i;
			break;
		}
	}

	if (ctx->videoStream == -1) {
		emit errorOccurred(tr("No video stream found"));
		return false;
	}

	// 初始化解码器
	AVCodecParameters* codecPar = ctx->formatCtx->streams[ctx->videoStream]->codecpar;
	const AVCodec* codec = avcodec_find_decoder(codecPar->codec_id);
	if (!codec) {
		emit errorOccurred(tr("Unsupported codec"));
		return false;
	}

	ctx->codecCtx = avcodec_alloc_context3(codec);
	if ((ret = avcodec_parameters_to_context(ctx->codecCtx, codecPar)) < 0) {
		emit errorOccurred(tr("Failed to init codec context"));
		return false;
	}

	if ((ret = avcodec_open2(ctx->codecCtx, codec, nullptr)) < 0) {
		emit errorOccurred(tr("Failed to open codec"));
		return false;
	}

	// 初始化图像转换器
	ctx->swsCtx = sws_getContext(
		codecPar->width, codecPar->height, ctx->codecCtx->pix_fmt,
		codecPar->width, codecPar->height, AV_PIX_FMT_RGB32,
		SWS_BILINEAR, nullptr, nullptr, nullptr
	);

	return true;
}

void VideoDecoderThread::freeCurrentContext()
{
	QMutexLocker lock(&m_mutex);
	if (m_ctx) {
		if (m_ctx->formatCtx) avformat_close_input(&m_ctx->formatCtx);
		if (m_ctx->codecCtx) {
			avcodec_close(m_ctx->codecCtx);
			avcodec_free_context(&m_ctx->codecCtx);
		}
		if (m_ctx->swsCtx) sws_freeContext(m_ctx->swsCtx);
		delete m_ctx;
		m_ctx = nullptr;
	}
}

bool VideoDecoderThread::checkConnectionValidity()
{
	QMutexLocker lock(&m_mutex);
	if (!m_ctx) return false;

	// 最近收到帧的时间检查
	const qint64 currentTime = QDateTime::currentSecsSinceEpoch();
	if (currentTime - m_ctx->lastFrameTime > m_checkInterval * 2) {
		return false;
	}

	// 主动验证流状态
	AVStream* stream = m_ctx->formatCtx->streams[m_ctx->videoStream];
	const qint64 duration = stream->duration * av_q2d(stream->time_base);
	return duration > 0 || stream->nb_frames > 0;
}

void VideoDecoderThread::setupInterruptCallback(StreamContext* ctx)
{
	ctx->formatCtx->interrupt_callback.callback = &VideoDecoderThread::interruptCallback;
	ctx->formatCtx->interrupt_callback.opaque = &g_interruptFlag;
}

int VideoDecoderThread::interruptCallback(void* ctx)
{
	return g_interruptFlag.load() ? 1 : 0;
}

// 错误处理相关方法
void VideoDecoderThread::handleReadError(int errorCode)
{
	switch (errorCode) {
	case AVERROR_EOF:
		qDebug() << "End of stream";
		emit errorOccurred(tr("End of stream"));
		break;
	case AVERROR(EIO):
		qDebug() << "I/O error occurred";
		emit errorOccurred(tr("I/O error occurred"));
		break;
	default:
		qDebug() << "Unknown read error";
		emit errorOccurred(tr("Unknown read error"));
	}

	QThread::sleep(m_retryInterval);
	m_needSwitch.store(true);
}

void VideoDecoderThread::handleConnectionError()
{
	if (++m_currentRetries <= m_maxRetries) {
		qWarning() << "Reconnecting attempt" << m_currentRetries << "/" << m_maxRetries;
		QThread::sleep(m_retryInterval * m_currentRetries); // 指数退避
		m_needSwitch.store(true);
	}
	else {
		emit errorOccurred(tr("Maximum retries reached"));
		stop();
	}
}

// 解码处理（线程安全的帧发射）
void VideoDecoderThread::safeEmitFrame(const QImage &frame)
{
	QMetaObject::invokeMethod(this, [this, frame]() {
		emit frameReady(frame);
	}, Qt::QueuedConnection);
}

void VideoDecoderThread::processPacket(AVPacket &packet)
{
	if (packet.stream_index != m_ctx->videoStream) return;

	int ret = avcodec_send_packet(m_ctx->codecCtx, &packet);
	if (ret < 0) return;

	AVFrame* frame = av_frame_alloc();
	while ((ret = avcodec_receive_frame(m_ctx->codecCtx, frame)) >= 0) {
		QImage image(frame->width, frame->height, QImage::Format_RGB32);
		uint8_t* dest[] = { image.bits() };
		int destLinesize[] = { static_cast<int>(image.bytesPerLine()) };

		sws_scale(m_ctx->swsCtx,
			frame->data, frame->linesize,
			0, frame->height,
			dest, destLinesize);

		safeEmitFrame(image);
		av_frame_unref(frame);
	}
	av_frame_free(&frame);
}