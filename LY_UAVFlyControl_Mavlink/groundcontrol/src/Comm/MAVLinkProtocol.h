#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H


#include <QObject>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include <QLoggingCategory>

#include "LinkInterface.h"
#include "QGCMAVLink.h"
#include "QGC.h"
#include "QGCTemporaryFile.h"

class LinkManager;
// lysd // class MultiVehicleManager;
class QGCApplication;

Q_DECLARE_LOGGING_CATEGORY(MAVLinkProtocolLog)

/**
 * @brief MAVLink micro air vehicle protocol reference implementation.
 *
 * MAVLink is a generic communication protocol for micro air vehicles.
 * for more information, please see the official website: https://mavlink.io
 **/
class MAVLinkProtocol : public QObject
{
    Q_OBJECT

public:
    MAVLinkProtocol(QGCApplication* app);
    ~MAVLinkProtocol();

    /** @brief Get the human-friendly name of this protocol */
    QString getName();
    /** @brief Get the system id of this application */
    int getSystemId() const;
    /** @brief Get the component id of this application */
    int getComponentId() const;

    /** @brief Get protocol version check state */
    bool versionCheckEnabled() const {
        return m_enable_version_check;
    }
    /** @brief Get the protocol version */
    int getVersion() {
        return MAVLINK_VERSION;
    }
    /** @brief Get the currently configured protocol version */
    unsigned getCurrentVersion() const{
        return _current_version;
    }
    /**
     * Reset the counters for all metadata for this link.
     */
    virtual void resetMetadataForLink(LinkInterface *link);

    /// Suspend/Restart logging during replay.
    void suspendLogForReplay(bool suspend);

    /// Set protocol version
    void setVersion(unsigned version);

    // Override from QGCTool
    virtual void setToolbox();

public slots:
    /** @brief Receive bytes from a communication interface */
    void receiveBytes(LinkInterface* link, unsigned long long int weaponId, QByteArray b);

    /** @brief Log bytes sent from a communication interface */
    void logSentBytes(LinkInterface* link, QByteArray b);

    /** @brief Set the system id of this application */
    void setSystemId(int id);

    /** @brief Enable / disable version check */
    void enableVersionCheck(bool enabled);

    /** @brief Load protocol settings */
    void loadSettings();
    /** @brief Store protocol settings */
    void storeSettings();

    /// @brief Deletes any log files which are in the temp directory
    static void deleteTempLogFiles(void);

    /// Checks for lost log files
    void checkForLostLogFiles(void);

protected:
    bool        m_enable_version_check;                         ///< Enable checking of version match of MAV and QGC
    bool        m_enable_init_version_1;                        ///< Enable init of version 1
    uint8_t     lastIndex[256][256];                            ///< Store the last received sequence ID for each system/componenet pair
    uint8_t     firstMessage[256][256];                         ///< First message flag
    uint64_t    totalReceiveCounter[MAVLINK_COMM_NUM_BUFFERS + 1];  ///< The total number of successfully received messages
    uint64_t    totalLossCounter[MAVLINK_COMM_NUM_BUFFERS + 1];     ///< Total messages lost during transmission.
    float       runningLossPercent[MAVLINK_COMM_NUM_BUFFERS + 1];   ///< Loss rate

    mavlink_message_t _message;
    mavlink_status_t _status;

    bool        versionMismatchIgnore;
    int         systemId;
    unsigned    _current_version;
    int         _radio_version_mismatch_count;

signals:
    /// Heartbeat received on link
    void vehicleHeartbeatInfo(LinkInterface* link, unsigned long long int  weaponId, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);

    /** @brief Message received and directly copied via signal */
    void messageReceived(LinkInterface* link, unsigned long long int  weaponId, mavlink_message_t message);
    /** @brief Emitted if version check is enabled / disabled */
    void versionCheckChanged(bool enabled);
    /** @brief Emitted if a message from the protocol should reach the user */
    void protocolStatusMessage(const QString& title, const QString& message);
    /** @brief Emitted if a new system ID was set */
    void systemIdChanged(int systemId);

    void mavlinkMessageStatus(unsigned long long int  weaponId, int uasId, uint64_t totalSent, uint64_t totalReceived, uint64_t totalLoss, float lossPercent);

    /**
     * @brief Emitted if a new radio status packet received
     *
     * @param rxerrors receive errors
     * @param fixed count of error corrected packets
     * @param rssi local signal strength in dBm
     * @param remrssi remote signal strength in dBm
     * @param txbuf how full the tx buffer is as a percentage
     * @param noise background noise level
     * @param remnoise remote background noise level
     */
    void radioStatusChanged(LinkInterface* link, unsigned rxerrors, unsigned fixed, int rssi, int remrssi,
    unsigned txbuf, unsigned noise, unsigned remnoise);

    /// Emitted when a temporary telemetry log file is ready for saving
    void saveTelemetryLog(QString tempLogfile);

    /// Emitted when a telemetry log is started to save.
    void checkTelemetrySavePath(void);

private slots:
    void _vehicleCountChanged(void);

private:
    bool _closeLogFile(void);
    void _startLogging(void);
    void _stopLogging(void);

    bool _logSuspendError;      ///< true: Logging suspended due to error
    bool _logSuspendReplay;     ///< true: Logging suspended due to replay
    bool _vehicleWasArmed;      ///< true: Vehicle was armed during log sequence

    // lysd // QGCTemporaryFile    _tempLogFile;            ///< File to log to
    static const char*  _tempLogFileTemplate;    ///< Template for temporary log file
    static const char*  _logFileExtension;       ///< Extension for log files

    // lysd // LinkManager*            _linkMgr;
    // lysd // MultiVehicleManager*    _multiVehicleManager;
};

#endif // MAVLINKPROTOCOL_H
