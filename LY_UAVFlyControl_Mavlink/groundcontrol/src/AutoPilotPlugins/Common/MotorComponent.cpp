#include "MotorComponent.h"

MotorComponent::MotorComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent) :
    VehicleComponent(vehicle, autopilot, parent),
    _name(tr("Motors"))
{

}

QString MotorComponent::name(void) const
{
    return _name;
}

QString MotorComponent::description(void) const
{
    return tr("Motors Setup is used to manually test motor control and direction.");
}

QString MotorComponent::iconResource(void) const
{
    return QStringLiteral("/qmlimages/MotorComponentIcon.svg");
}

bool MotorComponent::requiresSetup(void) const
{
    return false;
}

bool MotorComponent::setupComplete(void) const
{
    return true;
}

QStringList MotorComponent::setupCompleteChangedTriggerList(void) const
{
    return QStringList();
}

QUrl MotorComponent::setupSource(void) const
{
    return QUrl::fromUserInput(QStringLiteral("qrc:/qml/MotorComponent.qml"));
}

QUrl MotorComponent::summaryQmlSource(void) const
{
    return QUrl();
}
