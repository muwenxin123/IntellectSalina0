#ifndef IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROLL_H
#define IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROLL_H

#include <QtMath>
#include <QGeoCoordinate>
#include <time.h>
#include <float.h>

#define MAVLINK_USE_MESSAGE_INFO
#define MAVLINK_EXTERNAL_RX_STATUS
#include <stddef.h>

#ifdef __GNUC__

#if __GNUC__ > 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif

#else
#pragma warning(push, 0)
#endif

#ifdef MAVLINK_COMM_NUM_BUFFERS
#error __FILE__
#endif

#ifndef MAVLINK_COMM_NUM_BUFFERS
#if (defined linux) | (defined __linux) | (defined  __MACH__) | (defined _WIN32)
# define MAVLINK_COMM_NUM_BUFFERS 255
#else
# define MAVLINK_COMM_NUM_BUFFERS 4
#endif
#endif

#include "Protocol/mavlink/v2.0/mavlink_types.h"
extern mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];
#include "Protocol/mavlink/v2.0/ardupilotmega/mavlink.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#else
#pragma warning(pop, 0)
#endif

#ifdef __GNUC__
#define PACKED_STRUCT( __Declaration__ ) __Declaration__ __attribute__((packed))
#else
#define PACKED_STRUCT( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif

#include "IProtocolParseXygsServerImpl_Control.h"
#include "QGCTemporaryFile.h"

#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include "Protocol/XygsSeverClientControl/Mavlink_Control_SeverClient.h"
#include "DataManage/NebulaUAVState/NebulaUAVState.h"

#define LY_PROTOCOLPARSE_XYGSSERVER_IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROL_RECV_MESSAGE
#define  LY_PROTOCOLPARSE_XYGSSERVER_IPROTOCOLPARSEXYGSSERVERIMPL_MAVLINK_CONTROL_SEND_MESSAGE

namespace qnzkna {

namespace NetSeverClient {

class IProtocolParseXygsServerImpl_Mavlink_Control : public IProtocolParseXygsServerImpl_Control
{
public:
    virtual void start() override;
    virtual void stop() override;
    virtual bool Pack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) override;
    virtual bool UnPack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) override;
    virtual bool NetHeader_XygsServer(const int nLength, const char * const pRecvBuf) override;
    virtual bool NetRecv_XygsServer(const int nLength, const char * const pRecvBuf) override;

private:
    IProtocolParseXygsServerImpl_Mavlink_Control();
    virtual ~IProtocolParseXygsServerImpl_Mavlink_Control() override;
    IProtocolParseXygsServerImpl_Mavlink_Control(const IProtocolParseXygsServerImpl_Mavlink_Control&) = delete;
    IProtocolParseXygsServerImpl_Mavlink_Control(const IProtocolParseXygsServerImpl_Mavlink_Control&&) = delete;
    IProtocolParseXygsServerImpl_Mavlink_Control& operator=(const IProtocolParseXygsServerImpl_Mavlink_Control&) = delete;
    IProtocolParseXygsServerImpl_Mavlink_Control& operator=(const IProtocolParseXygsServerImpl_Mavlink_Control&&) = delete;

private:
    void setVersion(unsigned version);
    inline bool SendMsg(const char * const msgbuffer, const unsigned int msgBufferLen, const qnzkna::framework::MessageReferenceProps& props, qnzkna::framework::IRegistry* pContext);

private:
    bool _closeLogFile(void);
    void _startLogging(void);
    void _stopLogging(void);

    bool _closeCmdLogFile(void);
    void _startCmdLogging(void);
    void _stopCmdLogging(void);

    bool _logSuspendError;
    bool _logSuspendReplay;
    bool _vehicleWasArmed;

    bool                _tempLog              = true;
    bool                _tempLogRemoveZero    = false;
    bool                _tempCmdLog           = true;
    bool                _tempCmdLogRemoveZero = false;
    QGCTemporaryFile    _tempLogFile;
    QGCTemporaryFile    _tempCmdLogFile;
    static const char*  _tempLogFileTemplate;
    static const char*  _logFileExtension;

private:
    struct Mavlink_Control_Msg {
        uint8_t           mavlinkChannel             = 0;
        bool              decodedFirstMavlinkPacket  = false;
        bool              isPX4Flow                  = false;
        int               vehicleReferenceCount      = 0;

        bool                            _updateBag;
        xygsJL_20                       _bag;

        std::string*                    _strUnWeaponIdKey;

        uint8_t     _id;
        uint8_t     _defaultComponentId;
        uint8_t     _sysid;
        uint8_t     _defaultSysComponentId;
        uint8_t     _chan;

        bool        _mavStartTimer;
        double      _loadProgress;
        bool        _parametersReady;
        bool        _missingParameters;
        bool        _initialLoadComplete;
        bool        _waitingForDefaultComponent;
        bool        _saveRequired;
        bool        _metaDataAddedToFacts;
        bool        _logReplay;

        bool                                          _initialRequestTimeoutTimer;
        time_t                                        _requestTimeoutTime;
        static const int                              _initialRequestTimeoutTime = 5000;
        static const int                              _maxInitialRequestListRetry = 5;
        int                                           _initialRequestRetryCount;
        static const int                              _maxInitialLoadRetrySingleParam = 10;
        static const int                              _maxReadWriteRetry = 5;
        bool                                          _disableAllRetries;

        bool                                          _waitingParamTimeoutTimer;
        time_t                                        _waitingParamTimeoutTime;
        static const int                              _waitingParamTimeoutMSecs = 5000;
        bool                                          _indexBatchQueueActive;

        bool                                 _mavCommandAckTimer;
        time_t                               _mavCommandAckTime;
        int                                  _mavCommandRetryCount;
        static const int                     _mavCommandMaxRetryCount = 3;
        static const int                     _mavCommandAckTimeoutMSecs = 3000;
        static const int                     _mavCommandAckTimeoutMSecsHighLatency = 120000;
        static const int                     _gcsHeartbeatRateMSecs = 1000;
        time_t                               _gcsHeartbeatTime;

        bool             _active;
        bool             _offlineEditingVehicle;
        MAV_MISSION_TYPE _planType;

        static const int versionNotSetValue = -1;
        MAV_AUTOPILOT    _firmwareType;
        MAV_TYPE         _vehicleType;

        bool             _coordinateChanged;
        QGeoCoordinate    _coordinate;
        bool             _homePositionChanged;
        QGeoCoordinate    _homePosition;

        bool            _flying;
        bool            _landing;
        bool            _vtolInFwdFlight;
        int             _rcRSSI;
        double          _rcRSSIstore;
        uint32_t        _onboardControlSensorsPresent;
        uint32_t        _onboardControlSensorsEnabled;
        uint32_t        _onboardControlSensorsHealth;
        uint32_t        _onboardControlSensorsUnhealthy;
        uint8_t         _fix_type;
        bool            _gpsRawIntMessageAvailable;
        bool            _globalPositionIntMessageAvailable;
        bool            _armed;
        bool            _vehicleWasArmed;
        uint8_t         _base_mode;
        uint32_t        _custom_mode;
        ENebulaUAVFlyCtrlMode _nebula_uav_mode;

