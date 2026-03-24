#include "MAVLinkProtocol.h"
#include <inttypes.h>
#include <iostream>

#include <QDebug>
#include <QTime>
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QtEndian>
#include <QMetaType>
#include <QDir>
#include <QFileInfo>


#include "QGCLoggingCategory.h"
#include "SettingsManager.h"
#include "MAVLinkProtocol.h"
#include "QGCMAVLink.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"

Q_DECLARE_METATYPE(mavlink_message_t)

QGC_LOGGING_CATEGORY(MAVLinkProtocolLog, "MAVLinkProtocolLog")

const char* MAVLinkProtocol::_tempLogFileTemplate   = "FlightDataXXXXXX";   ///< Template for temporary log file
const char* MAVLinkProtocol::_logFileExtension      = "mavlink";            ///< Extension for log files

/**
 * The default constructor will create a new MAVLink object sending heartbeats at
 * the MAVLINK_HEARTBEAT_DEFAULT_RATE to all connected links.
 */
MAVLinkProtocol::MAVLinkProtocol(QGCApplication* app)
    : QObject(app)
    , m_enable_version_check(true)
    , m_enable_init_version_1(false)
    , _message({})
    , _status({})
    , versionMismatchIgnore(false)
    , systemId(255)
    , _current_version(100)
    , _radio_version_mismatch_count(0)
    , _logSuspendError(false)
    , _logSuspendReplay(false)
    , _vehicleWasArmed(false)
    // lysd // , _tempLogFile(QString("%2.%3").arg(_tempLogFileTemplate).arg(_logFileExtension))
    // lysd // , _linkMgr(nullptr)
    // lysd // , _multiVehicleManager(nullptr)
{
    memset(totalReceiveCounter, 0, sizeof(totalReceiveCounter));
    memset(totalLossCounter,    0, sizeof(totalLossCounter));
    memset(runningLossPercent,  0, sizeof(runningLossPercent));
    memset(firstMessage,        1, sizeof(firstMessage));
    memset(&_status,            0, sizeof(_status));
    memset(&_message,           0, sizeof(_message));

    setToolbox();
}

MAVLinkProtocol::~MAVLinkProtocol()
{
    storeSettings();
    // lysd // _closeLogFile();
}

void MAVLinkProtocol::setVersion(unsigned version)
{
    // lysd // QList<SharedLinkInterfacePtr> sharedLinks = _linkMgr->links();

    if(!QGCApplication::multiVehicleManager()){
        return;
    }

    auto* vehiclesMap = QGCApplication::multiVehicleManager()->vehicles();
    if(!vehiclesMap){
         return;
    }

    for (int i = 0; i < vehiclesMap->size(); i++) {
        mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(vehiclesMap->at(i)->mavlinkChannel());

        // Set flags for version
        if (version < 200) {
            mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        } else {
            mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
        }
    }

    _current_version = version;
}

void MAVLinkProtocol::setToolbox()
{


    qRegisterMetaType<mavlink_message_t>("mavlink_message_t");
    qRegisterMetaType<unsigned long long int>("unsigned long long int");

    loadSettings();

    if(m_enable_init_version_1){
        uint32_t                            _mavlinkChannelsUsedBitMask = 1;
        // Find a mavlink channel to use for this link
        for (uint8_t mavlinkChannel = 0; mavlinkChannel < MAVLINK_COMM_NUM_BUFFERS; mavlinkChannel++) {
            // lysd // qCDebug(MAVLinkProtocolLog) << "allocateMavlinkChannel" << mavlinkChannel;
            // lysd // if (!(_mavlinkChannelsUsedBitMask & 1 << mavlinkChannel))
            {
                mavlink_reset_channel_status(mavlinkChannel);
                // Start the channel on Mav 1 protocol
                mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
                mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
                _mavlinkChannelsUsedBitMask |= 1 << mavlinkChannel;
                // lysd // qCDebug(MAVLinkProtocolLog) << "allocateMavlinkChannel" << mavlinkChannel << " : " << mavlinkStatus->flags << MAVLINK_STATUS_FLAG_OUT_MAVLINK1;

            }
        }
        qWarning(MAVLinkProtocolLog) << "allocateMavlinkChannel: all channels reserved!";
    }

    // All the *Counter variables are not initialized here, as they should be initialized
    // on a per-link basis before those links are used. @see resetMetadataForLink().

    // lysd // connect(this, &MAVLinkProtocol::protocolStatusMessage,   _app, &QGCApplication::criticalMessageBoxOnMainThread);
    // lysd // connect(this, &MAVLinkProtocol::saveTelemetryLog,        _app, &QGCApplication::saveTelemetryLogOnMainThread);
    // lysd // connect(this, &MAVLinkProtocol::checkTelemetrySavePath,  _app, &QGCApplication::checkTelemetrySavePathOnMainThread);
    // lysd //
    // lysd // connect(_multiVehicleManager, &MultiVehicleManager::vehicleAdded, this, &MAVLinkProtocol::_vehicleCountChanged);
    // lysd // connect(_multiVehicleManager, &MultiVehicleManager::vehicleRemoved, this, &MAVLinkProtocol::_vehicleCountChanged);

    emit versionCheckChanged(m_enable_version_check);
}

