#include "FactPanelController.h"
#include "MultiVehicleManager.h"
#include "UAS.h"
#include "QGCApplication.h"
// lysd // #include "ParameterManager.h"

// lysd // #include <QQmlEngine>

/// @file
///     @author Don Gagne <don@thegagnes.com>

QGC_LOGGING_CATEGORY(FactPanelControllerLog, "FactPanelControllerLog")

FactPanelController::FactPanelController()
{
    // lysd // _vehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
    // lysd // if (_vehicle) {
    // lysd //     _uas = _vehicle->uas();
    // lysd //     _autopilot = _vehicle->autopilotPlugin();
    // lysd // } else {
    // lysd //     _vehicle = qgcApp()->toolbox()->multiVehicleManager()->offlineEditingVehicle();
    // lysd // }

    // lysd // _missingParametersTimer.setInterval(500);
    // lysd // _missingParametersTimer.setSingleShot(true);
    // lysd // connect(&_missingParametersTimer, &QTimer::timeout, this, &FactPanelController::_checkForMissingParameters);
}

void FactPanelController::_reportMissingParameter(int componentId, const QString& name)
{
    // lysd // if (componentId == FactSystem::defaultComponentId) {
    // lysd //     componentId = _vehicle->defaultComponentId();
    // lysd // }

    // lysd // qgcApp()->reportMissingParameter(componentId, name);
    // lysd // qCWarning(FactPanelControllerLog) << "Missing parameter:" << QString("%1:%2").arg(componentId).arg(name);
}

bool FactPanelController::_allParametersExists(int componentId, QStringList names)
{
    bool noMissingFacts = true;

    // lysd // foreach (const QString &name, names) {
    // lysd //     if (_vehicle && !_vehicle->parameterManager()->parameterExists(componentId, name)) {
    // lysd //         _reportMissingParameter(componentId, name);
    // lysd //         noMissingFacts = false;
    // lysd //     }
    // lysd // }

    return noMissingFacts;
}


Fact* FactPanelController::getParameterFact(int componentId, const QString& name, bool reportMissing)
{
    // lysd // if (_vehicle && _vehicle->parameterManager()->parameterExists(componentId, name)) {
    // lysd //     Fact* fact = _vehicle->parameterManager()->getParameter(componentId, name);
    // lysd //     QQmlEngine::setObjectOwnership(fact, QQmlEngine::CppOwnership);
    // lysd //     return fact;
    // lysd // } else {
    // lysd //     if (reportMissing) {
    // lysd //         _reportMissingParameter(componentId, name);
    // lysd //     }
    // lysd //     return nullptr;
    // lysd // }
    return nullptr;
}

bool FactPanelController::parameterExists(int componentId, const QString& name)
{
    // lysd // return _vehicle ? _vehicle->parameterManager()->parameterExists(componentId, name) : false;
    return false;
}

void FactPanelController::getMissingParameters(QStringList rgNames)
{
    // lysd // for (const QString& name: rgNames) {
    // lysd //     _missingParameterWaitList.append(name);
    // lysd //     _vehicle->parameterManager()->refreshParameter(MAV_COMP_ID_AUTOPILOT1, name);
    // lysd // }

    // lysd // _missingParametersTimer.start();
}

void FactPanelController::_checkForMissingParameters(void)
{
    QStringList waitList = _missingParameterWaitList;
    // lysd // for (const QString& name: waitList) {
    // lysd //     if (_vehicle->parameterManager()->parameterExists(MAV_COMP_ID_AUTOPILOT1, name)) {
    // lysd //         _missingParameterWaitList.removeOne(name);
    // lysd //     }
    // lysd // }

    // lysd // if (_missingParameterWaitList.isEmpty()) {
    // lysd //     emit missingParametersAvailable();
    // lysd // } else {
    // lysd //     _missingParametersTimer.start();
    // lysd // }
}
