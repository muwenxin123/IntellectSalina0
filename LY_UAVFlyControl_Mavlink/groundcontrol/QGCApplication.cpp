#include "QGCApplication.h"


#include <QJsonArray>
#include <QJsonDocument>
#include <QTextCodec>


#include "JsonHelper.h"
#include "LocalLink.h"
#include "SettingsManager.h"
#include "UnitsSettings.h"
#include "APMMavlinkStreamRateSettings.h"
#include "FirmwarePluginManager.h"
#include "MissionCommandTree.h"
#include "MAVLinkProtocol.h"
#include "FactMetaData.h"
#include "MissionCommandTree.h"
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "UASMessageHandler.h"
#include "AudioOutput.h"
#include "QGCPalette.h"
#include "AppSettings.h"


#include "UAVFlyManage_MavlinkMessageIndicatorDialog.h"
#include "IUAVFlyControl_MavlinkActivator.h"
#include "LY_UAVFlyControl_Mavlink.h"



QGCApplication*                              QGCApplication::_app                                                  = nullptr;

double                                       QGCApplication::_maxGoToLocationDistance                              = 10000;
double                                       QGCApplication::_takeoffMinimumAltitude                               = 10;
double                                       QGCApplication::_guidedMinimumAltitude                                = 2;
double                                       QGCApplication::_guidedMaximumAltitude                                = 121.92;
double                                       QGCApplication::_cruiseSpeed                                          = 15.0;
double                                       QGCApplication::_minCruiseSpeed                                       = 1.0;
double                                       QGCApplication::_hoverSpeed                                           = 5.0;
double                                       QGCApplication::_minHoverSpeed                                        = 1.0;
double                                       QGCApplication::_altitudeAMSL                                         = 10.0;
double                                       QGCApplication::_defaultMissionItemAltitude                           = 20.0;
double                                       QGCApplication::_vtolTransitionDistance                               = 20.0;
double                                       QGCApplication::_vtolTransitionDistanceMin                            = 10.0;
double                                       QGCApplication::_batteryPercentRemainingAnnounce                      = 20.0;

int                                          QGCApplication::_maxApplicationSettingGridRows                        = 3;
int                                          QGCApplication::_maxApplicationSettingGridColumns                     = 4;
int                                          QGCApplication::_selectApplicationSettingPointSize                    = 3;


QGeoPositionInfo                             QGCApplication::_geoPositionInfo;
QGeoCoordinate                               QGCApplication::_gcsPosition;
qreal                                        QGCApplication::_gcsHeading                                                   = qQNaN();
qreal                                        QGCApplication::_gcsPositionHorizontalAccuracy                                = std::numeric_limits<qreal>::infinity();


FirmwarePluginManager*                       QGCApplication::_firmwarePluginManager                                        = nullptr;
MissionCommandTree*                          QGCApplication::_missionCommandTree                                           = nullptr;
SettingsManager*                             QGCApplication::_settingsManager                                              = nullptr;
UnitsSettings*                               QGCApplication::_unitsSettings                                                = nullptr;
APMMavlinkStreamRateSettings*                QGCApplication::_apmMavlinkStreamRateSettings                                 = nullptr;
MAVLinkProtocol*                             QGCApplication::_mavlinkProtocol                                              = nullptr;
LocalLink*                                   QGCApplication::_localLink                                                    = nullptr;
MultiVehicleManager*                         QGCApplication::_multiVehicleManager                                          = nullptr;
UASMessageHandler*                           QGCApplication::_UASMessageHandler                                            = nullptr;
AudioOutput*                                 QGCApplication::_audioOutput                                                  = nullptr;
QGCPalette*                                  QGCApplication::_qgcPalette                                                   = nullptr;
AppSettings*                                 QGCApplication::_appSettings                                                  = nullptr;


QString                                      QGCApplication::_telemetrySavePath                                            = "";

bool                                         QGCApplication::_qgcApplicationMavlinkVehicleControlSendCommandMessage        = false;
bool                                         QGCApplication::_qgcApplicationMainDialogShow                                 = true;
bool                                         QGCApplication::_qgcApplicationMavlinkSelectDialogShow                        = true;
bool                                         QGCApplication::_qgcApplicationMavlinkControlDialogShow                       = true;
bool                                         QGCApplication::_qgcApplicationMavlinkTaskManageDialogShow                    = false;
bool                                         QGCApplication::_qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow = false;
bool                                         QGCApplication::_qgcApplicationMavlinkVehicleControlsDialogShow               = true;
bool                                         QGCApplication::_qgcApplicationMavlinkVehicleStatusInfoDialogShow             = false;
bool                                         QGCApplication::_qgcApplicationMavlinkParameterManageEditDialogShow           = true;
bool                                         QGCApplication::_qgcApplicationSettingDialogShow                              = true;
bool                                         QGCApplication::_qgcApplicationVehicleManageDialogShow                        = true;
bool                                         QGCApplication::_qgcApplicationStatusInfoDialogShow                           = false;
bool                                         QGCApplication::_qgcApplicationParameterManageDialogShow                      = false;
bool                                         QGCApplication::_qgcApplicationMessageIndicatorDialogShow                     = false;
bool                                         QGCApplication::_createVehicleFromJsonFileEnabled                             = false;
bool                                         QGCApplication::_createVehicleMissionLineFromTaskEnabled                      = true;
bool                                         QGCApplication::_upDownVehicleAllMissionEnabled                               = false;
bool                                         QGCApplication::_vehicleControlAdvanceOperEnabled                             = false;
bool                                         QGCApplication::_initWeaponInfoListOnceEnable                                 = false;
bool                                         QGCApplication::_multiVehicleEnabled                                          = true;
bool                                         QGCApplication::_gcsHeartbeatEnabled                                          = false;
bool                                         QGCApplication::_saveCsvTelemetry                                             = false;
bool                                         QGCApplication::_useConditionGate                                             = false;
bool                                         QGCApplication::_overrideSettingsGroupVisibility                              = false;
bool                                         QGCApplication::_apmStartMavlinkStreams                                       = true;
bool                                         QGCApplication::_audioMuted                                                   = false;


bool                                         QGCApplication::_initialConnectStateMachineNoRequestParameters                = false;
bool                                         QGCApplication::_initialConnectStateMachineNoRequestMission                   = false;


bool                                         QGCApplication::_serverimplMavlinkControlLoggingTempLog                       = true;
bool                                         QGCApplication::_serverimplMavlinkControlLoggingTempLogRemoveZero             = true;
bool                                         QGCApplication::_serverimplMavlinkControlLoggingTempCmdLog                    = true;
bool                                         QGCApplication::_serverimplMavlinkControlLoggingTempCmdLogRemoveZero          = true;

