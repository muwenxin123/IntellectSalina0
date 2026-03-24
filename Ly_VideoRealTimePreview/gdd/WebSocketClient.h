#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QAbstractSocket>

class QWebSocket;
class WebSocketClient : public QObject
{
	Q_OBJECT

public:
	WebSocketClient(QObject *parent = nullptr) : QObject(parent) {}
	void connectToServer(const QString &url, const QString &token);

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