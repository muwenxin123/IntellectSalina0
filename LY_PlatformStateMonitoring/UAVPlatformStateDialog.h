#ifndef UAVDIALOG_H
#define UAVDIALOG_H

#include <QWidget>

#include <Protocol/XygsJL/sharedFile.h>

namespace Ui
{
class UAVPlatformStateDialog;
}

class UAVPlatformStateDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlatformStateDialog(QWidget *parent = nullptr);
    ~UAVPlatformStateDialog();

    void updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation& telemetryInfo);
    void updateAttitude();

protected:

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVPlatformStateDialog *ui;
    bool                       m_show;
    QPalette                   m_PalNone;
    QPalette                   m_PalRed;
};

#endif
