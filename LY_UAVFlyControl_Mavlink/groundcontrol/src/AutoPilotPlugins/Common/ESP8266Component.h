#ifndef ESP8266COMPONENT_H
#define ESP8266COMPONENT_H

#include "VehicleComponent.h"

class ESP8266Component : public VehicleComponent
{
    Q_OBJECT
public:
    ESP8266Component            (Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent = nullptr);

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

private:
    const QString   _name;
    QVariantList    _summaryItems;
};

#endif // ESP8266COMPONENT_H
