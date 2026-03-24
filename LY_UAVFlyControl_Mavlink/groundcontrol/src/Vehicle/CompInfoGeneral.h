#ifndef COMPINFOGENERAL_H
#define COMPINFOGENERAL_H

#include "CompInfo.h"
#include "QGCMAVLink.h"
#include "QGCLoggingCategory.h"
#include "FactMetaData.h"

#include <QObject>
#include <QMap>

class FactMetaData;
class Vehicle;
class FirmwarePlugin;

Q_DECLARE_LOGGING_CATEGORY(CompInfoGeneralLog)

class CompInfoGeneral : public CompInfo
{
    Q_OBJECT

public:
    CompInfoGeneral(uint8_t compId, Vehicle* vehicle, QObject* parent = nullptr);

    bool isMetaDataTypeSupported(COMP_METADATA_TYPE type) { return _supportedTypes.contains(type); }

    void setUris(CompInfo& compInfo) const;

    // Overrides from CompInfo
    void setJson(const QString& metadataJsonFileName, const QString& translationJsonFileName) override;

private:
    QMap<COMP_METADATA_TYPE, Uris>   _supportedTypes;

    static const char*          _jsonMetadataTypesKey;
};

#endif // COMPINFOGENERAL_H
