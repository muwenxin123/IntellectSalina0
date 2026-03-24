#include "SocketUtil.h"
#include <QDebug>


SocketUtil::SocketUtil(SocketType type, QObject *parent)
    : QObject(parent),
      m_type(type),
      m_tcpClient(nullptr),
      m_tcpServer(nullptr),
      m_udpSocket(nullptr),
      m_heartbeatTimer(nullptr),
      m_reconnectTimer(nullptr),
      m_autoRemoveClients(true),
      m_heartbeatEnabled(false),
      m_reconnectEnabled(false),
      m_isConnected(false)
{
    clearError();

    // 根据类型初始化
    switch (m_type) {
    case TCP_CLIENT:
        initTcpClient();
        break;
    case TCP_SERVER:
        initTcpServer();
        break;
    case UDP_SOCKET:
        initUdpSocket();
        break;
    }

    // 初始化心跳定时器
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(false);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &SocketUtil::sendHeartbeat);

    // 初始化重连定时器
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(false);
    connect(m_reconnectTimer, &QTimer::timeout, this, &SocketUtil::tryReconnect);
}

SocketUtil::~SocketUtil()
{
    // 清理资源
    stopServer();
    disconnectFromHost();
    unbindUdp();

    if (m_heartbeatTimer) {
        m_heartbeatTimer->stop();
        delete m_heartbeatTimer;
    }

    if (m_reconnectTimer) {
        m_reconnectTimer->stop();
        delete m_reconnectTimer;
    }
}

// ==================== TCP服务器方法 ====================

bool SocketUtil::startServer(quint16 port)
{
    if (m_type != TCP_SERVER) {
        m_lastError = "Operation only available for TCP server";
        return false;
    }

    if (m_tcpServer->isListening()) {
        m_lastError = "Server is already running";
        return false;
    }

    clearError();

    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        m_lastError = m_tcpServer->errorString();
        return false;
    }

    m_isConnected = true;
    emit serverStarted(m_tcpServer->serverPort());
    return true;
}

void SocketUtil::stopServer()
{
    if (m_type != TCP_SERVER || !m_tcpServer->isListening()) {
        return;
    }

    // 关闭所有客户端连接
    for (QTcpSocket *client : qAsConst(m_clientSockets)) {
        client->disconnectFromHost();
        if (client->state() != QAbstractSocket::UnconnectedState) {
            client->waitForDisconnected(1000);
        }
        client->deleteLater();
    }
    m_clientSockets.clear();

    // 停止服务器
    m_tcpServer->close();
    m_isConnected = false;
    emit serverStopped();
}

QList<QTcpSocket*> SocketUtil::getClients() const
{
    return m_clientSockets;
}

// ==================== TCP客户端方法 ====================

bool SocketUtil::connectToHost(const QString &host, quint16 port)
{
    if (m_type != TCP_CLIENT) {
        m_lastError = "Operation only available for TCP client";
        return false;
    }

    if (m_tcpClient->state() == QAbstractSocket::ConnectedState) {
        m_lastError = "Already connected to a host";
        return false;
    }

    clearError();

    // 保存连接信息用于重连
    m_reconnectHost = host;
    m_reconnectPort = port;

    m_tcpClient->connectToHost(host, port);

    if (!m_tcpClient->waitForConnected(3000)) {
        m_lastError = m_tcpClient->errorString();

        // 如果启用了自动重连，启动重连定时器
        if (m_reconnectEnabled) {
            m_reconnectTimer->start(m_reconnectTimer->interval());
        }

        return false;
    }

    m_isConnected = true;
    return true;
}

void SocketUtil::disconnectFromHost()
{
    if (m_type != TCP_CLIENT || !m_isConnected) {
        return;
    }

    m_reconnectTimer->stop();
    m_tcpClient->disconnectFromHost();
    if (m_tcpClient->state() != QAbstractSocket::UnconnectedState) {
        m_tcpClient->waitForDisconnected(1000);
    }
    m_isConnected = false;
}

// ==================== UDP方法 ====================

bool SocketUtil::bindUdp(quint16 port, const QHostAddress &address)
{
    if (m_type != UDP_SOCKET) {
        m_lastError = "Operation only available for UDP socket";
        return false;
    }

    if (m_udpSocket->state() == QAbstractSocket::BoundState) {
        m_lastError = "Socket is already bound";
        return false;
    }

    clearError();

    if (!m_udpSocket->bind(address, port)) {
        m_lastError = m_udpSocket->errorString();
        return false;
    }

    m_isConnected = true;
    return true;
}

