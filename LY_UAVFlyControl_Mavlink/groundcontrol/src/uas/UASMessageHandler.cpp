#include "UASMessageHandler.h"

#include "QGCApplication.h"
#include "UASMessageHandler.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"

UASMessage::UASMessage(int componentid, int severity, QString text)
{
    _compId   = componentid;
    _severity = severity;
    _text     = text;
}

bool UASMessage::severityIsError() const
{
    switch (_severity) {
        case MAV_SEVERITY_EMERGENCY:
        case MAV_SEVERITY_ALERT:
        case MAV_SEVERITY_CRITICAL:
        case MAV_SEVERITY_ERROR:
            return true;
        default:
            return false;
    }
}

UASMessageHandler::UASMessageHandler(QGCApplication* app)
    : QObject(app)
    // lysd // , _activeVehicle(nullptr)
    , _activeComponent(-1)
    , _multiComp(false)
    , _errorCount(0)
    , _errorCountTotal(0)
    , _warningCount(0)
    , _normalCount(0)
    , _showErrorsInToolbar(false)
    , _multiVehicleManager(nullptr)
{
    setToolbox();
}

UASMessageHandler::~UASMessageHandler()
{
    clearMessages();
}

void UASMessageHandler::setToolbox()
{

   _multiVehicleManager = QGCApplication::multiVehicleManager();

   // lysd // connect(_multiVehicleManager, &MultiVehicleManager::activeVehicleChanged, this, &UASMessageHandler::_activeVehicleChanged);
   emit textMessageReceived(nullptr);
   emit textMessageCountChanged(0);
}

void UASMessageHandler::clearMessages()
{
    _mutex.lock();
    // lysd // while(_messages.count()) {
    // lysd //     delete _messages.last();
    // lysd //     _messages.pop_back();
    // lysd // }
    _errorCount   = 0;
    _warningCount = 0;
    _normalCount  = 0;
    _mutex.unlock();
    emit textMessageCountChanged(0);
}

void UASMessageHandler::_activeVehicleChanged(Vehicle* vehicle)
{
    // lysd // // If we were already attached to an autopilot, disconnect it.
    // lysd // if (_activeVehicle) {
    // lysd //     disconnect(_activeVehicle, &Vehicle::textMessageReceived, this, &UASMessageHandler::handleTextMessage);
    // lysd //     _activeVehicle = nullptr;
    // lysd //     clearMessages();
    // lysd //     emit textMessageReceived(nullptr);
    // lysd // }

    // lysd // // And now if there's an autopilot to follow, set up the UI.
    // lysd // if (vehicle) {
    // lysd //     // Connect to the new UAS.
    // lysd //     clearMessages();
    // lysd //     _activeVehicle = vehicle;
    // lysd //     connect(_activeVehicle, &Vehicle::textMessageReceived, this, &UASMessageHandler::handleTextMessage);
    // lysd // }
}

void UASMessageHandler::handleTextMessage(int, int compId, int severity, QString text)
{
    // Hack to prevent calibration messages from cluttering things up
    // lysd // if (_activeVehicle->px4Firmware() && text.startsWith(QStringLiteral("[cal] "))) {
    // lysd //     return;
    // lysd // }

    // Color the output depending on the message severity. We have 3 distinct cases:
    // 1: If we have an ERROR or worse, make it bigger, bolder, and highlight it red.
    // 2: If we have a warning or notice, just make it bold and color it orange.
    // 3: Otherwise color it the standard color, white.

    _mutex.lock();

    if (_activeComponent < 0) {
        _activeComponent = compId;
    }

    if (compId != _activeComponent) {
        _multiComp = true;
    }

    // So first determine the styling based on the severity.
    QString style;
    switch (severity)
    {
    case MAV_SEVERITY_EMERGENCY:
    case MAV_SEVERITY_ALERT:
    case MAV_SEVERITY_CRITICAL:
    case MAV_SEVERITY_ERROR:
        style = QString("<#E>");
        _errorCount++;
        _errorCountTotal++;
        break;
    case MAV_SEVERITY_NOTICE:
    case MAV_SEVERITY_WARNING:
        style = QString("<#I>");
        _warningCount++;
        break;
    default:
        style = QString("<#N>");
        _normalCount++;
        break;
    }

    // And determine the text for the severitie
    QString severityText;
    switch (severity)
    {
    case MAV_SEVERITY_EMERGENCY:
        severityText = tr(" EMERGENCY:");
        break;
    case MAV_SEVERITY_ALERT:
        severityText = tr(" ALERT:");
        break;
    case MAV_SEVERITY_CRITICAL:
        severityText = tr(" Critical:");
        break;
    case MAV_SEVERITY_ERROR:
        severityText = tr(" Error:");
        break;
    case MAV_SEVERITY_WARNING:
        severityText = tr(" Warning:");
        break;
    case MAV_SEVERITY_NOTICE:
        severityText = tr(" Notice:");
        break;
    case MAV_SEVERITY_INFO:
        severityText = tr(" Info:");
        break;
    case MAV_SEVERITY_DEBUG:
        severityText = tr(" Debug:");
        break;
    default:
        break;
    }

    // Finally preppend the properly-styled text with a timestamp.
    QString dateString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    UASMessage* message = new UASMessage(compId, severity, text);
    QString compString;
    if (_multiComp) {
        compString = QString(" COMP:%1").arg(compId);
    }
    message->_setFormatedText(QString("<font style=\"%1\">[%2%3]%4 %5</font><br/>").arg(style).arg(dateString).arg(compString).arg(severityText).arg(text.toHtmlEscaped()));

    if (message->severityIsError()) {
        _latestError = severityText + " " + text;
    }

    _mutex.unlock();

    emit textMessageReceived(message);

    // lysd // _messages.append(message);
    // lysd // int count = _messages.count();
    // lysd // emit textMessageCountChanged(count);

    if (_showErrorsInToolbar && message->severityIsError()) {
        QGCApplication::showCriticalVehicleMessage(message->getText().toHtmlEscaped());
    }else{
        QGCApplication::showCriticalVehicleMessage(message->getText().toHtmlEscaped());
    }
    qDebug() << __FILE__ << __LINE__;

    delete message;
}

