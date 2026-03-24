#include "GenericAutoPilotPlugin.h"

#include <QDebug>

GenericAutoPilotPlugin::GenericAutoPilotPlugin(Vehicle* vehicle, QObject* parent) :
    AutoPilotPlugin(vehicle, parent)
{
    if (!vehicle) {
        qWarning() << "Internal error";
    }
}

const QVariantList& GenericAutoPilotPlugin::vehicleComponents(void)
{
    static QVariantList emptyList;

    return emptyList;
}

QString GenericAutoPilotPlugin:: prerequisiteSetup(VehicleComponent* component) const
{
    Q_UNUSED(component);
    return QString();
}
