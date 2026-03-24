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
	virtual ~AIWebSocketClient();
	void connectToServer(const QString &url, const QString &token);
	ModelProcessor* m_modelProcessor;
	cv::Mat qimageToMat(const QImage &img);
	void sendMessage(const QString &msg);
	bool SendWsMsg(const QString &id, const QByteArray &msg);
	void onMessageReceived(const QString &message);
	int getDynamicFontSize(const QImage& image);
	bool StartAI(const char *lpszHost);
	bool StartMultiStreamAI(const char *lpszHost);
	static void CleanupAllConnections();
	struct StreamConfig {
		QString streamId;
		QString rtspUrl;
		int port;
		QString description;
	};
	static void setApplicationShuttingDown();
	// ∑µªÿµ±«∞≈‰÷√
	static QList<StreamConfig> getCurrentStreamConfigs();

signals:
	void disconnected();

private slots:
	void onConnected();
	void onDisconnected();
	void onBinaryMessageReceived(const QByteArray &message);
	void onError(QAbstractSocket::SocketError error);
private:
	QWebSocket *m_webSocket;
};


#endif