void UASMessageHandler::handleTextMessage(const int , const int compId, const int severity, const QString &text, const QString& title)
{
    // Hack to prevent calibration messages from cluttering things up
    // lysd // if (_activeVehicle->px4Firmware() && text.startsWith(QStringLiteral("[cal] "))) {
    // lysd //     return;
    // lysd // }

    // Color the output depending on the message severity. We have 3 distinct cases:
    // 1: If we have an ERROR or worse, make it bigger, bolder, and highlight it red.
    // 2: If we have a warning or notice, just make it bold and color it orange.
    // 3: Otherwise color it the standard color, white.

    // lysd // _mutex.lock();

    // lysd // if (_activeComponent < 0) {
    // lysd //     _activeComponent = compId;
    // lysd // }
    // lysd //
    // lysd // if (compId != _activeComponent) {
    // lysd //     _multiComp = true;
    // lysd // }

    // So first determine the styling based on the severity.
    QString style;
    QColor  textColor;
    switch (severity)
    {
    case MAV_SEVERITY_EMERGENCY:
    case MAV_SEVERITY_ALERT:
    case MAV_SEVERITY_CRITICAL:
    case MAV_SEVERITY_ERROR:
        style = QString("<#E>");
        textColor = QGCApplication::qgcPalette()->warningText();
        _errorCount++;
        _errorCountTotal++;
        break;
    case MAV_SEVERITY_NOTICE:
    case MAV_SEVERITY_WARNING:
        style = QString("<#I>");
        textColor = QGCApplication::qgcPalette()->warningText();
        _warningCount++;
        break;
    default:
        style = QString("<#N>");
        textColor = QGCApplication::qgcPalette()->text();
        _normalCount++;
        break;
    }

    // And determine the text for the severitie
    QString severityText;
    switch (severity)
    {
    case MAV_SEVERITY_EMERGENCY:
        severityText = tr(" EMERGENCY:");
        break;
    case MAV_SEVERITY_ALERT:
        severityText = tr(" ALERT:");
        break;
    case MAV_SEVERITY_CRITICAL:
        severityText = tr(" Critical:");
        break;
    case MAV_SEVERITY_ERROR:
        severityText = tr(" Error:");
        break;
    case MAV_SEVERITY_WARNING:
        severityText = tr(" Warning:");
        break;
    case MAV_SEVERITY_NOTICE:
        severityText = tr(" Notice:");
        break;
    case MAV_SEVERITY_INFO:
        severityText = tr(" Info:");
        break;
    case MAV_SEVERITY_DEBUG:
        severityText = tr(" Debug:");
        break;
    default:
        break;
    }

    // Finally preppend the properly-styled text with a timestamp.
    // lysd //QString dateString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    // lysd // UASMessage* message = new UASMessage(compId, severity, text);
    // lysd //QString compString;
    // lysd // if (_multiComp) {
        // lysd //compString = QString(" COMP:%1").arg(compId);
    // lysd // }
    // lysd // message->_setFormatedText(QString("<font style=\"%1\">[%2%3]%4 %5</font><br/>").arg(style).arg(dateString).arg(compString).arg(severityText).arg(text.toHtmlEscaped()));

    // lysd // if (message->severityIsError()) {
       // lysd // _latestError = severityText + " " + text;
    // lysd // }

    // lysd // _mutex.unlock();

    // lysd // emit textMessageReceived(message);

    // lysd // _messages.append(message);
    // lysd // int count = _messages.count();
    // lysd // emit textMessageCountChanged(count);

    // lysd // if (_showErrorsInToolbar && message->severityIsError()) {
    // lysd //     QGCApplication::showCriticalVehicleMessage(message->getText().toHtmlEscaped());
    // lysd // }else{
    // lysd //     QGCApplication::showCriticalVehicleMessage(message->getText().toHtmlEscaped());
    // lysd // }

    // lysd // delete message;


    qDebug() << __FILE__ << __LINE__;

    QString compString = QString(" COMP:%1").arg(compId);

    QGCApplication::showAppnewFormattedMessage(severity, QString("[%1%2]%3 %4").arg(title).arg(compString).arg(severityText).arg(text));
}

int UASMessageHandler::getErrorCountTotal() {
    _mutex.lock();
    int c = _errorCountTotal;
    _mutex.unlock();
    return c;
}

int UASMessageHandler::getErrorCount() {
    _mutex.lock();
    int c = _errorCount;
    _errorCount = 0;
    _mutex.unlock();
    return c;
}

int UASMessageHandler::getWarningCount() {
    _mutex.lock();
    int c = _warningCount;
    _warningCount = 0;
    _mutex.unlock();
    return c;
}

int UASMessageHandler::getNormalCount() {
    _mutex.lock();
    int c = _normalCount;
    _normalCount = 0;
    _mutex.unlock();
    return c;
}
