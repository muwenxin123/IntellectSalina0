#ifndef UAVFLYMANAGEDIALOG_H
#define UAVFLYMANAGEDIALOG_H

#include <QWidget>

#include "DataManage/ISystemMaintenanceService.h"

#include "QGCApplication.h"

class UAVTakeOffBeforeCheckDialog;

namespace Ui {
class UAVFlyManage_MavlinkDialog;
}

class LyWidget;

class UAVFlyManage_MavlinkDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkDialog();

    void setVehicleSelectedDialog(QWidget* vehicleSelectedDialog);

    bool OnCommand(int nID, void *lParam);

    const UAVTakeOffBeforeCheckDialog *getPUAVTakeOffBeforeCheckDialog() const;

private slots:

    void BombControl_BombLock();
    void BombControl_BombUnLock();
    void BombControl_BombShot();

    void InteAgri_PesticideSpraying_Start();
    void InteAgri_PesticideSpraying_Stop();

    void Mavlink_FlyMode_Manual();
    void Mavlink_FlyMode_Stabilized();
    void Mavlink_FlyMode_Acro();
    void Mavlink_FlyMode_Rattitude();
    void Mavlink_FlyMode_Altitude();
    void Mavlink_FlyMode_Offboard();
    void Mavlink_FlyMode_Position();
    void Mavlink_FlyMode_Hold();
    void Mavlink_FlyMode_Mission();
    void Mavlink_FlyMode_Return();
    void Mavlink_FlyMode_FollowMe();
    void Mavlink_FlyMode_Guided();

    void Mavlink_CommOper_Arm();
    void Mavlink_CommOper_Disarm();
    void Mavlink_CommOper_TakeOff();
    void Mavlink_CommOper_Land();
    void Mavlink_CommOper_StartMisson();
    void Mavlink_CommOper_Pause();
    void Mavlink_CommOper_RTL();
    void Mavlink_CommOper_TakeControl();

    void Mavlink_AdvanceOper_Reboot();
    void Mavlink_AdvanceOper_EmergencyStop();

	void  Mavlink_horizontalSlider_valueChanged(int value);

	void Mavlink_NongYe_SetValue();

protected:

    virtual void changeEvent(QEvent *event);

private:
    void WeaponControl_Before(Vehicle *vehicle);
    void WeaponControl_After(Vehicle *vehicle);
    void SendFlyCmdFun(std::function<void (Vehicle* vehicle)>& vehicleFun);

private:
    Ui::UAVFlyManage_MavlinkDialog*		ui;
	int															m_nValue;
};

#endif
