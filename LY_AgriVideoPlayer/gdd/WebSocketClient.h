#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QAbstractSocket>
#include "../LY_AgriVideoPlayer.h"

class QWebSocket;
class WebSocketClient : public QObject
{
	Q_OBJECT

public:
	WebSocketClient(QObject *parent = nullptr) : QObject(parent) {
	
	}
	virtual ~WebSocketClient();
	void connectToServer(const QString &url, const QString &token);
	ModelProcessor* m_modelProcessor;
	cv::Mat qimageToMat(const QImage &img);
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



bool StartGdd(const char *lpszHost);

#endif