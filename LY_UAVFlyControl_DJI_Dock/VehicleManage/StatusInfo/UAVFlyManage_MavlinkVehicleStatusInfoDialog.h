#ifndef UAVFLYMANAGE_MAVLINKVEHICLESTATUSINFODIALOG_H
#define UAVFLYMANAGE_MAVLINKVEHICLESTATUSINFODIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkVehicleStatusInfoDialog;
}

class Vehicle;

class UAVFlyManage_MavlinkVehicleStatusInfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkVehicleStatusInfoDialog(QWidget *parent = nullptr, Vehicle *vehicle = nullptr, int vehicleId = -1);
    ~UAVFlyManage_MavlinkVehicleStatusInfoDialog() override;

    Vehicle *getVehicle() const;

    QString getVehicleInfo() const;

    bool getSelected() const;
    void setSelected(bool Selected);

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    void MavlinkVehicleConnect();
    void MavlinkVehicleDisconnect();

private:
    Ui::UAVFlyManage_MavlinkVehicleStatusInfoDialog   *ui;
    bool                                               m_Show;

    Vehicle*                                           m_vehicle;
    int                                                m_vehicleId;
    bool                                               m_refreshParameter;
    bool                                               m_Selected;
};

#endif
