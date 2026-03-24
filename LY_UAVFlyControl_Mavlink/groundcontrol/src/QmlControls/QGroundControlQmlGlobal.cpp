#include "QGroundControlQmlGlobal.h"

#include <QSettings>
#include <QLineF>
#include <QPointF>

static const char* kQmlGlobalKeyName = "QGCQml";

const char* QGroundControlQmlGlobal::_flightMapPositionSettingsGroup =          "FlightMapPosition";
const char* QGroundControlQmlGlobal::_flightMapPositionLatitudeSettingsKey =    "Latitude";
const char* QGroundControlQmlGlobal::_flightMapPositionLongitudeSettingsKey =   "Longitude";
const char* QGroundControlQmlGlobal::_flightMapZoomSettingsKey =                "FlightMapZoom";

QGeoCoordinate   QGroundControlQmlGlobal::_coord = QGeoCoordinate(0.0,0.0);
double           QGroundControlQmlGlobal::_zoom = 2;

QGroundControlQmlGlobal::QGroundControlQmlGlobal(QGCApplication* app)
    : QObject(app)
{

}

QGroundControlQmlGlobal::~QGroundControlQmlGlobal()
{
}

void QGroundControlQmlGlobal::setToolbox()
{

}
