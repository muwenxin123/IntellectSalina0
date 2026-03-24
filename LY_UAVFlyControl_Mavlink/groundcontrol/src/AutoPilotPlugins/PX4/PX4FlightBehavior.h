#ifndef PX4FLIGHTBEHAVIOR_H
#define PX4FLIGHTBEHAVIOR_H

#include "VehicleComponent.h"

class PX4FlightBehavior : public VehicleComponent
{
    Q_OBJECT

public:
    PX4FlightBehavior(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent = nullptr);

    // Virtuals from VehicleComponent
    QStringList setupCompleteChangedTriggerList() const final;

    // Virtuals from VehicleComponent
    QString name() const final;
    QString description() const final;
    QString iconResource() const final;
    bool requiresSetup() const final;
    bool setupComplete() const final;
    QUrl setupSource() const final;
    QUrl summaryQmlSource() const final;
    bool allowSetupWhileArmed() const final { return true; }
    bool allowSetupWhileFlying() const final { return true; }

private:
    const QString   _name;
};

#endif // PX4FLIGHTBEHAVIOR_H
