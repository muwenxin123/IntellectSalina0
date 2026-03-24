#ifndef UAVFLYMANAGE_MAVLINKMESSAGEINDICATORDIALOG_H
#define UAVFLYMANAGE_MAVLINKMESSAGEINDICATORDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkMessageIndicatorDialog;
}

class UAVFlyManage_MavlinkMessageIndicatorDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkMessageIndicatorDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkMessageIndicatorDialog();

    void showAppMessage(const QString& message, const QString& title = QString());
    void showAppnewFormattedMessage(const int severity, const QString& message);
    void showRebootAppMessage(const QString& message, const QString& title = QString());
    void showCriticalVehicleMessage(const QString& message);

private:
    void appendAppMessage(const QString& message);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButton_UAVFlyManage_MavlinkMessageIndicator_Clear_clicked();

    void on_pushButton_UAVFlyManage_MavlinkMessageIndicator_Save_clicked();

private:
    Ui::UAVFlyManage_MavlinkMessageIndicatorDialog *ui;
};

#endif
