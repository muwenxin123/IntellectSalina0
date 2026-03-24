#ifndef COMPINFOPARAM_H
#define COMPINFOPARAM_H

#include "CompInfo.h"
#include "QGCMAVLink.h"
#include "QGCLoggingCategory.h"
#include "FactMetaData.h"

#include <QObject>

class FactMetaData;
class Vehicle;
class FirmwarePlugin;

Q_DECLARE_LOGGING_CATEGORY(CompInfoParamLog)

class CompInfoParam : public CompInfo
{
    Q_OBJECT

public:
    CompInfoParam(uint8_t compId, Vehicle* vehicle, QObject* parent = nullptr);

    FactMetaData* factMetaDataForName(const QString& name, FactMetaData::ValueType_t type);

    // Overrides from CompInfo
    void setJson(const QString& metadataJsonFileName, const QString& translationJsonFileName) override;

    static void _cachePX4MetaDataFile(const QString& metaDataFile);

private:
    QObject* _getOpaqueParameterMetaData(void);

    static FirmwarePlugin*  _anyVehicleTypeFirmwarePlugin   (MAV_AUTOPILOT firmwareType);
    static QString          _parameterMetaDataFile          (Vehicle* vehicle, MAV_AUTOPILOT firmwareType, int& majorVersion, int& minorVersion);

    typedef QPair<QString /* indexed name */, FactMetaData*> RegexFactMetaDataPair_t;

    bool                                _noJsonMetadata             = true;
    FactMetaData::NameToMetaDataMap_t   _nameToMetaDataMap;
    QList<RegexFactMetaDataPair_t>      _indexedNameMetaDataList;
    QObject*                            _opaqueParameterMetaData    = nullptr;

    static const char* _cachedMetaDataFilePrefix;
    static const char* _jsonParametersKey;
    static const char* _indexedNameTag;
};


#endif // COMPINFOPARAM_H
