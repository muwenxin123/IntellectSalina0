/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include <QElapsedTimer>
#include <cmath>

#include "FirmwarePlugin.h"
#include "MAVLinkProtocol.h"
#include "FollowMe.h"
#include "Vehicle.h"
#include "SettingsManager.h"

QGC_LOGGING_CATEGORY(FollowMeLog, "FollowMeLog")

FollowMe::FollowMe(QGCApplication* app)
    : QObject(app)
{
    _gcsMotionReportTimer.setSingleShot(false);

    setToolbox();
}

void FollowMe::setToolbox()
{
    connect(&_gcsMotionReportTimer,                                     &QTimer::timeout,       this, &FollowMe::_sendGCSMotionReport);
    // lysd // connect(toolbox->settingsManager()->appSettings()->followTarget(),  &Fact::rawValueChanged, this, &FollowMe::_settingsChanged);

    _settingsChanged();
}

void FollowMe::_settingsChanged()
{
    // lysd // _currentMode = _toolbox->settingsManager()->appSettings()->followTarget()->rawValue().toUInt();
    // lysd //
    // lysd // switch (_currentMode) {
    // lysd // case MODE_NEVER:
    // lysd //     disconnect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleAdded,     this, &FollowMe::_vehicleAdded);
    // lysd //     disconnect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleRemoved,   this, &FollowMe::_vehicleRemoved);
    // lysd //     _disableFollowSend();
    // lysd //     break;
    // lysd // case MODE_ALWAYS:
    // lysd //     connect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleAdded,    this, &FollowMe::_vehicleAdded);
    // lysd //     connect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleRemoved,  this, &FollowMe::_vehicleRemoved);
    // lysd //     _enableFollowSend();
    // lysd //     break;
    // lysd // case MODE_FOLLOWME:
    // lysd //     connect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleAdded,    this, &FollowMe::_vehicleAdded);
    // lysd //     connect(_toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleRemoved,  this, &FollowMe::_vehicleRemoved);
    // lysd //     _enableIfVehicleInFollow();
    // lysd //     break;
    // lysd // }
}

void FollowMe::_enableFollowSend()
{
    if (!_gcsMotionReportTimer.isActive()) {
        // lysd // _gcsMotionReportTimer.setInterval(qMin(QGCApplication::updateInterval(), 250));
        _gcsMotionReportTimer.start();
    }
}

void FollowMe::_disableFollowSend()
{
    if (_gcsMotionReportTimer.isActive()) {
        _gcsMotionReportTimer.stop();
    }
}

