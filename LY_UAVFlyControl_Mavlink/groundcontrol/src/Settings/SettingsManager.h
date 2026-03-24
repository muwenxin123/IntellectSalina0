#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>


#include "QGCLoggingCategory.h"

#if defined(QGC_AIRMAP_ENABLED)
#include "AirMapSettings.h"
#endif
#include <QVariantList>


class  QGCApplication;

class  AirMapSettings;

class  AppSettings;
class  UnitsSettings;
class  AutoConnectSettings;
class  VideoSettings;
class  FlightMapSettings;
class  RTKSettings;
class  FlyViewSettings;
class  PlanViewSettings;
class  BrandImageSettings;
class  OfflineMapsSettings;
class  FirmwareUpgradeSettings;
class  ADSBVehicleManagerSettings;

class  APMMavlinkStreamRateSettings;

/// Provides access to all app settings
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    SettingsManager(QGCApplication* app);

#if defined(QGC_AIRMAP_ENABLED)
    Q_PROPERTY(QObject* airMapSettings                  READ airMapSettings                 CONSTANT)
#endif
    // Q_PROPERTY(QObject* appSettings                     READ appSettings                    CONSTANT)
    // Q_PROPERTY(QObject* unitsSettings                   READ unitsSettings                  CONSTANT)
    // Q_PROPERTY(QObject* autoConnectSettings             READ autoConnectSettings            CONSTANT)
    // Q_PROPERTY(QObject* videoSettings                   READ videoSettings                  CONSTANT)
    // Q_PROPERTY(QObject* flightMapSettings               READ flightMapSettings              CONSTANT)
    // Q_PROPERTY(QObject* rtkSettings                     READ rtkSettings                    CONSTANT)
    // Q_PROPERTY(QObject* flyViewSettings                 READ flyViewSettings                CONSTANT)
    // Q_PROPERTY(QObject* planViewSettings                READ planViewSettings               CONSTANT)
    // Q_PROPERTY(QObject* brandImageSettings              READ brandImageSettings             CONSTANT)
    // Q_PROPERTY(QObject* offlineMapsSettings             READ offlineMapsSettings            CONSTANT)
    // Q_PROPERTY(QObject* firmwareUpgradeSettings         READ firmwareUpgradeSettings        CONSTANT)
    // Q_PROPERTY(QObject* adsbVehicleManagerSettings      READ adsbVehicleManagerSettings     CONSTANT)
#if !defined(NO_ARDUPILOT_DIALECT)
    // Q_PROPERTY(QObject* apmMavlinkStreamRateSettings    READ apmMavlinkStreamRateSettings   CONSTANT)
#endif
    // Override from QGCTool
    virtual void setToolbox();

#if defined(QGC_AIRMAP_ENABLED)
    AirMapSettings*         airMapSettings      (void) { return _airMapSettings; }
#endif
    AppSettings*                    appSettings                 (void) { return _appSettings; }
    UnitsSettings*                  unitsSettings               (void) { return _unitsSettings; }
    AutoConnectSettings*            autoConnectSettings         (void) { return _autoConnectSettings; }
    VideoSettings*                  videoSettings               (void) { return _videoSettings; }
    FlightMapSettings*              flightMapSettings           (void) { return _flightMapSettings; }
    RTKSettings*                    rtkSettings                 (void) { return _rtkSettings; }
    FlyViewSettings*                flyViewSettings             (void) { return _flyViewSettings; }
    PlanViewSettings*               planViewSettings            (void) { return _planViewSettings; }
    BrandImageSettings*             brandImageSettings          (void) { return _brandImageSettings; }
    OfflineMapsSettings*            offlineMapsSettings         (void) { return _offlineMapsSettings; }
    FirmwareUpgradeSettings*        firmwareUpgradeSettings     (void) { return _firmwareUpgradeSettings; }
    ADSBVehicleManagerSettings*     adsbVehicleManagerSettings  (void) { return _adsbVehicleManagerSettings; }
#if !defined(NO_ARDUPILOT_DIALECT)
    APMMavlinkStreamRateSettings*   apmMavlinkStreamRateSettings(void) { return _apmMavlinkStreamRateSettings; }
#endif
private:
#if defined(QGC_AIRMAP_ENABLED)
    AirMapSettings*         _airMapSettings;
#endif
    AppSettings*                    _appSettings;
    UnitsSettings*                  _unitsSettings;
    AutoConnectSettings*            _autoConnectSettings;
    VideoSettings*                  _videoSettings;
    FlightMapSettings*              _flightMapSettings;
    RTKSettings*                    _rtkSettings;
    FlyViewSettings*                _flyViewSettings;
    PlanViewSettings*               _planViewSettings;
    BrandImageSettings*             _brandImageSettings;
    OfflineMapsSettings*            _offlineMapsSettings;
    FirmwareUpgradeSettings*        _firmwareUpgradeSettings;
    ADSBVehicleManagerSettings*     _adsbVehicleManagerSettings;
#if !defined(NO_ARDUPILOT_DIALECT)
    APMMavlinkStreamRateSettings*   _apmMavlinkStreamRateSettings;
#endif
};


#endif // SETTINGSMANAGER_H
