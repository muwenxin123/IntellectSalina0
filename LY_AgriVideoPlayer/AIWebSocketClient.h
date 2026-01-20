#ifndef ALWEBSOCKETCLIENT_H
#define ALWEBSOCKETCLIENT_H

#include <QAbstractSocket>
#include "LY_AgriVideoPlayer.h"

class QWebSocket;
namespace MyAi {
	QWebSocket;
}
class AIWebSocketClient : public QObject
{
	Q_OBJECT

public:
	//explicit AIWebSocketClient(QObject *parent = nullptr);
	//AIWebSocketClient(int windowId, const QString& videoId, QObject *parent = nullptr);

	virtual ~AIWebSocketClient();
	void connectToServer(const QString &url, const QString &token);
	ModelProcessor* m_modelProcessor;
	cv::Mat qimageToMat(const QImage &img);
	void sendMessage(const QString &msg);
	bool SendWsMsg(const QString &id, const QByteArray &msg);
	void onMessageReceived(const QString &message);


	//// 获取窗口ID和视频ID
	//int getWindowId() const { return m_windowId; }
	//QString getVideoId() const { return m_videoId; }

signals:
	void disconnected();
	//void detectionDataReceived(const WS::DetectionData& data, int windowId, const QString& videoId);

private slots:
	void onConnected();
	void onDisconnected();
	void onBinaryMessageReceived(const QByteArray &message);
	//void onstatechange(QAbstractSocket::SocketState s);
	void onError(QAbstractSocket::SocketError error);
	/*void emitjsondata();*/
private:
	QWebSocket *m_webSocket;
	//int m_windowId;  // 窗口ID
	//QString m_videoId;  // 视频流id
};



bool StartAI(const char *lpszHost);
bool StartMultiStreamAI(const char *lpszHost);
//bool StartMultipleAI(const char *lpszHost);
//bool StartSingleAI(const char *lpszHost, int windowId, const QString& videoId, const QString& customUrl = "");
#endif