void MAVLinkProtocol::loadSettings()
{
    // Load defaults from settings
    QSettings settings("../data/conf/MAVLinkProtocol.ini", QSettings::IniFormat);
    settings.beginGroup("QGC_MAVLINK_PROTOCOL");
    enableVersionCheck(settings.value("VERSION_CHECK_ENABLED", m_enable_version_check).toBool());
    enableVersionCheck(settings.value("VERSION_INIT_VERSION_1_ENABLED", m_enable_init_version_1).toBool());


    // Only set system id if it was valid
    int temp = settings.value("GCS_SYSTEM_ID", systemId).toInt();
    if (temp > 0 && temp < 256)
    {
        systemId = temp;
    }
}

void MAVLinkProtocol::storeSettings()
{
    // Store settings
    QSettings settings("../data/conf/MAVLinkProtocol.ini", QSettings::IniFormat);
    settings.beginGroup("QGC_MAVLINK_PROTOCOL");
    settings.setValue("VERSION_CHECK_ENABLED", m_enable_version_check);
    settings.setValue("VERSION_INIT_VERSION_1_ENABLED", m_enable_init_version_1);
    settings.setValue("GCS_SYSTEM_ID", systemId);
    // Parameter interface settings
}

void MAVLinkProtocol::resetMetadataForLink(LinkInterface *link)
{
    // lysd // int channel = link->mavlinkChannel();
    // lysd // totalReceiveCounter[channel] = 0;
    // lysd // totalLossCounter[channel]    = 0;
    // lysd // runningLossPercent[channel]  = 0.0f;
    // lysd // for(int i = 0; i < 256; i++) {
    // lysd //     firstMessage[channel][i] =  1;
    // lysd // }
    // lysd // link->setDecodedFirstMavlinkPacket(false);
}

/**
 * This method parses all outcoming bytes and log a MAVLink packet.
 * @param link The interface to read from
 * @see LinkInterface
 **/

void MAVLinkProtocol::logSentBytes(LinkInterface* link, QByteArray b){

    // lysd // uint8_t bytes_time[sizeof(quint64)];

    Q_UNUSED(link)
    Q_UNUSED(b)
    // lysd // if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {
    // lysd //
    // lysd //     quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);
    // lysd //
    // lysd //     qToBigEndian(time,bytes_time);
    // lysd //
    // lysd //     b.insert(0,QByteArray((const char*)bytes_time,sizeof(bytes_time)));
    // lysd //
    // lysd //     int len = b.count();
    // lysd //
    // lysd //     if(_tempLogFile.write(b) != len)
    // lysd //     {
    // lysd //         // If there's an error logging data, raise an alert and stop logging.
    // lysd //         emit protocolStatusMessage(tr("MAVLink Protocol"), tr("MAVLink Logging failed. Could not write to file %1, logging disabled.").arg(_tempLogFile.fileName()));
    // lysd //         // lysd // _stopLogging();
    // lysd //         _logSuspendError = true;
    // lysd //     }
    // lysd // }

}

/**
 * This method parses all incoming bytes and constructs a MAVLink packet.
 * It can handle multiple links in parallel, as each link has it's own buffer/
 * parsing state machine.
 * @param link The interface to read from
 * @see LinkInterface
 **/

