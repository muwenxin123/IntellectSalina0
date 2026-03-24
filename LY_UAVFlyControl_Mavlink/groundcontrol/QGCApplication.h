#ifndef QGCAPPLICATION_H
#define QGCAPPLICATION_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QScopedPointer>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGeoCoordinate>
#include <QGeoPositionInfo>
#include <QRegularExpression>
#include <QTranslator>
#include <QSettings>


#include "QGCMAVLink.h"
#include "QGCPalette.h"


class FirmwarePluginManager;
class MissionCommandTree;
class SettingsManager;
class UnitsSettings;
class APMMavlinkStreamRateSettings;
class MAVLinkProtocol;
class LocalLink;
class Vehicle;
class MultiVehicleManager;
class UASMessageHandler;
class AudioOutput;
class QGCPalette;
class AppSettings;



class UAVFlyManage_MavlinkApplicationSetting;

class QGCApplication : public QObject
{
    Q_OBJECT
public:
    explicit QGCApplication(QObject *parent = nullptr);
    virtual ~QGCApplication();

    void createVehicleFromJsonFile();
    void removeVehicleFromJsonFile();

    static QString                     applicationName         () { return "lysd"; }
    static FirmwarePluginManager*      firmwarePluginManager   () { return _firmwarePluginManager; }
    static MissionCommandTree*         missionCommandTree      () { return _missionCommandTree; }
    static AudioOutput*                audioOutput             () { return _audioOutput; }
    static QGCPalette*                 qgcPalette              () { return _qgcPalette; }
    static AppSettings*                appSettings             () { return _appSettings; }

    static QMap<QString, QString>      missionSupportCommandMavCmd();
    static QMap<QString, QString>      missionAddSupportCommandMavCmd();
    static QMap<QString, QString>      missionUnSupportCommandMavCmd();

    static QMap<QString, QString>      missionSupportCommandMavFrame();
    static QMap<QString, QString>      missionUnSupportCommandMavFrame();

    static const QString               vehicleName(const unsigned long long weaponId, const unsigned long long vehicleId);
    static void                        updateVehicleName(const unsigned long long weaponId, const QString& name);

    static void                        updateMissionLineColor(unsigned long long weaponId, int vehicleId, unsigned int& lineColor);


    static bool                        qgcApplicationMavlinkVehicleControlSendCommandMessage();
    static bool                        qgcApplicationMainDialogShow();
    static bool                        qgcApplicationMavlinkSelectDialogShow();
    static bool                        qgcApplicationMavlinkControlDialogShow();
    static bool                        qgcApplicationMavlinkTaskManageDialogShow();
    static bool                        qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow();
    static bool                        qgcApplicationMavlinkVehicleControlsDialogShow();
    static bool                        qgcApplicationMavlinkVehicleStatusInfoDialogShow();
    static bool                        qgcApplicationMavlinkParameterManageEditDialogShow();
    static bool                        qgcApplicationSettingDialogShow();
    static bool                        qgcApplicationVehicleManageDialogShow();
    static bool                        qgcApplicationStatusInfoDialogShow();
    static bool                        qgcApplicationParameterManageDialogShow();
    static bool                        qgcApplicationMessageIndicatorDialogShow();
    static bool                        createVehicleFromJsonFileEnabled();
    static bool                        createVehicleMissionLineFromTaskEnabled();
    static bool                        upDownVehicleAllMissionEnabled();
    static bool                        vehicleControlAdvanceOperEnabled();
    static bool                        initWeaponInfoListOnceEnable();
    static bool                        multiVehicleEnabled();
    static bool                        gcsHeartbeatEnabled();
    static LocalLink                  *localLink();
    static MultiVehicleManager        *multiVehicleManager();
    static UASMessageHandler          *uasMessageHandler();

    static QString                    telemetrySavePath(void);
    static bool                       updateHomePosition(){return false;}
    static QGeoCoordinate             gcsPosition         (void) { return _gcsPosition; }
    static qreal                      gcsHeading          (void) { return _gcsHeading; }
    static qreal                      gcsPositionHorizontalAccuracy(void)  { return _gcsPositionHorizontalAccuracy; }
    static QGeoPositionInfo           geoPositionInfo     (void)  { return _geoPositionInfo; }

    static QString                    rawName(MAV_CMD command);

    static QList<Vehicle*>*           vehicles(void);
    static uint64_t                   msecsSinceBoot(void);
    static void                       say(const QString& text, const QString &title);
    static bool                       runningUnitTests();

    static void                       sleep(unsigned long);
    static void                       msleep(unsigned long);
    static void                       usleep(unsigned long);

    // static QCursor                   *overrideCursor();
    static void                       setOverrideCursor(const QCursor &);
    static void                       changeOverrideCursor(const QCursor &);
    static void                       restoreOverrideCursor();
    static void                       processEvents(QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
    static void                       processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime);

    static void                       showSetupView();

    /// Show modal application message to the user
    static void                       showAppMessage(const QString& message, const QString& title = QString());
    static void                       showAppnewFormattedMessage(const int severity, const QString& message);
    static void                       showRebootAppMessage(const QString& message, const QString& title = QString());
    static void                       showCriticalVehicleMessage(const QString& message);

