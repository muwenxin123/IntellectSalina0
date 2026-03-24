#include "AIWebSocketClient.h"
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QDebug>
#include "gdd/httplib.h"
#include <nlohmann/json.hpp>
#include "gdd/wsProtocol.h"  // 包含新的结构体定义
#include <thread>
#include "LY_AgriVideoPlayer.h"
#include "modelprocessor.h"
#include <QXmlStreamReader>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <atomic>

using json = nlohmann::json;

// 全局安全标志
static std::atomic<bool> g_isApplicationRunning(true);

// 提供一个函数来设置标志
void AIWebSocketClient::setApplicationShuttingDown()
{
	g_isApplicationRunning = false;
}

AIWebSocketClient::~AIWebSocketClient()
{
	if (m_webSocket) {
		m_webSocket->close();
		m_webSocket->deleteLater();
		m_webSocket = nullptr;
	}
}

// AIWebSocketClient.cpp
//AIWebSocketClient::~AIWebSocketClient()
//{
//	this->disconnect();
//	// 先断开信号槽连接，避免异步回调
//	if (m_webSocket) {
//		m_webSocket->disconnect();  // 断开所有信号槽
//
//									// 如果是打开状态，先关闭
//		if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
//			m_webSocket->close();
//			QEventLoop loop;
//			QTimer::singleShot(500, &loop, &QEventLoop::quit);
//			loop.exec();  // 等待关闭完成
//		}
//
//		m_webSocket->deleteLater();
//		m_webSocket = nullptr;
//	}
//}

void AIWebSocketClient::connectToServer(const QString &url, const QString &token)
{
	//ISelectYoloModelDialog* a = new ISelectYoloModelDialog();
	//QUrl qurl(url);
	//int port = qurl.port();
	m_webSocket = new QWebSocket();
	m_webSocket->setProxy(QNetworkProxy::NoProxy);
	connect(m_webSocket, &QWebSocket::connected, this, &AIWebSocketClient::onConnected);
	connect(m_webSocket, &QWebSocket::disconnected, this, &AIWebSocketClient::onDisconnected);
	/////////
	connect(m_webSocket, &QWebSocket::textMessageReceived,this, &AIWebSocketClient::onMessageReceived);
	//connect(m_webSocket, &QWebSocket::stateChanged, this, &AIWebSocketClient::onstatechange);
	connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &AIWebSocketClient::onBinaryMessageReceived);
	connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &AIWebSocketClient::onError);

	// 设置自定义头部
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());

	m_webSocket->open(request);
}

// 在 AIWebSocketClient 类中或相关位置
void AIWebSocketClient::onMessageReceived(const QString &message)
{
	qDebug() << "收到WebSocket消息:" << message;

	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	if (doc.isObject()) {
		QJsonObject obj = doc.object();
		QString op = obj.value("op").toString();
		qDebug() << "操作类型:" << op;

		if (op == "ack") {
			qDebug() << "操作已确认";
			QJsonObject enabled = obj.value("enabled").toObject();
			qDebug() << "当前模型状态:" << enabled;
		}
	}
}

void AIWebSocketClient::onConnected() {
	qDebug() << "WebSocket 连接已建立";
}

void AIWebSocketClient::onDisconnected() {

	qDebug() << "WebSocket 连接已断开";
	//QCoreApplication::quit();
}