        static const int _sendMessageMultipleRetries = 5;
        static const int _sendMessageMultipleIntraMessageDelay = 500;

        bool                _sendMultipleTimer;
        int                 _nextSendMessageMultipleIndex;

        bool             _flightTimer;
        bool             _mapTrajectoryTimer;
        time_t           _mapTrajectoryTime;
        QGeoCoordinate    _mapTrajectoryLastCoordinate;
        bool             _mapTrajectoryHaveFirstCoordinate;
        static const int _mapTrajectoryMsecsBetweenPoints = 1000;

        unsigned        _maxProtoVersion;
        bool            _vehicleCapabilitiesKnown;
        uint64_t        _capabilityBits;
        bool            _highLatencyLink;

        int _firmwareMajorVersion;
        int _firmwareMinorVersion;
        int _firmwarePatchVersion;
        int _firmwareCustomMajorVersion;
        int _firmwareCustomMinorVersion;
        int _firmwareCustomPatchVersion;
        FIRMWARE_VERSION_TYPE _firmwareVersionType;

        std::string   _gitHash;
        uint64_t      _uid;

        double _altitudeRelativeFact;
        double _altitudeLocalFact;
        double _altitudeAMSLFact;
        double _flightDistanceFact;
        double _flightTimeFact;
        double _distanceToHomeFact;
        double _headingToHomeFact;
        double _distanceToGCSFact;

        static const int cMaxRcChannels = 18;

        Mavlink_Control_Msg()
            : _updateBag(false)
            , _bag()
            , _strUnWeaponIdKey(NULL)
            , _id(0)
            , _defaultComponentId(0)
            , _sysid(255)
            , _defaultSysComponentId(MAV_COMP_ID_ALL)
            , _chan(0)
            , _mavStartTimer(false)
            , _loadProgress(0)
            , _parametersReady(false)
            , _missingParameters(false)
            , _initialLoadComplete(false)
            , _waitingForDefaultComponent(false)
            , _saveRequired(false)
            , _metaDataAddedToFacts(false)
            , _logReplay(false)
            , _initialRequestTimeoutTimer(false)
            , _initialRequestRetryCount(0)
            , _disableAllRetries(false)
            , _waitingParamTimeoutTimer(false)
            , _waitingParamTimeoutTime(0)
            , _indexBatchQueueActive(false)
            , _mavCommandAckTimer(false)
            , _mavCommandAckTime(0)
            , _mavCommandRetryCount(0)
            , _gcsHeartbeatTime(0)
            , _active(false)
            , _offlineEditingVehicle(false)
            , _planType(MAV_MISSION_TYPE::MAV_MISSION_TYPE_MISSION)
            , _firmwareType(MAV_AUTOPILOT::MAV_AUTOPILOT_ENUM_END)
            , _vehicleType(MAV_TYPE::MAV_TYPE_ENUM_END)
            , _coordinateChanged(false)
            , _coordinate()
            , _homePositionChanged(false)
            , _homePosition()
            , _flying(false)
            , _landing(false)
            , _vtolInFwdFlight(false)
            , _rcRSSI(255)
            , _rcRSSIstore(255.0)
            , _onboardControlSensorsPresent(0)
            , _onboardControlSensorsEnabled(0)
            , _onboardControlSensorsHealth(0)
            , _onboardControlSensorsUnhealthy(0)
            , _fix_type(GPS_FIX_TYPE::GPS_FIX_TYPE_ENUM_END)
            , _gpsRawIntMessageAvailable(false)
            , _globalPositionIntMessageAvailable(false)
            , _armed(false)
            , _base_mode(0)
            , _custom_mode(0)
            , _nebula_uav_mode((ENebulaUAVFlyCtrlMode)0)
            , _flightTimer(false)
            , _mapTrajectoryTimer(false)
            , _mapTrajectoryTime(0)
            , _mapTrajectoryLastCoordinate()
            , _mapTrajectoryHaveFirstCoordinate(false)
            , _maxProtoVersion(0)
            , _vehicleCapabilitiesKnown(false)
            , _capabilityBits(0)
            , _highLatencyLink(false)
            , _firmwareMajorVersion(versionNotSetValue)
            , _firmwareMinorVersion(versionNotSetValue)
            , _firmwarePatchVersion(versionNotSetValue)
            , _firmwareCustomMajorVersion(versionNotSetValue)
            , _firmwareCustomMinorVersion(versionNotSetValue)
            , _firmwareCustomPatchVersion(versionNotSetValue)
            , _firmwareVersionType(FIRMWARE_VERSION_TYPE_OFFICIAL)
            , _gitHash("-1")
            , _uid(0)
            , _altitudeRelativeFact(0)
            , _altitudeLocalFact(0)
            , _altitudeAMSLFact(0)
            , _flightDistanceFact(0)
            , _flightTimeFact(0)
            , _distanceToHomeFact(0)
            , _headingToHomeFact(0)
            , _distanceToGCSFact(0)
        {}

