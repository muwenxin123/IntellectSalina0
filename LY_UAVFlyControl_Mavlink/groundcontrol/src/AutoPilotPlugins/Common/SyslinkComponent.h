#ifndef SYSLINKCOMPONENT_H
#define SYSLINKCOMPONENT_H

#include "VehicleComponent.h"

class SyslinkComponent : public VehicleComponent
{
    Q_OBJECT
public:
    SyslinkComponent            (Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent = nullptr);

    // Virtuals from VehicleComponent
    QStringList setupCompleteChangedTriggerList() const;

    // Virtuals from VehicleComponent
    QString name                () const;
    QString description         () const;
    QString iconResource        () const;
    bool    requiresSetup       () const;
    bool    setupComplete       () const;
    QUrl    setupSource         () const;
    QUrl    summaryQmlSource    () const;
    QString prerequisiteSetup   () const;

private:
    const QString   _name;
    QVariantList    _summaryItems;
};
#endif // SYSLINKCOMPONENT_H