void MAVLinkProtocol::receiveBytes(LinkInterface* link, unsigned long long weaponId, QByteArray b)
{
    // Since receiveBytes signals cross threads we can end up with signals in the queue
    // that come through after the link is disconnected. For these we just drop the data
    // since the link is closed.
    // lysd //     SharedLinkInterfacePtr linkPtr = _linkMgr->sharedLinkInterfacePointerForLink(link, true);
    // lysd //     if (!linkPtr) {
    // lysd //         qCDebug(MAVLinkProtocolLog) << "receiveBytes: link gone!" << b.size() << " bytes arrived too late";
    // lysd //         return;
    // lysd //     }
    // lysd //
    // lysd //     uint8_t mavlinkChannel = link->mavlinkChannel();
    // lysd //
    // lysd //     for (int position = 0; position < b.size(); position++) {
    // lysd //         if (mavlink_parse_char(mavlinkChannel, static_cast<uint8_t>(b[position]), &_message, &_status)) {
    // lysd //             // Got a valid message
    // lysd //             if (!link->decodedFirstMavlinkPacket()) {
    // lysd //                 link->setDecodedFirstMavlinkPacket(true);
    // lysd //                 mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
    // lysd //                 if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
    // lysd //                     qCDebug(MAVLinkProtocolLog) << "Switching outbound to mavlink 2.0 due to incoming mavlink 2.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
    // lysd //                     mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
    // lysd //                     // Set all links to v2
    // lysd //                     setVersion(200);
    // lysd //                 }
    // lysd //             }
    // lysd //
    // lysd //             //-----------------------------------------------------------------
    // lysd //             // MAVLink Status
    // lysd //             uint8_t lastSeq = lastIndex[_message.sysid][_message.compid];
    // lysd //             uint8_t expectedSeq = lastSeq + 1;
    // lysd //             // Increase receive counter
    // lysd //             totalReceiveCounter[mavlinkChannel]++;
    // lysd //             // Determine what the next expected sequence number is, accounting for
    // lysd //             // never having seen a message for this system/component pair.
    // lysd //             if(firstMessage[_message.sysid][_message.compid]) {
    // lysd //                 firstMessage[_message.sysid][_message.compid] = 0;
    // lysd //                 lastSeq     = _message.seq;
    // lysd //                 expectedSeq = _message.seq;
    // lysd //             }
    // lysd //             // And if we didn't encounter that sequence number, record the error
    // lysd //             //int foo = 0;
    // lysd //             if (_message.seq != expectedSeq)
    // lysd //             {
    // lysd //                 //foo = 1;
    // lysd //                 int lostMessages = 0;
    // lysd //                 //-- Account for overflow during packet loss
    // lysd //                 if(_message.seq < expectedSeq) {
    // lysd //                     lostMessages = (_message.seq + 255) - expectedSeq;
    // lysd //                 } else {
    // lysd //                     lostMessages = _message.seq - expectedSeq;
    // lysd //                 }
    // lysd //                 // Log how many were lost
    // lysd //                 totalLossCounter[mavlinkChannel] += static_cast<uint64_t>(lostMessages);
    // lysd //             }
    // lysd //
    // lysd //             // And update the last sequence number for this system/component pair
    // lysd //             lastIndex[_message.sysid][_message.compid] = _message.seq;;
    // lysd //             // Calculate new loss ratio
    // lysd //             uint64_t totalSent = totalReceiveCounter[mavlinkChannel] + totalLossCounter[mavlinkChannel];
    // lysd //             float receiveLossPercent = static_cast<float>(static_cast<double>(totalLossCounter[mavlinkChannel]) / static_cast<double>(totalSent));
    // lysd //             receiveLossPercent *= 100.0f;
    // lysd //             receiveLossPercent = (receiveLossPercent * 0.5f) + (runningLossPercent[mavlinkChannel] * 0.5f);
    // lysd //             runningLossPercent[mavlinkChannel] = receiveLossPercent;
    // lysd //
    // lysd //             //qDebug() << foo << _message.seq << expectedSeq << lastSeq << totalLossCounter[mavlinkChannel] << totalReceiveCounter[mavlinkChannel] << totalSentCounter[mavlinkChannel] << "(" << _message.sysid << _message.compid << ")";
    // lysd //
    // lysd //             //-----------------------------------------------------------------
    // lysd //             // MAVLink forwarding
    // lysd //             bool forwardingEnabled = _app->toolbox()->settingsManager()->appSettings()->forwardMavlink()->rawValue().toBool();
    // lysd //             if (forwardingEnabled) {
    // lysd //                 SharedLinkInterfacePtr forwardingLink = _linkMgr->mavlinkForwardingLink();
    // lysd //
    // lysd //                 if (forwardingLink) {
    // lysd //                     uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    // lysd //                     int len = mavlink_msg_to_send_buffer(buf, &_message);
    // lysd //                     forwardingLink->writeBytesThreadSafe((const char*)buf, len);
    // lysd //                 }
    // lysd //             }
    // lysd //
    // lysd //             //-----------------------------------------------------------------
    // lysd //             // Log data
    // lysd //             if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {
    // lysd //                 uint8_t buf[MAVLINK_MAX_PACKET_LEN+sizeof(quint64)];
    // lysd //
    // lysd //                 // Write the uint64 time in microseconds in big endian format before the message.
    // lysd //                 // This timestamp is saved in UTC time. We are only saving in ms precision because
    // lysd //                 // getting more than this isn't possible with Qt without a ton of extra code.
    // lysd //                 quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);
    // lysd //                 qToBigEndian(time, buf);
    // lysd //
    // lysd //                 // Then write the message to the buffer
    // lysd //                 int len = mavlink_msg_to_send_buffer(buf + sizeof(quint64), &_message);
    // lysd //
    // lysd //                 // Determine how many bytes were written by adding the timestamp size to the message size
    // lysd //                 len += sizeof(quint64);
    // lysd //
    // lysd //                 // Now write this timestamp/message pair to the log.
    // lysd //                 QByteArray b(reinterpret_cast<const char*>(buf), len);
    // lysd //                 if(_tempLogFile.write(b) != len)
    // lysd //                 {
    // lysd //                     // If there's an error logging data, raise an alert and stop logging.
    // lysd //                     emit protocolStatusMessage(tr("MAVLink Protocol"), tr("MAVLink Logging failed. Could not write to file %1, logging disabled.").arg(_tempLogFile.fileName()));
    // lysd //                     _stopLogging();
    // lysd //                     _logSuspendError = true;
    // lysd //                 }
    // lysd //
    // lysd //                 // Check for the vehicle arming going by. This is used to trigger log save.
    // lysd //                 if (!_vehicleWasArmed && _message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
    // lysd //                     mavlink_heartbeat_t state;
    // lysd //                     mavlink_msg_heartbeat_decode(&_message, &state);
    // lysd //                     if (state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY) {
    // lysd //                         _vehicleWasArmed = true;
    // lysd //                     }
    // lysd //                 }
    // lysd //             }
    // lysd //
    // lysd //             if (_message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
    // lysd //                 _startLogging();
    // lysd //                 mavlink_heartbeat_t heartbeat;
    // lysd //                 mavlink_msg_heartbeat_decode(&_message, &heartbeat);
    // lysd //                 emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, heartbeat.autopilot, heartbeat.type);
    // lysd //             } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY) {
    // lysd //                 _startLogging();
    // lysd //                 mavlink_high_latency_t highLatency;
    // lysd //                 mavlink_msg_high_latency_decode(&_message, &highLatency);
    // lysd //                 // HIGH_LATENCY does not provide autopilot or type information, generic is our safest bet
    // lysd //                 emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);
    // lysd //             } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY2) {
    // lysd //                 _startLogging();
    // lysd //                 mavlink_high_latency2_t highLatency2;
    // lysd //                 mavlink_msg_high_latency2_decode(&_message, &highLatency2);
    // lysd //                 emit vehicleHeartbeatInfo(link, _message.sysid, _message.compid, highLatency2.autopilot, highLatency2.type);
    // lysd //             }
    // lysd //
    // lysd // #if 0
    // lysd //             // Given the current state of SiK Radio firmwares there is no way to make the code below work.
    // lysd //             // The ArduPilot implementation of SiK Radio firmware always sends MAVLINK_MSG_ID_RADIO_STATUS as a mavlink 1
    // lysd //             // packet even if the vehicle is sending Mavlink 2.
    // lysd //
    // lysd //             // Detect if we are talking to an old radio not supporting v2
    // lysd //             mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
    // lysd //             if (_message.msgid == MAVLINK_MSG_ID_RADIO_STATUS && _radio_version_mismatch_count != -1) {
    // lysd //                 if ((mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1)
    // lysd //                 && !(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
    // lysd //                     _radio_version_mismatch_count++;
    // lysd //                 }
    // lysd //             }
    // lysd //
    // lysd //             if (_radio_version_mismatch_count == 5) {
    // lysd //                 // Warn the user if the radio continues to send v1 while the link uses v2
    // lysd //                 emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Detected radio still using MAVLink v1.0 on a link with MAVLink v2.0 enabled. Please upgrade the radio firmware."));
    // lysd //                 // Set to flag warning already shown
    // lysd //                 _radio_version_mismatch_count = -1;
    // lysd //                 // Flick link back to v1
    // lysd //                 qDebug() << "Switching outbound to mavlink 1.0 due to incoming mavlink 1.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
    // lysd //                 mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
    // lysd //             }
    // lysd // #endif
    // lysd //
    // lysd //             // Update MAVLink status on every 32th packet
    // lysd //             if ((totalReceiveCounter[mavlinkChannel] & 0x1F) == 0) {
    // lysd //                 emit mavlinkMessageStatus(_message.sysid, totalSent, totalReceiveCounter[mavlinkChannel], totalLossCounter[mavlinkChannel], receiveLossPercent);
    // lysd //             }
    // lysd //
    // lysd //             // The packet is emitted as a whole, as it is only 255 - 261 bytes short
    // lysd //             // kind of inefficient, but no issue for a groundstation pc.
    // lysd //             // It buys as reentrancy for the whole code over all threads
    // lysd //             emit messageReceived(link, _message);
    // lysd //
    // lysd //             // Anyone handling the message could close the connection, which deletes the link,
    // lysd //             // so we check if it's expired
    // lysd //             if (1 == linkPtr.use_count()) {
    // lysd //                 break;
    // lysd //             }
    // lysd //
    // lysd //             // Reset message parsing
    // lysd //             memset(&_status,  0, sizeof(_status));
    // lysd //             memset(&_message, 0, sizeof(_message));
    // lysd //         }
    // lysd //     }

    if(b.size() != sizeof (mavlink_message_t)){
        return ;
    }

    const mavlink_message_t* const pMessage = (const mavlink_message_t* const)(b.constData());

    _message = *pMessage;


    uint8_t mavlinkChannel = MAVLINK_COMM_0;


    if(!QGCApplication::multiVehicleManager()){
        return;
    }

    auto* vehiclesMap = QGCApplication::multiVehicleManager()->weapon_vehicles();
    if(!vehiclesMap){
         return;
    }

    auto vehiclesIter = vehiclesMap->find(MultiVehicleManager::WeaponVehicle_ID(weaponId, _message.sysid));

    if(vehiclesMap->end() != vehiclesIter){
        mavlinkChannel = vehiclesIter.value()->mavlinkChannel();
        if(mavlinkChannel >= MAVLINK_COMM_NUM_BUFFERS){
            mavlinkChannel = MAVLINK_COMM_0;
        }
    }

    if(link){
       if (!link->decodedFirstMavlinkPacket() && vehiclesMap->end() != vehiclesIter) {
           link->setDecodedFirstMavlinkPacket(true);
           mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
           if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
               qCDebug(MAVLinkProtocolLog) << "Switching outbound to mavlink 2.0 due to incoming mavlink 2.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
               mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
               // Set all links to v2
               setVersion(200);
           }
       }
    }

    //-----------------------------------------------------------------
    // MAVLink Status
    uint8_t lastSeq = lastIndex[_message.sysid][_message.compid];
    uint8_t expectedSeq = lastSeq + 1;
    // Increase receive counter
    totalReceiveCounter[mavlinkChannel]++;
    // Determine what the next expected sequence number is, accounting for
    // never having seen a message for this system/component pair.
    if(firstMessage[_message.sysid][_message.compid]) {
        firstMessage[_message.sysid][_message.compid] = 0;
        lastSeq     = _message.seq;
        expectedSeq = _message.seq;
    }
    // And if we didn't encounter that sequence number, record the error
    //int foo = 0;
    if (_message.seq != expectedSeq)
    {
        //foo = 1;
        int lostMessages = 0;
        //-- Account for overflow during packet loss
        if(_message.seq < expectedSeq) {
            lostMessages = (_message.seq + 255) - expectedSeq;
        } else {
            lostMessages = _message.seq - expectedSeq;
        }
        // Log how many were lost
        totalLossCounter[mavlinkChannel] += static_cast<uint64_t>(lostMessages);
    }

    // And update the last sequence number for this system/component pair
    lastIndex[_message.sysid][_message.compid] = _message.seq;
    // Calculate new loss ratio
    uint64_t totalSent = totalReceiveCounter[mavlinkChannel] + totalLossCounter[mavlinkChannel];
    float receiveLossPercent = static_cast<float>(static_cast<double>(totalLossCounter[mavlinkChannel]) / static_cast<double>(totalSent));
    receiveLossPercent *= 100.0f;
    receiveLossPercent = (receiveLossPercent * 0.5f) + (runningLossPercent[mavlinkChannel] * 0.5f);
    runningLossPercent[mavlinkChannel] = receiveLossPercent;

    //qDebug() << foo << _message.seq << expectedSeq << lastSeq << totalLossCounter[mavlinkChannel] << totalReceiveCounter[mavlinkChannel] << totalSentCounter[mavlinkChannel] << "(" << _message.sysid << _message.compid << ")";

    // lysd //  //-----------------------------------------------------------------
    // lysd //  // MAVLink forwarding
    // lysd //  bool forwardingEnabled = _app->toolbox()->settingsManager()->appSettings()->forwardMavlink()->rawValue().toBool();
    // lysd //  if (forwardingEnabled) {
    // lysd //      SharedLinkInterfacePtr forwardingLink = _linkMgr->mavlinkForwardingLink();
    // lysd //
    // lysd //      if (forwardingLink) {
    // lysd //          uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    // lysd //          int len = mavlink_msg_to_send_buffer(buf, &_message);
    // lysd //          forwardingLink->writeBytesThreadSafe((const char*)buf, len);
    // lysd //      }
    // lysd //  }

    //-----------------------------------------------------------------
    // Log data
    // lysd // if (!_logSuspendError && !_logSuspendReplay && _tempLogFile.isOpen()) {
    // lysd //     uint8_t buf[MAVLINK_MAX_PACKET_LEN+sizeof(quint64)];
    // lysd //
    // lysd //     // Write the uint64 time in microseconds in big endian format before the message.
    // lysd //     // This timestamp is saved in UTC time. We are only saving in ms precision because
    // lysd //     // getting more than this isn't possible with Qt without a ton of extra code.
    // lysd //     quint64 time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch() * 1000);
    // lysd //     qToBigEndian(time, buf);
    // lysd //
    // lysd //     // Then write the message to the buffer
    // lysd //     int len = mavlink_msg_to_send_buffer(buf + sizeof(quint64), &_message);
    // lysd //
    // lysd //     // Determine how many bytes were written by adding the timestamp size to the message size
    // lysd //     len += sizeof(quint64);
    // lysd //
    // lysd //     // Now write this timestamp/message pair to the log.
    // lysd //     // lysd // QByteArray b(reinterpret_cast<const char*>(buf), len);
    // lysd //     // lysd // if(_tempLogFile.write(b) != len)
    // lysd //     // lysd // {
    // lysd //     // lysd //     // If there's an error logging data, raise an alert and stop logging.
    // lysd //     // lysd //     emit protocolStatusMessage(tr("MAVLink Protocol"), tr("MAVLink Logging failed. Could not write to file %1, logging disabled.").arg(_tempLogFile.fileName()));
    // lysd //     // lysd //     _stopLogging();
    // lysd //     // lysd //     _logSuspendError = true;
    // lysd //     // lysd // }
    // lysd //
    // lysd //     // Check for the vehicle arming going by. This is used to trigger log save.
    // lysd //     if (!_vehicleWasArmed && _message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
    // lysd //         mavlink_heartbeat_t state;
    // lysd //         mavlink_msg_heartbeat_decode(&_message, &state);
    // lysd //         if (state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY) {
    // lysd //             _vehicleWasArmed = true;
    // lysd //         }
    // lysd //     }
    // lysd // }

    if (_message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        _startLogging();
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(&_message, &heartbeat);
        // lysd // emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, heartbeat.autopilot, heartbeat.type);

        if(vehiclesMap->end() == vehiclesIter){
            emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, heartbeat.autopilot, heartbeat.type);

            auto vehiclesIter2 = vehiclesMap->find(MultiVehicleManager::WeaponVehicle_ID(weaponId, _message.sysid));
            if(vehiclesMap->end() != vehiclesIter2){
                vehiclesIter2.value()->_mavlinkMessageReceived(link, weaponId, _message);
            }
        }
    } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY) {
        _startLogging();
        mavlink_high_latency_t highLatency;
        mavlink_msg_high_latency_decode(&_message, &highLatency);
        // HIGH_LATENCY does not provide autopilot or type information, generic is our safest bet
        // lysd // emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);

        if(vehiclesMap->end() == vehiclesIter){
            emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);

            auto vehiclesIter2 = vehiclesMap->find(MultiVehicleManager::WeaponVehicle_ID(weaponId, _message.sysid));
            if(vehiclesMap->end() != vehiclesIter2){
                vehiclesIter2.value()->_mavlinkMessageReceived(link, weaponId, _message);
            }
        }
    } else if (_message.msgid == MAVLINK_MSG_ID_HIGH_LATENCY2) {
        _startLogging();
        mavlink_high_latency2_t highLatency2;
        mavlink_msg_high_latency2_decode(&_message, &highLatency2);
        // lysd // emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, highLatency2.autopilot, highLatency2.type);

        if(vehiclesMap->end() == vehiclesIter){
            emit vehicleHeartbeatInfo(link, weaponId, _message.sysid, _message.compid, highLatency2.autopilot, highLatency2.type);

            auto vehiclesIter2 = vehiclesMap->find(MultiVehicleManager::WeaponVehicle_ID(weaponId, _message.sysid));
            if(vehiclesMap->end() != vehiclesIter2){
                vehiclesIter2.value()->_mavlinkMessageReceived(link, weaponId, _message);
            }
        }
    }