        void _handleReceivedMavlinkMessage(const mavlink_message_t& message, const qnzkna::framework::MessageReferenceProps& props, qnzkna::framework::IRegistry* pContext) {
            switch (message.msgid) {
            case MAVLINK_MSG_ID_HOME_POSITION:
                _handleHomePosition(message);
                break;
            case MAVLINK_MSG_ID_HEARTBEAT:
                _handleHeartbeat(message);
                break;
            case MAVLINK_MSG_ID_SYSTEM_TIME:
                _handleSystemTime(message);
                break;
            case MAVLINK_MSG_ID_RADIO_STATUS:
                _handleRadioStatus(message);
                break;
            case MAVLINK_MSG_ID_RC_CHANNELS:
                _handleRCChannels(message);
                break;
            case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
                _handleRCChannelsRaw(message);
                break;
            case MAVLINK_MSG_ID_BATTERY_STATUS:
                _handleBatteryStatus(message);
                break;
            case MAVLINK_MSG_ID_SYS_STATUS:
                _handleSysStatus(message);
                break;
            case MAVLINK_MSG_ID_RAW_IMU:
                _handleMavlinkRawImu(message);
                break;
            case MAVLINK_MSG_ID_SCALED_IMU:
                _handleMavlinkScaledImu1(message);
                break;
            case MAVLINK_MSG_ID_SCALED_IMU2:
                _handleMavlinkScaledImu2(message);
                break;
            case MAVLINK_MSG_ID_SCALED_IMU3:
                _handleMavlinkScaledImu3(message);
                break;
            case MAVLINK_MSG_ID_VIBRATION:
                _handleVibration(message);
                break;
            case MAVLINK_MSG_ID_EXTENDED_SYS_STATE:
                _handleExtendedSysState(message);
                break;
            case MAVLINK_MSG_ID_COMMAND_ACK:
                _handleCommandAck(message);
                break;
            case MAVLINK_MSG_ID_COMMAND_LONG:
                _handleCommandLong(message);
                break;
            case MAVLINK_MSG_ID_AUTOPILOT_VERSION:

                break;
            case MAVLINK_MSG_ID_PROTOCOL_VERSION:

                break;
            case MAVLINK_MSG_ID_WIND_COV:
                _handleWindCov(message);
                break;
            case MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS:
                _handleHilActuatorControls(message);
                break;
            case MAVLINK_MSG_ID_LOGGING_DATA:
                _handleMavlinkLoggingData(message);
                break;
            case MAVLINK_MSG_ID_LOGGING_DATA_ACKED:
                _handleMavlinkLoggingDataAcked(message);
                break;
            case MAVLINK_MSG_ID_GPS_RAW_INT:
                _handleGpsRawInt(message);
                break;
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                _handleGlobalPositionInt(message);
                break;
            case MAVLINK_MSG_ID_ALTITUDE:
                _handleAltitude(message);
                break;
            case MAVLINK_MSG_ID_VFR_HUD:
                _handleVfrHud(message);
                break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE:
                _handleScaledPressure(message);
                break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE2:
                _handleScaledPressure2(message);
                break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE3:
                _handleScaledPressure3(message);
                break;
            case MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED:
                _handleCameraImageCaptured(message);
                break;
            case MAVLINK_MSG_ID_ADSB_VEHICLE:
                _handleADSBVehicle(message);
                break;
            case MAVLINK_MSG_ID_HIGH_LATENCY:
                _handleHighLatency(message);
                break;
            case MAVLINK_MSG_ID_HIGH_LATENCY2:
                _handleHighLatency2(message);
                break;
            case MAVLINK_MSG_ID_ATTITUDE:
                _handleAttitude(message);
                break;
            case MAVLINK_MSG_ID_ATTITUDE_QUATERNION:
                _handleAttitudeQuaternion(message);
                break;
            case MAVLINK_MSG_ID_ATTITUDE_TARGET:

                break;
            case MAVLINK_MSG_ID_DISTANCE_SENSOR:
                _handleDistanceSensor(message);
                break;
            case MAVLINK_MSG_ID_ESTIMATOR_STATUS:
                _handleEstimatorStatus(message);
                break;
            case MAVLINK_MSG_ID_STATUSTEXT:

                break;

            case MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS:
                _handleOrbitExecutionStatus(message);
                break;
            case MAVLINK_MSG_ID_PING:
                _handlePing(message);
                break;
            case MAVLINK_MSG_ID_SERIAL_CONTROL:
                _handleSerialControl(message);
                break;

            case MAVLINK_MSG_ID_CAMERA_FEEDBACK:
                _handleCameraFeedback(message);
                break;
            case MAVLINK_MSG_ID_WIND:
                _handleWind(message);
                break;
            case MAVLINK_MSG_ID_HIL_CONTROLS:
                _handleHilControls(message);
                break;
            case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
                _handleServoOutputRaw(message);
                break;
            case MAVLINK_MSG_ID_PARAM_VALUE:
                _handleParamValueMsg(message);
                break;
            case MAVLINK_MSG_ID_HIGHRES_IMU:
                _handleHighResIMU(message);
                break;
            case MAVLINK_MSG_ID_HIL_SENSOR:
                _handleHilSensor(message);
                break;
            case MAVLINK_MSG_ID_POWER_STATUS:
                _handlePowerStatus(message);
                break;
            case MAVLINK_MSG_ID_HWSTATUS:
                _handleHWrStatus(message);
                break;
            default: {

            }
                break;
            }

            if(_updateBag){
                _updateBag = false;
                _sendMsgUAVRealTimeStateBag(props, pContext);
            }
        }

        uint8_t chan() const { return _chan; }
        bool px4Firmware() const { return _firmwareType == MAV_AUTOPILOT_PX4; }
        bool apmFirmware() const { return _firmwareType == MAV_AUTOPILOT_ARDUPILOTMEGA; }
        bool capabilitiesKnown(void) const { return _vehicleCapabilitiesKnown; }
        uint64_t capabilityBits(void) const { return _capabilityBits; }

        void _sendMsgUAVRealTimeStateBag(const qnzkna::framework::MessageReferenceProps& props, qnzkna::framework::IRegistry* pContext)
        {
            if(!pContext){
                return ;
            }

            pContext->messageNotify(new qnzkna::framework::MessageReference("MsgUAVRealTimeState",
                                                                            (const char*)&(_bag),
                                                                            sizeof(_bag),
                                                                            props));
        }

        void _handleExtendedSysState(const mavlink_message_t& message)
        {
            mavlink_extended_sys_state_t extendedState;
            mavlink_msg_extended_sys_state_decode(&message, &extendedState);

            switch (extendedState.landed_state) {
            case MAV_LANDED_STATE_ON_GROUND:
                _setFlying(false);
                _setLanding(false);
                break;
            case MAV_LANDED_STATE_TAKEOFF:
            case MAV_LANDED_STATE_IN_AIR:
                _setFlying(true);
                _setLanding(false);
                break;
            case MAV_LANDED_STATE_LANDING:
                _setFlying(true);
                _setLanding(true);
                break;
            default:
                break;
            }

        }

        void _handleCommandAck(const mavlink_message_t& message)
        {

            bool showError = false;

            mavlink_command_ack_t ack;
            mavlink_msg_command_ack_decode(&message, &ack);

            if (ack.command == MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES && ack.result != MAV_RESULT_ACCEPTED) {

                _setCapabilities(0);
            }

            if (ack.command == MAV_CMD_REQUEST_PROTOCOL_VERSION && ack.result != MAV_RESULT_ACCEPTED) {

                if (_maxProtoVersion == 0) {

                    _setMaxProtoVersion(100);
                }
                else {

                }

            }

            if (ack.command == MAV_CMD_NAV_TAKEOFF && ack.command == MAV_RESULT_ACCEPTED) {

                if (_armed != true) {
                    _updateArmed(_armed);
                }
            }

        }

        void _setHomePosition(QGeoCoordinate& homeCoord)
        {
            if (homeCoord != _homePosition) {
                _homePosition = homeCoord;
                _homePositionChanged = true;

            }
        }

        bool _apmArmingNotRequired(void)
        {
            std::string armingRequireParam("ARMING_REQUIRE");

            return true;
        }

        void _handlePing(const mavlink_message_t& message)
        {

        }

        void _handleHomePosition(const mavlink_message_t& message)
        {
            mavlink_home_position_t homePos;

            mavlink_msg_home_position_decode(&message, &homePos);

            QGeoCoordinate newHomePosition(homePos.latitude / 10000000.0,
                                           homePos.longitude / 10000000.0,
                                           homePos.altitude / 1000.0);
            _setHomePosition(newHomePosition);

            if (_homePositionChanged) {
                _homePositionChanged = false;

            }
        }

