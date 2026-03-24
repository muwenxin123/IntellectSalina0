#include "LY_ProtocolParse_XygsServer.h"
#include "LY_ProtocolParse_XygsServerInterface.h"
#include "IFunctionInterface.h"
#include "IProtocolParseXygsServerActivator.h"
#include "IProtocolParseXygsServerImpl_Control.h"
#include "netheader.h"

#include <QSettings>
#include <QtEndian>

#define XYGSSERVER_SEND_LOG_DIR       "../data/Record/Record_XygsServerLog/SendLog/"
#define XYGSSERVER_RECV_LOG_DIR       "../data/Record/Record_XygsServerLog/RecvLog/"

#define XYGSSERVER_MESSAGE_LENGTH_MAX 2048

#define XYGSSERVER_HEADER             "Lysd_Server"

bool                 LY_ProtocolParse_XygsServer::s_bLog_XygsServer_Send = false;
bool                 LY_ProtocolParse_XygsServer::s_bLog_XygsServer_Recv = false;

CLogRecordXygsServer LY_ProtocolParse_XygsServer::s_Log_XygsServer_Send(XYGSSERVER_HEADER, XYGSSERVER_SEND_LOG_DIR, "XygsServer_Send", "ServerLog");
CLogRecordXygsServer LY_ProtocolParse_XygsServer::s_Log_XygsServer_Recv(XYGSSERVER_HEADER, XYGSSERVER_RECV_LOG_DIR, "XygsServer_Recv", "ServerLog");

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

LY_ProtocolParse_XygsServer::LY_ProtocolParse_XygsServer()
{

#ifdef USE_UDP_SOCKET_SEND_SELF_SERVER
    {
        _hostAddress = QHostAddress("225.1.1.5");
        _hostPort    = 1688;
        QSettings settings("../data/conf/ly.ini", QSettings::IniFormat);
        settings.beginGroup("Multicast");
        if(settings.contains("GroupIP") && settings.value("GroupIP").canConvert(QVariant::Type::String)){
            QString strHost = settings.value("GroupIP", "225.1.1.5").toString();
            if(!QHostAddress(strHost).isNull()){
                _hostAddress = QHostAddress(strHost);
            }
        }
        if(settings.contains("PeerPort") && settings.value("PeerPort").canConvert(QVariant::Type::UInt)){
            bool ok = false;
            unsigned int port = settings.value("PeerPort", "1688").toUInt(&ok);
            if(ok){
                _hostPort = port;
            }
        }
        settings.endGroup();

        _udpSocket.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    }
#endif

    QSettings settings("../data/conf/XygsServer_Log_Settings.ini", QSettings::IniFormat);
    settings.beginGroup("LY_PROTOCOLPARSE_XYGSSERVER_LOG_SETTINGS");
    if(settings.contains("Log_XygsServer_Send") && settings.value("Log_XygsServer_Send").canConvert(QVariant::Type::Bool)){
        s_bLog_XygsServer_Send    = settings.value("Log_XygsServer_Send", "false").toBool();
    }
    if(settings.contains("Log_XygsServer_Recv") && settings.value("Log_XygsServer_Recv").canConvert(QVariant::Type::Bool)){
        s_bLog_XygsServer_Recv    = settings.value("Log_XygsServer_Recv", "false").toBool();
    }
    settings.endGroup();
}

void LY_ProtocolParse_XygsServer::Startup()
{
    IFunctionInterface &ifi = LY_ProtocolParse_XygsServerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IProtocolParseXygsServerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_ProtocolParse_XygsServer::Shutdown()
{
    IFunctionInterface &ifi = LY_ProtocolParse_XygsServerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IProtocolParseXygsServerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_ProtocolParse_XygsServer::Reset()
{

}

bool LY_ProtocolParse_XygsServer::DrawGraph( QPainter &painter, IProj *pProj )
{
    return false;
}

bool LY_ProtocolParse_XygsServer::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return false;
}

bool LY_ProtocolParse_XygsServer::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    return bReturn;
}