QString                                      QGCApplication::_MissionWaypointMavFrameDefault                               = "MAV_FRAME_GLOBAL_RELATIVE_ALT";

QElapsedTimer                                QGCApplication::_msecsElapsedTime;

QMap<QString, QString>                       QGCApplication::_missionSupportCommandMavCmd;
QMap<QString, QString>                       QGCApplication::_missionAddSupportCommandMavCmd;
QMap<QString, QString>                       QGCApplication::_missionUnSupportCommandMavCmd;

QMap<QString, QString>                       QGCApplication::_missionSupportCommandMavFrame;
QMap<QString, QString>                       QGCApplication::_missionUnSupportCommandMavFrame;

QMap<QString, unsigned int>                  QGCApplication::_missionLineColor;

QMap<unsigned long long, QString>            QGCApplication::_vehicleName;


/// @brief Returns the QGCApplication object singleton.
QGCApplication* qgcApp(void)
{
    return QGCApplication::_app;
}

QGCApplication *MavlinkApp()
{
    return QGCApplication::_app;
}

QGCApplication::QGCApplication(QObject *parent) : QObject(parent)
{
    _app = this;

    _settingsManager                 = new SettingsManager(this);
    _unitsSettings                   = new UnitsSettings(this);
    _apmMavlinkStreamRateSettings    = new APMMavlinkStreamRateSettings(this);
    _mavlinkProtocol                 = new MAVLinkProtocol(this);
    _firmwarePluginManager           = new FirmwarePluginManager(this);
    _missionCommandTree              = new MissionCommandTree(this);
    _multiVehicleManager             = new MultiVehicleManager(this);
    _audioOutput                     = new AudioOutput(this);
    _qgcPalette                      = new QGCPalette(this);
    _appSettings                     = new AppSettings(this);

    _UASMessageHandler               = new UASMessageHandler(this);
    _UASMessageHandler->showErrorsInToolbar();

    SharedLinkConfigurationPtr localConfiguration = std::make_shared<LocalConfiguration>("LocalLink");
    _localLink = new LocalLink(localConfiguration);

    _missionLineColor.clear();

    // storeSettings();
    loadSettings();
    loadSupportCommandMavCmd();
    loadSupportCommandMavFrame();

    if(createVehicleFromJsonFileEnabled()){
        createVehicleFromJsonFile();
    }

    if(_localLink){
        _localLink->run();
        connect(_localLink, &LinkInterface::bytesReceived,       _mavlinkProtocol,    &MAVLinkProtocol::receiveBytes);
    }
}

QGCApplication::~QGCApplication()
{
    storeSettings();
    storeSupportCommandMavCmd();
    storeSupportCommandMavFrame();

    disconnect(_localLink, &LinkInterface::bytesReceived,    _mavlinkProtocol,    &MAVLinkProtocol::receiveBytes);

    if(_localLink){
        _localLink->deleteLater();
        _localLink = nullptr;
    }

    if(_multiVehicleManager){
        _multiVehicleManager->deleteLater();
        _multiVehicleManager = nullptr;
    }

    _app = nullptr;
}

void QGCApplication::createVehicleFromJsonFile()
{
    const QString& vehicleArrayJsonFilename(QStringLiteral("../data/qgc/vehicleList.json"));

    const char* qgcVehicleFileType = "VehicleArrayData";

    QString errorString;
    int version;
    QJsonObject jsonObject = JsonHelper::openInternalQGCJsonFile(vehicleArrayJsonFilename, qgcVehicleFileType, 1, 1, version, errorString);
    if (!errorString.isEmpty()) {
        qWarning() << "Internal Error: " << errorString;
        return;
    }
    QJsonArray factArray;
    QMap<QString /* define name */, QString /* define value */> defineMap;

    const char* jsonMetaDataDefinesName              = "QGC.VehicleData.Defines";
    const char* jsonMetaDataFactsName                = "QGC.VehicleData.Vehicles";

    QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { jsonMetaDataDefinesName,   QJsonValue::Object, false },
        { jsonMetaDataFactsName,     QJsonValue::Array,  true },
    };

    if (!JsonHelper::validateKeys(jsonObject, keyInfoList, errorString)) {
        qWarning() << "Json document incorrect format:" << errorString;
        return;
    }

    auto loadJsonDefines = [jsonMetaDataDefinesName](const QJsonObject& jsonDefinesObject, QMap<QString, QString>& defineMap)
    {
        for (const QString& defineName: jsonDefinesObject.keys()) {
            QString mapKey = jsonMetaDataDefinesName + QString(".") + defineName;
            defineMap[mapKey] = jsonDefinesObject[defineName].toString();
        }
    };

    loadJsonDefines(jsonObject[jsonMetaDataDefinesName].toObject(), defineMap);
    factArray = jsonObject[jsonMetaDataFactsName].toArray();


    struct VehicleJsonObject{
        unsigned long long int ID;
        unsigned long long int ComponentID;
        QString                Vehicle_MAV_AUTOPILOT;
        QString                Vehicle_MAV_TYPE;
        unsigned long long int WeaponID;
        unsigned int           Color;
    };

    auto createMapFromJsonArray =  [](const QJsonArray jsonArray, QMap<QString, QString>& defineMap)
    {
        const char* Vehicle_ID = "ID";
        const char* Vehicle_ComponentID = "ComponentID";
        const char* Vehicle_MAV_AUTOPILOT = "MAV_AUTOPILOT";
        const char* Vehicle_MAV_TYPE = "MAV_TYPE";
        const char* Vehicle_WeaponID = "WeaponID";
        const char* Vehicle_LineColor = "Color";

        QMap<unsigned long long int, VehicleJsonObject> vehicleDataMap;
        for (int i=0; i<jsonArray.count(); i++) {
            QJsonValue jsonValue = jsonArray.at(i);
            if (!jsonValue.isObject()) {
                qWarning() << QStringLiteral("JsonValue at index %1 not an object").arg(i);
                continue;
            }
            QJsonObject jsonObject = jsonValue.toObject();

            if( jsonObject.contains(Vehicle_ID) && jsonObject.value(Vehicle_ID).isDouble() &&
                    jsonObject.contains(Vehicle_ComponentID) && jsonObject.value(Vehicle_ComponentID).isDouble() &&
                    jsonObject.contains(Vehicle_MAV_AUTOPILOT) && jsonObject.value(Vehicle_MAV_AUTOPILOT).isString() &&
                    jsonObject.contains(Vehicle_MAV_TYPE) && jsonObject.value(Vehicle_MAV_TYPE).isString() &&
                    jsonObject.contains(Vehicle_WeaponID) && jsonObject.value(Vehicle_WeaponID).isDouble()
                    ){
                double id = jsonObject.value(Vehicle_ID).toDouble(-1);
                if(id <= 0){
                    continue ;
                }

                double componentID = jsonObject.value(Vehicle_ComponentID).toDouble(-1);
                if(componentID < 0){
                    continue ;
                }

                double weaponID = jsonObject.value(Vehicle_WeaponID).toDouble(-1);
                if(weaponID <= 0){
                    continue ;
                }

                unsigned int color = 0;
                if(jsonObject.contains(Vehicle_LineColor) && jsonObject.value(Vehicle_LineColor).isString()){
                    color = jsonObject.value(Vehicle_LineColor).toString("").toUInt();
                }

                VehicleJsonObject vehicleJsonObject;
                vehicleJsonObject.ID          = id;
                vehicleJsonObject.ComponentID = componentID;
                vehicleJsonObject.Vehicle_MAV_AUTOPILOT = jsonObject.value(Vehicle_MAV_AUTOPILOT).toString();
                vehicleJsonObject.Vehicle_MAV_TYPE = jsonObject.value(Vehicle_MAV_TYPE).toString();
                vehicleJsonObject.WeaponID   = weaponID;
                vehicleJsonObject.Color      = color;

                vehicleDataMap.insert(id, vehicleJsonObject);
            }

        }
        return vehicleDataMap;
    };

    QMap<unsigned long long int, VehicleJsonObject> vehicleDataMap = createMapFromJsonArray(factArray, defineMap);


    LocalLink*              localLink = _localLink;

    for( auto& data : vehicleDataMap){
        unsigned long long int  vehicleId          = data.ID;
        unsigned long long int  defaultComponentId = data.ComponentID;
        MAV_AUTOPILOT           firmwareType       = QGCMAVLink::StringToAutopilot(data.Vehicle_MAV_AUTOPILOT);
        MAV_TYPE                vehicleType        = QGCMAVLink::StringToMavType(data.Vehicle_MAV_TYPE);
        unsigned long long int  weaponID           = data.WeaponID;
        unsigned int            color              = data.Color;

        if(vehicleId <= 0 ||
                defaultComponentId < 0 ||
                firmwareType == MAV_AUTOPILOT::MAV_AUTOPILOT_ENUM_END ||
                vehicleType  == MAV_TYPE::MAV_TYPE_ENUM_END
                ){
            continue;
        }
        multiVehicleManager()->addVehicles(localLink, weaponID, vehicleId, defaultComponentId, firmwareType, vehicleType);

        if(color != 0){
            _missionLineColor.insert(QString("%1%2").arg(weaponID).arg(vehicleId), color);
        }
    }
}

