#ifndef UAVPLANESTATESDIALOG_H
#define UAVPLANESTATESDIALOG_H

#include <QWidget>
#include "DataManage/IUAVRealTimeStateManageService.h"

namespace Ui
{
class UAVPlaneStatesDialog;
}

class UAVPlaneStatesDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlaneStatesDialog(QWidget *parent = nullptr);
    ~UAVPlaneStatesDialog();

    void initData();
    void updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo);

protected:

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVPlaneStatesDialog *ui;
    bool                  m_show;
};

#endif