//void AIWebSocketClient::onBinaryMessageReceived(const QByteArray &message)
//{
//	// 安全检查
//	if (!g_isApplicationRunning) {
//		qDebug() << "应用正在关闭，跳过消息处理";
//		return;
//	}
//
//	if (!QCoreApplication::instance()) {
//		qDebug() << "Qt应用已退出，跳过消息处理";
//		return;
//	}
//
//	// 检查消息是否有效
//	if (message.isEmpty()) {
//		qDebug() << "收到空消息";
//		return;
//	}
//	try {
//		QByteArray messageCopy = message;
//		const unsigned char *p = (const unsigned char *)messageCopy.constData();
//		WS::DetectionData data;
//		// 1. 解析图像数据
//		int l = *(int*)p;
//		if (l > 0)
//		{
//			bool bOK = data.image.loadFromData(p + 4, l);
//
//			qDebug() << "✅ 图像加载:" << bOK << "尺寸:" << data.image.size();
//			p += l;
//		}
//		p += 4;
//
//		// 2. 解析JSON数据
//		try {
//			// 1. 将原始数据指针p转换为const char*并解析为JSON对象
//			json j = json::parse((const char*)p);
//			qDebug() << tr2("✅ JSON解析成功");
//			// 3. 使用专门的解析器将JSON对象转换为业务数据结构
//			bool parseSuccess = WS::DetectionParser::parseFromJson(j, data);
//
//			qDebug() << tr2("解析器结果:") << parseSuccess;
//			qDebug() << tr2("解析后bboxes:") << data.bboxes.size();
//			qDebug() << tr2("解析后polygons:") << data.polygons.size();
//
//			// 🔥 关键：检查是否有检测目标，如果有则记录事件
//			if (!data.bboxes.empty()) {
//				QPainter painter(&data.image);
//				int totalBoxCount = data.bboxes.size();
//				// 直接使用 DetectionData 创建 EventData
//				//for (auto box : data.bboxes)
//				WS::EventData eventData;
//				
//				eventData.time = QTime::currentTime();
//				eventData.type = 0;
//				eventData.videoid = data.metadata.videoid;
//				eventData.totalBoxesInImage = totalBoxCount;
//				eventData.bboxes = data.bboxes;
//				eventData.image = data.image.copy();
//
//				for (int i=0; i <totalBoxCount; i++)
//				{
//					const auto& box = data.bboxes[i];
//					eventData.image = data.image.copy();
//					
//					//QString ttrack = QString(box.trackId);
//					//painter.setFont(QFont("Arial", 12)); // 设置字体和大小
//					//painter.drawRect(QRect(box.bbox[0], box.bbox[1], box.bbox[2], box.bbox[3]));
//					//painter.setPen(Qt::red); // 设置画笔颜色为红色
//					//painter.drawText(box.bbox[0], box.bbox[1], QString(box.label.c_str())); // 在指定位置绘制文字
//
//					// 绘制边框（加粗）
//					painter.setPen(QPen(Qt::darkBlue, 1));
//					painter.drawRect(box.bbox[0], box.bbox[1], box.bbox[2], box.bbox[3]);
//
//					// 绘制带背景的文本
//					//运用正则表达式把label中的:后面的类别名拆分出来
//					QString label = QString(box.label.c_str());
//					QRegularExpression regex1(":(.*)$");
//					QRegularExpressionMatch match1 = regex1.match(label);
//					QString result;
//					QString ttrackId = QString::number(box.trackId);
//					result = "track_id:" + ttrackId;
//					if (match1.hasMatch()) {
//						result += match1.captured(1);
//					}
//					else {
//						result += label;
//					}
//
//					QString content = tr2("AI 识别到");
//					QString sscore = QString::number(box.score, 'f', 2);
//					result += " ";
//					result += sscore;
//					content += result;
//
//					eventData.eventcontent = content;
//					LY_AgriVideoPlayer::Get().updateEventData(eventData);
//
//					int fontSize = getDynamicFontSize(data.image);
//					QFont font("Arial", fontSize, QFont::Bold);
//					painter.setFont(font);
//
//					QFontMetrics fm(font);
//					int textWidth = fm.horizontalAdvance(result);
//					//int textHeight = fm.height();
//					int textAscent = fm.ascent();      // 基线以上的高度
//					int textDescent = fm.descent();    // 基线以下的高度
//					int textRealHeight = textAscent + textDescent;  // 文本实际高度
//
//					// 绘制文本背景 
//					painter.fillRect(box.bbox[0], box.bbox[1] - textAscent,
//						textWidth + 6, textRealHeight, Qt::darkBlue);
//
//					// 绘制文本
//					painter.setPen(Qt::white);
//					painter.drawText(box.bbox[0], box.bbox[1], result);
//
//					qDebug() << "🎯 检测到目标，已记录事件:" << eventData.eventcontent;
//					//eventData.image = m_modelProcessor->matToQImage(matFrame);
//					LY_AgriVideoPlayer::Get().updateDataResult(eventData);
//					ttrackId.clear();
//				}
//				painter.end(); // 结束绘图操作，释放资源
//			}
//		}
//		catch (const std::exception& e) {
//			qDebug() << tr2("❌ JSON解析异常:") << e.what();
//		}
//
//		LY_AgriVideoPlayer::Get().updateDetectionData(data);
//	}
//	catch (const std::exception& e) {
//		qDebug() << "处理二进制消息时异常:" << e.what();
//	}
//	catch (...) {
//		qDebug() << "处理二进制消息时未知异常";
//	}
//}

