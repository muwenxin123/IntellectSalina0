#include "WebSocketClient.h"
#include <QWebSocket>
#include <QTimer>
#include <QDebug>
#include <gdd/httplib.h>
#include <nlohmann/json.hpp>
#include "wsProtocol.h"  // 包含新的结构体定义
#include <thread>

using json = nlohmann::json;

void WebSocketClient::connectToServer(const QString &url, const QString &token)
{
	m_webSocket = new QWebSocket();
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
		time_t tNow;
		time(&tNow);
		char szT[128];
		tm *pTM = localtime(&tNow);
		sprintf(szT, "%04d%02d%02d%02d%02d%02d%02d.jpg",
			pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
			pTM->tm_hour, pTM->tm_min, pTM->tm_sec, nT++ % 100);
		FILE *fp = fopen(szT, "wb");
		if (fp != NULL)
		{
			fwrite(p + 4, 1, l, fp);
			fclose(fp);

			// 🔥 加载图像到 data.image
			bool bOK = data.image.load(szT);
			remove(szT);
			int kk = 0;
		}
		//bool bOK = data.image.loadFromData(p + 4, 1, "JPEG");
		p += l;
	}
	p += 4;

	// 2. 解析JSON数据
	try {
		json j = json::parse((const char*)p);

		// 使用新的解析器解析完整数据
		data = WS::DetectionParser::parseFromJson(j);

		// 🔥 关键：将所有数据更新到管理类
		WS::DetectionManager::instance().updateData(data);
	
	}
	catch (const std::exception& e) {
	}
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

bool StartGdd(const char *lpszHost)
{
	new std::thread([&lpszHost]()
	{
		std::string access_token = get_access_token(lpszHost);
		if (access_token.empty()) {
			return false;
		}

		WebSocketClient client;

		// 构建 URL
		QString url = "ws://";
		url += lpszHost;
		url += ":9090/api/inflet/v1/task/preview?id=43720508-6eba-46f9-833e-f44f602e19b1&nodeId=JsonSerializer_v3&mode=0&token=";
		url += QString::fromStdString(access_token);
		client.connectToServer(url, QString::fromStdString(access_token));
	});
	return true;
}