bool LY_ProtocolParse_XygsServer::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ProtocolParse_XygsServer::OnCommand( int nID )
{
    switch(nID)
    {
    case 43385:
    {

    }
        break;
    case 44777:
    {

    }
        break;
    case 44778:
    {

    }
        break;
    default:
        break;
    }
    return false;
}

bool LY_ProtocolParse_XygsServer::OnCreate( int wParam, QTabWidget *pTab )
{
    switch(wParam)
    {
    case 0:
    {

    }
        break;
    case 1:
    {

    }
        break;
    case 2:
    {

    }
        break;
    case 3:
    {

    }
        break;
    case 4: break;
    }
    return false;
}

bool LY_ProtocolParse_XygsServer::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

int LY_ProtocolParse_XygsServer::SendData(const char *pSendBuf, const int nLength)
{
    if(nLength <= 0 || pSendBuf == nullptr){
        return 0;
    }

#ifdef USE_UDP_SOCKET_SEND_SELF_SERVER
    const int result = _udpSocket.writeDatagram(pSendBuf, nLength, _hostAddress, _hostPort);

#else
    const int result = LY_ProtocolParse_XygsServerInterface::Get().SendData(pSendBuf, nLength);
#endif

    if (s_bLog_XygsServer_Send) {
        if (!s_Log_XygsServer_Send.isopen()) {
            s_Log_XygsServer_Send.open();
        }
        if (s_Log_XygsServer_Send.isopen() && nLength <= XYGSSERVER_MESSAGE_LENGTH_MAX) {

            const int   messageLength = nLength;
            const char *message       = pSendBuf;

            static char bytes_log[sizeof(XYGSSERVER_HEADER) + sizeof(quint64) + sizeof(quint64) ] = { XYGSSERVER_HEADER };
            const quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);

            qToBigEndian(time, bytes_log + sizeof(XYGSSERVER_HEADER));

            const quint64 msgLen = messageLength;

            qToBigEndian(msgLen, bytes_log + sizeof(XYGSSERVER_HEADER) + sizeof(quint64));

            const int len = sizeof(XYGSSERVER_HEADER) + sizeof(quint64) + sizeof(quint64);

            s_Log_XygsServer_Send.write(bytes_log, len, true);
            s_Log_XygsServer_Send.write(message, messageLength, false);
        }
    }

    return result;
}

bool LY_ProtocolParse_XygsServer::OnNetRecv(int nLength, char *lpData)
{
    if( nLength <= sizeof (NetHeader) ) {
        return false;
    }

    qnzkna::NetSeverClient::IProtocolParseXygsServerImpl_RecvMessage(nLength, lpData);

    if (s_bLog_XygsServer_Recv) {
        if (!s_Log_XygsServer_Recv.isopen()) {
            s_Log_XygsServer_Recv.open();
        }
        if (s_Log_XygsServer_Recv.isopen() && nLength <= XYGSSERVER_MESSAGE_LENGTH_MAX) {

            const int   messageLength = nLength;
            const char *message       = lpData;

            static char bytes_log[sizeof(XYGSSERVER_HEADER) + sizeof(quint64)  + sizeof(quint64) ] = { XYGSSERVER_HEADER };
            const quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);

            qToBigEndian(time, bytes_log + sizeof(XYGSSERVER_HEADER));

            const quint64 msgLen = messageLength;

            qToBigEndian(msgLen, bytes_log + sizeof(XYGSSERVER_HEADER) + sizeof(quint64));

            const int len = sizeof(XYGSSERVER_HEADER) + sizeof(quint64) + sizeof(quint64);

            s_Log_XygsServer_Recv.write(bytes_log, len, true);
            s_Log_XygsServer_Recv.write(message, messageLength, false);
        }
    }

    return false;
}

