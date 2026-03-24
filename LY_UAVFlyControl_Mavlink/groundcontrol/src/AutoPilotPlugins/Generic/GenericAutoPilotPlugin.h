#ifndef GENERICAUTOPILOTPLUGIN_H
#define GENERICAUTOPILOTPLUGIN_H

#include <QObject>

#include "AutoPilotPlugin.h"

class GenericAutoPilotPlugin : public AutoPilotPlugin
{
    Q_OBJECT
public:
    GenericAutoPilotPlugin(Vehicle* vehicle, QObject* parent = nullptr);

    // Overrides from AutoPilotPlugin
    const QVariantList& vehicleComponents(void) final;
    QString prerequisiteSetup(VehicleComponent* component) const final;
};

#endif // GENERICAUTOPILOTPLUGIN_H
