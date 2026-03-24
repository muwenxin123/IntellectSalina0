#ifndef COMPINFOEVENTS_H
#define COMPINFOEVENTS_H

#include "CompInfo.h"

#include <QObject>

class FactMetaData;
class Vehicle;
class FirmwarePlugin;

class CompInfoEvents : public CompInfo
{
    Q_OBJECT

public:
    CompInfoEvents(uint8_t compId, Vehicle* vehicle, QObject* parent = nullptr);

    // Overrides from CompInfo
    void setJson(const QString& metadataJsonFileName, const QString& translationJsonFileName) override;

private:
};


#endif // COMPINFOEVENTS_H
