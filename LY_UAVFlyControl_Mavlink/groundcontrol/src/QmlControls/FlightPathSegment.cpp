#include "FlightPathSegment.h"

#include "QGC.h"

QGC_LOGGING_CATEGORY(FlightPathSegmentLog, "FlightPathSegmentLog")

FlightPathSegment::FlightPathSegment(SegmentType segmentType, const QGeoCoordinate& coord1, double amslCoord1Alt, const QGeoCoordinate& coord2, double amslCoord2Alt, bool queryTerrainData, QObject* parent)
    : QObject           (parent)
    , _coord1           (coord1)
    , _coord2           (coord2)
    , _coord1AMSLAlt     (amslCoord1Alt)
    , _coord2AMSLAlt     (amslCoord2Alt)
    , _queryTerrainData (queryTerrainData)
    , _segmentType      (segmentType)
{
    _delayedTerrainPathQueryTimer.setSingleShot(true);
    _delayedTerrainPathQueryTimer.setInterval(200);
    _delayedTerrainPathQueryTimer.callOnTimeout(this, &FlightPathSegment::_sendTerrainPathQuery);
    _updateTotalDistance();

    qCDebug(FlightPathSegmentLog) << this << "new" << coord1 << coord2 << amslCoord1Alt << amslCoord2Alt << _totalDistance;

    _sendTerrainPathQuery();
}

void FlightPathSegment::setCoordinate1(const QGeoCoordinate &coordinate)
{
    if (_coord1 != coordinate) {
        _coord1 = coordinate;
        emit coordinate1Changed(_coord1);
        _delayedTerrainPathQueryTimer.start();
        _updateTotalDistance();
    }
}

void FlightPathSegment::setCoordinate2(const QGeoCoordinate &coordinate)
{
    if (_coord2 != coordinate) {
        _coord2 = coordinate;
        emit coordinate2Changed(_coord2);
        _delayedTerrainPathQueryTimer.start();
        _updateTotalDistance();
    }
}

void FlightPathSegment::setCoord1AMSLAlt(double alt)
{
    if (!QGC::fuzzyCompare(alt, _coord1AMSLAlt)) {
        _coord1AMSLAlt = alt;
        emit coord1AMSLAltChanged();
        _updateTerrainCollision();
    }
}

void FlightPathSegment::setCoord2AMSLAlt(double alt)
{
    if (!QGC::fuzzyCompare(alt, _coord2AMSLAlt)) {
        _coord2AMSLAlt = alt;
        emit coord2AMSLAltChanged();
        _updateTerrainCollision();
    }
}

void FlightPathSegment::setSpecialVisual(bool specialVisual)
{
    if (_specialVisual != specialVisual) {
        _specialVisual = specialVisual;
        emit specialVisualChanged(specialVisual);
    }
}

void FlightPathSegment::_sendTerrainPathQuery(void)
{
    // lysd // if (_queryTerrainData && _coord1.isValid() && _coord2.isValid()) {
    // lysd //     qCDebug(FlightPathSegmentLog) << this << "_sendTerrainPathQuery";
    // lysd //     // Clear any previous query
    // lysd //     if (_currentTerrainPathQuery) {
    // lysd //         // We are already waiting on another query. We don't care about those results any more.
    // lysd //         disconnect(_currentTerrainPathQuery, &TerrainPathQuery::terrainDataReceived, this, &FlightPathSegment::_terrainDataReceived);
    // lysd //         _currentTerrainPathQuery = nullptr;
    // lysd //     }

    // lysd //     // Clear old terrain data
    // lysd //     _amslTerrainHeights.clear();
    // lysd //     _distanceBetween = 0;
    // lysd //     _finalDistanceBetween = 0;
    // lysd //     emit distanceBetweenChanged(0);
    // lysd //     emit finalDistanceBetweenChanged(0);
    // lysd //     emit amslTerrainHeightsChanged();

    // lysd //     _currentTerrainPathQuery = new TerrainPathQuery(true /* autoDelete */);
    // lysd //     connect(_currentTerrainPathQuery, &TerrainPathQuery::terrainDataReceived, this, &FlightPathSegment::_terrainDataReceived);
    // lysd //     _currentTerrainPathQuery->requestData(_coord1, _coord2);
    // lysd // }
}

// lysd // void FlightPathSegment::_terrainDataReceived(bool success, const TerrainPathQuery::PathHeightInfo_t& pathHeightInfo)
// lysd // {
// lysd //     qCDebug(FlightPathSegmentLog) << this << "_terrainDataReceived" << success << pathHeightInfo.heights.count();
// lysd //     if (success) {
// lysd //         if (!QGC::fuzzyCompare(pathHeightInfo.distanceBetween, _distanceBetween)) {
// lysd //             _distanceBetween = pathHeightInfo.distanceBetween;
// lysd //             emit distanceBetweenChanged(_distanceBetween);
// lysd //         }
// lysd //         if (!QGC::fuzzyCompare(pathHeightInfo.finalDistanceBetween, _finalDistanceBetween)) {
// lysd //             _finalDistanceBetween = pathHeightInfo.finalDistanceBetween;
// lysd //             emit finalDistanceBetweenChanged(_finalDistanceBetween);
// lysd //         }
// lysd //
// lysd //         _amslTerrainHeights.clear();
// lysd //         for (const double& amslTerrainHeight: pathHeightInfo.heights) {
// lysd //             _amslTerrainHeights.append(amslTerrainHeight);
// lysd //         }
// lysd //         emit amslTerrainHeightsChanged();
// lysd //     }
// lysd //
// lysd //     _currentTerrainPathQuery->deleteLater();
// lysd //     _currentTerrainPathQuery = nullptr;
// lysd //
// lysd //     _updateTerrainCollision();
// lysd // }

void FlightPathSegment::_updateTotalDistance(void)
{
    double newTotalDistance = 0;
    if (_coord1.isValid() && _coord2.isValid()) {
        newTotalDistance = _coord1.distanceTo(_coord2);
    }

    if (!QGC::fuzzyCompare(newTotalDistance, _totalDistance)) {
        _totalDistance = newTotalDistance;
        emit totalDistanceChanged(_totalDistance);
    }
}

void FlightPathSegment::_updateTerrainCollision(void)
{
    bool newTerrainCollision = false;

    if (_segmentType != SegmentTypeTerrainFrame) {
        double slope =      (_coord2AMSLAlt - _coord1AMSLAlt) / _totalDistance;
        double yIntercept = _coord1AMSLAlt;

        double x = 0;
        for (int i=0; i<_amslTerrainHeights.count(); i++) {
            bool ignoreCollision = false;
            if (_segmentType == SegmentTypeTakeoff && x < _collisionIgnoreMeters) {
                ignoreCollision = true;
            } else if (_segmentType == SegmentTypeLand && x > _totalDistance - _collisionIgnoreMeters) {
                ignoreCollision = true;
            }

            if (!ignoreCollision) {
                double y = _amslTerrainHeights[i].value<double>();
                if (y > (slope * x) + yIntercept) {
                    newTerrainCollision = true;
                    break;
                }
            }

            if (i == _amslTerrainHeights.count() - 2) {
                x += _finalDistanceBetween;
            } else {
                x += _distanceBetween;
            }
        }
    }

    qCDebug(FlightPathSegmentLog) << this << "_updateTerrainCollision new:old" << newTerrainCollision << _terrainCollision;

    if (newTerrainCollision != _terrainCollision) {
        _terrainCollision = newTerrainCollision;
        emit terrainCollisionChanged(_terrainCollision);
    }
}
