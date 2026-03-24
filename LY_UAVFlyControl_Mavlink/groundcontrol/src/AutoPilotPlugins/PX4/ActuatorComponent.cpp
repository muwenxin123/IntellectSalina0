#include "ActuatorComponent.h"

#include "QGCApplication.h"

static bool imageProviderAdded{false};

ActuatorComponent::ActuatorComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent) :
    VehicleComponent(vehicle, autopilot, parent),
    _name(tr("Actuators"))
  // lysd // , _actuators(*vehicle->actuators())
{
    // lysd // if (!imageProviderAdded) {
    // lysd //     qgcApp()->qmlAppEngine()->addImageProvider(QLatin1String("actuators"), GeometryImage::VehicleGeometryImageProvider::instance());
    // lysd //     imageProviderAdded = true;
    // lysd // }

    // lysd // connect(&_actuators, &Actuators::hasUnsetRequiredFunctionsChanged, this, [this]() { _triggerUpdated({}); });
}

QString ActuatorComponent::name(void) const
{
    return _name;
}

QString ActuatorComponent::description(void) const
{
    return "";
}

QString ActuatorComponent::iconResource(void) const
{
    return QStringLiteral("/qmlimages/MotorComponentIcon.svg");
}

bool ActuatorComponent::requiresSetup(void) const
{
    return true;
}

bool ActuatorComponent::setupComplete(void) const
{
    // lysd // return !_actuators.hasUnsetRequiredFunctions();
    return false;
}

QStringList ActuatorComponent::setupCompleteChangedTriggerList(void) const
{
    return QStringList();
}

QUrl ActuatorComponent::setupSource(void) const
{
    return QUrl::fromUserInput(QStringLiteral("qrc:/qml/ActuatorComponent.qml"));
}

QUrl ActuatorComponent::summaryQmlSource(void) const
{
    return QUrl();
}
