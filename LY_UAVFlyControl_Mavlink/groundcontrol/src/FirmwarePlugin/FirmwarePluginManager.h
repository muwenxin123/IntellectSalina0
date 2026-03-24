#ifndef FIRMWAREPLUGINMANAGER_H
#define FIRMWAREPLUGINMANAGER_H

#include "FirmwarePlugin.h"
#include "QGCMAVLink.h"


class QGCApplication;

/// FirmwarePluginManager is a singleton which is used to return the correct FirmwarePlugin for a MAV_AUTOPILOT type.

class FirmwarePluginManager : public QObject
{
    Q_OBJECT

public:
    FirmwarePluginManager(QGCApplication* app);
    ~FirmwarePluginManager();

    /// Returns list of firmwares which are supported by the system
    QList<QGCMAVLink::FirmwareClass_t> supportedFirmwareClasses(void);

    /// Returns the list of supported vehicle types for the specified firmware
    QList<QGCMAVLink::VehicleClass_t> supportedVehicleClasses(QGCMAVLink::FirmwareClass_t firmwareClass);

    /// Returns appropriate plugin for autopilot type.
    ///     @param firmwareType Type of firmwware to return plugin for.
    ///     @param vehicleType Vehicle type to return plugin for.
    /// @return Singleton FirmwarePlugin instance for the specified MAV_AUTOPILOT.
    FirmwarePlugin* firmwarePluginForAutopilot(MAV_AUTOPILOT firmwareType, MAV_TYPE vehicleType);

private:
    FirmwarePluginFactory* _findPluginFactory(QGCMAVLink::FirmwareClass_t firmwareClass);

    FirmwarePlugin*                     _genericFirmwarePlugin;
    QList<QGCMAVLink::FirmwareClass_t>  _supportedFirmwareClasses;
};


#endif // FIRMWAREPLUGINMANAGER_H