#if 0
    // Given the current state of SiK Radio firmwares there is no way to make the code below work.
    // The ArduPilot implementation of SiK Radio firmware always sends MAVLINK_MSG_ID_RADIO_STATUS as a mavlink 1
    // packet even if the vehicle is sending Mavlink 2.

    // Detect if we are talking to an old radio not supporting v2
    mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
    if (_message.msgid == MAVLINK_MSG_ID_RADIO_STATUS && _radio_version_mismatch_count != -1) {
        if ((mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1)
                && !(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)) {
            _radio_version_mismatch_count++;
        }
    }

    if (_radio_version_mismatch_count == 5) {
        // Warn the user if the radio continues to send v1 while the link uses v2
        emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Detected radio still using MAVLink v1.0 on a link with MAVLink v2.0 enabled. Please upgrade the radio firmware."));
        // Set to flag warning already shown
        _radio_version_mismatch_count = -1;
        // Flick link back to v1
        qDebug() << "Switching outbound to mavlink 1.0 due to incoming mavlink 1.0 packet:" << mavlinkStatus << mavlinkChannel << mavlinkStatus->flags;
        mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
    }
#endif


    if(vehiclesMap->end() != vehiclesIter){
        // Update MAVLink status on every 32th packet
        if ((totalReceiveCounter[mavlinkChannel] & 0x1F) == 0) {
            // lysd // emit mavlinkMessageStatus(weaponId, _message.sysid, totalSent, totalReceiveCounter[mavlinkChannel], totalLossCounter[mavlinkChannel], receiveLossPercent);
            vehiclesIter.value()->_mavlinkMessageStatus(weaponId, _message.sysid, totalSent, totalReceiveCounter[mavlinkChannel], totalLossCounter[mavlinkChannel], receiveLossPercent);
        }

        // The packet is emitted as a whole, as it is only 255 - 261 bytes short
        // kind of inefficient, but no issue for a groundstation pc.
        // It buys as reentrancy for the whole code over all threads
        // lysd // emit messageReceived(link, weaponId, _message);
        vehiclesIter.value()->_mavlinkMessageReceived(link, weaponId, _message);
    }
    // Anyone handling the message could close the connection, which deletes the link,
    // so we check if it's expired

    // Reset message parsing
    memset(&_status,  0, sizeof(_status));
    memset(&_message, 0, sizeof(_message));
}

