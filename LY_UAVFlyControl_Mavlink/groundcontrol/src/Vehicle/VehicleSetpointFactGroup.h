#ifndef VEHICLESETPOINTFACTGROUP_H
#define VEHICLESETPOINTFACTGROUP_H

#include "FactGroup.h"
#include "QGCMAVLink.h"

class VehicleSetpointFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleSetpointFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* roll       READ roll       CONSTANT)
    Q_PROPERTY(Fact* pitch      READ pitch      CONSTANT)
    Q_PROPERTY(Fact* yaw        READ yaw        CONSTANT)
    Q_PROPERTY(Fact* rollRate   READ rollRate   CONSTANT)
    Q_PROPERTY(Fact* pitchRate  READ pitchRate  CONSTANT)
    Q_PROPERTY(Fact* yawRate    READ yawRate    CONSTANT)

    Fact* roll      () { return &_rollFact; }
    Fact* pitch     () { return &_pitchFact; }
    Fact* yaw       () { return &_yawFact; }
    Fact* rollRate  () { return &_rollRateFact; }
    Fact* pitchRate () { return &_pitchRateFact; }
    Fact* yawRate   () { return &_yawRateFact; }

    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _rollFactName;
    static const char* _pitchFactName;
    static const char* _yawFactName;
    static const char* _rollRateFactName;
    static const char* _pitchRateFactName;
    static const char* _yawRateFactName;

private:
    Fact _rollFact;
    Fact _pitchFact;
    Fact _yawFact;
    Fact _rollRateFact;
    Fact _pitchRateFact;
    Fact _yawRateFact;
};


#endif // VEHICLESETPOINTFACTGROUP_H
