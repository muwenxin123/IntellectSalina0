#ifndef PX4AUTOPILOTPLUGIN_H
#define PX4AUTOPILOTPLUGIN_H

#include <QObject>
#include "AutoPilotPlugin.h"


class  PX4AirframeLoader;
class  AirframeComponent;
class  PX4RadioComponent;
class  ESP8266Component;
class  FlightModesComponent;
class  SensorsComponent;
class  SafetyComponent;
class  CameraComponent;
class  PowerComponent;
class  MotorComponent;
class  ActuatorComponent;
class  PX4TuningComponent;
class  PX4FlightBehavior;
class  SyslinkComponent;

class PX4AutoPilotPlugin : public AutoPilotPlugin
{
    Q_OBJECT

public:
    PX4AutoPilotPlugin(Vehicle* vehicle, QObject* parent);
    ~PX4AutoPilotPlugin();

    // Overrides from AutoPilotPlugin
    const QVariantList& vehicleComponents(void) override;
    void parametersReadyPreChecks(void) override;
    QString prerequisiteSetup(VehicleComponent* component) const override;

protected:
    bool                    _incorrectParameterVersion; ///< true: parameter version incorrect, setup not allowed
    PX4AirframeLoader*      _airframeFacts;
    AirframeComponent*      _airframeComponent;
    PX4RadioComponent*      _radioComponent;
    ESP8266Component*       _esp8266Component;
    FlightModesComponent*   _flightModesComponent;
    SensorsComponent*       _sensorsComponent;
    SafetyComponent*        _safetyComponent;
    CameraComponent*        _cameraComponent;
    PowerComponent*         _powerComponent;
    MotorComponent*         _motorComponent;
    ActuatorComponent*      _actuatorComponent;
    PX4TuningComponent*     _tuningComponent;
    PX4FlightBehavior*      _flightBehavior;
    SyslinkComponent*       _syslinkComponent;

private:
    QVariantList            _components;
};

#endif // PX4AUTOPILOTPLUGIN_H