    /// Used to report a missing Parameter. Warning will be displayed to user. Method may be called
    /// multiple times.
    static void                       reportMissingParameter(int componentId, const QString& name);


    static bool                       useConditionGate();

    static double                     altitudeAMSL();
    static double                     defaultMissionItemAltitude();
    static double                     vtolTransitionDistance();
    static double                     vtolTransitionDistanceMin();
    static double                     batteryPercentRemainingAnnounce();

    static int                        maxApplicationSettingGridRow();

    static int                        maxApplicationSettingGridColumns();

    static int                        selectApplicationSettingPointSize();

    static bool                       overrideSettingsGroupVisibility();

    static bool                       overrideSettingsGroupVisibility(QString name);

    static bool                       apmStartMavlinkStreams();

    static bool                       audioMuted();

    static bool                       initialConnectStateMachineNoRequestParameters();
    static bool                       initialConnectStateMachineNoRequestMission();

    static const QString              missionWaypointMavFrameDefault();

    static UnitsSettings              *unitsSettings();


    APMMavlinkStreamRateSettings     *apmMavlinkStreamRateSettings();

    static SettingsManager            *settingsManager();

    static double                     cruiseSpeed();

    static double                     hoverSpeed();

    static MAVLinkProtocol            *mavlinkProtocol();

    static double                     maxGoToLocationDistance();
    static double                     takeoffMinimumAltitude();
    static double                     guidedMinimumAltitude();
    static double                     guidedMaximumAltitude();

    static bool                       saveCsvTelemetry();


    void                              loadSupportCommandMavCmd();
    void                              storeSupportCommandMavCmd();

    void                              loadSupportCommandMavFrame();
    void                              storeSupportCommandMavFrame();


    QTranslator&                      qgcJSONTranslator(void) { return _qgcTranslatorJSON; }

    /// Allows a plugin to override the specified color name from the palette
    static void paletteOverride(const QString& /* colorName */, QGCPalette::PaletteColorInfo_t& /* colorInfo */){}


public:
    static QGCApplication*                            _app;   ///< Our own singleton. Should be reference directly by qgcApp

public slots:
    void handleFlightModeChanged(unsigned long long int weaponId, int id, int componentId, const QString& flightMode);

private:

    template <typename T>
    struct AppSettingsValue
    {
        const QString    fileName;
        const bool       readOnly;
        const bool       show;
        const QString    groupKey;
        const QString    valueKey;
        mutable QString  valueName;
        // const int     decimals;
        // const int     minimum;
        // const int     maximum;
        const T          defaultValue;
        T&               value;

        static void readAppSettingsValue(QSettings& settings, const QList<AppSettingsValue<T>>& appSettingsValueList){
            for( auto& settingsValue : appSettingsValueList){
                if(!settings.fileName().endsWith(settingsValue.fileName)){
                    continue;
                }
                settings.beginGroup(QString("QGC_MAVLINK_") + settingsValue.groupKey.toUpper());
                if(!settings.contains(settingsValue.valueKey)) {
                    settings.endGroup();
                    continue ;
                }

                const QVariant valueVariant = settings.value(settingsValue.valueKey, settingsValue.defaultValue);
                if(valueVariant.isValid() && valueVariant.canConvert<T>()){
                    settingsValue.value = valueVariant.value<T>();
                }

                const QString valueName = settingsValue.valueKey + "Name";
                if(!settings.contains(valueName)) {
                    settings.endGroup();
                    continue ;
                }

                QVariant nameVariant = settings.value(valueName, settingsValue.valueKey);
                if(nameVariant.isValid() && nameVariant.canConvert<QString>()){
                    settingsValue.valueName = nameVariant.toString();
                }

                settings.endGroup();
            }
        }

        static void writeAppSettingsValue(QSettings& settings, const QList<AppSettingsValue<T>>& appSettingsValueList){
            for( auto& settingsValue : appSettingsValueList){
                if(settingsValue.readOnly || !settings.fileName().endsWith(settingsValue.fileName)){
                    continue;
                }
                settings.beginGroup(QString("QGC_MAVLINK_") + settingsValue.groupKey.toUpper());
                settings.setValue(settingsValue.valueKey, settingsValue.value);
                settings.setValue( (settingsValue.valueKey + "Name"), settingsValue.valueName);
                settings.endGroup();
            }
        }
    };

    const QList<QGCApplication::AppSettingsValue<QString>> &AppSettingsValueList(QString);
    const QList<QGCApplication::AppSettingsValue<double>>  &AppSettingsValueList(double);
    const QList<QGCApplication::AppSettingsValue<int>>     &AppSettingsValueList(int);
    const QList<QGCApplication::AppSettingsValue<bool>>    &AppSettingsValueList(bool);
    void                                                   loadSettings();
    void                                                   storeSettings();

private:

