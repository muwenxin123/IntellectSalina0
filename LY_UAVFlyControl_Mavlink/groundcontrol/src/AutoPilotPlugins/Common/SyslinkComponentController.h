#ifndef SYSLINKCOMPONENTCONTROLLER_H
#define SYSLINKCOMPONENTCONTROLLER_H

#include "FactPanelController.h"
#include "UASInterface.h"
#include "QGCLoggingCategory.h"
#include "AutoPilotPlugin.h"

Q_DECLARE_LOGGING_CATEGORY(SyslinkComponentControllerLog)

namespace Ui {
    class SyslinkComponentController;
}

class SyslinkComponentController : public FactPanelController
{
    Q_OBJECT

public:
    SyslinkComponentController      ();
    ~SyslinkComponentController     ();

    Q_PROPERTY(int              radioChannel    READ radioChannel       WRITE setRadioChannel       NOTIFY radioChannelChanged)
    Q_PROPERTY(QString          radioAddress    READ radioAddress       WRITE setRadioAddress       NOTIFY radioAddressChanged)
    Q_PROPERTY(int              radioRate       READ radioRate          WRITE setRadioRate          NOTIFY radioRateChanged)
    Q_PROPERTY(QStringList      radioRates      READ radioRates                                     CONSTANT)
    Q_PROPERTY(Vehicle*         vehicle         READ vehicle                                        CONSTANT)

    Q_INVOKABLE void resetDefaults();

    int             radioChannel    ();
    QString         radioAddress    ();
    int             radioRate       ();
    QStringList     radioRates      () { return _dataRates; }
    Vehicle*        vehicle         () { return _vehicle; }

    void        setRadioChannel     (int num);
    void        setRadioAddress     (QString str);
    void        setRadioRate        (int idx);


signals:
    void        radioChannelChanged     ();
    void        radioAddressChanged     ();
    void        radioRateChanged        ();

private slots:
    void        _channelChanged     (QVariant value);
    void        _addressChanged     (QVariant value);
    void        _rateChanged        (QVariant value);

private:
    QStringList _dataRates;

};
#endif // SYSLINKCOMPONENTCONTROLLER_H