        void _handleHeartbeat(const mavlink_message_t& message)
        {

            mavlink_heartbeat_t heartbeat;
            mavlink_msg_heartbeat_decode(&message, &heartbeat);
            bool newArmed = false;
            if (heartbeat.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY) {
                newArmed = true;
            }

            if (newArmed && !_armed) {
                _bag.param.lockStatus = 0x02;
                _bag.param.flyPeriod  = ENebulaUAVFlyPhase::ENebulaUAVFlyPhase_Prepare;
            }else if (newArmed) {
                _bag.param.lockStatus = 0x02;
                _bag.param.flyPeriod  = ENebulaUAVFlyPhase::ENebulaUAVFlyPhase_ReadyFly;
            }
            else if(!newArmed){
                _bag.param.lockStatus = 0x02;
                _bag.param.flyPeriod  = ENebulaUAVFlyPhase::ENebulaUAVFlyPhase_Lock;
            }

            if (apmFirmware()) {
                if (!_apmArmingNotRequired() || !(_onboardControlSensorsPresent & MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS)) {

                    _updateArmed(newArmed);
                }
            }
            else {

                _updateArmed(newArmed);
            }

            if (heartbeat.base_mode != _base_mode || heartbeat.custom_mode != _custom_mode) {
                std::string previousFlightMode;
                if (_base_mode != 0 || _custom_mode != 0) {

                    previousFlightMode = flightMode();
                }
                _base_mode             = heartbeat.base_mode;
                _custom_mode           = heartbeat.custom_mode;
                if (previousFlightMode != flightMode()) {
                    _bag.param.controlMode = _nebula_uav_mode;

                }

                _bag.param.controlMode          = NEBULAUAV_ECTRLMODE_CUSTOM;
            }

            if(_armed && _flying && !_landing){
                _bag.param.lockStatus = 0x04;
                _bag.param.flyPeriod = ENebulaUAVFlyPhase::ENebulaUAVFlyPhase_BeginCruise;
            }else if(_armed && _flying && _landing){
                _bag.param.lockStatus = 0x04;
                _bag.param.flyPeriod = ENebulaUAVFlyPhase::ENebulaUAVFlyPhase_Landing;
            }

            _updateBag = true;

            if(_firmwareType != (MAV_AUTOPILOT)heartbeat.autopilot || _vehicleType != (MAV_TYPE)heartbeat.type){
                _firmwareType = (MAV_AUTOPILOT)heartbeat.autopilot;
                _vehicleType  = (MAV_TYPE)heartbeat.type;

            }
        }

        void _handleSystemTime(const mavlink_message_t& message)
        {
            const uint64_t time_unix_usec = mavlink_msg_system_time_get_time_unix_usec(&message);
            {
                struct tm Tm;
                time_t _Time = (time_unix_usec / 1000000);

                localtime_s(&Tm, &_Time);
                _bag.param._year  = Tm.tm_year % 100;
                _bag.param._month = Tm.tm_mon + 1;
                _bag.param._day   = Tm.tm_mday;
                _bag.param._hour  = Tm.tm_hour;
                _bag.param._min   = Tm.tm_min;
                _bag.param._sec   = Tm.tm_sec;
            }
        }

        void _handleRadioStatus(const mavlink_message_t& message)
        {
        }

        void _handleRCChannels(const mavlink_message_t& message)
        {
            mavlink_rc_channels_t channels;

            mavlink_msg_rc_channels_decode(&message, &channels);

            uint16_t* _rgChannelvalues[cMaxRcChannels] = {
                &channels.chan1_raw,
                &channels.chan2_raw,
                &channels.chan3_raw,
                &channels.chan4_raw,
                &channels.chan5_raw,
                &channels.chan6_raw,
                &channels.chan7_raw,
                &channels.chan8_raw,
                &channels.chan9_raw,
                &channels.chan10_raw,
                &channels.chan11_raw,
                &channels.chan12_raw,
                &channels.chan13_raw,
                &channels.chan14_raw,
                &channels.chan15_raw,
                &channels.chan16_raw,
                &channels.chan17_raw,
                &channels.chan18_raw,
            };
            int pwmValues[cMaxRcChannels];

            for (int i = 0; i < cMaxRcChannels; i++) {
                uint16_t channelValue = *_rgChannelvalues[i];

                if (i < channels.chancount) {
                    pwmValues[i] = ((channelValue == UINT16_MAX) ? -1 : channelValue);
                } else {
                    pwmValues[i] = -1;
                }
            }

            _remoteControlRSSIChanged(channels.rssi);

            int inputChannels = 0;
            _bag.param.inputChannel01 = pwmValues[  inputChannels];
            _bag.param.inputChannel02 = pwmValues[++inputChannels];
            _bag.param.inputChannel03 = pwmValues[++inputChannels];
            _bag.param.inputChannel04 = pwmValues[++inputChannels];
            _bag.param.inputChannel05 = pwmValues[++inputChannels];
            _bag.param.inputChannel06 = pwmValues[++inputChannels];
            _bag.param.inputChannel07 = pwmValues[  inputChannels];

            _updateBag = true;
        }

        void _handleRCChannelsRaw(const mavlink_message_t& message)
        {

            mavlink_rc_channels_raw_t channels;

            mavlink_msg_rc_channels_raw_decode(&message, &channels);

            uint16_t* _rgChannelvalues[cMaxRcChannels] = {
                &channels.chan1_raw,
                &channels.chan2_raw,
                &channels.chan3_raw,
                &channels.chan4_raw,
                &channels.chan5_raw,
                &channels.chan6_raw,
                &channels.chan7_raw,
                &channels.chan8_raw,
            };

            int pwmValues[cMaxRcChannels];
            int channelCount = 0;

            for (int i = 0; i < cMaxRcChannels; i++) {
                pwmValues[i] = -1;
            }

            for (int i = 0; i < 8; i++) {
                uint16_t channelValue = *_rgChannelvalues[i];

                if (channelValue == UINT16_MAX) {
                    pwmValues[i] = -1;
                }
                else {
                    channelCount = i + 1;
                    pwmValues[i] = channelValue;
                }
            }
            for (int i = 9; i < 18; i++) {
                pwmValues[i] = -1;
            }

            _remoteControlRSSIChanged(channels.rssi);

            int inputChannels = 0;
            _bag.param.inputChannel01 = pwmValues[  inputChannels];
            _bag.param.inputChannel02 = pwmValues[++inputChannels];
            _bag.param.inputChannel03 = pwmValues[++inputChannels];
            _bag.param.inputChannel04 = pwmValues[++inputChannels];
            _bag.param.inputChannel05 = pwmValues[++inputChannels];
            _bag.param.inputChannel06 = pwmValues[++inputChannels];
            _bag.param.inputChannel07 = pwmValues[  inputChannels];

            _updateBag = true;
        }

