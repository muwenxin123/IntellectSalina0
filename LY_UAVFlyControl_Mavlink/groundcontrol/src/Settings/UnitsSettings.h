#ifndef UNITSSETTINGS_H
#define UNITSSETTINGS_H

#include <QObject>

#include "SettingsGroup.h"

class UnitsSettings : public SettingsGroup
{
    Q_OBJECT

public:
    UnitsSettings(QObject* parent = nullptr);

    enum HorizontalDistanceUnits {
        HorizontalDistanceUnitsFeet = 0,
        HorizontalDistanceUnitsMeters
    };

    enum VerticalDistanceUnits {
        VerticalDistanceUnitsFeet = 0,
        VerticalDistanceUnitsMeters
    };

    enum AreaUnits {
        AreaUnitsSquareFeet = 0,
        AreaUnitsSquareMeters,
        AreaUnitsSquareKilometers,
        AreaUnitsHectares,
        AreaUnitsAcres,
        AreaUnitsSquareMiles,
    };

    enum SpeedUnits {
        SpeedUnitsFeetPerSecond = 0,
        SpeedUnitsMetersPerSecond,
        SpeedUnitsMilesPerHour,
        SpeedUnitsKilometersPerHour,
        SpeedUnitsKnots,
    };

    enum TemperatureUnits {
        TemperatureUnitsCelsius = 0,
        TemperatureUnitsFarenheit,
    };

    enum WeightUnits {
        WeightUnitsGrams = 0,
        WeightUnitsKg,
        WeightUnitsOz,
        WeightUnitsLbs
    };

    Q_ENUM(HorizontalDistanceUnits)
    Q_ENUM(VerticalDistanceUnits)
    Q_ENUM(AreaUnits)
    Q_ENUM(SpeedUnits)
    Q_ENUM(TemperatureUnits)
    Q_ENUM(WeightUnits)

    DEFINE_SETTING_NAME_GROUP()

    static uint horizontalDistanceUnits();
    static uint verticalDistanceUnits();
    static uint areaUnits();
    static uint speedUnits();
    static uint temperatureUnits();
    static uint weightUnits();


    static uint _horizontalDistanceUnits;
    static uint _verticalDistanceUnits;
    static uint _areaUnits;
    static uint _speedUnits;
    static uint _temperatureUnits;
    static uint _weightUnits;

    // lysd // DEFINE_SETTINGFACT(horizontalDistanceUnits)
    // lysd // DEFINE_SETTINGFACT(verticalDistanceUnits)
    // lysd // DEFINE_SETTINGFACT(areaUnits)
    // lysd // DEFINE_SETTINGFACT(speedUnits)
    // lysd // DEFINE_SETTINGFACT(temperatureUnits)
    // lysd // DEFINE_SETTINGFACT(weightUnits)
};

#endif // UNITSSETTINGS_H