/**
 * @return The name of this protocol
 **/
QString MAVLinkProtocol::getName()
{
    return tr("MAVLink protocol");
}

/** @return System id of this application */
int MAVLinkProtocol::getSystemId() const
{
    return systemId;
}

void MAVLinkProtocol::setSystemId(int id)
{
    systemId = id;
}

/** @return Component id of this application */
int MAVLinkProtocol::getComponentId() const
{
    return MAV_COMP_ID_MISSIONPLANNER;
}

void MAVLinkProtocol::enableVersionCheck(bool enabled)
{
    m_enable_version_check = enabled;
    emit versionCheckChanged(enabled);
}

void MAVLinkProtocol::_vehicleCountChanged(void)
{
    // lysd // int count = _multiVehicleManager->vehicles()->count();
    // lysd // if (count == 0) {
    // lysd //     // Last vehicle is gone, close out logging
    // lysd //     _stopLogging();
    // lysd //     _radio_version_mismatch_count = 0;
    // lysd // }
}

/// @brief Closes the log file if it is open
bool MAVLinkProtocol::_closeLogFile(void)
{
    // lysd // if (_tempLogFile.isOpen()) {
    // lysd //     if (_tempLogFile.size() == 0) {
    // lysd //         // Don't save zero byte files
    // lysd //         _tempLogFile.remove();
    // lysd //         return false;
    // lysd //     } else {
    // lysd //         _tempLogFile.flush();
    // lysd //         _tempLogFile.close();
    // lysd //         return true;
    // lysd //     }
    // lysd // }
    return false;
}