void AIWebSocketClient::onBinaryMessageReceived(const QByteArray &message)
{
	// 安全检查
	if (!g_isApplicationRunning) {
		qDebug() << "应用正在关闭，跳过消息处理";
		return;
	}

	if (!QCoreApplication::instance()) {
		qDebug() << "Qt应用已退出，跳过消息处理";
		return;
	}

	// 检查消息是否有效
	if (message.isEmpty()) {
		qDebug() << "收到空消息";
		return;
	}

	try {
		QByteArray messageCopy = message;
		const unsigned char *p = (const unsigned char *)messageCopy.constData();
		WS::DetectionData data;

		// 1. 解析图像数据
		int l = *(int*)p;
		if (l > 0) {
			bool bOK = data.image.loadFromData(p + 4, l);
			qDebug() << "✅ 图像加载:" << bOK << "尺寸:" << data.image.size();
			p += l;
		}
		p += 4;

		// 2. 解析JSON数据
		try {
			json j = json::parse((const char*)p);
			bool parseSuccess = WS::DetectionParser::parseFromJson(j, data);

			qDebug() << "解析后bboxes:" << data.bboxes.size();

			// ========== 1. 处理检测目标 ==========
			if (!data.bboxes.empty()) {
				QPainter painter(&data.image);  // 直接在 data.image 上绘制
				int totalBoxCount = data.bboxes.size();

				// ========== 1. 先绘制所有框 ==========
				for (int i = 0; i < totalBoxCount; i++) {
					const auto& box = data.bboxes[i];

					// 解析类别名
					QString label = QString(box.label.c_str());
					QString className = label.contains(":") ? label.split(":").last() : label;
					QString result = QString("track_id:%1 %2 %3")
						.arg(box.trackId)
						.arg(className)
						.arg(box.score, 0, 'f', 2);

					// 绘制边框
					painter.setPen(QPen(Qt::darkBlue, 2));
					painter.drawRect(box.bbox[0], box.bbox[1], box.bbox[2], box.bbox[3]);

					// 绘制文本
					int fontSize = getDynamicFontSize(data.image);
					QFont font("Arial", fontSize, QFont::Bold);
					painter.setFont(font);

					QFontMetrics fm(font);
					int textWidth = fm.horizontalAdvance(result);
					int textAscent = fm.ascent();
					int textRealHeight = fm.height();

					painter.fillRect(box.bbox[0], box.bbox[1] - textAscent,
						textWidth + 6, textRealHeight, Qt::darkBlue);
					painter.setPen(Qt::white);
					painter.drawText(box.bbox[0], box.bbox[1], result);
				}
				painter.end();

				// ========== 2. 绘制完成后，再创建 EventData 并发送 ==========
				WS::EventData imageEventData;
				imageEventData.time = QTime::currentTime();
				imageEventData.type = 0;
				imageEventData.videoid = data.metadata.videoid;
				imageEventData.totalBoxesInImage = totalBoxCount;
				imageEventData.bboxes = data.bboxes;
				imageEventData.image = data.image.copy();  // 现在图片上已经有框了！

														   // 生成汇总事件内容
				QStringList classNames;
				for (const auto& box : data.bboxes) {
					QString label = QString(box.label.c_str());
					QString className = label.contains(":") ? label.split(":").last() : label;
					classNames << className;
				}
				imageEventData.eventcontent = QString(tr2("检测到: %1")).arg(classNames.join(", "));

				// 发送到 DataResult
				LY_AgriVideoPlayer::Get().updateDataResult(imageEventData);

				// ========== 3. 每个框单独的事件记录 ==========
				for (int i = 0; i < totalBoxCount; i++) {
					const auto& box = data.bboxes[i];
					QString label = QString(box.label.c_str());
					QString className = label.contains(":") ? label.split(":").last() : label;

					WS::EventData singleEventData;
					singleEventData.image = data.image.copy();  // 图片上已经有框了
					singleEventData.time = QTime::currentTime();
					singleEventData.type = 1;
					singleEventData.videoid = data.metadata.videoid;
					singleEventData.totalBoxesInImage = totalBoxCount;
					singleEventData.bboxes = { box };
					singleEventData.eventcontent = QString(tr2("AI识别到 %1 (置信度:%2) track_id:%3"))
						.arg(className)
						.arg(box.score, 0, 'f', 2)
						.arg(box.trackId);

					LY_AgriVideoPlayer::Get().updateEventData(singleEventData);
				}
				painter.end();
			}

			// ========== 3. 发送到视频窗口（实时显示） ==========
			LY_AgriVideoPlayer::Get().updateDetectionData(data);

		}
		catch (const std::exception& e) {
			qDebug() << "❌ JSON解析异常:" << e.what();
		}

	}
	catch (const std::exception& e) {
		qDebug() << "处理二进制消息时异常:" << e.what();
	}
	catch (...) {
		qDebug() << "处理二进制消息时未知异常";
	}
}

