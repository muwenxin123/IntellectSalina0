#ifndef DBHEARTBEATTHREAD_H
#define DBHEARTBEATTHREAD_H

#include <QObject>
#include <QThread>
#include <QMap>

class DBHeartbeatThread : public QThread
{
    Q_OBJECT
public:
    enum DBType : quint8
	{
        MySQL = 0,
        SqlServer,
        Oracle
    };

    struct ConnectParam
	{
		quint16 port;
		QString ip;
		QString dbName;
		QString userName;
		QString password;
    };

public:
    explicit DBHeartbeatThread(QObject *parent = nullptr);
    explicit DBHeartbeatThread(const DBType &type, const ConnectParam &param, QObject *parent = nullptr);

public:
    void setDBTypeAndConnectParm(const DBType &type, const ConnectParam &param);
    void setCheckIpIsOnlineInterval(int msec);
    void setConnectDBInterval(int msec);
    void setCheckDBHeartbeatInterval(int msec);
    QString lastError() const;

protected:
    void run() override;

signals:
    void signal_connectedDB();
    void signal_disconnectedDB();
    void signal_connectedNet();
    void signal_disconnectedNet();

public:
	bool m_bConnected;

private:
    DBType m_type;
    const int kCheckIpFailTime = 3;
    const int kCheckDBFailTime = 3;
    int m_checkIpIsOnlineInterval;
    int m_connectDBInterval;
    int m_checkDBHeartbeatInterval;

	QString m_lastError;
    ConnectParam m_param;
    QMap<DBType, QString> m_mapDBTypeDriver;
};

#endif