void SocketUtil::unbindUdp()
{
    if (m_type != UDP_SOCKET || !m_udpSocket->isOpen()) {
        return;
    }

    m_udpSocket->close();
    m_isConnected = false;
}

// ==================== 通用方法 ====================

bool SocketUtil::sendData(const QByteArray &data, const QString &targetAddress, quint16 targetPort)
{
    clearError();

    qint64 bytesWritten = -1;

    switch (m_type) {
    case TCP_CLIENT:
        if (!m_isConnected) {
            m_lastError = "Not connected to any host";
            return false;
        }
        bytesWritten = m_tcpClient->write(data);
        break;

    case TCP_SERVER:
        if (m_clientSockets.isEmpty()) {
            m_lastError = "No clients connected";
            return false;
        }

        // 如果没有指定目标，发送给所有客户端
        if (targetAddress.isEmpty() || targetPort == 0) {
            for (QTcpSocket *client : qAsConst(m_clientSockets)) {
                client->write(data);
            }
            bytesWritten = data.size();
        }
        // 发送给特定客户端
        else {
            for (QTcpSocket *client : qAsConst(m_clientSockets)) {
                if (client->peerAddress().toString() == targetAddress &&
                    client->peerPort() == targetPort) {
                    bytesWritten = client->write(data);
                    break;
                }
            }

            if (bytesWritten == -1) {
                m_lastError = "Target client not found";
            }
        }
        break;

    case UDP_SOCKET:
        if (!m_isConnected) {
            m_lastError = "Socket not bound";
            return false;
        }

        if (targetAddress.isEmpty() || targetPort == 0) {
            m_lastError = "UDP requires target address and port";
            return false;
        }

        bytesWritten = m_udpSocket->writeDatagram(data, QHostAddress(targetAddress), targetPort);
        break;
    }

    if (bytesWritten == -1) {
        if (m_lastError.isEmpty()) {
            m_lastError = "Failed to send data";
        }
        return false;
    }

    if (bytesWritten != data.size()) {
        m_lastError = QString("Partial data sent: %1/%2 bytes").arg(bytesWritten).arg(data.size());
        return false;
    }

    return true;
}

bool SocketUtil::isConnected() const
{
    return m_isConnected;
}

QString SocketUtil::getLocalAddress() const
{
    switch (m_type) {
    case TCP_CLIENT:
        return m_tcpClient->localAddress().toString();
    case TCP_SERVER:
        return m_tcpServer->serverAddress().toString();
    case UDP_SOCKET:
        return m_udpSocket->localAddress().toString();
    }
    return "";
}

quint16 SocketUtil::getLocalPort() const
{
    switch (m_type) {
    case TCP_CLIENT:
        return m_tcpClient->localPort();
    case TCP_SERVER:
        return m_tcpServer->serverPort();
    case UDP_SOCKET:
        return m_udpSocket->localPort();
    }
    return 0;
}

void SocketUtil::setHeartbeatEnabled(bool enabled, int interval)
{
    m_heartbeatEnabled = enabled;

    if (enabled) {
        m_heartbeatTimer->setInterval(interval);
        m_heartbeatTimer->start();
    } else {
        m_heartbeatTimer->stop();
    }
}

void SocketUtil::setReconnectEnabled(bool enabled, int interval)
{
    m_reconnectEnabled = enabled;

    if (enabled) {
        m_reconnectTimer->setInterval(interval);
        // 仅在断开连接时启动
    } else {
        m_reconnectTimer->stop();
    }
}

void SocketUtil::setAutoRemoveDisconnectedClients(bool enabled)
{
    m_autoRemoveClients = enabled;
}

QString SocketUtil::lastError() const
{
    return m_lastError;
}

// ==================== 私有方法 ====================