int AIWebSocketClient::getDynamicFontSize(const QImage& image) {
	int width = image.width();
	int height = image.height();

	//基于图像对角线（考虑长宽比）
	double diagonal = sqrt(width * width + height * height);
	int fontSize = diagonal / 125; // 对角线长度的1/125

	fontSize = std::max(8, std::min(fontSize, 30));

	qDebug() << "图像:" << width << "×" << height
		<< "，计算字体大小:" << fontSize;

	return fontSize;
}

void AIWebSocketClient::onError(QAbstractSocket::SocketError error) {
	qDebug() << "WebSocket 错误:" << error << m_webSocket->errorString();
}

static std::string get_access_token(const char *lpszHost) {
	httplib::Client cli(lpszHost, 9090);

	cli.set_connection_timeout(5);
	cli.set_read_timeout(5);

	httplib::Headers headers = {
		{ "Content-Type", "application/json" },
		{ "Accept", "application/json" }
	};

	json login_data = {
		{ "username", "admin" },
		{ "password", "123456" }
	};

	auto res = cli.Post("/api/open/v1/user/access_token", headers,
		login_data.dump(), "application/json");

	if (res && res->status == 200) {
		try {
			json response_json = json::parse(res->body);
			if (response_json.contains("access_token")) {
				std::string access_token = response_json["access_token"];
				std::cout << "成功获取 access_token" << std::endl;
				return access_token;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "解析 access_token 失败: " << e.what() << std::endl;
		}
	}
	else {
		if (res) {
			std::cerr << "HTTP 错误: " << res->status << ", 响应: " << res->body << std::endl;
		}
		else {
			std::cerr << "网络连接失败" << std::endl;
		}
	}

	return "";
}


cv::Mat AIWebSocketClient::qimageToMat(const QImage &img)
{
	switch (img.format()) {
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		return cv::Mat(img.height(), img.width(), CV_8UC4,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	case QImage::Format_RGB888:
		return cv::Mat(img.height(), img.width(), CV_8UC3,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	case QImage::Format_Grayscale8:
		return cv::Mat(img.height(), img.width(), CV_8UC1,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	default:
		QImage converted = img.convertToFormat(QImage::Format_RGB888);
		return cv::Mat(converted.height(), converted.width(), CV_8UC3,
			const_cast<uchar*>(converted.bits()),
			static_cast<size_t>(converted.bytesPerLine()));
	}
}


// 新增函数：从XML文件读取模型名并格式化为查询字符串
QString getModelsFromXml(const QString& xmlFilePath) {
	QFile file(xmlFilePath);
	if (!file.exists()) {
		qDebug() << "❌ XML文件不存在:" << xmlFilePath;
		return "best.pt,wight.pt"; // 返回默认值
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "❌ 无法打开XML文件:" << xmlFilePath;
		return "best.pt,wight.pt";
	}

	QXmlStreamReader xml(&file);
	QStringList modelNames;

	while (!xml.atEnd() && !xml.hasError()) {
		QXmlStreamReader::TokenType token = xml.readNext();

		if (token == QXmlStreamReader::StartElement) {
			if (xml.name() == QString("model")) {
				xml.readNext(); // 移动到文本节点
				if (xml.tokenType() == QXmlStreamReader::Characters) {
					QString modelName = xml.text().toString().trimmed();
					if (!modelName.isEmpty() &&
						(modelName.endsWith(".pt", Qt::CaseInsensitive) ||
							modelName.endsWith(".pth", Qt::CaseInsensitive))) {
						modelNames.append(modelName);
					}
				}
			}
		}
	}

	file.close();

	if (xml.hasError()) {
		qDebug() << "❌ XML解析错误:" << xml.errorString();
		return "best.pt,wight.pt";
	}

	if (modelNames.isEmpty()) {
		qDebug() << "⚠️ XML文件中未找到模型名，使用默认值";
		return "best.pt,wight.pt";
	}

	// 排序并去重
	modelNames.sort();
	modelNames.removeDuplicates();

	QString result = modelNames.join(",");
	qDebug() << "✅ 从XML读取" << modelNames.size() << "个模型:" << result;

	return result;
}

//bool StartAI(const char *lpszHost)
//{
//	// 创建独立的网络线程
//	QThread* networkThread = []() {
//		QThread* thread = new QThread();
//		thread->start();
//		return thread;
//	}();
//
//	// 在网络线程中创建 AIWebSocketClient
//	QTimer::singleShot(0, [host = std::string(lpszHost), networkThread]() {
//		static std::unique_ptr<AIWebSocketClient> client = std::make_unique<AIWebSocketClient>();
//		client->moveToThread(networkThread);
//
//		//std::string access_token = get_access_token(host.c_str());
//		//if (access_token.empty()) {
//		//	qDebug() << "❌ 获取 access_token 失败";
//		//	return;
//		//}
//		// 在网络线程中执行连接
//		std::string access_token = "";
//		QString url = "ws://" + QString::fromStdString(host) + ":9000/" + QString::fromStdString(access_token);
//		qDebug() << "连接 URL:" << url;
//
//		QMetaObject::invokeMethod(client.get(), [client = client.get(), url, access_token]() {
//			client->connectToServer(url, QString::fromStdString(access_token));
//		}, Qt::QueuedConnection);
//	});
//
//	return true;
//}

bool AIWebSocketClient::StartAI(const char *lpszHost)
{
	// 创建独立的网络线程
	QThread* networkThread = []() {
		QThread* thread = new QThread();
		thread->start();
		return thread;
	}();

	// 在网络线程中创建 AIWebSocketClient
	QTimer::singleShot(0, 
		[host = std::string(lpszHost), networkThread]() 
	{
		static std::unique_ptr<AIWebSocketClient> client = std::make_unique<AIWebSocketClient>();
		client->moveToThread(networkThread);


		// 构建正确的WebSocket URL
		QString host_qstr = QString::fromStdString(host);
		//QString rtspUrl = QString("rtsp://%1:8554/live/testfire").arg(host_qstr);
		/*QString rtspUrl = QString("rtsp://192.168.1.138:8554/live/right_11");*/
		QString rtspUrl = QString("rtsp://admin:lysd2018@192.168.1.73:554/h264/ch1/main/av_stream");
	
		QString encodedRtspUrl = QUrl::toPercentEncoding(rtspUrl);

		QString modelurl = "best.pt";
		/*QString wsUrl = QString("ws://%1:19091/ws/rtsp?url=%2&url_input=rtsp/server&models=aircraft.pt&stride=1&only_when_detected=false&jpeg_quality=95")*/
		QString wsUrl = QString("ws://%1:19091/ws/rtsp?url=%2&url_input=rtsp/server&models=aircraft.pt&stride=1&only_when_detected=false&jpeg_quality=95")
		.arg(host_qstr)
		.arg(encodedRtspUrl);

		qDebug() << "连接 URL:" << wsUrl;


		// 在网络线程中执行连接
		QMetaObject::invokeMethod(client.get(),
			[client = client.get(), wsUrl]() {
			// 注意：FastAPI不需要token，所以传空字符串
			client->connectToServer(wsUrl, "");
		},
			Qt::QueuedConnection
			);
	}
	);

	return true;
}





// 使用静态数组存储
// 在全局变量 clients 附近添加互斥锁
static QMutex g_clientsMutex;  // 全局互斥锁
static AIWebSocketClient* clients[5] = { nullptr };
static QThread* threads[5] = { nullptr };
static QList<AIWebSocketClient::StreamConfig> g_currentConfigs;
QList<AIWebSocketClient::StreamConfig> AIWebSocketClient::getCurrentStreamConfigs()
{
	QMutexLocker locker(&g_clientsMutex);
	return g_currentConfigs;
}
bool AIWebSocketClient::StartMultiStreamAI(const char *lpszHost)
{
	QString host_qstr = QString::fromStdString(lpszHost);
	QString modelsQuery = getModelsFromXml("../data/models.xml");

	// 加锁保护 clients 数组
	QMutexLocker locker(&g_clientsMutex);

	//// 定义多个视频流配置
	//struct StreamConfig {
	//	QString streamId;
	//	QString rtspUrl;
	//	int port;
	//	QString description;
	//};

	//QList<StreamConfig> configs = {
	//	{ "v1", "rtsp://192.168.1.138:8554/live/qjd",               19090, "qjd" },
	//	{ "v2", "rtsp://192.168.1.138:8554/live/right_11",          19091, "right_11" },
	//	{ "v3", "rtsp://192.168.1.138:8554/live/AgriculturalPatrol",19092, "AgriculturalPatrol" },
	//	//{ "v4", "rtsp://192.168.1.138:8554/live/testfire",          19093, "testfire" },
	//	{ "v4", "rtsp://192.168.1.119/554",          19093, "554" },
	//	{ "v5", "rtsp://admin:lysd2018@192.168.1.72:554/h264/ch1/main/av_stream",          19094, "av_stream" }
	//	//{ "v5", "rtsp://admin:123456@192.168.1.122/stream0",          19094, "stream0" }
	//};

	g_currentConfigs = {
			{ "v1", "rtsp://192.168.1.138:8554/live/qjd",               19090, "qjd" },
			{ "v2", "rtsp://192.168.1.138:8554/live/right_11",          19091, "right_11" },
			{ "v3", "rtsp://192.168.1.138:8554/live/AgriculturalPatrol",19092, "AgriculturalPatrol" },
			{ "v4", "rtsp://192.168.1.138:8554/live/testfire",          19093, "testfire" },
			//{ "554", "rtsp://192.168.1.119/554",          19093, "/*554*/" },
			{ "av_stream", "rtsp://admin:lysd2018@192.168.1.72:554/h264/ch1/main/av_stream",          19094, "av_stream" }
			//{ "v5", "rtsp://admin:123456@192.168.1.122/stream0",          19094, "stream0" }
		};




	for (int i = 0; i < g_currentConfigs.size(); i++) {
		const auto& config = g_currentConfigs[i];

		// 清理之前的连接（如果存在）
		if (threads[i]) {
			threads[i]->quit();
			threads[i]->wait();
			delete threads[i];
		}

		if (clients[i]) {
			clients[i]->deleteLater();
		}

		// 创建新连接
		threads[i] = new QThread();
		threads[i]->start();

		clients[i] = new AIWebSocketClient();
		clients[i]->moveToThread(threads[i]);

		// 设置流标识
		clients[i]->setProperty("streamId", config.streamId);

		QUrl wsUrl;
		wsUrl.setScheme("ws");
		wsUrl.setHost(host_qstr);
		wsUrl.setPort(config.port);
		wsUrl.setPath("/ws/rtsp");

		QUrlQuery query;
		query.addQueryItem("url", config.rtspUrl);
		query.addQueryItem("url_input", "rtsp/server");
		//query.addQueryItem("models", "helmet.pt,landing_site.pt,oiltank.pt");
		query.addQueryItem("tracking", "true");
		query.addQueryItem("models", modelsQuery);
		query.addQueryItem("stride", "1");
		query.addQueryItem("only_when_detected", "false");
		query.addQueryItem("jpeg_quality", "95");

		wsUrl.setQuery(query);

		QString wsUrlString = wsUrl.toString();

		qDebug() << "========================================";
		qDebug() << "启动视频流:" << config.streamId;
		qDebug() << "描述:" << config.description;
		qDebug() << "RTSP地址:" << config.rtspUrl;
		qDebug() << "服务端口:" << config.port;
		qDebug() << "WebSocket URL:" << wsUrl;
		qDebug() << "========================================";
		
		// 在线程中建立连接
		QMetaObject::invokeMethod(clients[i],
			[client = clients[i], wsUrlString, streamId = config.streamId]() {
			client->connectToServer(wsUrlString, "");
			qDebug() << "✅ 视频流" << streamId << "连接成功";
		},
			Qt::QueuedConnection
			);
	}

	return true;
}


void AIWebSocketClient::sendMessage(const QString &msg)
{
	// 添加详细的日志和检查
	qDebug() << "[sendMessage] 开始发送消息，线程:" << QThread::currentThread();
	qDebug() << "[sendMessage] 消息内容:" << msg;

	if (m_webSocket != NULL)
	{
		m_webSocket->sendTextMessage(msg);
	}
}

bool AIWebSocketClient::SendWsMsg(const QString &id, /*const QString &msg*/ const QByteArray &msg)
{
	if (id == "v1")
	{
		clients[0]->sendMessage(msg);
	}
	if (id == "v2")
	{
		clients[1]->sendMessage(msg);
	}
	if (id == "v3")
	{
		clients[2]->sendMessage(msg);
	}
	if (id == "v4")
	{
		clients[3]->sendMessage(msg);
	}
	if (id == "v5")
	{
		clients[4]->sendMessage(msg);
	}
	return true;
}

//void AIWebSocketClient::CleanupAllConnections()
//{
//
//	// 1. 先关闭所有 WebSocket 连接
//	for (int i = 0; i < 5; ++i) {
//		if (clients[i]) {
//			// 发送关闭请求
//			clients[i]->m_webSocket->disconnect();
//			clients[i]->m_webSocket->close();
//			clients[i]->m_webSocket->deleteLater();
//			clients[i]->m_webSocket = nullptr;
//
//			// 停止事件处理
//			clients[i]->deleteLater();
//			clients[i] = nullptr;
//		}
//	}
//
//	// 2. 等待线程安全退出
//	for (int i = 0; i < 5; ++i) {
//		if (threads[i]) {
//			threads[i]->quit();
//			threads[i]->wait(2000);  // 等待1秒
//			delete threads[i];
//			threads[i] = nullptr;
//		}
//	}
//}

void AIWebSocketClient::CleanupAllConnections()
{
    qDebug() << "开始清理所有连接...";
    
    // 1. 立即设置退出标志（阻止新的 onBinaryMessageReceived 执行）
    setApplicationShuttingDown();
    
    // 2. 立即关闭所有 WebSocket 连接（阻止接收新消息）
    for (int i = 0; i < 5; ++i) {
        if (clients[i] && clients[i]->m_webSocket) {
            qDebug() << "立即关闭 WebSocket" << i;
            
            // 断开所有信号连接（包括 binaryMessageReceived）
            clients[i]->m_webSocket->disconnect();
            
            // 立即关闭 socket
            clients[i]->m_webSocket->abort();  // 使用 abort() 而不是 close()
            clients[i]->m_webSocket->deleteLater();
        }
    }
    
    // 3. 给正在执行的 onBinaryMessageReceived 一点时间完成
    QThread::msleep(500);  // 等待500ms
    
    // 4. 现在可以安全删除客户端
    for (int i = 0; i < 5; ++i) {
        if (clients[i]) {
            clients[i]->deleteLater();
            clients[i] = nullptr;
        }
        
        if (threads[i]) {
            threads[i]->quit();
            threads[i] = nullptr;
        }
    }
    
    qDebug() << "清理完成";
}