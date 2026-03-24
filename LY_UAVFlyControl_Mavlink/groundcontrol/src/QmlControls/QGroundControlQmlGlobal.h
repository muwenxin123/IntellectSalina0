#ifndef QGROUNDCONTROLQMLGLOBAL_H
#define QGROUNDCONTROLQMLGLOBAL_H

#include <QObject>
#include "QGCApplication.h"
#include "SettingsFact.h"
// lysd // #include "SimulatedPosition.h"
#include "QGCLoggingCategory.h"
// lysd // #include "AppSettings.h"
// lysd // #include "AirspaceManager.h"
// lysd // #include "ADSBVehicleManager.h"
// lysd // #include "QGCPalette.h"
// lysd // #include "QmlUnitsConversion.h"
// lysd // #if defined(QGC_ENABLE_PAIRING)
// lysd // #include "PairingManager.h"
// lysd // #endif
// lysd // #if defined(QGC_GST_TAISYNC_ENABLED)
// lysd // #include "TaisyncManager.h"
// lysd // #else
// lysd // class TaisyncManager;
// lysd // #endif
// lysd // #if defined(QGC_GST_MICROHARD_ENABLED)
// lysd // #include "MicrohardManager.h"
// lysd // #else
// lysd // class MicrohardManager;
// lysd // #endif

// lysd // #ifdef QT_DEBUG
// lysd // #include "MockLink.h"
// lysd // #endif


class QGroundControlQmlGlobal : public QObject
{
    Q_OBJECT

public:
    QGroundControlQmlGlobal(QGCApplication* app);
    ~QGroundControlQmlGlobal() override;

    enum AltMode {
        AltitudeModeMixed,              // Used by global altitude mode for mission planning
        AltitudeModeRelative,           // MAV_FRAME_GLOBAL_RELATIVE_ALT
        AltitudeModeAbsolute,           // MAV_FRAME_GLOBAL
        AltitudeModeCalcAboveTerrain,   // Absolute altitude above terrain calculated from terrain data
        AltitudeModeTerrainFrame,       // MAV_FRAME_GLOBAL_TERRAIN_ALT
        AltitudeModeNone,               // Being used as distance value unrelated to ground (for example distance to structure)
    };
    Q_ENUM(AltMode)

    // Overrides from QGCTool
    virtual void setToolbox();

    static const char* _flightMapPositionSettingsGroup;
    static const char* _flightMapPositionLatitudeSettingsKey;
    static const char* _flightMapPositionLongitudeSettingsKey;
    static const char* _flightMapZoomSettingsKey;

    static QGeoCoordinate   _coord;
    static double           _zoom;

};


#endif // QGROUNDCONTROLQMLGLOBAL_H
