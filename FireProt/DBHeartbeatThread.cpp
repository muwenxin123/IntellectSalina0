#include "DBHeartbeatThread.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <QCString.h>

DBHeartbeatThread::DBHeartbeatThread(QObject *parent) : QThread(parent)
{
    m_bConnected = false;
    m_checkIpIsOnlineInterval = 5000;
    m_connectDBInterval = 5000;
    m_checkDBHeartbeatInterval = 5000;
    m_lastError = "";

    m_mapDBTypeDriver = { { MySQL, "QMYSQL" }, { SqlServer, "QODBC" }, { Oracle, "QOCI" } };
}

DBHeartbeatThread::DBHeartbeatThread(const DBType &type, const ConnectParam &param, QObject *parent)
    : DBHeartbeatThread(parent)
{
    m_bConnected = false;
    m_type = type;
    m_param = param;
}

void DBHeartbeatThread::setDBTypeAndConnectParm(const DBHeartbeatThread::DBType &type,
                                                const DBHeartbeatThread::ConnectParam &param)
{
    m_type = type;
    m_param = param;
}

void DBHeartbeatThread::setCheckIpIsOnlineInterval(int msec)
{
    m_checkIpIsOnlineInterval = msec;
}

void DBHeartbeatThread::setConnectDBInterval(int msec)
{
    m_connectDBInterval = msec;
}

void DBHeartbeatThread::setCheckDBHeartbeatInterval(int msec)
{
    m_checkDBHeartbeatInterval = msec;
}

QString DBHeartbeatThread::lastError() const
{
    return m_lastError;
}

void DBHeartbeatThread::run()
{

    QSqlDatabase db;

    auto openDB = [&]() -> bool
    {
        static QString connectName = "heartbeatCheck";
        if (QSqlDatabase::contains(connectName))
        {
            db = QSqlDatabase::database(connectName);
        }
        else
        {
            db = QSqlDatabase::addDatabase(m_mapDBTypeDriver.value(m_type), connectName);
        }

        if (db.isOpen())
        {
            db.close();
        }

        db.setHostName(m_param.ip);
        db.setPort(m_param.port);
        db.setDatabaseName(m_param.dbName);
        db.setUserName(m_param.userName);
        db.setPassword(m_param.password);

        QTime startTime = QTime::currentTime();
        if (db.open())
        {
            QTime endTime = QTime::currentTime();
            qDebug() << tr2("数据库打开成功耗时: ") << startTime.msecsTo(endTime) << "ms";
            return true;
        }

        QTime endTime = QTime::currentTime();
        qDebug() << tr2("数据库打开失败耗时: ") << startTime.msecsTo(endTime) << "ms";
        m_lastError = tr2("数据库打开失败，原因：%1").arg(db.lastError().text());
        return false;
    };

    auto checkDBIsOnline = [&db]() -> bool
    {
        QSqlQuery query(db);
        QString sql = "select 1";

        QTime startTime = QTime::currentTime();
        if (query.exec(sql))
        {
            return true;
        }

        QTime endTime = QTime::currentTime();
        qDebug() << tr2("检测心跳断开耗时：") << startTime.msecsTo(endTime) << "ms";
        return false;
    };

    auto checkIpIsOnline = [](const QString & ip) -> bool
    {
        QProcess cmd;
        QString command = "";
#ifdef Q_OS_WIN

        command = QStringLiteral("ping %1 -n 1 -w 1000").arg(ip);
#else

        command = QStringLiteral("ping -s 1 -c 1 %1").arg(ip);
#endif
        cmd.start(command);
        cmd.waitForFinished(1000 * 1);

        QString retStr = cmd.readAll();
        if (retStr.indexOf("ttl", 0, Qt::CaseInsensitive) == -1)
        {
            return false;
        }

        return true;
    };

    if (checkIpIsOnline(m_param.ip))
    {
        if (checkIpIsOnline(m_param.ip))
        {
            if (openDB())
            {
                qDebug() << tr2("连接数据库成功，开始心跳检测");
                emit signal_connectedDB();
            }
        }
    }

    QTimer timerCheckIp;
    QTimer timerConnectDB;
    QTimer timerDBHeartbeat;

    timerCheckIp.setInterval(m_checkIpIsOnlineInterval);
    connect(&timerCheckIp, &QTimer::timeout, [&]
    {
        timerCheckIp.stop();
        if (checkIpIsOnline(m_param.ip))
        {
            qDebug()
                    << tr2("连接网络成功，正在连接数据库");
            emit signal_connectedNet();
            timerConnectDB.start();
        }
        else
        {
            timerCheckIp.start();
        }
    });

    timerConnectDB.setInterval(m_connectDBInterval);
    connect(&timerConnectDB, &QTimer::timeout, [&]
    {
        timerConnectDB.stop();
        if (checkIpIsOnline(m_param.ip))
        {
            if (openDB())
            {
                qDebug() << tr2("连接数据库成功，开始心跳检测");
                emit signal_connectedDB();
                timerDBHeartbeat.start();
            }
            else
            {
                timerConnectDB.start();
            }
        }
        else
        {
            qDebug() << tr2("网络断开，正在重连");
            m_bConnected = false;
            emit signal_disconnectedNet();
            timerCheckIp.start();
        }
    });

    timerDBHeartbeat.setInterval(m_checkDBHeartbeatInterval);
    connect(&timerDBHeartbeat, &QTimer::timeout, [&]
    {
        static int checkIpFailTime = 0;
        static int checkDBFailTime = 0;
        timerDBHeartbeat.stop();

        if (checkIpIsOnline(m_param.ip))
        {
            checkIpFailTime = 0;

            if (checkDBIsOnline())
            {
                checkDBFailTime = 0;
                timerDBHeartbeat.start();
            }
            else
            {
                ++checkDBFailTime;
                if (checkDBFailTime == kCheckDBFailTime)
                {
                    checkDBFailTime = 0;
                    qDebug() << tr2("数据库断开，正在重连");
                    m_bConnected = false;
                    emit signal_disconnectedDB();
                    timerConnectDB.start();
                }
                else
                {
                    timerDBHeartbeat.start();
                }
            }
        }
        else
        {
            ++checkIpFailTime;
            if (checkIpFailTime == kCheckIpFailTime)
            {
                checkIpFailTime = 0;
                qDebug() << tr2("网络断开，正在重连");
                m_bConnected = false;
                emit signal_disconnectedNet();
                timerCheckIp.start();
            }
            else
            {
                timerDBHeartbeat.start();
            }
        }
    });

    timerCheckIp.start();
    qDebug() << tr2("正在连接网络");
    exec();
}
