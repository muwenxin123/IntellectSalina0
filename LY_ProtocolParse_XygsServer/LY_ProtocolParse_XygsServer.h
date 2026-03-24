#ifndef LY_PROTOCOLPARSE_XYGSSERVER_H
#define LY_PROTOCOLPARSE_XYGSSERVER_H

#include <QMetaType>

#include "LY_ProtocolParse_XygsServer_global.h"
#include "LogRecord.h"

#include <QCString.h>

#define USE_UDP_SOCKET_SEND_SELF_SERVER

#ifdef USE_UDP_SOCKET_SEND_SELF_SERVER
#include <QUdpSocket>
#endif

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_PROTOCOLPARSE_XYGSSERVERSHARED_EXPORT LY_ProtocolParse_XygsServer
{
public:
    LY_ProtocolParse_XygsServer();
    inline static LY_ProtocolParse_XygsServer &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QTabWidget *pTab);
    bool OnTimer(int wParam);
    int SendData(const char *pSendBuf, const int nLength);
    bool OnNetRecv(int nLength, char *lpData);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    }
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    }
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    }
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    }
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    }

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

#ifdef USE_UDP_SOCKET_SEND_SELF_SERVER
    QHostAddress                                       _hostAddress;
    quint16                                            _hostPort;
    QUdpSocket                                         _udpSocket;
#endif

    static bool                                        s_bLog_XygsServer_Send;
    static bool                                        s_bLog_XygsServer_Recv;
    static CLogRecordXygsServer                        s_Log_XygsServer_Send;
    static CLogRecordXygsServer                        s_Log_XygsServer_Recv;
};

inline LY_ProtocolParse_XygsServer &LY_ProtocolParse_XygsServer::Get()
{
    static LY_ProtocolParse_XygsServer sLY_ProtocolParse_XygsServer;
    return sLY_ProtocolParse_XygsServer;
}

#endif
