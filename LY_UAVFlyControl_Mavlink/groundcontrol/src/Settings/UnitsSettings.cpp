#include "UnitsSettings.h"

uint UnitsSettings::_horizontalDistanceUnits = HorizontalDistanceUnitsMeters;
uint UnitsSettings::_verticalDistanceUnits = VerticalDistanceUnitsMeters;
uint UnitsSettings::_areaUnits = AreaUnitsSquareMeters;
uint UnitsSettings::_speedUnits = SpeedUnitsMetersPerSecond;
uint UnitsSettings::_temperatureUnits = TemperatureUnitsFarenheit;
uint UnitsSettings::_weightUnits = WeightUnitsKg;

DECLARE_SETTINGGROUP(Units, "Units")
{

}

uint UnitsSettings::weightUnits()
{
    return _weightUnits;
}

uint UnitsSettings::temperatureUnits()
{
    return _temperatureUnits;
}

uint UnitsSettings::speedUnits()
{
    return _speedUnits;
}

uint UnitsSettings::areaUnits()
{
    return _areaUnits;
}

uint UnitsSettings::verticalDistanceUnits()
{
    return _verticalDistanceUnits;
}

uint UnitsSettings::horizontalDistanceUnits()
{
    return _horizontalDistanceUnits;
}

// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, horizontalDistanceUnits)
// lysd // {
// lysd //     if (!_horizontalDistanceUnitsFact) {
// lysd //         // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "Feet" << "Meters";
// lysd //         enumValues << QVariant::fromValue(static_cast<uint32_t>(HorizontalDistanceUnitsFeet))
// lysd //                    << QVariant::fromValue(static_cast<uint32_t>(HorizontalDistanceUnitsMeters));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(horizontalDistanceUnitsName);
// lysd //         metaData->setShortDescription("Distance units");
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //
// lysd //         HorizontalDistanceUnits defaultHorizontalDistanceUnit = HorizontalDistanceUnitsMeters;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem: {
// lysd //                 defaultHorizontalDistanceUnit = HorizontalDistanceUnitsMeters;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //             case QLocale::ImperialUKSystem:
// lysd //                 defaultHorizontalDistanceUnit = HorizontalDistanceUnitsFeet;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultHorizontalDistanceUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _horizontalDistanceUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _horizontalDistanceUnitsFact;
// lysd // }
// lysd //
// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, verticalDistanceUnits)
// lysd // {
// lysd //     if (!_verticalDistanceUnitsFact) {
// lysd //         // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "Feet" << "Meters";
// lysd //         enumValues << QVariant::fromValue(static_cast<uint32_t>(VerticalDistanceUnitsFeet))
// lysd //                    << QVariant::fromValue(static_cast<uint32_t>(VerticalDistanceUnitsMeters));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(verticalDistanceUnitsName);
// lysd //         metaData->setShortDescription("Altitude units");
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //         VerticalDistanceUnits defaultVerticalAltitudeUnit = VerticalDistanceUnitsMeters;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem: {
// lysd //                 defaultVerticalAltitudeUnit = VerticalDistanceUnitsMeters;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //             case QLocale::ImperialUKSystem:
// lysd //                 defaultVerticalAltitudeUnit = VerticalDistanceUnitsFeet;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultVerticalAltitudeUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _verticalDistanceUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _verticalDistanceUnitsFact;
// lysd // }
// lysd //
// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, areaUnits)
// lysd // {
// lysd //     if (!_areaUnitsFact) {
// lysd //         // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "SquareFeet" << "SquareMeters" << "SquareKilometers" << "Hectares" << "Acres" << "SquareMiles";
// lysd //         enumValues <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareFeet)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareMeters)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareKilometers)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsHectares)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsAcres)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareMiles));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(areaUnitsName);
// lysd //         metaData->setShortDescription("Area units");
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //
// lysd //         AreaUnits defaultAreaUnit = AreaUnitsSquareMeters;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem: {
// lysd //                 defaultAreaUnit = AreaUnitsSquareMeters;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //             case QLocale::ImperialUKSystem:
// lysd //                 defaultAreaUnit = AreaUnitsSquareMiles;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultAreaUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _areaUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _areaUnitsFact;
// lysd // }
// lysd //
// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, speedUnits)
// lysd // {
// lysd //     if (!_speedUnitsFact) {
// lysd //         // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "Feet/second" << "Meters/second" << "Miles/hour" << "Kilometers/hour" << "Knots";
// lysd //         enumValues <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsFeetPerSecond)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsMetersPerSecond)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsMilesPerHour)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsKilometersPerHour)) <<
// lysd //             QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsKnots));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(speedUnitsName);
// lysd //         metaData->setShortDescription("Speed units");
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //
// lysd //         SpeedUnits defaultSpeedUnit = SpeedUnitsMetersPerSecond;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem: {
// lysd //                 defaultSpeedUnit = SpeedUnitsMetersPerSecond;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //             case QLocale::ImperialUKSystem:
// lysd //                 defaultSpeedUnit = SpeedUnitsMilesPerHour;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultSpeedUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _speedUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _speedUnitsFact;
// lysd // }
// lysd //
// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, temperatureUnits)
// lysd // {
// lysd //     if (!_temperatureUnitsFact) {
// lysd //         // Units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "Celsius" << "Farenheit";
// lysd //         enumValues << QVariant::fromValue(static_cast<uint32_t>(TemperatureUnitsCelsius)) << QVariant::fromValue(static_cast<uint32_t>(TemperatureUnitsFarenheit));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(temperatureUnitsName);
// lysd //         metaData->setShortDescription("Temperature units");
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //
// lysd //         TemperatureUnits defaultTemperatureUnit = TemperatureUnitsCelsius;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem: {
// lysd //                 defaultTemperatureUnit = TemperatureUnitsCelsius;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //             case QLocale::ImperialUKSystem:
// lysd //                 defaultTemperatureUnit = TemperatureUnitsFarenheit;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultTemperatureUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _temperatureUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _temperatureUnitsFact;
// lysd // }
// lysd //
// lysd // DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, weightUnits)
// lysd // {
// lysd //     if (!_weightUnitsFact) {
// lysd //         // Units settings can't be loaded from json since it creates an infinite loop of meta data loading.
// lysd //         QStringList     enumStrings;
// lysd //         QVariantList    enumValues;
// lysd //         enumStrings << "Grams" << "Kilograms" << "Ounces" << "Pounds";
// lysd //         enumValues
// lysd //             << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsGrams))
// lysd //             << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsKg))
// lysd //             << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsOz))
// lysd //             << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsLbs));
// lysd //         FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
// lysd //         metaData->setName(weightUnitsName);
// lysd //         metaData->setShortDescription(UnitsSettings::tr("Weight units"));
// lysd //         metaData->setEnumInfo(enumStrings, enumValues);
// lysd //         WeightUnits defaultWeightUnit = WeightUnitsGrams;
// lysd //         switch(QLocale::system().measurementSystem()) {
// lysd //             case QLocale::MetricSystem:
// lysd //             case QLocale::ImperialUKSystem: {
// lysd //                 defaultWeightUnit = WeightUnitsGrams;
// lysd //             } break;
// lysd //             case QLocale::ImperialUSSystem:
// lysd //                 defaultWeightUnit = WeightUnitsOz;
// lysd //                 break;
// lysd //         }
// lysd //         metaData->setRawDefaultValue(defaultWeightUnit);
// lysd //         metaData->setQGCRebootRequired(true);
// lysd //         _weightUnitsFact = new SettingsFact(_settingsGroup, metaData, this);
// lysd //     }
// lysd //     return _weightUnitsFact;
// lysd // }


