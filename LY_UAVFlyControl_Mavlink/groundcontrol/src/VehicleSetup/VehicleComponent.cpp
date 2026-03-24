#include "VehicleComponent.h"

#include "VehicleComponent.h"
#include "AutoPilotPlugin.h"
#include "ParameterManager.h"

VehicleComponent::VehicleComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent) :
    QObject(parent),
    _vehicle(vehicle),
    _autopilot(autopilot)
{
    if (!vehicle || !autopilot) {
        qWarning() << "Internal error";
    }
}

VehicleComponent::~VehicleComponent()
{

}

// lysd // void VehicleComponent::addSummaryQmlComponent(QQmlContext* context, QQuickItem* parent)
// lysd // {
// lysd //     if (context) {
// lysd //         // FIXME: We own this object now, need to delete somewhere
// lysd //         QQmlComponent component(context->engine(), QUrl::fromUserInput("qrc:/qml/VehicleComponentSummaryButton.qml"));
// lysd //         if (component.status() == QQmlComponent::Error) {
// lysd //             qWarning() << component.errors();
// lysd //         } else {
// lysd //             QQuickItem* item = qobject_cast<QQuickItem*>(component.create(context));
// lysd //             if (item) {
// lysd //                 item->setParentItem(parent);
// lysd //                 item->setProperty("vehicleComponent", QVariant::fromValue(this));
// lysd //             } else {
// lysd //                 qWarning() << "Internal error";
// lysd //             }
// lysd //         }
// lysd //     } else {
// lysd //         qWarning() << "Internal error";
// lysd //     }
// lysd // }

void VehicleComponent::setupTriggerSignals(void)
{
    // Watch for changed on trigger list params
    for (const QString &paramName: setupCompleteChangedTriggerList()) {
        if (_vehicle->parameterManager()->parameterExists(FactSystem::defaultComponentId, paramName)) {
            Fact* fact = _vehicle->parameterManager()->getParameter(FactSystem::defaultComponentId, paramName);
            connect(fact, &Fact::valueChanged, this, &VehicleComponent::_triggerUpdated);
        }
    }
}

void VehicleComponent::_triggerUpdated(QVariant /*value*/)
{
    emit setupCompleteChanged(setupComplete());
}
