#include "Mavlink_Control.h"

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <QSettings>
#include <QtEndian>
#include <QByteArray>

mavlink_status_t       m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];

using namespace qnzkna::NetSeverClient;

#define XYGS_MAVLINK_LOG_DIR        "../data/Record/Record_UAVFlightLog/"
#define XYGS_MAVLINK_LOG_CMD_DIR    "../data/Record/Record_UAVFlightLog/Cmd/"

const char* IProtocolParseXygsServerImpl_Mavlink_Control::_tempLogFileTemplate   = "FlightDataXXXXXX";
const char* IProtocolParseXygsServerImpl_Mavlink_Control::_logFileExtension      = "mavlink";

#define MAX_LOGRECORD_FILE_SIZE (1024 * 1024 * 100)
#define MAX_LOGRECORD_FILE_ROW  99999

static QLoggingCategory s_ProtocolLog("../data/Record/Record_UAVFlightLog/Xygs/ProtocolMavlink.xygs");

IProtocolParseXygsServerImpl_Mavlink_Control::IProtocolParseXygsServerImpl_Mavlink_Control(void)
    : IProtocolParseXygsServerImpl_Control("MsgUAVMavlinkControlSend"
                                           , NETHEADER_UCTYPE_MAVLINK_CONTROL
                                           , NODE_TYPE_WEAPONCOMPONENT
                                           )
    , _logSuspendError(false)
    , _logSuspendReplay(false)
    , _vehicleWasArmed(false)
    , _tempLogFile(QStringLiteral("%1").arg(XYGS_MAVLINK_LOG_DIR), QStringLiteral("%1.%2").arg(_tempLogFileTemplate).arg(_logFileExtension))
    , _tempCmdLogFile(QStringLiteral("%1").arg(XYGS_MAVLINK_LOG_CMD_DIR), QStringLiteral("%1.%2").arg(_tempLogFileTemplate).arg(_logFileExtension))
    , _message({})
    , _status({})
{
    s_MavlinkControlMap.clear();

    qDebug() << sizeof (mavlink_message_t) << MAVLINK_MAX_PACKET_LEN;
    memset(&_status,            0, sizeof(_status));
    memset(&_message,           0, sizeof(_message));

    s_ProtocolLog.setEnabled(QtDebugMsg,   true);
    s_ProtocolLog.setEnabled(QtInfoMsg,    true);
    s_ProtocolLog.setEnabled(QtWarningMsg, true);

    QSettings settings("../data/conf/ServerImpl_Mavlink_Control.ini", QSettings::IniFormat);

    settings.beginGroup("QGC_MAVLINK_SERVERIMPL_MAVLINK_CONTROL_LOGGING");
    _tempLog              = settings.value("ServerimplMavlinkControlLoggingTempLog", "true").toBool();
    _tempLogRemoveZero    = settings.value("ServerimplMavlinkControlLoggingTempLogRemoveZero", "false").toBool();
    _tempCmdLog           = settings.value("ServerimplMavlinkControlLoggingTempCmdLog", "true").toBool();
    _tempCmdLogRemoveZero = settings.value("ServerimplMavlinkControlLoggingTempCmdLogRemoveZero", "false").toBool();
    settings.endGroup();

    if(_tempLog){
        _startLogging();
    }

    if(_tempCmdLog){
        _startCmdLogging();
    }
}

IProtocolParseXygsServerImpl_Mavlink_Control::~IProtocolParseXygsServerImpl_Mavlink_Control(void)
{

}

void IProtocolParseXygsServerImpl_Mavlink_Control::start()
{

}