void MAVLinkProtocol::_startLogging(void)
{
    //-- Are we supposed to write logs?
    // lysd //     if (qgcApp()->runningUnitTests()) {
    // lysd //         return;
    // lysd //     }
    // lysd //     AppSettings* appSettings = _app->toolbox()->settingsManager()->appSettings();
    // lysd //     if(appSettings->disableAllPersistence()->rawValue().toBool()) {
    // lysd //         return;
    // lysd //     }
    // lysd // #ifdef __mobile__
    // lysd //     //-- Mobile build don't write to /tmp unless told to do so
    // lysd //     if (!appSettings->telemetrySave()->rawValue().toBool()) {
    // lysd //         return;
    // lysd //     }
    // lysd // #endif
    // lysd //     //-- Log is always written to a temp file. If later the user decides they want
    // lysd //     //   it, it's all there for them.
    // lysd //     if (!_tempLogFile.isOpen()) {
    // lysd //         if (!_logSuspendReplay) {
    // lysd //             if (!_tempLogFile.open()) {
    // lysd //                 emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Opening Flight Data file for writing failed. "
    // lysd //                                                                       "Unable to write to %1. Please choose a different file location.").arg(_tempLogFile.fileName()));
    // lysd //                 _closeLogFile();
    // lysd //                 _logSuspendError = true;
    // lysd //                 return;
    // lysd //             }
    // lysd //
    // lysd //             qCDebug(MAVLinkProtocolLog) << "Temp log" << _tempLogFile.fileName();
    // lysd //             emit checkTelemetrySavePath();
    // lysd //
    // lysd //             _logSuspendError = false;
    // lysd //         }
    // lysd //     }
}

