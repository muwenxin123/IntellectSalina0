#ifndef PX4PARAMETERMETADATA_H
#define PX4PARAMETERMETADATA_H

#include <QObject>

#include "AutoPilotPlugin.h"
#include "Vehicle.h"

Q_DECLARE_LOGGING_CATEGORY(PX4ParameterMetaDataLog)

//#define GENERATE_PARAMETER_JSON

/// Loads and holds parameter fact meta data for PX4 stack
class PX4ParameterMetaData : public QObject
{
    Q_OBJECT

public:
    PX4ParameterMetaData(void);

    void            loadParameterFactMetaDataFile   (const QString& metaDataFile);
    FactMetaData*   getMetaDataForFact              (const QString& name, MAV_TYPE vehicleType, FactMetaData::ValueType_t type);

    static void getParameterMetaDataVersionInfo(const QString& metaDataFile, int& majorVersion, int& minorVersion);

private:
    enum {
        XmlStateNone,
        XmlStateFoundParameters,
        XmlStateFoundVersion,
        XmlStateFoundGroup,
        XmlStateFoundParameter,
        XmlStateDone
    };

    QVariant _stringToTypedVariant(const QString& string, FactMetaData::ValueType_t type, bool* convertOk);
    static void _outputFileWarning(const QString& metaDataFile, const QString& error1, const QString& error2);

#ifdef GENERATE_PARAMETER_JSON
    void _generateParameterJson();
#endif

    bool                                _parameterMetaDataLoaded        = false;    ///< true: parameter meta data already loaded
    FactMetaData::NameToMetaDataMap_t   _mapParameterName2FactMetaData;             ///< Maps from a parameter name to FactMetaData
};

#endif // PX4PARAMETERMETADATA_H