        void _handleBatteryStatus(const mavlink_message_t& message)
        {
            mavlink_battery_status_t batteryStatus;
            mavlink_msg_battery_status_decode(&message, &batteryStatus);

        }

        void _handleSysStatus(const mavlink_message_t& message)
        {
            mavlink_sys_status_t sysStatus;
            mavlink_msg_sys_status_decode(&message, &sysStatus);

            bool insHealth = false;
            if ((sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR_3D_GYRO | sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR_3D_GYRO2)
                    && (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR_3D_ACCEL | sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR_3D_ACCEL2)
                    && (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR_3D_MAG | sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR_3D_MAG2)) {
                _bag.param._status = _bag.param._status | NebulaUAVState_INS;
                insHealth = true;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_INS));
                insHealth = false;
            }

            bool gpsHealth = false;
            if (   (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_GPS)

                   && (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_GPS)) {
                _bag.param._status = _bag.param._status | NebulaUAVState_GPS;
                gpsHealth = true;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_GPS));
                gpsHealth = false;
            }

            bool rcHealth = false;
            if (   (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_RC_RECEIVER)
                   && (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_RC_RECEIVER)
                   && (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_RC_RECEIVER)) {
                _bag.param._status = _bag.param._status | NebulaUAVState_RC;
                rcHealth = true;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_RC));
                rcHealth = false;
            }

            bool motorHealth = false;
            if (   (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS)
                   && (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS)
                   && (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS)) {
                _bag.param._status = _bag.param._status | NebulaUAVState_MC;
                motorHealth = true;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_MC));
                motorHealth = false;
            }

            bool batteryHealth = false;
            if (   (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_BATTERY)
                   && (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_BATTERY)
                   && (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_BATTERY)) {
                _bag.param._status = _bag.param._status | NebulaUAVState_Power;
                batteryHealth = true;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_Power));
                batteryHealth = false;
            }

            bool health = false;
            if (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL) {
                if (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL) {
                    health = (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL) != 0;
                }
            }
            else {
                health = true;
            }

            if (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION) {
                if (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION) {
                    health = (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION) != 0;
                }
            }
            else {
                health = true;
            }

            if (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_YAW_POSITION) {
                if (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_YAW_POSITION) {
                    health = (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_YAW_POSITION) != 0;
                }
            }
            else {
                health = true;
            }

            if (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL) {
                if (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL) {
                    health = (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL) != 0;
                }
            }
            else {
                health = true;
            }

            if (sysStatus.onboard_control_sensors_present & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL) {
                if (sysStatus.onboard_control_sensors_enabled & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL) {
                    health = (sysStatus.onboard_control_sensors_health  & MAV_SYS_STATUS_SENSOR::MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL) != 0;
                }
            }
            else {
                health = true;
            }

            struct sensorInfo_s {
                uint32_t    bit;
            };
            static const sensorInfo_s rgSensorInfo[] = {
                { MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE      },
                { MAV_SYS_STATUS_SENSOR_DIFFERENTIAL_PRESSURE  },
                { MAV_SYS_STATUS_SENSOR_OPTICAL_FLOW           },
                { MAV_SYS_STATUS_SENSOR_VISION_POSITION        },
                { MAV_SYS_STATUS_SENSOR_LASER_POSITION         },
                { MAV_SYS_STATUS_SENSOR_EXTERNAL_GROUND_TRUTH  },
                { MAV_SYS_STATUS_GEOFENCE                      },
                { MAV_SYS_STATUS_AHRS                          },
                { MAV_SYS_STATUS_TERRAIN                       },
                { MAV_SYS_STATUS_REVERSE_MOTOR                 },
                { MAV_SYS_STATUS_LOGGING                       },
            };

            if (health) {
                for (size_t i = 0; i < sizeof(rgSensorInfo) / sizeof(sensorInfo_s); i++) {
                    const sensorInfo_s* pSensorInfo = &rgSensorInfo[i];
                    if (sysStatus.onboard_control_sensors_present & pSensorInfo->bit) {
                        continue;
                    }

                    if (!sysStatus.onboard_control_sensors_enabled & pSensorInfo->bit) {
                        health = false;
                        break;
                    }

                    if (!sysStatus.onboard_control_sensors_health  & pSensorInfo->bit) {
                        health = false;
                        break;
                    }
                }
            }

            if (health && rcHealth && motorHealth && batteryHealth && insHealth && gpsHealth) {
                _bag.param._status = _bag.param._status | NebulaUAVState_Sensor;
            }
            else {
                _bag.param._status = _bag.param._status & (~(NebulaUAVState_Sensor));
            }

            if (sysStatus.current_battery == -1) {
                _bag.param._current = -1.0;
            }
            else {

                _bag.param._current = (float)sysStatus.current_battery / 100.0f;
            }

            if (sysStatus.voltage_battery == UINT16_MAX) {
                _bag.param._voltage = -1.0;
            }
            else {
                _bag.param._voltage     = (double)sysStatus.voltage_battery / 1000.0f;

                _bag.param.powervoltage = (double)sysStatus.voltage_battery / 1000.0f;

                _bag.param._powerconsumption = (char)(((float)(sysStatus.current_battery * sysStatus.voltage_battery) / (100000.0)));
            }

            _bag.param._powerconsumption = sysStatus.battery_remaining;

            _updateBag = true;

            if (apmFirmware() && _apmArmingNotRequired()) {
                _updateArmed( (_onboardControlSensorsEnabled & MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS) != 0 );
            }
        }

        void _handleMavlinkRawImu(const mavlink_message_t& message)
        {
        }

        void _handleMavlinkScaledImu1(const mavlink_message_t& message)
        {
        }

        void _handleMavlinkScaledImu2(const mavlink_message_t& message)
        {
        }

        void _handleMavlinkScaledImu3(const mavlink_message_t& message)
        {
        }

        void _handleWindCov(const mavlink_message_t& message)
        {
        }

        void _handleVibration(const mavlink_message_t& message)
        {
        }

        void _handleCommandLong(const mavlink_message_t& message)
        {
        }

        void _handleHilActuatorControls(const mavlink_message_t& message)
        {
        }

        void _handleMavlinkLoggingData(const mavlink_message_t& message)
        {
        }

        void _handleMavlinkLoggingDataAcked(const mavlink_message_t& message)
        {
        }

        void _handleGpsRawInt(const mavlink_message_t& message)
        {
            mavlink_gps_raw_int_t gpsRawInt;
            mavlink_msg_gps_raw_int_decode(&message, &gpsRawInt);

            _gpsRawIntMessageAvailable = true;

            if (gpsRawInt.fix_type >= GPS_FIX_TYPE_3D_FIX) {
                if (!_globalPositionIntMessageAvailable) {
                    QGeoCoordinate newPosition(gpsRawInt.lat / (double)1E7, gpsRawInt.lon / (double)1E7, gpsRawInt.alt / 1000.0);
                    if (newPosition != _coordinate) {
                        _coordinate                = newPosition;

                        _bag.param.position.lat    = newPosition.latitude();
                        _bag.param.position.lng    = newPosition.longitude();
                        _bag.param.position.heigth = (short)newPosition.altitude();
                        _bag.param.gpsHeigth       = newPosition.altitude() / 10.0f;
                        _coordinateChanged         = true;

                        _addNewMapTrajectoryPoint(_bag);
                    }
                    _altitudeAMSLFact = (gpsRawInt.alt / 1000.0);
                }
            }

            _bag.param.gpsNumber         = (gpsRawInt.satellites_visible == 255 ? 0 : gpsRawInt.satellites_visible);
            _bag.param.levelPrecision    = (gpsRawInt.eph == UINT16_MAX ? std::numeric_limits<double>::quiet_NaN() : gpsRawInt.eph / 100.0);
            _bag.param.verticalPrecision = (gpsRawInt.epv == UINT16_MAX ? std::numeric_limits<double>::quiet_NaN() : gpsRawInt.epv / 100.0);
            _bag.param.groundVelocity    = (gpsRawInt.vel == UINT16_MAX ? std::numeric_limits<double>::quiet_NaN() : gpsRawInt.vel / 100.0);
            _bag.param.velocityPrecision = (gpsRawInt.vel_acc == UINT16_MAX ? std::numeric_limits<double>::quiet_NaN() : gpsRawInt.vel_acc / 100.0);
            _bag.param._rudder           = (gpsRawInt.cog == UINT16_MAX ? std::numeric_limits<double>::quiet_NaN() : gpsRawInt.cog / 100.0);

            _bag.param._status           = (gpsRawInt.satellites_visible == 255 ? _bag.param._status | NebulaUAVState_GPS : _bag.param._status | NebulaUAVState_GPS);
            _bag.param._status           = (gpsRawInt.cog == UINT16_MAX ? _bag.param._status & !NebulaUAVState_SatelliteDirection : _bag.param._status | NebulaUAVState_SatelliteDirection);

            if (_fix_type != gpsRawInt.fix_type) {
                _fix_type = gpsRawInt.fix_type;
                if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_NO_GPS || gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_NO_FIX) {
                    _bag.param.differenceStatus = 0;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_2D_FIX || gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_3D_FIX) {
                    _bag.param.differenceStatus = 1;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_DGPS) {
                    _bag.param.differenceStatus = 3;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_RTK_FLOAT) {
                    _bag.param.differenceStatus = 5;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_RTK_FIXED) {
                    _bag.param.differenceStatus = 4;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_STATIC) {
                    _bag.param.differenceStatus = 7;
                }
                else if (gpsRawInt.fix_type == GPS_FIX_TYPE::GPS_FIX_TYPE_PPP) {
                    _bag.param.differenceStatus = 8;
                }
            }

            _updateBag = true;
        }

        void _handleGlobalPositionInt(const mavlink_message_t& message)
        {
            mavlink_global_position_int_t globalPositionInt;
            mavlink_msg_global_position_int_decode(&message, &globalPositionInt);

            if (globalPositionInt.lat == 0 && globalPositionInt.lon == 0) {
                return;
            }

            _globalPositionIntMessageAvailable = true;
            QGeoCoordinate newPosition(globalPositionInt.lat / (double)1E7, globalPositionInt.lon / (double)1E7, globalPositionInt.alt / 1000.0);
            if (newPosition != _coordinate) {
                _coordinate = newPosition;
                _bag.param.position.lat    = newPosition.latitude();
                _bag.param.position.lng    = newPosition.longitude();
                _bag.param.position.heigth = (short)newPosition.altitude();
                _bag.param.gpsHeigth       = newPosition.altitude() / 10.0f;

                _altitudeRelativeFact = newPosition.altitude() - _homePosition.altitude();
                _altitudeAMSLFact     = (globalPositionInt.alt / 1000.0);
                _bag.param.relativeOriginHeigth = globalPositionInt.relative_alt / 1000.0;
                _bag.param.relativeGroundHeigth = globalPositionInt.relative_alt / 1000.0;
                _coordinateChanged = true;

                _updateBag = true;

                _addNewMapTrajectoryPoint(_bag);
            }
        }

        void _handleAltitude(const mavlink_message_t& message)
        {
            mavlink_altitude_t altitude;
            mavlink_msg_altitude_decode(&message, &altitude);

            {
                struct tm Tm;
                time_t _Time = (altitude.time_usec / 1000000);

                localtime_s(&Tm, &_Time);
                _bag.param._year  = Tm.tm_year;
                _bag.param._month = Tm.tm_mon + 1;
                _bag.param._day   = Tm.tm_mday;
                _bag.param._hour  = Tm.tm_hour;
                _bag.param._min   = Tm.tm_min;
                _bag.param._sec   = Tm.tm_sec;
            }

            if (!_globalPositionIntMessageAvailable) {
                _altitudeRelativeFact = (altitude.altitude_relative);
                _altitudeLocalFact    = (altitude.altitude_local);
                if (!_gpsRawIntMessageAvailable) {
                    _altitudeAMSLFact = (altitude.altitude_amsl);
                }
                _bag.param.relativeOriginHeigth = altitude.altitude_relative;
                _bag.param.relativeGroundHeigth = altitude.altitude_terrain;
            }

            _updateBag = true;
        }

        void _handleVfrHud(const mavlink_message_t& message)
        {
            mavlink_vfr_hud_t vfrHud;
            mavlink_msg_vfr_hud_decode(&message, &vfrHud);
            _bag.param.groundVelocity = vfrHud.groundspeed;
            _bag.param._rudder        = vfrHud.heading;
            _bag.param._throttle      = vfrHud.throttle;
            _bag.param._elevator      = vfrHud.climb;

            _updateBag = true;

        }

        void _handleScaledPressure(const mavlink_message_t& message)
        {
        }

        void _handleScaledPressure2(const mavlink_message_t& message)
        {
        }

        void _handleScaledPressure3(const mavlink_message_t& message)
        {
        }

        void _handleHighLatency(const mavlink_message_t& message)
        {
            mavlink_high_latency_t high_latency;
            mavlink_msg_high_latency_decode(&message, &high_latency);

            std::string previousFlightMode;
            if (_base_mode != 0 || _custom_mode != 0) {

                previousFlightMode = flightMode();
            }

            if (apmFirmware()) {
                _custom_mode = high_latency.custom_mode;
            }
            else {
                union px4_custom_mode {
                    uint32_t data;
                    struct {
                        uint16_t reserved_hl;
                        uint16_t custom_mode_hl;
                    };
                };
                union px4_custom_mode px4_cm;
                px4_cm.data = 0;
                px4_cm.custom_mode_hl = high_latency.custom_mode;
                _custom_mode = px4_cm.data;
            }

            _base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

            if (_armed != true) {
                _armed = true;
                _updateArmed(_armed);
            }

            _altitudeAMSLFact = (high_latency.altitude_amsl);

            _bag.param.groundVelocity = (double)high_latency.groundspeed;
            _bag.param._elevator      = (double)high_latency.climb_rate;
            _bag.param._rudder        = (double)high_latency.heading * 2.0;

            _bag.param._throttle         = high_latency.throttle;
            _bag.param._powerconsumption = high_latency.battery_remaining;
            _bag.param.fkTemperature     = high_latency.temperature;
            _bag.param._temperature      = high_latency.temperature_air;

            QGeoCoordinate newPosition(high_latency.latitude / (double)1E7, high_latency.longitude / (double)1E7, high_latency.altitude_amsl);
            if (newPosition != _coordinate) {
                _coordinate = newPosition;

                _bag.param.position.lat    = newPosition.latitude();
                _bag.param.position.lng    = newPosition.longitude();
                _bag.param.position.heigth = newPosition.altitude();
                _bag.param.gpsHeigth       = newPosition.altitude() / 10.0f;
                _coordinateChanged         = true;

                _addNewMapTrajectoryPoint(_bag);
            }

            _updateBag = true;
        }

        void _handleHighLatency2(const mavlink_message_t& message)
        {
            mavlink_high_latency2_t highLatency2;
            mavlink_msg_high_latency2_decode(&message, &highLatency2);

            std::string previousFlightMode;
            if (_base_mode != 0 || _custom_mode != 0) {

                previousFlightMode = flightMode();
            }

            if (apmFirmware()) {
                _custom_mode = highLatency2.custom_mode;
            }
            else {
                union px4_custom_mode {
                    uint32_t data;
                    struct {
                        uint16_t reserved_hl;
                        uint16_t custom_mode_hl;
                    };
                };
                union px4_custom_mode px4_cm;
                px4_cm.data = 0;
                px4_cm.custom_mode_hl = highLatency2.custom_mode;
                _custom_mode = px4_cm.data;
            }

            _base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

            if (_armed != true) {
                _armed = true;
                _updateArmed(_armed);
            }

            _altitudeAMSLFact     = (highLatency2.altitude);

            _bag.param._throttle      = highLatency2.throttle ;
            _bag.param.groundVelocity = (double)highLatency2.groundspeed / 5.0;
            _bag.param._elevator      = (double)highLatency2.climb_rate / 10.0;
            _bag.param._rudder        = (double)highLatency2.heading * 2.0;

            _bag.param._powerconsumption = highLatency2.battery;
            _bag.param._temperature      = highLatency2.temperature_air;

            QGeoCoordinate newPosition(highLatency2.latitude / (double)1E7, highLatency2.longitude / (double)1E7, highLatency2.altitude);
            if (newPosition != _coordinate) {
                _coordinate = newPosition;

                _bag.param.position.lat = newPosition.latitude();
                _bag.param.position.lng = newPosition.longitude();
                _bag.param.position.heigth = (short)newPosition.altitude();
                _bag.param.gpsHeigth = newPosition.altitude() / 10.0f;
                _coordinateChanged = true;

                _addNewMapTrajectoryPoint(_bag);
            }

            _updateBag = true;
        }

        void _handleAttitudeWorker(double rollRadians, double pitchRadians, double yawRadians)
        {
        }

        void _handleAttitude(const mavlink_message_t& message)
        {
            mavlink_attitude_t attitude = {};
            mavlink_msg_attitude_decode(&message, &attitude);

            double roll  = limitAngleToPMPIf(attitude.roll);
            double pitch = limitAngleToPMPIf(attitude.pitch);
            double yaw   = limitAngleToPMPIf(attitude.yaw);

            roll  = RadiansToDegrees(roll);
            pitch = RadiansToDegrees(pitch);
            yaw   = RadiansToDegrees(yaw);

            if (yaw < 0.0) {
                yaw += 360.0;
            }

            _bag.param.angleofRoll   = roll;
            _bag.param.angleofPitch  = pitch;
            _bag.param.angleofRudder = yaw;
            _bag.param.angleofNose   = yaw;

            _updateBag = true;

        }

        void _handleAttitudeQuaternion(const mavlink_message_t& message)
        {
        }

        void _handleDistanceSensor(const mavlink_message_t& message)
        {
        }

        void _handleEstimatorStatus(const mavlink_message_t& message)
        {
        }

        void _handleOrbitExecutionStatus(const mavlink_message_t& message)
        {
        }

        void _handleSerialControl(const mavlink_message_t& message)
        {
        }

        void _handleCameraFeedback(const mavlink_message_t& message)
        {
        }

        void _handleWind(const mavlink_message_t& message)
        {
        }

        void _handleCameraImageCaptured(const mavlink_message_t& message)
        {
        }

        void _handleADSBVehicle(const mavlink_message_t& message)
        {
            mavlink_adsb_vehicle_t adsbVehicle;
            static const int maxTimeSinceLastSeen = 15;

            mavlink_msg_adsb_vehicle_decode(&message, &adsbVehicle);
            if (adsbVehicle.flags | ADSB_FLAGS::ADSB_FLAGS_VALID_COORDS) {
            }
        }

        void _handleHilControls(const mavlink_message_t& message)
        {
        }

        void _handleServoOutputRaw(const mavlink_message_t& message)
        {
            mavlink_servo_output_raw_t servo;
            mavlink_msg_servo_output_raw_decode(&message, &servo);

            uint16_t* _servoChannelvalues[cMaxRcChannels] = {
                &servo.servo1_raw,
                &servo.servo2_raw,
                &servo.servo3_raw,
                &servo.servo4_raw,
                &servo.servo5_raw,
                &servo.servo6_raw,
                &servo.servo7_raw,
                &servo.servo8_raw,
                &servo.servo9_raw,
                &servo.servo10_raw,
            };

            int pwmValues[cMaxRcChannels];
            int channelCount = 0;

            for (int i = 0; i < cMaxRcChannels; i++) {
                pwmValues[i] = -1;
            }

            for (int i = 0; i < 8; i++) {
                uint16_t channelValue = *_servoChannelvalues[i];

                if (channelValue == UINT16_MAX) {
                    pwmValues[i] = -1;
                }
                else {
                    channelCount = i + 1;
                    pwmValues[i] = channelValue;
                }
            }
            for (int i = 9; i < 18; i++) {
                pwmValues[i] = -1;
            }

            int inputChannels = 0;
            _bag.param._fkOutputChannal01 = pwmValues[  inputChannels];
            _bag.param._fkOutputChannal02 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal03 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal04 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal05 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal06 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal07 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal08 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal09 = pwmValues[++inputChannels];
            _bag.param._fkOutputChannal10 = pwmValues[  inputChannels];

            _updateBag = true;
        }

        void _handleParamValueMsg(const mavlink_message_t& message) {
            mavlink_param_value_t rawValue;
            mavlink_msg_param_value_decode(&message, &rawValue);

        }

        void _handleHighResIMU(const mavlink_message_t& message) {
            mavlink_highres_imu_t highres_imu;
            mavlink_msg_highres_imu_decode(&message, &highres_imu);
            _bag.param.fkTemperature = highres_imu.temperature;

            _updateBag = true;
        }

        void _handleHilSensor(const mavlink_message_t& message) {
            mavlink_hil_sensor_t hil_sensor;
            mavlink_msg_hil_sensor_decode(&message, &hil_sensor);
            _bag.param.fkTemperature = hil_sensor.temperature;

            _updateBag = true;
        }

        void _handlePowerStatus(const mavlink_message_t& message) {
            mavlink_power_status_t power_status;
            mavlink_msg_power_status_decode(&message, &power_status);

            _updateBag = true;
        }

        void _handleHWrStatus(const mavlink_message_t& message) {
            mavlink_hwstatus_t hwstatus;
            mavlink_msg_hwstatus_decode(&message, &hwstatus);
            _bag.param.fkvoltage    = (double)hwstatus.Vcc   / 1000.0f;

            _updateBag = true;
        }

        float limitAngleToPMPIf(double angle)
        {
            if (angle > -20.0f * M_PI && angle < 20.0f * M_PI)
            {
                while (angle > ((float)M_PI + FLT_EPSILON))
                {
                    angle -= 2.0f * (float)M_PI;
                }

                while (angle <= -((float)M_PI + FLT_EPSILON))
                {
                    angle += 2.0f * (float)M_PI;
                }
            }
            else
            {

                angle = fmodf((float)angle, (float)M_PI);
            }

            return (float)angle;
        }

        inline float DegreesToRadians(float degrees)
        {
            return degrees * float(M_PI / 180.0f);
        }

        inline double DegreesToRadians(double degrees)
        {
            return degrees * (M_PI / 180.0f);
        }

        inline float RadiansToDegrees(float radians)
        {
            return radians * float(180.0f / M_PI);
        }

        inline double RadiansToDegrees(double radians)
        {
            return radians * (180.0f / M_PI);
        }

        void setFirmwareVersion(int majorVersion, int minorVersion, int patchVersion, FIRMWARE_VERSION_TYPE versionType)
        {
            _firmwareMajorVersion = majorVersion;
            _firmwareMinorVersion = minorVersion;
            _firmwarePatchVersion = patchVersion;
            _firmwareVersionType = versionType;

        }

        void setFirmwareCustomVersion(int majorVersion, int minorVersion, int patchVersion)
        {
            _firmwareCustomMajorVersion = majorVersion;
            _firmwareCustomMinorVersion = minorVersion;
            _firmwareCustomPatchVersion = patchVersion;

        }

        uint64_t _getUnixTimeFromMs(uint64_t time)
        {
            uint64_t ret = 0;

            return ret;
        }

        void _setMaxProtoVersion(unsigned version)
        {

        }

        void _requestProtocolVersion(unsigned version)
        {

        }

        unsigned maxProtoVersion() const
        {
            return _maxProtoVersion;
        }

        void _setCapabilities(uint64_t capabilityBits)
        {
            _capabilityBits = capabilityBits;
            _vehicleCapabilitiesKnown = true;

            if (capabilityBits & MAV_PROTOCOL_CAPABILITY_MAVLINK2 && maxProtoVersion() < 200) {

            }

        }

        void _setFlying(bool flying)
        {
            if (_flying != flying) {
                _flying = flying;

            }
        }

        void _setLanding(bool landing)
        {
            if (_armed && _landing != landing) {
                _landing = landing;

            }
        }

        std::string flightMode(void)
        {
            std::string curFlightMode;

            return curFlightMode;
        }

        void _mapTrajectoryStart(void)
        {
            _mapTrajectoryHaveFirstCoordinate = false;
            _mapTrajectoryLastCoordinate = QGeoCoordinate();
            _mapTrajectoryTimer = true;
            _mapTrajectoryTime  = ::time(NULL);
            _flightTimer        = true;
            _flightDistanceFact = 0.0f;
            _flightTimeFact     = 0.0f;
        }

        void _mapTrajectoryStop(void)
        {
            _mapTrajectoryTimer = false;
        }

        void _updateArmed(bool armed)
        {
            if (_armed != armed) {
                _armed = armed;

                if (_armed) {
                    _mapTrajectoryStart();
                }
                else {
                    _mapTrajectoryStop();
                }
            }
        }

        void _addNewMapTrajectoryPoint(const xygsJL_20& bag)
        {
            if (!_mapTrajectoryTimer) {
                return;
            }

            if (::time(nullptr) - _mapTrajectoryTime < _mapTrajectoryMsecsBetweenPoints / 1000) {
                return;
            }
            _flightTimeFact    += (::time(nullptr) - _mapTrajectoryTime);
            _mapTrajectoryTime  = ::time(nullptr);

            if (_mapTrajectoryHaveFirstCoordinate) {

                _flightDistanceFact  = (_flightDistanceFact + _mapTrajectoryLastCoordinate.distanceTo(_coordinate));

            }
            _mapTrajectoryHaveFirstCoordinate = true;
            _mapTrajectoryLastCoordinate      = _coordinate;

            _bag.param.flyDistance = _flightDistanceFact;
            _bag.param.flyTime     = _flightTimeFact;

            _updateBag = true;
        }

        void _remoteControlRSSIChanged(uint8_t rssi)
        {

            if (rssi > 100) {
                if (_rcRSSI != 255) {
                    _rcRSSI = 255;

                    _bag.param._status = (_rcRSSI == 255 ? _bag.param._status & (~(NebulaUAVState_RC)) : _bag.param._status | NebulaUAVState_RC);
                }
                return;
            }

            if (_rcRSSIstore == 255.) {
                _rcRSSIstore = (double)rssi;
            }

            _rcRSSIstore = (_rcRSSIstore * 0.9f) + ((float)rssi * 0.1);
            uint8_t filteredRSSI = (uint8_t)ceil(_rcRSSIstore);
            if (_rcRSSIstore < 0.1) {
                filteredRSSI = 0;
            }
            if (_rcRSSI != filteredRSSI) {
                _rcRSSI = filteredRSSI;

                _bag.param._status = (_rcRSSI == 255 ? _bag.param._status & (~(NebulaUAVState_RC)) : _bag.param._status | NebulaUAVState_RC);
            }

            _updateBag = true;
        }
    };

    mavlink_message_t _message;
    mavlink_status_t  _status;
    std::map<std::string, Mavlink_Control_Msg> s_MavlinkControlMap;

    template<class classname>
    friend class IProtocolParseXygsServerImpl_Control_Handle;
};

}

}

#endif