void MAVLinkProtocol::_stopLogging(void)
{
    // lysd //     if (_tempLogFile.isOpen()) {
    // lysd //         if (_closeLogFile()) {
    // lysd //             if ((_vehicleWasArmed || _app->toolbox()->settingsManager()->appSettings()->telemetrySaveNotArmed()->rawValue().toBool()) &&
    // lysd //                 _app->toolbox()->settingsManager()->appSettings()->telemetrySave()->rawValue().toBool() &&
    // lysd //                 !_app->toolbox()->settingsManager()->appSettings()->disableAllPersistence()->rawValue().toBool()) {
    // lysd //                 emit saveTelemetryLog(_tempLogFile.fileName());
    // lysd //             } else {
    // lysd //                 QFile::remove(_tempLogFile.fileName());
    // lysd //             }
    // lysd //         }
    // lysd //     }
    // lysd //     _vehicleWasArmed = false;
}

/// @brief Checks the temp directory for log files which may have been left there.
///         This could happen if QGC crashes without the temp log file being saved.
///         Give the user an option to save these orphaned files.
void MAVLinkProtocol::checkForLostLogFiles(void)
{
    // lysd // QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    // lysd //
    // lysd // QString filter(QString("*.%1").arg(_logFileExtension));
    // lysd // QFileInfoList fileInfoList = tempDir.entryInfoList(QStringList(filter), QDir::Files);
    // lysd // //qDebug() << "Orphaned log file count" << fileInfoList.count();
    // lysd //
    // lysd // for(const QFileInfo& fileInfo: fileInfoList) {
    // lysd //     //qDebug() << "Orphaned log file" << fileInfo.filePath();
    // lysd //     if (fileInfo.size() == 0) {
    // lysd //         // Delete all zero length files
    // lysd //         QFile::remove(fileInfo.filePath());
    // lysd //         continue;
    // lysd //     }
    // lysd //     emit saveTelemetryLog(fileInfo.filePath());
    // lysd // }
}

void MAVLinkProtocol::suspendLogForReplay(bool suspend)
{
    _logSuspendReplay = suspend;
}

void MAVLinkProtocol::deleteTempLogFiles(void)
{
    // lysd // QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    // lysd //
    // lysd // QString filter(QString("*.%1").arg(_logFileExtension));
    // lysd // QFileInfoList fileInfoList = tempDir.entryInfoList(QStringList(filter), QDir::Files);
    // lysd //
    // lysd // for (const QFileInfo& fileInfo: fileInfoList) {
    // lysd //     QFile::remove(fileInfo.filePath());
    // lysd // }
}