void SocketUtil::initTcpClient()
{
    m_tcpClient = new QTcpSocket(this);

    connect(m_tcpClient, &QTcpSocket::connected, this, [this]() {
        m_isConnected = true;
        m_reconnectTimer->stop();
        emit connected();
    });

    connect(m_tcpClient, &QTcpSocket::disconnected, this, [this]() {
        m_isConnected = false;

        // 如果启用了自动重连，启动重连定时器
        if (m_reconnectEnabled) {
            m_reconnectTimer->start();
        }

        emit disconnected();
    });

    connect(m_tcpClient, &QTcpSocket::readyRead, this, &SocketUtil::onReadyRead);
    //connect(m_tcpClient, &QAbstractSocket::errorOccurred,this, &SocketUtil::onSocketError);
	connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));//显示错误
}

void SocketUtil::initTcpServer()
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &SocketUtil::onNewConnection);
}

void SocketUtil::initUdpSocket()
{
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &SocketUtil::onUdpReadyRead);
    //connect(m_udpSocket, &QUdpSocket::errorOccurred, this, &SocketUtil::onSocketError);
	connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));//显示错误
}

void SocketUtil::clearError()
{
    m_lastError.clear();
}

void SocketUtil::log(const QString &message)
{
    qDebug() << "[SocketUtil]" << message;
}

void SocketUtil::addClientSocket(QTcpSocket *socket)
{
    if (!m_clientSockets.contains(socket)) {
        m_clientSockets.append(socket);

        connect(socket, &QTcpSocket::disconnected, this, &SocketUtil::onClientDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &SocketUtil::onReadyRead);
        //connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &SocketUtil::onSocketError);
		connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));//显示错误
        emit clientConnected(socket->socketDescriptor(),
                            socket->peerAddress().toString(),
                            socket->peerPort());
    }
}

void SocketUtil::removeClientSocket(QTcpSocket *socket)
{
    if (m_clientSockets.removeOne(socket)) {
        emit clientDisconnected(socket->socketDescriptor(),
                              socket->peerAddress().toString(),
                              socket->peerPort());
        socket->deleteLater();
    }
}

void SocketUtil::sendHeartbeat()
{
    if (!m_isConnected) return;

    static const QByteArray heartbeat = "HEARTBEAT";

    switch (m_type) {
    case TCP_CLIENT:
        m_tcpClient->write(heartbeat);
        break;
    case TCP_SERVER:
        for (QTcpSocket *client : qAsConst(m_clientSockets)) {
            client->write(heartbeat);
        }
        break;
    case UDP_SOCKET:
        // UDP通常不需要心跳，但可以发送给广播地址
        m_udpSocket->writeDatagram(heartbeat, QHostAddress::Broadcast, getLocalPort());
        break;
    }

    emit heartbeatSent();
}

void SocketUtil::tryReconnect()
{
    if (m_type != TCP_CLIENT) return;

    if (!m_isConnected && m_reconnectEnabled) {
        log("Attempting to reconnect...");
        connectToHost(m_reconnectHost, m_reconnectPort);
    } else {
        m_reconnectTimer->stop();
    }
}

// ==================== 私有槽函数 ====================

void SocketUtil::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
        addClientSocket(clientSocket);
    }
}

void SocketUtil::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        if (m_autoRemoveClients) {
            removeClientSocket(socket);
        } else {
            // 即使不删除，也发出断开信号
		  emit clientDisconnected(socket->socketDescriptor(),
								  socket->peerAddress().toString(),
								  socket->peerPort());
        }
    }
}

void SocketUtil::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data, socket->peerAddress().toString(), socket->peerPort());
    }
}

void SocketUtil::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    QTcpSocket *tcpSocket = qobject_cast<QTcpSocket*>(sender());
    QUdpSocket *udpSocket = qobject_cast<QUdpSocket*>(sender());
    QTcpServer *tcpServer = qobject_cast<QTcpServer*>(sender());

    QString errorMsg;

    if (tcpSocket) {
        errorMsg = tcpSocket->errorString();
    } else if (udpSocket) {
        errorMsg = udpSocket->errorString();
    } else if (tcpServer) {
        errorMsg = tcpServer->errorString();
    } else {
        errorMsg = "Unknown socket error";
    }

    m_lastError = errorMsg;
    emit connectionError(errorMsg);
}

void SocketUtil::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());

        QHostAddress senderAddress;
        quint16 senderPort;

        qint64 bytesRead = m_udpSocket->readDatagram(datagram.data(), datagram.size(),
                                                   &senderAddress, &senderPort);

        if (bytesRead > 0) {
            emit dataReceived(datagram, senderAddress.toString(), senderPort);
        }
    }
}