void QGCApplication::removeVehicleFromJsonFile()
{
    auto* vehicleList = multiVehicleManager()->vehicles();
    if(vehicleList){
        for(;vehicleList->size() > 0;) {
            const auto weaponID = vehicleList->first()->weaponId();
            const auto vehicleId = vehicleList->first()->vehicleId();
            multiVehicleManager()->removeVehicles(weaponID, vehicleId);
        }
    }
}

QMap<QString, QString> QGCApplication::missionSupportCommandMavCmd()
{
    return _missionSupportCommandMavCmd;
}

QMap<QString, QString> QGCApplication::missionAddSupportCommandMavCmd()
{
    return _missionAddSupportCommandMavCmd;
}

QMap<QString, QString> QGCApplication::missionUnSupportCommandMavCmd()
{
    return _missionUnSupportCommandMavCmd;
}

QMap<QString, QString> QGCApplication::missionSupportCommandMavFrame()
{
    return _missionSupportCommandMavFrame;
}

QMap<QString, QString> QGCApplication::missionUnSupportCommandMavFrame()
{
    return _missionUnSupportCommandMavFrame;
}

const QString QGCApplication::vehicleName(const unsigned long long weaponId, const unsigned long long vehicleId)
{
    const auto& iter = _vehicleName.find(weaponId);
    if(iter != _vehicleName.end()){
        return QString("%1").arg(iter.value());
    }
    return QString("%1:%2").arg(weaponId).arg(vehicleId);
}

void QGCApplication::updateVehicleName(const unsigned long long weaponId, const QString &name)
{
    const auto& iter = _vehicleName.find(weaponId);
    if(iter != _vehicleName.end()){
        iter.value() = name;
    }else{
        _vehicleName.insert(weaponId, name);
    }
    // qDebug() << __FILE__ << __LINE__ << _vehicleName;
}

void QGCApplication::updateMissionLineColor(unsigned long long weaponId, int vehicleId, unsigned int &lineColor)
{
    const auto& iter = _missionLineColor.find(QString("%1%2").arg(weaponId).arg(vehicleId));
    if(iter != _missionLineColor.end() && iter.value() != 0){
        lineColor = iter.value();
    }
}

bool QGCApplication::createVehicleFromJsonFileEnabled()
{
    return _createVehicleFromJsonFileEnabled;
}

bool QGCApplication::createVehicleMissionLineFromTaskEnabled()
{
    return _createVehicleMissionLineFromTaskEnabled;
}

bool QGCApplication::upDownVehicleAllMissionEnabled()
{
    return _upDownVehicleAllMissionEnabled;
}

bool QGCApplication::vehicleControlAdvanceOperEnabled()
{
    return _vehicleControlAdvanceOperEnabled;
}

bool QGCApplication::initWeaponInfoListOnceEnable()
{
    return _initWeaponInfoListOnceEnable;
}

bool QGCApplication::qgcApplicationMavlinkVehicleControlSendCommandMessage()
{
    return _qgcApplicationMavlinkVehicleControlSendCommandMessage;
}

