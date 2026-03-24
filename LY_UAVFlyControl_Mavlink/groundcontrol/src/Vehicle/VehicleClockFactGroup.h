#ifndef VEHICLECLOCKFACTGROUP_H
#define VEHICLECLOCKFACTGROUP_H

#include "FactGroup.h"
#include "QGCMAVLink.h"

class Vehicle;

class VehicleClockFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleClockFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* currentTime READ currentTime CONSTANT)
    Q_PROPERTY(Fact* currentUTCTime READ currentUTCTime CONSTANT)
    Q_PROPERTY(Fact* currentDate READ currentDate CONSTANT)

    Fact* currentTime () { return &_currentTimeFact; }
    Fact* currentUTCTime () { return &_currentUTCTimeFact; }
    Fact* currentDate () { return &_currentDateFact; }

    static const char* _currentTimeFactName;
    static const char* _currentUTCTimeFactName;
    static const char* _currentDateFactName;

    static const char* _settingsGroup;

private slots:
    void _updateAllValues() override;

private:
    Fact            _currentTimeFact;
    Fact            _currentUTCTimeFact;
    Fact            _currentDateFact;
};

#endif // VEHICLECLOCKFACTGROUP_H
