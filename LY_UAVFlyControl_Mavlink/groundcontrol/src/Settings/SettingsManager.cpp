#include "SettingsManager.h"

#include "QGCApplication.h"

#include "UnitsSettings.h"

SettingsManager::SettingsManager(QGCApplication* app)
    : QObject(app)
#if defined(QGC_AIRMAP_ENABLED)
    , _airMapSettings       (nullptr)
#endif
    , _appSettings                  (nullptr)
    , _unitsSettings                (nullptr)
    , _autoConnectSettings          (nullptr)
    , _videoSettings                (nullptr)
    , _flightMapSettings            (nullptr)
    , _rtkSettings                  (nullptr)
    , _flyViewSettings              (nullptr)
    , _planViewSettings             (nullptr)
    , _brandImageSettings           (nullptr)
    , _offlineMapsSettings          (nullptr)
    , _firmwareUpgradeSettings      (nullptr)
    , _adsbVehicleManagerSettings   (nullptr)
#if !defined(NO_ARDUPILOT_DIALECT)
    , _apmMavlinkStreamRateSettings (nullptr)
#endif
{
    setToolbox();
}

void SettingsManager::setToolbox()
{

#ifdef UNITSSETTINGS_H
    _unitsSettings =                new UnitsSettings               (this);        // Must be first since AppSettings references it
#endif

#ifdef APPSETTINGS_H
    _appSettings =                  new AppSettings                 (this);
#endif

#ifdef AUTOCONNECTSETTINGS_H
    _autoConnectSettings =          new AutoConnectSettings         (this);
#endif

#ifdef VIDEOSETTINGS_H
    _videoSettings =                new VideoSettings               (this);
#endif

#ifdef VIDEOSETTINGS_H
    _flightMapSettings =            new FlightMapSettings           (this);
#endif

#ifdef RTKSETTINGS_H
    _rtkSettings =                  new RTKSettings                 (this);
#endif

#ifdef FLYVIEWSETTINGS_H
    _flyViewSettings =              new FlyViewSettings             (this);
#endif

#ifdef PLANVIEWSETTINGS_H
    _planViewSettings =             new PlanViewSettings            (this);
#endif

#ifdef BRANDIMAGESETTINGS_H
    _brandImageSettings =           new BrandImageSettings          (this);
#endif

#ifdef OFFLINEMAPSSETTINGS_H
    _offlineMapsSettings =          new OfflineMapsSettings         (this);
#endif

#ifdef FIRMWAREUPGRADESETTINGS_H
    _firmwareUpgradeSettings =      new FirmwareUpgradeSettings     (this);
#endif

#ifdef ADSBVEHICLEMANAGERSETTINGS_H
    _adsbVehicleManagerSettings =   new ADSBVehicleManagerSettings  (this);
#endif

#if !defined(NO_ARDUPILOT_DIALECT)
#ifdef APMMAVLINKSTREAMRATESETTINGS_H
    _apmMavlinkStreamRateSettings = new APMMavlinkStreamRateSettings(this);
#endif
#endif
#if defined(QGC_AIRMAP_ENABLED)
    _airMapSettings =               new AirMapSettings          (this);
#endif
}