void FollowMe::_sendGCSMotionReport()
{
    QGeoPositionInfo    geoPositionInfo =   QGCApplication::geoPositionInfo();
    QGeoCoordinate      gcsCoordinate =     geoPositionInfo.coordinate();

    if (!geoPositionInfo.isValid()) {
        return;
    }

    // First check to see if any vehicles need follow me updates
    bool needFollowMe = false;
    if (_currentMode == MODE_ALWAYS) {
        needFollowMe = true;
    } else if (_currentMode == MODE_FOLLOWME) {
        // lysd // QmlObjectListModel* vehicles = _toolbox->multiVehicleManager()->vehicles();
        // lysd // for (int i=0; i<vehicles->count(); i++) {
        // lysd //     Vehicle* vehicle = vehicles->value<Vehicle*>(i);
        // lysd //     if (_isFollowFlightMode(vehicle, vehicle->flightMode())) {
        // lysd //         needFollowMe = true;
        // lysd //     }
        // lysd // }
    }
    if (!needFollowMe) {
        return;
    }

    GCSMotionReport motionReport = {};
    uint8_t         estimatation_capabilities = 0;

    // get the current location coordinates

    motionReport.lat_int =          static_cast<int>(gcsCoordinate.latitude()  * 1e7);
    motionReport.lon_int =          static_cast<int>(gcsCoordinate.longitude() * 1e7);
    motionReport.altMetersAMSL =    gcsCoordinate.altitude();
    estimatation_capabilities |=    (1 << POS);

    if (geoPositionInfo.hasAttribute(QGeoPositionInfo::Direction) == true) {
        estimatation_capabilities |= (1 << HEADING);
        motionReport.headingDegrees = geoPositionInfo.attribute(QGeoPositionInfo::Direction);
    }

    // get the current eph and epv

    if (geoPositionInfo.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)) {
        motionReport.pos_std_dev[0] = motionReport.pos_std_dev[1] = geoPositionInfo.attribute(QGeoPositionInfo::HorizontalAccuracy);
    }

    if (geoPositionInfo.hasAttribute(QGeoPositionInfo::VerticalAccuracy)) {
        motionReport.pos_std_dev[2] = geoPositionInfo.attribute(QGeoPositionInfo::VerticalAccuracy);
    }

    // calculate z velocity if it's available

    if (geoPositionInfo.hasAttribute(QGeoPositionInfo::VerticalSpeed)) {
        motionReport.vzMetersPerSec = geoPositionInfo.attribute(QGeoPositionInfo::VerticalSpeed);
    }

    // calculate x,y velocity if it's available

    if (geoPositionInfo.hasAttribute(QGeoPositionInfo::Direction) && geoPositionInfo.hasAttribute(QGeoPositionInfo::GroundSpeed) == true) {
        estimatation_capabilities |= (1 << VEL);

        qreal direction = _degreesToRadian(geoPositionInfo.attribute(QGeoPositionInfo::Direction));
        qreal velocity  = geoPositionInfo.attribute(QGeoPositionInfo::GroundSpeed);

        motionReport.vxMetersPerSec = cos(direction)*velocity;
        motionReport.vyMetersPerSec = sin(direction)*velocity;
    } else {
        motionReport.vxMetersPerSec = 0;
        motionReport.vyMetersPerSec = 0;
    }

    // lysd //  QmlObjectListModel* vehicles = _toolbox->multiVehicleManager()->vehicles();

    // lysd // for (int i=0; i<vehicles->count(); i++) {
    // lysd //     Vehicle* vehicle = vehicles->value<Vehicle*>(i);
    // lysd //     if (_currentMode == MODE_ALWAYS || _isFollowFlightMode(vehicle, vehicle->flightMode())) {
    // lysd //         qCDebug(FollowMeLog) << "sendGCSMotionReport latInt:lonInt:altMetersAMSL" << motionReport.lat_int << motionReport.lon_int << motionReport.altMetersAMSL;
    // lysd //         vehicle->firmwarePlugin()->sendGCSMotionReport(vehicle, motionReport, estimatation_capabilities);
    // lysd //     }
    // lysd // }
}

double FollowMe::_degreesToRadian(double deg)
{
    return deg * M_PI / 180.0;
}

void FollowMe::_vehicleAdded(Vehicle* vehicle)
{
    connect(vehicle, &Vehicle::flightModeChanged, this, &FollowMe::_enableIfVehicleInFollow);
    _enableIfVehicleInFollow();
}

void FollowMe::_vehicleRemoved(Vehicle* vehicle)
{
    disconnect(vehicle, &Vehicle::flightModeChanged, this, &FollowMe::_enableIfVehicleInFollow);
    _enableIfVehicleInFollow();
}

void FollowMe::_enableIfVehicleInFollow(void)
{
    if (_currentMode == MODE_ALWAYS) {
        // System always enabled
        return;
    }

    // Any vehicle in follow mode will enable the system
    // lysd // QmlObjectListModel* vehicles = _toolbox->multiVehicleManager()->vehicles();
    // lysd //
    // lysd // for (int i=0; i< vehicles->count(); i++) {
    // lysd //     Vehicle* vehicle = vehicles->value<Vehicle*>(i);
    // lysd //     if (_isFollowFlightMode(vehicle, vehicle->flightMode())) {
    // lysd //         _enableFollowSend();
    // lysd //         return;
    // lysd //     }
    // lysd // }

    _disableFollowSend();
}

bool FollowMe::_isFollowFlightMode (Vehicle* vehicle, const QString& flightMode)
{
    return flightMode.compare(vehicle->followFlightMode()) == 0;
}