bool QGCApplication::qgcApplicationMainDialogShow()
{
    return _qgcApplicationMainDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkSelectDialogShow()
{
    return _qgcApplicationMavlinkSelectDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkControlDialogShow()
{
    return _qgcApplicationMavlinkControlDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkTaskManageDialogShow()
{
    return _qgcApplicationMavlinkTaskManageDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow()
{
    return _qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkVehicleControlsDialogShow()
{
    return _qgcApplicationMavlinkVehicleControlsDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkVehicleStatusInfoDialogShow()
{
    return _qgcApplicationMavlinkVehicleStatusInfoDialogShow;
}

bool QGCApplication::qgcApplicationMavlinkParameterManageEditDialogShow()
{
    return _qgcApplicationMavlinkParameterManageEditDialogShow;
}

bool QGCApplication::qgcApplicationSettingDialogShow()
{
    return _qgcApplicationSettingDialogShow;
}

bool QGCApplication::qgcApplicationVehicleManageDialogShow()
{
    return _qgcApplicationVehicleManageDialogShow;
}

bool QGCApplication::qgcApplicationStatusInfoDialogShow()
{
    return _qgcApplicationStatusInfoDialogShow;
}

bool QGCApplication::qgcApplicationParameterManageDialogShow()
{
    return _qgcApplicationParameterManageDialogShow;
}

bool QGCApplication::qgcApplicationMessageIndicatorDialogShow()
{
    return _qgcApplicationMessageIndicatorDialogShow;
}

bool QGCApplication::multiVehicleEnabled()
{
    return _multiVehicleEnabled;
}

bool QGCApplication::gcsHeartbeatEnabled()
{
    return _gcsHeartbeatEnabled;
}

LocalLink *QGCApplication::localLink()
{
    return _localLink;
}

MultiVehicleManager *QGCApplication::multiVehicleManager()
{
    return _multiVehicleManager;
}

UASMessageHandler *QGCApplication::uasMessageHandler()
{
    return _UASMessageHandler;
}

QString QGCApplication::telemetrySavePath()
{
    QString path = _telemetrySavePath;
    if (!path.isEmpty() && QDir(path).exists()) {
        QDir dir(path);
        return dir.filePath(_telemetrySavePath);
    }
    return QString();
}

QString QGCApplication::rawName(MAV_CMD command)
{
    if(_missionCommandTree){
        return _missionCommandTree->rawName(command);
    }
    return QStringLiteral("MAV_CMD(%1)").arg((int)command);
}

QList<Vehicle *> *QGCApplication::vehicles()
{
    static QList<Vehicle *> tempVehicleList;
    if(!multiVehicleManager()){
        return &tempVehicleList;
    }
    return multiVehicleManager()->vehicles();
}

uint64_t QGCApplication::msecsSinceBoot()
{
    return _msecsElapsedTime.elapsed();
}

void QGCApplication::say(const QString &text, const QString &title)
{
    qDebug() << "QGCApplication say:"<< text;
    if(audioOutput()){
        audioOutput()->say(text);
    }

    if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()){
        LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()->showAppMessage(text, title);
    }
}

bool QGCApplication::runningUnitTests()
{
    return false;
}

void QGCApplication::sleep(unsigned long)
{

}

void QGCApplication::msleep(unsigned long)
{

}

void QGCApplication::usleep(unsigned long)
{

}

// QCursor *QGCApplication::overrideCursor()
// {
//
// }

void QGCApplication::setOverrideCursor(const QCursor &)
{

}

void QGCApplication::changeOverrideCursor(const QCursor &)
{

}

void QGCApplication::restoreOverrideCursor()
{

}

void QGCApplication::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    QCoreApplication::processEvents(flags);
}

void QGCApplication::processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime)
{
    QCoreApplication::processEvents(flags, maxtime);
}

/// Show modal application message to the user
void QGCApplication::showSetupView()
{

}

/// Show modal application message to the user
void QGCApplication::showAppMessage(const QString& message, const QString& title)
{
    qDebug() << "showAppMessage:"<< message << ", title:" << title;
    if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()){
        LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()->showAppMessage(message,title);
    }
}

/// Show modal application message to the user
void QGCApplication::showAppnewFormattedMessage(const int severity, const QString& message)
{
    qDebug() << "showAppnewFormattedMessage:"<< message;
    if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()){
        LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()->showAppnewFormattedMessage(severity, message);
    }
}

void QGCApplication::showRebootAppMessage(const QString &message, const QString &title)
{
    qDebug() << "showRebootAppMessage:"<< message << ", title:" << title;
    if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()){
        LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()->showRebootAppMessage(message, title);
    }
}

void QGCApplication::showCriticalVehicleMessage(const QString &message)
{
    qDebug() << "showCriticalVehicleMessage:"<< message;
    if(LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()){
        LY_UAVFlyControl_Mavlink::Get().getPUAVFlyManage_MavlinkMessageIndicatorDialog()->showCriticalVehicleMessage(message);
    }
}

void QGCApplication::reportMissingParameter(int componentId, const QString& name)
{
//    QPair<int, QString>  missingParam(componentId, name);

//    if (!_missingParams.contains(missingParam)) {
//        _missingParams.append(missingParam);
//    }
//    _missingParamsDelayedDisplayTimer.start();

    QString param = QStringLiteral("%1:%2").arg(componentId).arg(name);
    showAppMessage(tr("Parameters are missing from firmware. You may be running a version of firmware which is not fully supported or your firmware has a bug in it. Missing params: %1").arg(param));
}

bool QGCApplication::useConditionGate()
{
    return _useConditionGate;
}

double QGCApplication::altitudeAMSL()
{
    return _altitudeAMSL;
}

double QGCApplication::defaultMissionItemAltitude()
{
    return _defaultMissionItemAltitude;
}

double QGCApplication::vtolTransitionDistance()
{
    return _vtolTransitionDistance;
}

double QGCApplication::vtolTransitionDistanceMin()
{
    return _vtolTransitionDistanceMin;
}

double QGCApplication::batteryPercentRemainingAnnounce()
{
    return _batteryPercentRemainingAnnounce;
}

int QGCApplication::maxApplicationSettingGridRow()
{
    return _maxApplicationSettingGridRows;
}

int QGCApplication::maxApplicationSettingGridColumns()
{
    return _maxApplicationSettingGridColumns;
}

int QGCApplication::selectApplicationSettingPointSize()
{
    return _selectApplicationSettingPointSize;
}

bool QGCApplication::overrideSettingsGroupVisibility()
{
    return _overrideSettingsGroupVisibility;
}

bool QGCApplication::overrideSettingsGroupVisibility(QString name)
{
    Q_UNUSED(name);

    // Always show all
    return true;
}

bool QGCApplication::apmStartMavlinkStreams()
{
    return _apmStartMavlinkStreams;
}

bool QGCApplication::audioMuted()
{
    return _audioMuted;
}

bool QGCApplication::initialConnectStateMachineNoRequestParameters()
{
    return _initialConnectStateMachineNoRequestParameters;
}

bool QGCApplication::initialConnectStateMachineNoRequestMission()
{
    return _initialConnectStateMachineNoRequestMission;
}

