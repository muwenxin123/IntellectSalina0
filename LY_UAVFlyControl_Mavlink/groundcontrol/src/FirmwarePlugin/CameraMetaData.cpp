#include "CameraMetaData.h"

CameraMetaData::CameraMetaData(const QString&   canonicalName,
                               const QString&   brand,
                               const QString&   model,
                               double           sensorWidth,
                               double           sensorHeight,
                               double           imageWidth,
                               double           imageHeight,
                               double           focalLength,
                               bool             landscape,
                               bool             fixedOrientation,
                               double           minTriggerInterval,
                               const QString&   deprecatedTranslatedName,
                               QObject*         parent)
    : QObject                   (parent)
    , canonicalName             (canonicalName)
    , brand                     (brand)
    , model                     (model)
    , sensorWidth               (sensorWidth)
    , sensorHeight              (sensorHeight)
    , imageWidth                (imageWidth)
    , imageHeight               (imageHeight)
    , focalLength               (focalLength)
    , landscape                 (landscape)
    , fixedOrientation          (fixedOrientation)
    , minTriggerInterval        (minTriggerInterval)
    , deprecatedTranslatedName  (deprecatedTranslatedName)
{

}