    static QGeoPositionInfo                           _geoPositionInfo;
    static QGeoCoordinate                             _gcsPosition;
    static qreal                                      _gcsHeading;
    static qreal                                      _gcsPositionHorizontalAccuracy;

    static class SettingsManager*                     _settingsManager;
    static class UnitsSettings*                       _unitsSettings;
    static class APMMavlinkStreamRateSettings*        _apmMavlinkStreamRateSettings;
    static class MAVLinkProtocol*                     _mavlinkProtocol;
    static class MultiVehicleManager*                 _multiVehicleManager;
    static class LocalLink*                           _localLink;
    static class UASMessageHandler*                   _UASMessageHandler;


    static FirmwarePluginManager*                     _firmwarePluginManager;
    static MissionCommandTree*                        _missionCommandTree;
    static AudioOutput*                               _audioOutput;
    static QGCPalette*                                _qgcPalette;
    static AppSettings*                               _appSettings;

    static double                                     _maxGoToLocationDistance;
    static double                                     _takeoffMinimumAltitude;
    static double                                     _guidedMinimumAltitude;
    static double                                     _guidedMaximumAltitude;
    static double                                     _cruiseSpeed;
    static double                                     _minCruiseSpeed;
    static double                                     _hoverSpeed;
    static double                                     _minHoverSpeed;
    static double                                     _altitudeAMSL;
    static double                                     _defaultMissionItemAltitude;
    static double                                     _vtolTransitionDistance;
    static double                                     _vtolTransitionDistanceMin;
    static double                                     _batteryPercentRemainingAnnounce;

    static int                                        _maxApplicationSettingGridRows;
    static int                                        _maxApplicationSettingGridColumns;
    static int                                        _selectApplicationSettingPointSize;

    static bool                                       _qgcApplicationMavlinkVehicleControlSendCommandMessage;
    static bool                                       _qgcApplicationMainDialogShow;
    static bool                                       _qgcApplicationMavlinkSelectDialogShow;
    static bool                                       _qgcApplicationMavlinkControlDialogShow;
    static bool                                       _qgcApplicationMavlinkTaskManageDialogShow;
    static bool                                       _qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow;
    static bool                                       _qgcApplicationMavlinkVehicleControlsDialogShow;
    static bool                                       _qgcApplicationMavlinkVehicleStatusInfoDialogShow;
    static bool                                       _qgcApplicationMavlinkParameterManageEditDialogShow;
    static bool                                       _qgcApplicationSettingDialogShow;
    static bool                                       _qgcApplicationVehicleManageDialogShow;
    static bool                                       _qgcApplicationStatusInfoDialogShow;
    static bool                                       _qgcApplicationParameterManageDialogShow;
    static bool                                       _qgcApplicationMessageIndicatorDialogShow;
    static bool                                       _createVehicleFromJsonFileEnabled;
    static bool                                       _createVehicleMissionLineFromTaskEnabled;
    static bool                                       _upDownVehicleAllMissionEnabled;
    static bool                                       _vehicleControlAdvanceOperEnabled;
    static bool                                       _initWeaponInfoListOnceEnable;
    static bool                                       _multiVehicleEnabled;
    static bool                                       _gcsHeartbeatEnabled;
    static QString                                    _telemetrySavePath;
    static bool                                       _saveCsvTelemetry;
    static bool                                       _useConditionGate;
    static bool                                       _overrideSettingsGroupVisibility;
    static bool                                       _apmStartMavlinkStreams;
    static bool                                       _audioMuted;

    static bool                                       _initialConnectStateMachineNoRequestParameters;
    static bool                                       _initialConnectStateMachineNoRequestMission;


    static bool                                       _serverimplMavlinkControlLoggingTempLog;
    static bool                                       _serverimplMavlinkControlLoggingTempLogRemoveZero;
    static bool                                       _serverimplMavlinkControlLoggingTempCmdLog;
    static bool                                       _serverimplMavlinkControlLoggingTempCmdLogRemoveZero;

    static QString                                    _MissionWaypointMavFrameDefault;

    static QElapsedTimer                              _msecsElapsedTime;

    static QMap<QString, QString>                     _missionSupportCommandMavCmd;
    static QMap<QString, QString>                     _missionAddSupportCommandMavCmd;
    static QMap<QString, QString>                     _missionUnSupportCommandMavCmd;

    static QMap<QString, QString>                     _missionSupportCommandMavFrame;
    static QMap<QString, QString>                     _missionUnSupportCommandMavFrame;


    static QMap<QString, unsigned int>                _missionLineColor;

    static QMap<unsigned long long, QString>          _vehicleName;

    QTranslator                                       _qgcTranslatorSourceCode;           ///< translations for source code C++/Qml
    QTranslator                                       _qgcTranslatorJSON;                 ///< translations for json files
    QTranslator                                       _qgcTranslatorQtLibs;               ///< tranlsations for Qt libraries

    friend class UAVFlyManage_MavlinkApplicationSetting;
};

/// @brief Returns the QGCApplication object singleton.
QGCApplication* qgcApp(void);
QGCApplication* MavlinkApp(void);

#endif // QGCAPPLICATION_H
