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
using json = nlohmann::json;

AIWebSocketClient::~AIWebSocketClient()
{

}

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

//void AIWebSocketClient::onstatechange(QAbstractSocket::SocketState s) {
//	qDebug() << "[WS] state=" << s;
//}

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

void AIWebSocketClient::onBinaryMessageReceived(const QByteArray &message)
{

	static unsigned int nT = 0;
	//qDebug() << "收到消息:" << message;

	const unsigned char *p = (const unsigned char *)message.data();
	// 🔥 创建 DetectionData 对象
	WS::DetectionData data;
	// 1. 解析图像数据
	int l = *(int*)p;
	if (l > 0)
	{
		// 		time_t tNow;
		// 		time(&tNow);
		// 		char szT[128];
		// 		tm *pTM = localtime(&tNow);
		// 		sprintf(szT, "%04d%02d%02d%02d%02d%02d%02d.jpg",
		// 			pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
		// 			pTM->tm_hour, pTM->tm_min, pTM->tm_sec, nT++ % 100);
		// 		FILE *fp = fopen(szT, "wb");
		// 		if (fp != NULL)
		// 		{
		// 			fwrite(p + 4, 1, l, fp);
		// 			fclose(fp);
		// 
		// 			// 🔥 加载图像到 data.image
		// 			bool bOK = data.image.load(szT);
		// 			remove(szT);
		// 			int kk = 0;
		// 		}
		bool bOK = data.image.loadFromData(p + 4, l);

		qDebug() << "✅ 图像加载:" << bOK << "尺寸:" << data.image.size();
		p += l;
	}
	p += 4;

	// 2. 解析JSON数据
	try {
		// 1. 将原始数据指针p转换为const char*并解析为JSON对象
		json j = json::parse((const char*)p);
		qDebug() << tr2("✅ JSON解析成功");
		// 3. 使用专门的解析器将JSON对象转换为业务数据结构
		bool parseSuccess = WS::DetectionParser::parseFromJson(j, data);

		qDebug() << tr2("解析器结果:") << parseSuccess;
		qDebug() << tr2("解析后bboxes:") << data.bboxes.size();
		qDebug() << tr2("解析后polygons:") << data.polygons.size();

		// 🔥 关键：检查是否有检测目标，如果有则记录事件
		if (!data.bboxes.empty() || !data.polygons.empty() ||
			!data.lines.empty() || !data.circles.empty()) {
			QPainter painter(&data.image);

			// 直接使用 DetectionData 创建 EventData
			for (auto box : data.bboxes)
			{
				
				WS::EventData eventData;
				eventData.image = data.image;
				eventData.time = QTime::currentTime();
				eventData.type = 0;
				eventData.videoid = data.metadata.videoid;
				QString content = tr2("AI 识别到");
				content += QString::fromStdString(box.label);
				eventData.eventcontent = content;
				LY_AgriVideoPlayer::Get().updateEventData(eventData);



				//detections.push_back(box);
				//LY_AgriVideoPlayer::Get().updateDataResult(eventData);

// 				将QImage转换为OpenCV的Mat格式，便于使用OpenCV的图像处理功能
// 								cv::Mat matFrame = m_modelProcessor->qimageToMat(data.image);
// 				
// 				
// 				
// 								// 根据BBox的坐标和尺寸创建OpenCV的矩形区域
// 								// box.bbox[0] = x坐标, box.bbox[1] = y坐标, box.bbox[2] = 宽度, box.bbox[3] = 高度
// 								cv::Rect rec((int)(box.bbox[0] * data.metadata.width), (int)(box.bbox[1] * data.metadata.height), (int)(box.bbox[2] * data.metadata.width), (int)(box.bbox[3] * data.metadata.height));
// 				
// 								// 在图像上绘制红色矩形框
// 								// matFrame: 目标图像
// 								// rec: 矩形区域
// 								// cv::Scalar(0, 0, 255): BGR颜色值（蓝色=0, 绿色=0, 红色=255）→ 红色框
// 								// 2: 线宽（2像素）
// 								cv::rectangle(matFrame, rec, cv::Scalar(0, 0, 255), 2);
// 				
// 								// 创建字符串流对象，用于格式化输出
// 								std::stringstream ss;
// 				
// 								// 设置输出格式：固定小数格式，保留2位小数
// 								ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << box.score;

				painter.setFont(QFont("Arial", 12)); // 设置字体和大小
				painter.drawRect(QRect(box.bbox[0], box.bbox[1], box.bbox[2], box.bbox[3]));
				painter.setPen(Qt::red); // 设置画笔颜色为红色
				painter.drawText(box.bbox[0], box.bbox[1], QString(box.label.c_str())); // 在指定位置绘制文字

				// 初始化标签字符串为空
				std::string label = "";

				qDebug() << "🎯 检测到目标，已记录事件:" << eventData.eventcontent;
				//eventData.image = m_modelProcessor->matToQImage(matFrame);
				LY_AgriVideoPlayer::Get().updateDataResult(eventData);
			}
			painter.end(); // 结束绘图操作，释放资源
		}
	}
	catch (const std::exception& e) {
		qDebug() << tr2("❌ JSON解析异常:") << e.what();
	}

	LY_AgriVideoPlayer::Get().updateDetectionData(data);

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

//bool StartGdd(const char *lpszHost)
//{
//	// 使用 Qt 的异步机制在主线程中启动
//	QTimer::singleShot(0, [host = std::string(lpszHost)]() {
//		qDebug() << "=== 在主线程中启动 WebSocket ===";
//
//		std::string access_token = get_access_token(host.c_str());
//		if (access_token.empty()) {
//			qDebug() << "❌ 获取 access_token 失败";
//			return;
//		}
//
//		// 注意：这里需要持久化 client，否则会被立即销毁
//		static std::unique_ptr<AIWebSocketClient> client = std::make_unique<AIWebSocketClient>();
//
//		QString url = "ws://" + QString::fromStdString(host) +
//			":9090/api/inflet/v1/task/preview?id=39ad5b59-17d8-4fc1-9595-945f45ce6e55&nodeId=JsonSerializer_v3&mode=0&token=" +
//			QString::fromStdString(access_token);
//
//		qDebug() << "连接 URL:" << url;
//		client->connectToServer(url, QString::fromStdString(access_token));
//	});
//
//	return true;
//}

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


//void sendTo192() {
//	// 创建网络管理器
//	static QNetworkAccessManager manager;
//
//	// 创建JSON对象
//	QJsonObject json;
//	json["video"] = "v1";
//	json["model"] = "m1";
//	json["enable"] = true;
//
//	// 转换为JSON字符串
//	QJsonDocument doc(json);
//	QByteArray data = doc.toJson();
//
//	// 创建网络请求
//	QNetworkRequest request;
//	request.setUrl(QUrl("http://192.168.1.138:9090"));  
//	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//
//	// 发送POST请求
//	QNetworkReply* reply = manager.post(request, data);
//
//	// 连接完成信号
//	QObject::connect(reply, &QNetworkReply::finished, [reply]() {
//		if (reply->error() == QNetworkReply::NoError) {
//			qDebug() << "✅ 发送成功到192.168.1.138";
//		}
//		else {
//			qDebug() << "❌ 发送失败:" << reply->errorString();
//		}
//		reply->deleteLater();
//	});
//}


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

bool StartAI(const char *lpszHost)
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
// 在全局变量 clients 附近添加互斥锁
static QMutex g_clientsMutex;  // 全局互斥锁
	static AIWebSocketClient* clients[5] = { nullptr };
bool StartMultiStreamAI(const char *lpszHost)
{
	QString host_qstr = QString::fromStdString(lpszHost);
	QString modelsQuery = getModelsFromXml("../data/models.xml");

	// 加锁保护 clients 数组
	QMutexLocker locker(&g_clientsMutex);

	// 定义多个视频流配置
	struct StreamConfig {
		QString streamId;
		QString rtspUrl;
		int port;
		QString description;
	};

	QList<StreamConfig> configs = {
		{ "v1", "rtsp://192.168.1.138:8554/live/qjd",               19090, "qjd" },
		/*{ "v1", "rtsp://admin:lysd2018@192.168.1.73:554/h264/ch1/main/av_stream",               19093, "av_stream" },*/
		{ "v2", "rtsp://192.168.1.138:8554/live/right_11",          19091, "right_11" },
		{ "v3", "rtsp://192.168.1.138:8554/live/AgriculturalPatrol",19092, "AgriculturalPatrol" },
		{ "v4", "rtsp://192.168.1.138:8554/live/testfire",          19093, "testfire" },
		{ "v5", "rtsp://admin:lysd2018@192.168.1.73:554/h264/ch1/main/av_stream",          19094, "av_stream" }
	};

	// 使用静态数组存储

	static QThread* threads[5] = { nullptr };

	for (int i = 0; i < configs.size(); i++) {
		const auto& config = configs[i];

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