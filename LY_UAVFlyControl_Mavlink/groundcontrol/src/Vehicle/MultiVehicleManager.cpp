#include "MultiVehicleManager.h"

#include <QTextCodec>

#include "ParameterManager.h"
#include "AutoPilotPlugin.h"
#include "MAVLinkProtocol.h"
#include "QGCApplication.h"
#include "FollowMe.h"
#include "SettingsManager.h"


QGC_LOGGING_CATEGORY(MultiVehicleManagerLog, "MultiVehicleManagerLog")

const char* MultiVehicleManager::_gcsHeartbeatEnabledKey = "GcsHeartbeatEnabled";

MultiVehicleManager::MultiVehicleManager(QGCApplication* app)
    : QObject(app)
    , _firmwarePluginManager(nullptr)
    , _mavlinkProtocol(nullptr)
    , _gcsHeartbeatEnabled(true)
{
    QSettings settings("../data/conf/MultiVehicleManager.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    // QSettings settings("../data/conf/GCApplication.ini", QSettings::IniFormat);
    settings.beginGroup("QGC_MAVLINK_GCSHEARTBEATENABLED");
    if(settings.contains(_gcsHeartbeatEnabledKey)){
        bool ok = settings.value(_gcsHeartbeatEnabledKey).canConvert(QVariant::Type::Bool);
        bool gcsHeartbeatEnabled = settings.value(_gcsHeartbeatEnabledKey, true).toBool();
        if(ok){
            _gcsHeartbeatEnabled = gcsHeartbeatEnabled;
        }
    }else{
        _gcsHeartbeatEnabled = QGCApplication::gcsHeartbeatEnabled();
    }
    _gcsHeartbeatTimer.setInterval(_gcsHeartbeatRateMSecs);
    _gcsHeartbeatTimer.setSingleShot(false);
    settings.endGroup();


    _vehicleId_vehicles.clear();

    setToolbox();
}

void MultiVehicleManager::setToolbox()
{
    _firmwarePluginManager = QGCApplication::firmwarePluginManager();
    _mavlinkProtocol       = QGCApplication::mavlinkProtocol();

    connect(_mavlinkProtocol, &MAVLinkProtocol::vehicleHeartbeatInfo, this, &MultiVehicleManager::_vehicleHeartbeatInfo);
    connect(&_gcsHeartbeatTimer, &QTimer::timeout, this, &MultiVehicleManager::_sendGCSHeartbeat);

    if (_gcsHeartbeatEnabled) {
        _gcsHeartbeatTimer.start();
    }
}

void MultiVehicleManager::_vehicleHeartbeatInfo(LinkInterface* link, unsigned long long int  weaponId, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType)
{
    // Special case PX4 Flow since depending on firmware it can have different settings. We force to the PX4 Firmware settings.
    if (link->isPX4Flow()) {
        vehicleId = 81;
        componentId = 50;//MAV_COMP_ID_AUTOPILOT1;
        vehicleFirmwareType = MAV_AUTOPILOT_GENERIC;
        vehicleType = 0;
    }

    if (componentId != MAV_COMP_ID_AUTOPILOT1) {
        // Special case for PX4 Flow
        if (vehicleId != 81 || componentId != 50) {
            // Don't create vehicles for components other than the autopilot
            qCDebug(MultiVehicleManagerLog()) << "Ignoring heartbeat from unknown component port:vehicleId:componentId:fwType:vehicleType"
                                              << link->linkConfiguration()->name()
                                              << vehicleId
                                              << componentId
                                              << vehicleFirmwareType
                                              << vehicleType;
            return;
        }
    }

#if !defined(NO_ARDUPILOT_DIALECT)
    // When you flash a new ArduCopter it does not set a FRAME_CLASS for some reason. This is the only ArduPilot variant which
    // works this way. Because of this the vehicle type is not known at first connection. In order to make QGC work reasonably
    // we assume ArduCopter for this case.
    if (vehicleType == 0 && vehicleFirmwareType == MAV_AUTOPILOT_ARDUPILOTMEGA) {
        vehicleType = MAV_TYPE_QUADROTOR;
    }
#endif

    if (_vehicles.count() > 0 && !QGCApplication::multiVehicleEnabled()) {
        return;
    }
    if (_ignoreVehicleIds.contains(vehicleId) || getVehicleByWeaponIdAndId(weaponId, vehicleId) || vehicleId == 0) {
        return;
    }

    switch (vehicleType) {
    case MAV_TYPE_GCS:
    case MAV_TYPE_ONBOARD_CONTROLLER:
    case MAV_TYPE_GIMBAL:
    case MAV_TYPE_ADSB:
        // These are not vehicles, so don't create a vehicle for them
        return;
    default:
        // All other MAV_TYPEs create vehicles
        break;
    }

    qCDebug(MultiVehicleManagerLog()) << "Adding new vehicle link:vehicleId:componentId:vehicleFirmwareType:vehicleType "
                                      << link->linkConfiguration()->name()
                                      << vehicleId
                                      << componentId
                                      << vehicleFirmwareType
                                      << vehicleType;

    if (vehicleId == _mavlinkProtocol->getSystemId()) {
        QGCApplication::showAppMessage(tr("Warning: A vehicle is using the same system id as %1: %2").arg(_mavlinkProtocol->getSystemId()).arg(vehicleId));
    }

    const QString vehicleName = QGCApplication::vehicleName(weaponId, vehicleId);
    Vehicle* vehicle = new Vehicle(link, vehicleName, weaponId, vehicleId, componentId, (MAV_AUTOPILOT)vehicleFirmwareType, (MAV_TYPE)vehicleType, _firmwarePluginManager);
    connect(vehicle,                        &Vehicle::requestProtocolVersion,           this, &MultiVehicleManager::_requestProtocolVersion);



    bool vehicleadd = false;
    {
        for (int i=0; i< _vehicles.count(); i++) {
            Vehicle* vehicleTemp = qobject_cast<Vehicle*>(_vehicles[i]);
            if (vehicleId < vehicleTemp->id()) {
                _vehicles.insert(i, vehicle);
                vehicleadd = true;
                break;
            }
        }
    }
    if(!vehicleadd){
        _vehicles.append(vehicle);
    }
    _vehicleId_vehicles.insert(WeaponVehicle_ID(weaponId, vehicleId), vehicle);

    // Send QGC heartbeat ASAP, this allows PX4 to start accepting commands
    _sendGCSHeartbeat();


    emit vehicleAdded(vehicle);

    if (_vehicles.count() > 1) {
        MavlinkApp()->showAppMessage(tr("Connected to Vehicle weaponId:%1, id:%2, MAV_AUTOPILOT:%3, MAV_TYPE:%4").arg(weaponId).arg(vehicleId).arg(vehicleFirmwareType).arg(vehicleType));
    } else {
        MavlinkApp()->showAppMessage(tr("Connected to Vehicle weaponId:%1, id:%2, MAV_AUTOPILOT:%3, MAV_TYPE:%4").arg(weaponId).arg(vehicleId).arg(vehicleFirmwareType).arg(vehicleType));
    }

#if defined (__ios__) || defined(__android__)
    if(_vehicles.count() == 1) {
        //-- Once a vehicle is connected, keep screen from going off
        qCDebug(MultiVehicleManagerLog) << "QAndroidJniObject::keepScreenOn";
        MobileScreenMgr::setKeepScreenOn(true);
    }
#endif
}

/// This slot is connected to the Vehicle::requestProtocolVersion signal such that the vehicle manager
/// tries to switch MAVLink to v2 if all vehicles support it
void MultiVehicleManager::_requestProtocolVersion(unsigned version)
{
    unsigned maxversion = 0;

    if (_vehicles.count() == 0) {
        _mavlinkProtocol->setVersion(version);
        return;
    }

    for (int i=0; i<_vehicles.count(); i++) {

        Vehicle *v = qobject_cast<Vehicle*>(_vehicles[i]);
        if (v && v->maxProtoVersion() > maxversion) {
            maxversion = v->maxProtoVersion();
        }
    }

    if (_mavlinkProtocol->getCurrentVersion() != maxversion) {
        _mavlinkProtocol->setVersion(maxversion);
    }
}

/// This slot is connected to the Vehicle::allLinksDestroyed signal such that the Vehicle is deleted
/// and all other right things happen when the Vehicle goes away.
void MultiVehicleManager::_deleteVehiclePhase1(Vehicle* vehicle)
{
    qCDebug(MultiVehicleManagerLog) << "_deleteVehiclePhase1" << vehicle;

    _vehiclesBeingDeleted << vehicle;

    // Remove from map
    bool found = false;
    for (int i=0; i<_vehicles.count(); i++) {
        if (_vehicles[i] == vehicle) {
            _vehicles.removeAt(i);
            found = true;
            break;
        }
    }
    if (!found) {
        qWarning() << "Vehicle not found in map!";
    }

    _vehicleId_vehicles.remove(WeaponVehicle_ID(vehicle->weaponId(), vehicle->id()));

    // First we must signal that a vehicle is no longer available.
    emit activeVehicleAvailableChanged(false);
    emit parameterReadyVehicleAvailableChanged(false);
    emit vehicleRemoved(vehicle);
    vehicle->prepareDelete();

#if defined (__ios__) || defined(__android__)
    if(_vehicles.count() == 0) {
        //-- Once no vehicles are connected, we no longer need to keep screen from going off
        qCDebug(MultiVehicleManagerLog) << "QAndroidJniObject::restoreScreenOn";
        MobileScreenMgr::setKeepScreenOn(false);
    }
#endif

    // We must let the above signals flow through the system as well as get back to the main loop event queue
    // before we can actually delete the Vehicle. The reason is that Qml may be holding on to references to it.
    // Even though the above signals should unload any Qml which has references, that Qml will not be destroyed
    // until we get back to the main loop. So we set a short timer which will then fire after Qt has finished
    // doing all of its internal nastiness to clean up the Qml. This works for both the normal running case
    // as well as the unit testing case which of course has a different signal flow!
    QTimer::singleShot(20, this, &MultiVehicleManager::_deleteVehiclePhase2);
}

void MultiVehicleManager::_deleteVehiclePhase2(void)
{
    qCDebug(MultiVehicleManagerLog) << "_deleteVehiclePhase2" << _vehiclesBeingDeleted[0];

    /// Qml has been notified of vehicle about to go away and should be disconnected from it by now.
    /// This means we can now clear the active vehicle property and delete the Vehicle for real.

    // lysd // Vehicle* newActiveVehicle = nullptr;
    // lysd // if (_vehicles.count()) {
    // lysd //     newActiveVehicle = qobject_cast<Vehicle*>(_vehicles[0]);
    // lysd // }




    delete _vehiclesBeingDeleted[0];
    _vehiclesBeingDeleted.removeAt(0);
}


// lysd // void MultiVehicleManager::_setActiveVehiclePhase2(void)
// lysd // {
// lysd //     qCDebug(MultiVehicleManagerLog) << "_setActiveVehiclePhase2 _vehicleBeingSetActive" << _vehicleBeingSetActive;
// lysd //
// lysd //     //-- Keep track of current vehicle's coordinates
// lysd //
// lysd //     if(_vehicleBeingSetActive) {
// lysd //         connect(_vehicleBeingSetActive, &Vehicle::coordinateChanged, this, &MultiVehicleManager::_coordinateChanged);
// lysd //     }
// lysd //
// lysd //
// lysd // }

void MultiVehicleManager::_coordinateChanged(QGeoCoordinate coordinate)
{
    _lastKnownLocation = coordinate;
    emit lastKnownLocationChanged();
}

void MultiVehicleManager::_vehicleParametersReadyChanged(bool parametersReady)
{
    auto* paramMgr = qobject_cast<ParameterManager*>(sender());

    if (!paramMgr) {
        qWarning() << "Dynamic cast failed!";
        return;
    }

}

void MultiVehicleManager::saveSetting(const QString &name, const QString& value)
{
    QSettings settings("../data/conf/MultiVehicleManager.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.setValue(name, value);
}

QString MultiVehicleManager::loadSetting(const QString &name, const QString& defaultValue)
{
    QSettings settings("../data/conf/MultiVehicleManager.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    return settings.value(name, defaultValue).toString();
}

Vehicle* MultiVehicleManager::getVehicleById(int vehicleId)
{
    for (int i=0; i< _vehicles.count(); i++) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(_vehicles[i]);
        if (vehicle->id() == vehicleId) {
            return vehicle;
        }
    }

    return nullptr;
}

Vehicle *MultiVehicleManager::getVehicleByWeaponIdAndId(unsigned long long weaponId, int vehicleId)
{
    for (int i=0; i< _vehicles.count(); i++) {
        Vehicle* vehicle = qobject_cast<Vehicle*>(_vehicles[i]);
        if (vehicle->weaponId() == weaponId && vehicle->id() == vehicleId) {
            return vehicle;
        }
    }

    return nullptr;
}

void MultiVehicleManager::addVehicles(LinkInterface *link, unsigned long long weaponId, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType)
{
    _vehicleHeartbeatInfo(link, weaponId, vehicleId, componentId, vehicleFirmwareType, vehicleType);
}

void MultiVehicleManager::removeVehicles(unsigned long long weaponId, int vehicleId)
{
    Vehicle * vehicle = getVehicleByWeaponIdAndId(weaponId, vehicleId);
    if(vehicle){
        _deleteVehiclePhase1(vehicle);
    }
}

void MultiVehicleManager::setGcsHeartbeatEnabled(bool gcsHeartBeatEnabled)
{
    if (gcsHeartBeatEnabled != _gcsHeartbeatEnabled) {
        _gcsHeartbeatEnabled = gcsHeartBeatEnabled;
        emit gcsHeartBeatEnabledChanged(gcsHeartBeatEnabled);

        QSettings settings("../data/conf/MultiVehicleManager.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.setValue(_gcsHeartbeatEnabledKey, gcsHeartBeatEnabled);

        if (gcsHeartBeatEnabled) {
            _gcsHeartbeatTimer.start();
        } else {
            _gcsHeartbeatTimer.stop();
        }
    }
}

void MultiVehicleManager::_sendGCSHeartbeat(void)
{
    // Send a heartbeat out on each link
    for (int i=0; i<_vehicles.count(); i++) {
        LinkInterface* link = _vehicles[i]->link();
        auto linkConfiguration = link->linkConfiguration();
        if (link->isConnected() && linkConfiguration && !linkConfiguration->isHighLatency()) {
            mavlink_message_t message;
            mavlink_msg_heartbeat_pack_chan(_mavlinkProtocol->getSystemId(),
                                            _mavlinkProtocol->getComponentId(),
                                            _vehicles[i]->mavlinkChannel(),
                                            &message,
                                            MAV_TYPE_GCS,            // MAV_TYPE
                                            MAV_AUTOPILOT_INVALID,   // MAV_AUTOPILOT
                                            MAV_MODE_MANUAL_ARMED,   // MAV_MODE
                                            0,                       // custom mode
                                            MAV_STATE_ACTIVE);       // MAV_STATE

            uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
            int len = mavlink_msg_to_send_buffer(buffer, &message);
            link->writeBytesThreadSafe(_vehicles[i]->weaponId() , (const char*)buffer, len);
        }
    }
}