const QString QGCApplication::missionWaypointMavFrameDefault()
{
    return _MissionWaypointMavFrameDefault;
}

SettingsManager *QGCApplication::settingsManager()
{
    return _settingsManager;
}

UnitsSettings *QGCApplication::unitsSettings()
{
    return _unitsSettings;
}

APMMavlinkStreamRateSettings *QGCApplication::apmMavlinkStreamRateSettings()
{
    return _apmMavlinkStreamRateSettings;
}

double QGCApplication::cruiseSpeed()
{
    return _cruiseSpeed;
}

double QGCApplication::hoverSpeed()
{
    return _hoverSpeed;
}

MAVLinkProtocol *QGCApplication::mavlinkProtocol()
{
    return _mavlinkProtocol;
}

double QGCApplication::maxGoToLocationDistance()
{
    return _maxGoToLocationDistance;
}

double QGCApplication::takeoffMinimumAltitude()
{
    return _takeoffMinimumAltitude;
}

double QGCApplication::guidedMinimumAltitude()
{
    return _guidedMinimumAltitude;
}

double QGCApplication::guidedMaximumAltitude()
{
    return _guidedMaximumAltitude;
}

bool QGCApplication::saveCsvTelemetry()
{
    return _saveCsvTelemetry;
}

const QList<QGCApplication::AppSettingsValue<QString>>& QGCApplication::AppSettingsValueList(QString)
{
    const static QList<AppSettingsValue<QString>> appSettingsStringValueList = {
        {"../data/qgc/GCApplication.ini",     false,    true,    "MISSIONWAYPOINTMAVFRAMEDEFAULT"          , "MissionWaypointMavFrameDefault"          , "MissionWaypointMavFrameDefault"          , _MissionWaypointMavFrameDefault          , _MissionWaypointMavFrameDefault           },
    };

    return appSettingsStringValueList;
}

const QList<QGCApplication::AppSettingsValue<double>>& QGCApplication::AppSettingsValueList(double)
{
    const static QList<AppSettingsValue<double>> appSettingsDoubleValueList = {
        {"../data/qgc/GCApplication.ini",    false,    true,    "MAXGOTOLOCATIONDISTANCE"                , "MaxGoToLocationDistance"                , "MaxGoToLocationDistance"          , _maxGoToLocationDistance          , _maxGoToLocationDistance           },
        {"../data/qgc/GCApplication.ini",    false,    false,   "TAKEOFFMINIMUMALTITUDE"                 , "TAKEOFFMINIMUMALTITUDE"                 , "TakeoffMinimumAltitude"           , _takeoffMinimumAltitude           , _takeoffMinimumAltitude            },
        {"../data/qgc/GCApplication.ini",    false,    true,    "GUIDEDMINIMUMALTITUDE"                  , "GuidedMinimumAltitude"                  , "GuidedMinimumAltitude"            , _guidedMinimumAltitude            , _guidedMinimumAltitude             },
        {"../data/qgc/GCApplication.ini",    false,    true,    "GUIDEDMAXIMUMALTITUDE"                  , "GuidedMaximumAltitude"                  , "GuidedMaximumAltitude"            , _guidedMaximumAltitude            , _guidedMaximumAltitude             },
        {"../data/qgc/GCApplication.ini",    false,    true,    "CRUISESPEED"                            , "CruiseSpeed"                            , "CruiseSpeed"                      , _cruiseSpeed                      , _cruiseSpeed                       },
        {"../data/qgc/GCApplication.ini",    false,    true,    "MINCRUISESPEED"                         , "MinCruiseSpeed"                         , "MinCruiseSpeed"                   , _minCruiseSpeed                   , _minCruiseSpeed                    },
        {"../data/qgc/GCApplication.ini",    false,    true,    "HOVERSPEED"                             , "HoverSpeed"                             , "HoverSpeed"                       , _hoverSpeed                       , _hoverSpeed                        },
        {"../data/qgc/GCApplication.ini",    false,    true,    "MINHOVERSPEED"                          , "MinHoverSpeed"                          , "MinHoverSpeed"                    , _minHoverSpeed                    , _minHoverSpeed                     },
        {"../data/qgc/GCApplication.ini",    false,    true,    "ALTITUDEAMSL"                           , "AltitudeAMSL"                           , "AltitudeAMSL"                     , _altitudeAMSL                     , _altitudeAMSL                      },
        {"../data/qgc/GCApplication.ini",    false,    true,    "DEFAULTMISSIONITEMALTITUDE"             , "DefaultMissionItemAltitude"             , "DefaultMissionItemAltitude"       , _defaultMissionItemAltitude       , _defaultMissionItemAltitude        },
        {"../data/qgc/GCApplication.ini",    false,    true,    "VTOLTRANSITIONDISTANCE"                 , "VtolTransitionDistance"                 , "VtolTransitionDistance"           , _vtolTransitionDistance           , _vtolTransitionDistance            },
        {"../data/qgc/GCApplication.ini",    false,    true,    "VTOLTRANSITIONDISTANCEMIN"              , "VtolTransitionDistanceMin"              , "VtolTransitionDistanceMin"        , _vtolTransitionDistanceMin        , _vtolTransitionDistanceMin         },
        {"../data/qgc/GCApplication.ini",    false,    true,    "BATTERYPERCENTREMAININGANNOUNCE"        , "BatteryPercentRemainingAnnounce"        , "BatteryPercentRemainingAnnounce"  , _batteryPercentRemainingAnnounce  , _batteryPercentRemainingAnnounce   },
    };

    return appSettingsDoubleValueList;
}

const QList<QGCApplication::AppSettingsValue<int>>& QGCApplication::AppSettingsValueList(int)
{

	//const QString    fileName;
	//const bool       readOnly;
	//const bool       show;
	//const QString    groupKey;
	//const QString    valueKey;
	//mutable QString  valueName;
	//// const int     decimals;
	//// const int     minimum;
	//// const int     maximum;
	//const T          defaultValue;

    const static QList<AppSettingsValue<int>> appSettingsIntValueList = {
        {"../data/qgc/GCApplication.ini",     false,    true,    "MAXAPPLICATIONSETTINGGRIDROWS"          , "MaxApplicationSettingGridRows"          , "MaxApplicationSettingGridRows"          , _maxApplicationSettingGridRows          , _maxApplicationSettingGridRows           },
        {"../data/qgc/GCApplication.ini",     false,    true,    "MAXAPPLICATIONSETTINGGRIDCOLUMNS"       , "MaxApplicationSettingGridColumns"       , "MaxApplicationSettingGridColumns"       , _maxApplicationSettingGridColumns       , _maxApplicationSettingGridColumns        },
        {"../data/qgc/GCApplication.ini",     false,    true,    "SELECTAPPLICATIONSETTINGPOINTSIZE"      , "SelectApplicationSettingPointSize"      , "SelectApplicationSettingPointSize"      , _selectApplicationSettingPointSize      , _selectApplicationSettingPointSize       },
    };

    return appSettingsIntValueList;
}

