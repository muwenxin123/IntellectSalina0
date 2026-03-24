#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "VehicleComponent.h"

/// @file
///     @brief  The Camera VehicleComponent is used to setup the camera modes and hardware
///             configuration to use it.
///     @author Gus Grubba <gus@auterion.com>

class CameraComponent : public VehicleComponent
{
    Q_OBJECT

public:
    CameraComponent (Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent = nullptr);

    // Virtuals from VehicleComponent
    QStringList     setupCompleteChangedTriggerList         (void) const;

    // Virtuals from VehicleComponent
    QString         name                                    (void) const final;
    QString         description                             (void) const final;
    QString         iconResource                            (void) const final;
    bool            requiresSetup                           (void) const final;
    bool            setupComplete                           (void) const final;
    QUrl            setupSource                             (void) const final;
    QUrl            summaryQmlSource                        (void) const final;
    bool            allowSetupWhileArmed                    (void) const final { return false; }

private:
    const QString   _name;
    QVariantList    _summaryItems;
};
#endif // CAMERACOMPONENT_H
