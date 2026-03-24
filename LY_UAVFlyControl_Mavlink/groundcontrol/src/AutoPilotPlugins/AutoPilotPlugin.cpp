#include "QGCApplication.h"

#include "AutoPilotPlugin.h"
#include "FirmwarePlugin.h"

AutoPilotPlugin::AutoPilotPlugin(Vehicle* vehicle, QObject* parent)
    : QObject(parent)
    , _vehicle(vehicle)
    , _firmwarePlugin(vehicle->firmwarePlugin())
    , _setupComplete(false)
{

}

AutoPilotPlugin::~AutoPilotPlugin()
{

}

void AutoPilotPlugin::_recalcSetupComplete(void)
{
    bool newSetupComplete = true;

    for(const QVariant& componentVariant: vehicleComponents()) {
        VehicleComponent* component = qobject_cast<VehicleComponent*>(qvariant_cast<QObject *>(componentVariant));
        if (component) {
            if (!component->setupComplete()) {
                newSetupComplete = false;
                break;
            }
        } else {
            qWarning() << "AutoPilotPlugin::_recalcSetupComplete Incorrectly typed VehicleComponent";
        }
    }

    if (_setupComplete != newSetupComplete) {
        _setupComplete = newSetupComplete;
        emit setupCompleteChanged(_setupComplete);
    }
}

bool AutoPilotPlugin::setupComplete(void) const
{
    return _setupComplete;
}

void AutoPilotPlugin::parametersReadyPreChecks(void)
{
    _recalcSetupComplete();

    // Connect signals in order to keep setupComplete up to date
    for(QVariant componentVariant: vehicleComponents()) {
        VehicleComponent* component = qobject_cast<VehicleComponent*>(qvariant_cast<QObject *>(componentVariant));
        if (component) {
            connect(component, &VehicleComponent::setupCompleteChanged, this, &AutoPilotPlugin::_recalcSetupComplete);
        } else {
            qWarning() << "AutoPilotPlugin::_recalcSetupComplete Incorrectly typed VehicleComponent";
        }
    }

    if (!_setupComplete) {
        QGCApplication::showAppMessage(_vehicle->weaponTitle() + tr("One or more vehicle components require setup prior to flight."));

        // Take the user to Vehicle Summary
        QGCApplication::showSetupView();
        QGCApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}
