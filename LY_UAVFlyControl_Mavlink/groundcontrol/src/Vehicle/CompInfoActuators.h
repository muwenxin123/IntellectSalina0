#ifndef COMPINFOACTUATORS_H
#define COMPINFOACTUATORS_H

#include "CompInfo.h"

#include <QObject>

class FactMetaData;
class Vehicle;
class FirmwarePlugin;

class CompInfoActuators : public CompInfo
{
    Q_OBJECT

public:
    CompInfoActuators(uint8_t compId, Vehicle* vehicle, QObject* parent = nullptr);

    // Overrides from CompInfo
    void setJson(const QString& metadataJsonFileName, const QString& translationJsonFileName) override;

private:
};


#endif // COMPINFOACTUATORS_H