void IProtocolParseXygsServerImpl_Mavlink_Control::stop()
{
    _stopLogging();
    _stopCmdLogging();
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::Pack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{

#ifdef LY_PROTOCOLPARSE_XYGSSERVER_IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROL_SEND_MESSAGE

    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    const unsigned int messageLength = msg->getMessageLength();
    if ((0 >= messageLength)
            || (messageLength > MAVLINK_MAX_PACKET_LEN )
            )
    {
        return false;
    }

    const qnzkna::framework::MessageReferenceProps& props = msg->getMessageProperties();
    const std::string& strTargetID = props.find("TargetID");

    if(strTargetID.empty()){
        return false;
    }

    bool ok = false;
    const unsigned long long unNodeID= QString::fromStdString(strTargetID).toULongLong(&ok);
    if(!ok){
        return false;
    }

    qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo netMsg;

    netMsg.header.ucType       = IProtocolParseXygsServerImpl_Control::m_ucType;
    netMsg.header.ucSubType    = NETHEADER_UCSUBTYPE_MAVLINK_CONTROL_UP_TO_SERVER;
    netMsg.header.unLength     = 0;
    netMsg.header.unMask       = NETHEADER_UNMASK_SERVER_CLIENT;

    netMsg.header.unNodeType   = IProtocolParseXygsServerImpl_Control::m_unNodeType;
    netMsg.header.unNodeID     = unNodeID;
    netMsg.header.unSeatType   = NETHEADER_UNSEATTYPE_CLIENT;
    netMsg.header.unSeatID     = IProtocolParseXygsServerImpl_Control::getSeatID();

    memcpy(&netMsg.msgbuffer,  msg->getMessage(), messageLength);
    netMsg.header.unLength = sizeof (NetHeader) + messageLength;

    IProtocolParseXygsServerImpl_Control::NetSend_XygsServer(netMsg.header.unLength,  (const char* const)(&netMsg));

    if(_tempCmdLog && netMsg.header.unLength <= sizeof (qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo))
    {
        static char bytes_log[sizeof(quint64) +  sizeof (qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo)] = {0};
        const quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);

        qToBigEndian(time, bytes_log);

        memcpy(bytes_log + sizeof(quint64), (const char*)(&netMsg), netMsg.header.unLength);

        const int len = (sizeof(quint64) + netMsg.header.unLength);
        if(_tempCmdLogFile.write(bytes_log, len) != len)
        {
            if(_tempCmdLogFile.size() >= MAX_LOGRECORD_FILE_SIZE){
                _closeCmdLogFile();
                _startCmdLogging();
            }
        }
    }

#endif

    return true;
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::NetHeader_XygsServer(const int nLength, const char * const pRecvBuf)
{
    if(nLength <= sizeof (NetHeader))
    {
        return false;
    }

    const auto* const pNetHeader = (const NetHeader* const)(pRecvBuf);
    if(pNetHeader->unLength != nLength){
        return false;
    }

    const int msgLength = pNetHeader->unLength - sizeof (NetHeader);

    if(msgLength <= 0
            || (msgLength > MAVLINKCONTROLINFO_MAVLINK_MAX_PACKET_LEN)
            )
    {
        return false;
    }

    if((pNetHeader->ucType == IProtocolParseXygsServerImpl_Control::m_ucType)
            && (pNetHeader->ucSubType == NETHEADER_UCSUBTYPE_MAVLINK_CONTROL_DOWN_TO_CLIENT)
            && (pNetHeader->unNodeType == IProtocolParseXygsServerImpl_Control::m_unNodeType)
            && (pNetHeader->unSeatType == NETHEADER_UNSEATTYPE_SERVER)
            )
    {
        return true;
    }

    return false;
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::UnPack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{
    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    return false;
}

void IProtocolParseXygsServerImpl_Mavlink_Control::setVersion(unsigned version)
{
    for (const auto& itemMavlink : s_MavlinkControlMap ) {
        mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(itemMavlink.second.mavlinkChannel);

        if (version < 200) {
            mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        } else {
            mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        }
    }
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::_closeLogFile(void)
{
    if (_tempLogFile.isOpen()) {
        if (_tempLogRemoveZero && _tempLogFile.size() == 0) {

            _tempLogFile.remove();
            return false;
        } else {
            _tempLogFile.flush();
            _tempLogFile.close();
            return true;
        }
    }
    return false;
}

void IProtocolParseXygsServerImpl_Mavlink_Control::_startLogging(void)
{

    if (!_tempLogFile.isOpen()) {
        if (!_logSuspendReplay) {
            if (!_tempLogFile.open()) {

                _closeLogFile();
                _logSuspendError = true;
                return;
            }

            qCDebug(s_ProtocolLog) << "Temp log" << _tempLogFile.fileName();

            _logSuspendError = false;
        }
    }
}

void IProtocolParseXygsServerImpl_Mavlink_Control::_stopLogging(void)
{
    if (_tempLogFile.isOpen()) {
        if (_closeLogFile()) {

        }
        else {
            QFile::remove(_tempLogFile.fileName());
        }
    }

    _vehicleWasArmed = false;
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::_closeCmdLogFile()
{
    if (_tempCmdLogFile.isOpen()) {
        if (_tempCmdLogRemoveZero && _tempCmdLogFile.size() == 0) {

            _tempCmdLogFile.remove();
            return false;
        } else {
            _tempCmdLogFile.flush();
            _tempCmdLogFile.close();
            return true;
        }
    }

    return false;
}

void IProtocolParseXygsServerImpl_Mavlink_Control::_startCmdLogging()
{
    if (!_tempCmdLogFile.isOpen()) {
        if (!_tempCmdLogFile.open()) {

            _closeCmdLogFile();
            return;
        }

        qCDebug(s_ProtocolLog) << "Cmd Temp log" << _tempCmdLogFile.fileName();
    }
}

void IProtocolParseXygsServerImpl_Mavlink_Control::_stopCmdLogging()
{
    if (_tempCmdLogFile.isOpen()) {
        if (_closeCmdLogFile()) {
        }
        else {
            QFile::remove(_tempCmdLogFile.fileName());
        }
    }

    _vehicleWasArmed = false;
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::NetRecv_XygsServer(const int nLength, const char * const pRecvBuf)
{

#ifdef LY_PROTOCOLPARSE_XYGSSERVER_IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROL_RECV_MESSAGE

    if(!NetHeader_XygsServer(nLength, pRecvBuf)){
        return false;
    }

    const auto* const pNetMsg = (const qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo* const)(pRecvBuf);

    if((pNetMsg->header.ucType != IProtocolParseXygsServerImpl_Control::m_ucType)
            || (pNetMsg->header.ucSubType != NETHEADER_UCSUBTYPE_ZC_CONTROL_DOWN_TO_CLIENT)
            || (pNetMsg->header.unNodeType != IProtocolParseXygsServerImpl_Control::m_unNodeType)
            || (pNetMsg->header.unSeatType != NETHEADER_UNSEATTYPE_SERVER)

            )
    {
        return false;
    }

    const int msgLength = pNetMsg->header.unLength - sizeof (NetHeader);

    if(msgLength <= 0 || msgLength > MAVLINKCONTROLINFO_MAVLINK_MAX_PACKET_LEN ){
        return false;
    }

    const QString& strNodeID = QString::number(pNetMsg->header.unNodeID);
    if (strNodeID.isEmpty()) {
        return false;
    }

    const char* const pInBuf     = ((const char* const)&(pNetMsg->msgbuffer));
    const unsigned int nInBufLen = msgLength;
    if (nInBufLen <= 0) {
        return true;
    }

    if(sizeof (qnzkna::NetSeverClient::XygsMavlinkControl::xygs_mavlink_message_t) != nInBufLen){
        return true;
    }

    _message.len = 0;

    qnzkna::NetSeverClient::XygsMavlinkControl::xygs_mavlink_message_t* xygs_msg = (qnzkna::NetSeverClient::XygsMavlinkControl::xygs_mavlink_message_t*)(pInBuf);

    _message.checksum       = xygs_msg->checksum;
    _message.magic          = xygs_msg->magic;
    _message.len            = xygs_msg->len;
    _message.incompat_flags = xygs_msg->incompat_flags;
    _message.compat_flags   = xygs_msg->compat_flags;
    _message.seq            = xygs_msg->seq;
    _message.sysid          = xygs_msg->sysid;
    _message.compid         = xygs_msg->compid;
    _message.msgid          = xygs_msg->msgid;
    memcpy(_message.payload64, xygs_msg->payload64, sizeof(_message.payload64));
    memcpy(_message.ck,        xygs_msg->ck, sizeof(_message.ck));
    memcpy(_message.signature, xygs_msg->signature, sizeof(_message.signature));

    if(_message.len != 0 && _message.len == xygs_msg->len){

        memcpy_s(&_message, sizeof(_message), pInBuf, sizeof(_message));

        const QString& strSysID = QString::number(_message.sysid);
        if (strSysID.isEmpty()) {
            return false;
        }

        const std::string& strTargetID = (strNodeID + strSysID).toStdString();

        auto iterMavlinkControl = s_MavlinkControlMap.find(strTargetID);
        if (s_MavlinkControlMap.end() == iterMavlinkControl) {
            const auto result = s_MavlinkControlMap.insert(std::pair<std::string, Mavlink_Control_Msg>(strTargetID, Mavlink_Control_Msg()));
            if (result.second == false) {
                return true;
            }
            iterMavlinkControl = result.first;
            if (s_MavlinkControlMap.end() == iterMavlinkControl) {
                return true;
            }

            iterMavlinkControl->second.mavlinkChannel = s_MavlinkControlMap.size() - 1;

            if (!iterMavlinkControl->second.decodedFirstMavlinkPacket) {
                iterMavlinkControl->second.decodedFirstMavlinkPacket = true;
                mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(iterMavlinkControl->second.mavlinkChannel);
                if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {

                }
            }
        }

        uint8_t mavlinkChannel = iterMavlinkControl->second.mavlinkChannel;

        if(mavlinkChannel >= MAVLINK_COMM_NUM_BUFFERS || mavlinkChannel < 0){
            qCDebug(s_ProtocolLog) << "MavlinkChannel, MAVLINK_COMM_NUM_BUFFERS : " << mavlinkChannel << ", " << MAVLINK_COMM_NUM_BUFFERS;
            return true;
        }

        if(_tempLog && pNetMsg->header.unLength <= sizeof (qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo))
        {

            if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {

                static char bytes_log[sizeof(quint64) +  sizeof (qnzkna::NetSeverClient::XygsMavlinkControl::MavlinkControlInfo)] = {0};
                const quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);

                qToBigEndian(time, bytes_log);

                memcpy(bytes_log + sizeof(quint64), (const char*)(pNetMsg), pNetMsg->header.unLength);

                const int len = (sizeof(quint64) + pNetMsg->header.unLength);

                if(_tempLogFile.write(bytes_log, len) != len)
                {

                }

                if(_tempLogFile.size() >= MAX_LOGRECORD_FILE_SIZE){
                    _closeLogFile();
                    _startLogging();
                }
            }
        }

        qnzkna::framework::IRegistry* pContext = IProtocolParseXygsServerActivator::Get().getIRegistryImpl();
        if(pContext == nullptr) {
            return false;
        }

        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
        props.put("TargetID",     strNodeID.toStdString());
        props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK);

        SendMsg((const char* const)(&_message), sizeof (_message), props, pContext);

        iterMavlinkControl->second._handleReceivedMavlinkMessage(_message, props, pContext);

        memset(&_status,  0, sizeof(_status));
        memset(&_message, 0, sizeof(_message));
    }

#if 0
    for (unsigned int position = 0; position < nInBufLen; position++) {
        if (mavlink_parse_char(mavlinkChannel, static_cast<uint8_t>(pInBuf[position]), &_message, &_status)) {

            if (!iterMavlinkControl->second.decodedFirstMavlinkPacket) {
                iterMavlinkControl->second.decodedFirstMavlinkPacket = (true);
                mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
                if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
                    qCDebug(s_ProtocolLog) << "Switching outbound to mavlink 2.0 due to incoming mavlink 2.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
                    mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;

                    setVersion(200);
                }
            }

#if 0

            if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {
                uint8_t buf[MAVLINK_MAX_PACKET_LEN+sizeof(quint64)];

                quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);
                qToBigEndian(time, buf);

                int len = mavlink_msg_to_send_buffer(buf + sizeof(quint64), &_message);

                len += sizeof(quint64);

                QByteArray b(reinterpret_cast<const char*>(buf), len);
                if(_tempLogFile.write(b) != len)
                {

                }

                if(_tempLogFile.size() >= MAX_LOGRECORD_FILE_SIZE){
                    _closeLogFile();
                    _startLogging();
                }

                if (!_vehicleWasArmed && _message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                    mavlink_heartbeat_t state;
                    mavlink_msg_heartbeat_decode(&_message, &state);
                    if (state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY) {
                        _vehicleWasArmed = true;
                    }
                }
            }
#endif

            {
                qnzkna::framework::IRegistry* pContext = IProtocolParseXygsServerActivator::Get().getIRegistryImpl();
                if(pContext == nullptr) {
                    return false;
                }

                qnzkna::framework::MessageReferenceProps props;
                props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
                props.put("TargetID",     strTargetID);
                props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK);

                SendMsg((char*)(&_message), sizeof (_message), props, pContext);
            }

            memset(&_status,  0, sizeof(_status));
            memset(&_message, 0, sizeof(_message));
        }
    }
#endif

#endif

    return false;
}

bool IProtocolParseXygsServerImpl_Mavlink_Control::SendMsg(const char* const msgBuffer, const unsigned int msgBufferLen, const qnzkna::framework::MessageReferenceProps& props, qnzkna::framework::IRegistry* pContext)
{
    if(msgBuffer == nullptr || pContext == nullptr){
        return false;
    }

    pContext->messageNotify(new qnzkna::framework::MessageReference("MsgUAVMavlinkControlRecv", msgBuffer, msgBufferLen, props));

    return true;
}

ADD_PROTOCOLPARSEXYGSSERVERIMPL_CONTROL_HANDLE(IProtocolParseXygsServerImpl_Mavlink_Control)
