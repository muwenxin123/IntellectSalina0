#include "APMMavlinkStreamRateSettings.h"
#include "QGCApplication.h"

// lysd // #include <QQmlEngine>
// lysd // #include <QtQml>

DECLARE_SETTINGGROUP(APMMavlinkStreamRate, "APMMavlinkStreamRate")
{
    // lysd // qmlRegisterUncreatableType<APMMavlinkStreamRateSettings>("QGroundControl.SettingsManager", 1, 0, "APMMavlinkStreamRateSettings", "Reference only");

    connect(streamRateRawSensors(),     &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateRawSensors);
    connect(streamRateExtendedStatus(), &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateExtendedStatus);
    connect(streamRateRCChannels(),     &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateRCChannels);
    connect(streamRatePosition(),       &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRatePosition);
    connect(streamRateExtra1(),         &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateExtra1);
    connect(streamRateExtra2(),         &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateExtra2);
    connect(streamRateExtra3(),         &Fact::rawValueChanged, this, &APMMavlinkStreamRateSettings::_updateStreamRateExtra3);
}

DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateRawSensors)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateExtendedStatus)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateRCChannels)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRatePosition)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateExtra1)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateExtra2)
DECLARE_SETTINGSFACT(APMMavlinkStreamRateSettings, streamRateExtra3)

void APMMavlinkStreamRateSettings::_updateStreamRateWorker(MAV_DATA_STREAM mavStream, QVariant rateVar)
{
    // lysd // Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
    // lysd //
    // lysd // if (activeVehicle) {
    // lysd //     int streamRate = rateVar.toInt();
    // lysd //     if (streamRate >= 0) {
    // lysd //         activeVehicle->requestDataStream(mavStream, static_cast<uint16_t>(streamRate));
    // lysd //     }
    // lysd // }
}

void APMMavlinkStreamRateSettings::_updateStreamRateRawSensors(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_RAW_SENSORS, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRateExtendedStatus(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_EXTENDED_STATUS, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRateRCChannels(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_RC_CHANNELS, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRatePosition(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_POSITION, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRateExtra1(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_EXTRA1, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRateExtra2(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_EXTRA2, value);
}

void APMMavlinkStreamRateSettings::_updateStreamRateExtra3(QVariant value)
{
    _updateStreamRateWorker(MAV_DATA_STREAM_EXTRA3, value);
}
