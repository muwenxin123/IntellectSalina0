#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QTimer>
#include <QDataStream>
#include <QHostInfo>
#include <QThread>

class SocketUtil : public QObject
{
    Q_OBJECT

public:
    enum SocketType {
        TCP_CLIENT,
        TCP_SERVER,
        UDP_SOCKET
    };

    // 构造函数
    explicit SocketUtil(SocketType type, QObject *parent = nullptr);

    // 析构函数
    ~SocketUtil();

    // TCP服务器相关方法
    bool startServer(quint16 port);
    void stopServer();
    QList<QTcpSocket*> getClients() const;

    // TCP客户端相关方法
    bool connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();

    // UDP相关方法
    bool bindUdp(quint16 port, const QHostAddress &address = QHostAddress::Any);
    void unbindUdp();

    // 通用方法
    bool sendData(const QByteArray &data, const QString &targetAddress = "", quint16 targetPort = 0);
    bool isConnected() const;
    QString getLocalAddress() const;
    quint16 getLocalPort() const;
    void setHeartbeatEnabled(bool enabled, int interval = 5000);
    void setReconnectEnabled(bool enabled, int interval = 3000);
    void setAutoRemoveDisconnectedClients(bool enabled);

    // 错误信息
    QString lastError() const;

signals:
    // 连接相关信号
    void connected();
	void disconnected();

    void connectionError(const QString &error);
    void clientConnected(qintptr socketDescriptor, const QString &peerAddress, quint16 peerPort);
    void clientDisconnected(qintptr socketDescriptor, const QString &peerAddress, quint16 peerPort);

    // 数据相关信号
    void dataReceived(const QByteArray &data, const QString &senderAddress, quint16 senderPort);
    void heartbeatSent();

    // 服务器状态信号
    void serverStarted(quint16 port);
    void serverStopped();

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    //void onHeartbeatTimeout();
    //void onReconnectTimeout();
    void onUdpReadyRead();

private:
    // 初始化方法
    void initTcpClient();
    void initTcpServer();
    void initUdpSocket();

    // 内部工具方法
    void clearError();
    void log(const QString &message);
    void addClientSocket(QTcpSocket *socket);
    void removeClientSocket(QTcpSocket *socket);
    void sendHeartbeat();
    void tryReconnect();

    SocketType m_type;
    QString m_lastError;

    // TCP客户端成员
    QTcpSocket *m_tcpClient;

    // TCP服务器成员
    QTcpServer *m_tcpServer;
    QList<QTcpSocket*> m_clientSockets;
    bool m_autoRemoveClients;

    // UDP成员
    QUdpSocket *m_udpSocket;

    // 心跳检测
    QTimer *m_heartbeatTimer;
    bool m_heartbeatEnabled;

    // 自动重连
    QTimer *m_reconnectTimer;
    bool m_reconnectEnabled;
    QString m_reconnectHost;
    quint16 m_reconnectPort;

    // 状态标志
    bool m_isConnected;
};

#endif // SOCKETUTIL_H
