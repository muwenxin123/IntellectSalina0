#ifndef VEHICLEHYGROMETERFACTGROUP_H
#define VEHICLEHYGROMETERFACTGROUP_H

#include "FactGroup.h"
#include "QGCMAVLink.h"

class VehicleHygrometerFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleHygrometerFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* hygroID            READ hygroID            CONSTANT)
    Q_PROPERTY(Fact* hygroTemp          READ hygroTemp          CONSTANT)
    Q_PROPERTY(Fact* hygroHumi          READ hygroHumi          CONSTANT)

    Fact* hygroID                           () { return &_hygroIDFact; }
    Fact* hygroTemp                         () { return &_hygroTempFact; }
    Fact* hygroHumi                         () { return &_hygroHumiFact; }

    // Overrides from FactGroup
    virtual void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static const char* _hygroIDFactName;
    static const char* _hygroTempFactName;
    static const char* _hygroHumiFactName;

protected:
    void _handleHygrometerSensor        (mavlink_message_t& message);

    Fact _hygroTempFact;
    Fact _hygroHumiFact;
    Fact _hygroIDFact;
};


#endif // VEHICLEHYGROMETERFACTGROUP_H
