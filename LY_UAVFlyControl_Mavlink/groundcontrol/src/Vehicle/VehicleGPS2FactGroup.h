#ifndef VEHICLEGPS2FACTGROUP_H
#define VEHICLEGPS2FACTGROUP_H

#include "VehicleGPSFactGroup.h"
#include "QGCMAVLink.h"

class VehicleGPS2FactGroup : public VehicleGPSFactGroup
{
    Q_OBJECT

public:
    VehicleGPS2FactGroup(QObject* parent = nullptr);

    // Overrides from VehicleGPSFactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

private:
    void _handleGps2Raw(mavlink_message_t& message);
};


#endif // VEHICLEGPS2FACTGROUP_H
