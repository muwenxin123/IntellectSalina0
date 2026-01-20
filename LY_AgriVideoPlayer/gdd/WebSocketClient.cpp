#include "WebSocketClient.h"
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QDebug>
#include "httplib.h"
#include <nlohmann/json.hpp>
#include "wsProtocol.h"  // 包含新的结构体定义
#include <thread>
#include "../LY_AgriVideoPlayer.h"
#include "../modelprocessor.h"
using json = nlohmann::json;

 WebSocketClient::~WebSocketClient()
 {
 
 }
 
 void WebSocketClient::connectToServer(const QString &url, const QString &token)
 {
 	m_webSocket = new QWebSocket();
 	m_webSocket->setProxy(QNetworkProxy::NoProxy);
 	connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
 	connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
 	connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketClient::onBinaryMessageReceived);
 	connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WebSocketClient::onError);
 
 	// 设置自定义头部
 	QNetworkRequest request;
 	request.setUrl(QUrl(url));
 	request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
 
 	m_webSocket->open(request);
 }
 
 void WebSocketClient::onConnected() {
 	qDebug() << "WebSocket 连接已建立";
 }
 
 void WebSocketClient::onDisconnected() {
 	qDebug() << "WebSocket 连接已断开";
 	//QCoreApplication::quit();
 }
 
 void WebSocketClient::onBinaryMessageReceived(const QByteArray &message)
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
 		json j = json::parse((const char*)p);
 		qDebug() << tr2("✅ JSON解析成功");
 		// 使用新的解析器解析完整数据
 		bool parseSuccess = WS::DetectionParser::parseFromJson(j, data);
 		qDebug() << tr2("解析器结果:") << parseSuccess;
 		qDebug() << tr2("解析后bboxes:") << data.bboxes.size();
 		qDebug() << tr2("解析后polygons:") << data.polygons.size();
 
 		// 🔥 关键：检查是否有检测目标，如果有则记录事件
 		if (!data.bboxes.empty() || !data.polygons.empty() ||
 			!data.lines.empty() || !data.circles.empty()) {
 
 			// 直接使用 DetectionData 创建 EventData
 			for (auto box : data.bboxes)
 			{
 				WS::EventData eventData;
 				eventData.image = data.image;
 				eventData.time = QTime::currentTime();
 				eventData.type = 0;
 				QString content = tr2("GDD 识别到");
 				content += QString::fromStdString(box.label);
 				eventData.eventcontent = content;
 				LY_AgriVideoPlayer::Get().updateEventData(eventData);
 				//detections.push_back(box);
 				//LY_AgriVideoPlayer::Get().updateDataResult(eventData);
 
 				// 将QImage转换为OpenCV的Mat格式，便于使用OpenCV的图像处理功能
 				cv::Mat matFrame = m_modelProcessor->qimageToMat(data.image);
 
 
 
 				// 根据BBox的坐标和尺寸创建OpenCV的矩形区域
 				// box.bbox[0] = x坐标, box.bbox[1] = y坐标, box.bbox[2] = 宽度, box.bbox[3] = 高度
 				cv::Rect rec((int)(box.bbox[0]* data.metadata.width), (int)(box.bbox[1] * data.metadata.height), (int)(box.bbox[2] * data.metadata.width), (int)(box.bbox[3] * data.metadata.height));
 
 				// 在图像上绘制红色矩形框
 				// matFrame: 目标图像
 				// rec: 矩形区域
 				// cv::Scalar(0, 0, 255): BGR颜色值（蓝色=0, 绿色=0, 红色=255）→ 红色框
 				// 2: 线宽（2像素）
 				cv::rectangle(matFrame, rec, cv::Scalar(0, 0, 255), 2);
 
 				// 创建字符串流对象，用于格式化输出
 				std::stringstream ss;
 
 				// 设置输出格式：固定小数格式，保留2位小数
 				ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << box.score;
 
 				// 初始化标签字符串为空
 				std::string label = "";
 
 				qDebug() << "🎯 检测到目标，已记录事件:" << eventData.eventcontent;
 				eventData.image = m_modelProcessor->matToQImage(matFrame);
 				LY_AgriVideoPlayer::Get().updateDataResult(eventData);
 			}
 
 			// 发送事件到 EventRecord
 			//EventRecord::setEventData(eventData);
 			
 			
 		}
 	}
 	catch (const std::exception& e) {
 		qDebug() << tr2("❌ JSON解析异常:") << e.what();
 	}
 	LY_AgriVideoPlayer::Get().updateDetectionData(data);
 	
 }
 
 void WebSocketClient::onError(QAbstractSocket::SocketError error) {
 	qDebug() << "WebSocket 错误:" << error << m_webSocket->errorString();
 }
 
 std::string get_access_token(const char *lpszHost) {
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
 //		static std::unique_ptr<WebSocketClient> client = std::make_unique<WebSocketClient>();
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
 
 cv::Mat WebSocketClient::qimageToMat(const QImage &img)
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
 
 
 bool StartGdd(const char *lpszHost)
 {
 	// 创建独立的网络线程
 	QThread* networkThread = []() {
 		QThread* thread = new QThread();
 		thread->start();
 		return thread;
 	}();
 
 	// 在网络线程中创建 WebSocketClient
 	QTimer::singleShot(0, [host = std::string(lpszHost), networkThread]() {
 		static std::unique_ptr<WebSocketClient> client = std::make_unique<WebSocketClient>();
 		client->moveToThread(networkThread);
 
 		std::string access_token = get_access_token(host.c_str());
 		if (access_token.empty()) {
 			qDebug() << "❌ 获取 access_token 失败";
 			return;
 		}
 
 		QString url = "ws://" + QString::fromStdString(host) +
 			":9090/api/inflet/v1/task/preview?id=758aaa85-2ae2-47fa-91db-bd795ea15901&nodeId=JsonSerializer_v3&mode=0&token=" +
 			QString::fromStdString(access_token);
 
 		qDebug() << "连接 URL:" << url;
 
 		// 在网络线程中执行连接
 		QMetaObject::invokeMethod(client.get(), [client = client.get(), url, access_token]() {
 			client->connectToServer(url, QString::fromStdString(access_token));
 		}, Qt::QueuedConnection);
 	});
 
 	return true;
 }
 
