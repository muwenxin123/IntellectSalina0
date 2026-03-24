#ifndef POWERCOMPONENTCONTROLLER_H
#define POWERCOMPONENTCONTROLLER_H

#include <QObject>
// lysd // #include <QQuickItem>

#include "UASInterface.h"
#include "FactPanelController.h"

/// Power Component MVC Controller for PowerComponent.qml.
class PowerComponentController : public FactPanelController
{
    Q_OBJECT

public:
    PowerComponentController(void);

    Q_INVOKABLE void calibrateEsc(void);
    Q_INVOKABLE void startBusConfigureActuators(void);
    Q_INVOKABLE void stopBusConfigureActuators(void);

signals:
    void oldFirmware(void);
    void newerFirmware(void);
    void incorrectFirmwareRevReporting(void);
    void connectBattery(void);
    void disconnectBattery(void);
    void batteryConnected(void);
    void calibrationFailed(const QString& errorMessage);
    void calibrationSuccess(const QStringList& warningMessages);

private slots:
    void _handleVehicleTextMessage(int vehicleId, int compId, int severity, QString text);

private:
    void _stopCalibration(void);
    void _stopBusConfig(void);

    QStringList _warningMessages;
    static const int _neededFirmwareRev = 1;
};

#endif // POWERCOMPONENTCONTROLLER_H