const QList<QGCApplication::AppSettingsValue<bool>>& QGCApplication::AppSettingsValueList(bool)
{
    const static QList<AppSettingsValue<bool>> appSettingsBoolValueList = {
        {"../data/qgc/MultiVehicleManager.ini",     false,    true,    "GCSHEARTBEATENABLED"                                             , "GcsHeartbeatEnabled"                                             , "GcsHeartbeatEnabled"                                             , _gcsHeartbeatEnabled                                             , _gcsHeartbeatEnabled                                             },
        {"../data/qgc/GCApplication.ini",           false,    true,    "APMSTARTMAVLINKSTREAMS"                                          , "ApmStartMavlinkStreams"                                          , "ApmStartMavlinkStreams"                                          , _apmStartMavlinkStreams                                          , _apmStartMavlinkStreams                                          },
        {"../data/qgc/GCApplication.ini",           false,    true,    "AUDIOMUTED"                                                      , "AudioMuted"                                                      , "AudioMuted"                                                      , _audioMuted                                                      , _audioMuted                                                      },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMAVLINKVEHICLECONTROLSENDCOMMANDMESSAGE"           , "QGCApplicationMavlinkVehicleControlSendCommandMessage"           , "QGCApplicationMavlinkVehicleControlSendCommandMessage"           , _qgcApplicationMavlinkVehicleControlSendCommandMessage           , _qgcApplicationMavlinkVehicleControlSendCommandMessage           },
        {"../data/qgc/GCApplication.ini",           true,     true,   "QGCAPPLICATIONMAINDIALOGSHOW"                                    , "QGCApplicationMainDialogShow"                                    , "QGCApplicationMainDialogShow"                                    , _qgcApplicationMainDialogShow                                    , _qgcApplicationMainDialogShow                                    },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMAVLINKSELECTDIALOGSHOW"                           , "QGCApplicationMavlinkSelectDialogShow"                           , "QGCApplicationMavlinkSelectDialogShow"                           , _qgcApplicationMavlinkSelectDialogShow                           , _qgcApplicationMavlinkSelectDialogShow                           },
        {"../data/qgc/GCApplication.ini",           true,     true,    "QGCAPPLICATIONMAVLINKCONTROLDIALOGSHOW"                          , "QGCApplicationMavlinkControlDialogShow"                          , "QGCApplicationMavlinkControlDialogShow"                          , _qgcApplicationMavlinkControlDialogShow                          , _qgcApplicationMavlinkControlDialogShow                          },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMAVLINKTASKMANAGEDIALOGSHOW"                       , "QGCApplicationMavlinkTaskManageDialogShow"                       , "QGCApplicationMavlinkTaskManageDialogShow"                       , _qgcApplicationMavlinkTaskManageDialogShow                       , _qgcApplicationMavlinkTaskManageDialogShow                       },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMAVLINKVEHICLEGUIDEDMODEGOTOLOCATIONDIALOGSHOW"    , "QGCApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow"    , "QGCApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow"    , _qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow    , _qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow    },
        {"../data/qgc/GCApplication.ini",           true,     true,   "QGCAPPLICATIONMAVLINKVEHICLECONTROLSDIALOGSHOW"                  , "QGCApplicationMavlinkVehicleControlsDialogShow"                  , "QGCApplicationMavlinkVehicleControlsDialogShow"                  , _qgcApplicationMavlinkVehicleControlsDialogShow                  , _qgcApplicationMavlinkVehicleControlsDialogShow                  },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMAVLINKVEHICLESTATUSINFODIALOGSHOW"                , "QGCApplicationMavlinkVehicleStatusInfoDialogShow"                , "QGCApplicationMavlinkVehicleStatusInfoDialogShow"                , _qgcApplicationMavlinkVehicleStatusInfoDialogShow                , _qgcApplicationMavlinkVehicleStatusInfoDialogShow                },
        {"../data/qgc/GCApplication.ini",           true,     true,   "QGCAPPLICATIONMAVLINKPARAMETERMANAGEEDITDIALOGSHOW"              , "QGCApplicationMavlinkParameterManageEditDialogShow"              , "QGCApplicationMavlinkParameterManageEditDialogShow"              , _qgcApplicationMavlinkParameterManageEditDialogShow              , _qgcApplicationMavlinkParameterManageEditDialogShow              },
        {"../data/qgc/GCApplication.ini",           true,     true,   "QGCAPPLICATIONSETTINGDIALOGSHOW"                                 , "QGCApplicationSettingDialogShow"                                 , "QGCApplicationSettingDialogShow"                                 , _qgcApplicationSettingDialogShow                                 , _qgcApplicationSettingDialogShow                                 },
        {"../data/qgc/GCApplication.ini",           true,     true,   "QGCAPPLICATIONVEHICLEMANAGEDIALOGSHOW"                           , "QGCApplicationVehicleManageDialogShow"                           , "QGCApplicationVehicleManageDialogShow"                           , _qgcApplicationVehicleManageDialogShow                           , _qgcApplicationVehicleManageDialogShow                           },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONSTATUSINFODIALOGSHOW"                              , "QGCApplicationStatusInfoDialogShow"                              , "QGCApplicationStatusInfoDialogShow"                              , _qgcApplicationStatusInfoDialogShow                              , _qgcApplicationStatusInfoDialogShow                              },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONPARAMETERMANAGEDIALOGSHOW"                         , "QGCApplicationParameterManageDialogShow"                         , "QGCApplicationParameterManageDialogShow"                         , _qgcApplicationParameterManageDialogShow                         , _qgcApplicationParameterManageDialogShow                         },
        {"../data/qgc/GCApplication.ini",           true,     false,   "QGCAPPLICATIONMESSAGEINDICATORDIALOGSHOW"                        , "QGCApplicationMessageIndicatorDialogShow"                        , "QGCApplicationMessageIndicatorDialogShow"                        , _qgcApplicationMessageIndicatorDialogShow                        , _qgcApplicationMessageIndicatorDialogShow                        },
        {"../data/qgc/GCApplication.ini",           true,     true,    "CREATEVEHICLEFROMJSONFILEENABLED"                                , "CreateVehicleFromJsonFileEnabled"                                , "CreateVehicleFromJsonFileEnabled"                                , _createVehicleFromJsonFileEnabled                                , _createVehicleFromJsonFileEnabled                                },
        {"../data/qgc/GCApplication.ini",           true,     true,    "CREATEVEHICLEMISSIONLINEFROMTASKENABLED"                         , "CreateVehicleMissionLineFromTaskEnabled"                         , "CreateVehicleMissionLineFromTaskEnabled"                         , _createVehicleMissionLineFromTaskEnabled                         , _createVehicleMissionLineFromTaskEnabled                         },
        {"../data/qgc/GCApplication.ini",           true,     true,    "UPDOWNVEHICLEALLMISSIONENABLED"                                  , "UpDownVehicleAllMissionEnabled"                                  , "UpDownVehicleAllMissionEnabled"                                  , _upDownVehicleAllMissionEnabled                                  , _upDownVehicleAllMissionEnabled                                  },
        {"../data/qgc/GCApplication.ini",           true,     true,    "VEHICLECONTROLADVANCEOPERENABLED"                                , "VehicleControlAdvanceOperEnabled"                                , "VehicleControlAdvanceOperEnabled"                                , _vehicleControlAdvanceOperEnabled                                , _vehicleControlAdvanceOperEnabled                                },
        {"../data/qgc/GCApplication.ini",           true,     true,    "INITWEAPONINFOLISTONCEENABLE"                                    , "InitWeaponInfoListOnceEnable"                                    , "InitWeaponInfoListOnceEnable"                                    , _initWeaponInfoListOnceEnable                                    , _initWeaponInfoListOnceEnable                                    },
        // {"../data/qgc/GCApplication.ini",        false,    true,    "SERVERIMPL_MAVLINK_CONTROL_LOGGING"                              , "ServerimplMavlinkControlLoggingTempLog"                          , "ServerimplMavlinkControlLoggingTempLog"                          , _serverimplMavlinkControlLoggingTempLog                          , _serverimplMavlinkControlLoggingTempLog                          },
        // {"../data/qgc/GCApplication.ini",        false,    true,    "SERVERIMPL_MAVLINK_CONTROL_LOGGING"                              , "ServerimplMavlinkControlLoggingTempLogRemoveZero"                , "ServerimplMavlinkControlLoggingTempLogRemoveZero"                , _serverimplMavlinkControlLoggingTempLogRemoveZero                , _serverimplMavlinkControlLoggingTempLogRemoveZero                },
        // {"../data/qgc/GCApplication.ini",        false,    true,    "SERVERIMPL_MAVLINK_CONTROL_LOGGING"                              , "ServerimplMavlinkControlLoggingTempCmdLog"                       , "ServerimplMavlinkControlLoggingTempCmdLog"                       , _serverimplMavlinkControlLoggingTempCmdLog                       , _serverimplMavlinkControlLoggingTempCmdLog                       },
        // {"../data/qgc/GCApplication.ini",        false,    true,    "SERVERIMPL_MAVLINK_CONTROL_LOGGING"                              , "ServerimplMavlinkControlLoggingTempCmdLogRemoveZero"             , "ServerimplMavlinkControlLoggingTempCmdLogRemoveZero"             , _serverimplMavlinkControlLoggingTempCmdLogRemoveZero             , _serverimplMavlinkControlLoggingTempCmdLogRemoveZero             },

    };
    return appSettingsBoolValueList;
}

