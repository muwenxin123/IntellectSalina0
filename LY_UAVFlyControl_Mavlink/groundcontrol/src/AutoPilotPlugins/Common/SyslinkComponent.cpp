#include "SyslinkComponent.h"
#include "AutoPilotPlugin.h"

SyslinkComponent::SyslinkComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent)
    : VehicleComponent(vehicle, autopilot, parent)
    , _name(tr("Syslink"))
{

}

QString SyslinkComponent::name(void) const
{
    return _name;
}

QString SyslinkComponent::description(void) const
{
    return tr("The Syslink Component is used to setup the radio connection on Crazyflies.");
}

QString SyslinkComponent::iconResource(void) const
{
    return "/qmlimages/wifi.svg";
}

bool SyslinkComponent::requiresSetup(void) const
{
    return false;
}

bool SyslinkComponent::setupComplete(void) const
{
    return true;
}

QStringList SyslinkComponent::setupCompleteChangedTriggerList(void) const
{
    return QStringList();
}

QUrl SyslinkComponent::setupSource(void) const
{
    return QUrl::fromUserInput("qrc:/qml/SyslinkComponent.qml");
}

QUrl SyslinkComponent::summaryQmlSource(void) const
{
    return QUrl();
}

QString SyslinkComponent::prerequisiteSetup(void) const
{
    return QString();
}