void QGCApplication::loadSettings()
{
    // QGCApplication settings load defaults from settings
    {
        QSettings settings("../data/qgc/GCApplication.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("GB2312"));
        settings.beginGroup("QGC_MAVLINK_CREATEVEHICLEFROMJSONFILEENABLED");
        _createVehicleFromJsonFileEnabled = settings.value("CreateVehicleFromJsonFileEnabled", "false").toBool();
        settings.endGroup();

        settings.beginGroup("QGC_MAVLINK_CREATEVEHICLEMISSIONLINEFROMTASKENABLED");
        _createVehicleMissionLineFromTaskEnabled = settings.value("CreateVehicleMissionLineFromTaskEnabled", "true").toBool();
        settings.endGroup();

        settings.beginGroup("QGC_MAVLINK_MULTIVEHICLEENABLED");
        _multiVehicleEnabled              = settings.value("MultiVehicleEnabled", "true").toBool();
        settings.endGroup();

        AppSettingsValue<QString>::readAppSettingsValue(settings, AppSettingsValueList(QString("")));
        AppSettingsValue<double>::readAppSettingsValue(settings, AppSettingsValueList(0.0));
        AppSettingsValue<int>::readAppSettingsValue(settings, AppSettingsValueList(0));
        AppSettingsValue<bool>::readAppSettingsValue(settings, AppSettingsValueList(true));
    }

    // MultiVehicleManager settings load defaults from settings
    {
        QSettings settings("../data/qgc/MultiVehicleManager.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));

        AppSettingsValue<QString>::readAppSettingsValue(settings, AppSettingsValueList(QString("")));
        AppSettingsValue<double>::readAppSettingsValue(settings, AppSettingsValueList(0.0));
        AppSettingsValue<int>::readAppSettingsValue(settings, AppSettingsValueList(0));
        AppSettingsValue<bool>::readAppSettingsValue(settings, AppSettingsValueList(false));
    }

}

void QGCApplication::storeSettings()
{
    // QGCApplication settings store defaults from settings
    {
        QSettings settings("../data/qgc/GCApplication.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("QGC_MAVLINK_CREATEVEHICLEFROMJSONFILEENABLED");
        settings.setValue("CreateVehicleFromJsonFileEnabled", _createVehicleFromJsonFileEnabled);
        settings.endGroup();

        settings.beginGroup("QGC_MAVLINK_CREATEVEHICLEMISSIONLINEFROMTASKENABLED");
        settings.setValue("CreateVehicleMissionLineFromTaskEnabled", _createVehicleMissionLineFromTaskEnabled);
        settings.endGroup();

        settings.beginGroup("QGC_MAVLINK_MULTIVEHICLEENABLED");
        settings.setValue("MultiVehicleEnabled", _multiVehicleEnabled);
        settings.endGroup();

        AppSettingsValue<QString>::writeAppSettingsValue(settings, AppSettingsValueList(QString("")));
        AppSettingsValue<double>::writeAppSettingsValue(settings, AppSettingsValueList(0.0));
        AppSettingsValue<int>::writeAppSettingsValue(settings, AppSettingsValueList(0));
        AppSettingsValue<bool>::writeAppSettingsValue(settings, AppSettingsValueList(false));
    }

    // MultiVehicleManager settings store defaults from settings
    {
        QSettings settings("../data/qgc/MultiVehicleManager.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));

        AppSettingsValue<QString>::writeAppSettingsValue(settings, AppSettingsValueList(QString("")));
        AppSettingsValue<double>::writeAppSettingsValue(settings, AppSettingsValueList(0.0));
        AppSettingsValue<int>::writeAppSettingsValue(settings, AppSettingsValueList(0));
        AppSettingsValue<bool>::writeAppSettingsValue(settings, AppSettingsValueList(false));
    }
}

void QGCApplication::loadSupportCommandMavCmd()
{
    const QString filename("../data/qgc/MavSupportCommandMavCmd.json");

    QString errorMessage("MavSupportCommandMavCmd: %1");
    QString errorString("");

    QFileInfo fileInfo(filename);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorString = file.errorString() + QStringLiteral(" ") + filename;
        QGCApplication::showAppMessage(errorMessage.arg(errorString));
        return;
    }


    QString         errorStr;
    QJsonDocument   jsonDoc;
    QByteArray      bytes = file.readAll();

    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return ;
    }

    QJsonObject json = jsonDoc.object();

    // _missionSupportCommandMavCmd
    _missionSupportCommandMavCmd.clear();
    {
        const QJsonArray jsonArraySupportCommandMavCmd(json["MissionSupportCommandMavCmd"].toArray());

        for(int i = 0; i < jsonArraySupportCommandMavCmd.count(); i++){
            QJsonValue jsonValue = jsonArraySupportCommandMavCmd.at(i);
            if (!jsonValue.isString()) {
                qWarning() << QStringLiteral("MissionSupportCommandMavCmd at index %1 not an String").arg(i);
                continue;
            }
            const auto& supportCommandMavCmdStr = jsonValue.toString();
            _missionSupportCommandMavCmd.insert(supportCommandMavCmdStr, supportCommandMavCmdStr);
        }
    }

    // _missionAddSupportCommandMavCmd
    _missionAddSupportCommandMavCmd.clear();
    {
        const QJsonArray jsonArrayAddSupportCommandMavCmd(json["MissionAddSupportCommandMavCmd"].toArray());

        for(int i = 0; i < jsonArrayAddSupportCommandMavCmd.count(); i++){
            QJsonValue jsonValue = jsonArrayAddSupportCommandMavCmd.at(i);
            if (!jsonValue.isString()) {
                qWarning() << QStringLiteral("MissionAddSupportCommandMavCmd at index %1 not an String").arg(i);
                continue;
            }
            const auto& addSupportCommandMavCmdStr = jsonValue.toString();
            _missionAddSupportCommandMavCmd.insert(addSupportCommandMavCmdStr, addSupportCommandMavCmdStr);
        }
    }

    // _missionUnSupportCommandMavCmd
    _missionUnSupportCommandMavCmd.clear();
    {
        const QJsonArray jsonArrayUnSupportCommandMavCmd(json["MissionUnSupportCommandMavCmd"].toArray());

        for(int i = 0; i < jsonArrayUnSupportCommandMavCmd.count(); i++){
            QJsonValue jsonValue = jsonArrayUnSupportCommandMavCmd.at(i);
            if (!jsonValue.isString()) {
                qWarning() << QStringLiteral("MissionUnSupportCommandMavCmd at index %1 not an String").arg(i);
                continue;
            }
            const auto& unSupportCommandMavCmdStr = jsonValue.toString();
            _missionUnSupportCommandMavCmd.insert(unSupportCommandMavCmdStr, unSupportCommandMavCmdStr);
        }
    }
}

void QGCApplication::storeSupportCommandMavCmd()
{

}

void QGCApplication::loadSupportCommandMavFrame()
{
    const QString filename("../data/qgc/MavSupportCommandMavFrame.json");

    QString errorMessage("MavSupportCommandMavFrame: %1");
    QString errorString("");

    QFileInfo fileInfo(filename);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorString = file.errorString() + QStringLiteral(" ") + filename;
        QGCApplication::showAppMessage(errorMessage.arg(errorString));
        return;
    }


    QString         errorStr;
    QJsonDocument   jsonDoc;
    QByteArray      bytes = file.readAll();

    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return ;
    }

    QJsonObject json = jsonDoc.object();

    // _missionSupportCommandMavFrame
    _missionSupportCommandMavFrame.clear();
    {
        const QJsonArray jsonArraySupportCommandMavFrame(json["MissionSupportCommandMavFrame"].toArray());

        for(int i = 0; i < jsonArraySupportCommandMavFrame.count(); i++){
            QJsonValue jsonValue = jsonArraySupportCommandMavFrame.at(i);
            if (!jsonValue.isString()) {
                qWarning() << QStringLiteral("MissionSupportCommandMavFrame at index %1 not an String").arg(i);
                continue;
            }
            const auto& supportCommandMavFrameStr = jsonValue.toString();
            _missionSupportCommandMavFrame.insert(supportCommandMavFrameStr, supportCommandMavFrameStr);
        }
    }

    // _missionUnSupportCommandMavFrame
    _missionUnSupportCommandMavFrame.clear();
    {
        const QJsonArray jsonArrayUnSupportCommandMavFrame(json["MissionUnSupportCommandMavFrame"].toArray());

        for(int i = 0; i < jsonArrayUnSupportCommandMavFrame.count(); i++){
            QJsonValue jsonValue = jsonArrayUnSupportCommandMavFrame.at(i);
            if (!jsonValue.isString()) {
                qWarning() << QStringLiteral("MissionUnSupportCommandMavFrame at index %1 not an String").arg(i);
                continue;
            }
            const auto& unSupportCommandMavFrameStr = jsonValue.toString();
            _missionUnSupportCommandMavFrame.insert(unSupportCommandMavFrameStr, unSupportCommandMavFrameStr);
        }
    }
}

void QGCApplication::storeSupportCommandMavFrame()
{

}

void QGCApplication::handleFlightModeChanged(unsigned long long int weaponId, int id, int componentId, const QString& flightMode)
{
    auto* const pIUAVRealTimeStateManageService = IUAVFlyControl_MavlinkActivator::Get().getIUAVRealTimeStateManageService();
    if (pIUAVRealTimeStateManageService != nullptr) {
        pIUAVRealTimeStateManageService->SetUAVRealTimeStateFlightMode(std::to_string(weaponId), flightMode.toStdString());
    }
}


