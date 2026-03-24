#include "UAVFlyManage_MavlinkDialog.h"
#include "ui_UAVFlyManage_MavlinkDialog.h"

#include "UAVPushButton.h"
#include "UAVSelectDialog.h"

#include "LY_UAVFlyControl_Mavlink.h"
#include "IUAVFlyControl_MavlinkActivator.h"

#include "lyUI/LyWidget.h"
#include "lyUI/LyMessageBox.h"

#include "QGCApplication.h"
#include "Vehicle.h"

#define MAVLINK_PWM_MAX	1800
#define MAVLINK_PWM_MIN		1100

#define USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#endif

#define __DEBUG__LOGINFO__
#if defined(__DEBUG__LOGINFO__)
#include <QDebug>

#define logInfo(format, ...) qDebug("[File:" __FILE__ " ,Line:%d,Func:%s] " format, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define logInfo(format, ...)
#endif
#include "LY_UAVFlyControl_MavlinkInterface.h"
#include "QSlider"

UAVFlyManage_MavlinkDialog::UAVFlyManage_MavlinkDialog(QWidget *parent)
    : QWidget(parent)
      , ui(new Ui::UAVFlyManage_MavlinkDialog)
{
    ui->setupUi(this);

	//this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明

    ui->groupBox_UAVFlyControl_UAVFlyManage_BombControl->setVisible(false);
    ui->groupBox_UAVFlyControl_UAVFlyManage_BombControl->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombLock->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombLock->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombUnLock->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombUnLock->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot->hide();

    ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_FlyAction->setVisible(false);
    ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_FlyAction->hide();
    ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_Remote->setVisible(false);
    ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_Remote->hide();

    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Acro->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Acro->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Offboard->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Offboard->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Rattitude->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Rattitude->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Manual->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Manual->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Return->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Return->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Hold->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Hold->hide();

    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Pause->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Pause->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_StartMisson->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_StartMisson->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->hide();
    ui->doubleSpinBox_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->setVisible(false);
    ui->doubleSpinBox_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->hide();
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeControl->setVisible(false);
    ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeControl->hide();

	//ui->horizontalSlider->setRange(0,100);

    if(!QGCApplication::vehicleControlAdvanceOperEnabled()){
        ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_AdvanceOper->setVisible(false);
        ui->groupBox_UAVFlyControl_UAVFlyManage_Mavlink_AdvanceOper->hide();
    }

    ui->doubleSpinBox_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->setValue(QGCApplication::takeoffMinimumAltitude());

    ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot->setEnabled(false);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombLock, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::BombControl_BombLock);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombUnLock, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::BombControl_BombUnLock);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::BombControl_BombShot);

    //ui->groupBox_UAVFlyControl_UAVFlyManage_InteAgri_PesticideSpraying->setVisible(false);
    //ui->pushButton_UAVFlyControl_UAVFlyManageInteAgri_PesticideSpraying_Start->setVisible(false);
    //ui->pushButton_UAVFlyControl_UAVFlyManageInteAgri_PesticideSpraying_Stop->setVisible(false);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManageInteAgri_PesticideSpraying_Start, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::InteAgri_PesticideSpraying_Start);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManageInteAgri_PesticideSpraying_Stop, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::InteAgri_PesticideSpraying_Stop);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Manual, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Manual);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Stabilized, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Stabilized);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Acro, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Acro);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Rattitude, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Rattitude);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Altitude, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Altitude);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Offboard, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Offboard);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Position, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Position);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Hold, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Hold);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Mission, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Mission);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Return, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Return);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_FollowMe, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_FollowMe);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_FlyMode_Guided, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Guided);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Arm, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Arm);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Disarm, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Disarm);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_TakeOff);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Land, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Land);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_StartMisson, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_StartMisson);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_Pause, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Pause);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_RTL, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_RTL);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeControl, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_CommOper_TakeControl);

    connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_AdvanceOper_Reboot,        &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_AdvanceOper_Reboot);
 
	connect(ui->pushButton_UAVFlyControl_UAVFlyManage_Mavlink_AdvanceOper_EmergencyStop, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_AdvanceOper_EmergencyStop);

	connect(ui->horizontalSlider, &QSlider::valueChanged, this, &UAVFlyManage_MavlinkDialog::Mavlink_horizontalSlider_valueChanged);

	connect(ui->pushButton, &QPushButton::clicked, this, &UAVFlyManage_MavlinkDialog::Mavlink_NongYe_SetValue);

}

void UAVFlyManage_MavlinkDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

UAVFlyManage_MavlinkDialog::~UAVFlyManage_MavlinkDialog()
{

    delete ui;
}

void UAVFlyManage_MavlinkDialog::setVehicleSelectedDialog(QWidget* vehicleSelectedDialog)
{
    if(vehicleSelectedDialog != nullptr && ui->groupBox_UAVFlyControl_UAVFlyManage_VehicleSelected != nullptr
         && ui->groupBox_UAVFlyControl_UAVFlyManage_VehicleSelected->layout() != nullptr)
    {
        ui->groupBox_UAVFlyControl_UAVFlyManage_VehicleSelected->layout()->addWidget(vehicleSelectedDialog);
    }
}

bool UAVFlyManage_MavlinkDialog::OnCommand(int nID, void *lParam)
{
#define USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE

    if(QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID != nID){
        return false;
    }

    if(lParam == nullptr){
        return false;
    }

    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl* const systemFireProtControl = static_cast<const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl* const>(lParam);
    if(systemFireProtControl == nullptr){
        return false;
    }

    if(systemFireProtControl->head.head1 != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_HEAD1
        || systemFireProtControl->head.head2 != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_HEAD2
        || systemFireProtControl->head.interface_msgid != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID){
        return false;
    }

    if(systemFireProtControl->head.id != qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMFIREPROTCONTROL){
        return false;
    }

    if(systemFireProtControl->vehicle_id == 0){
        return false;
    }

    if(systemFireProtControl->fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Nona){
        return false;
    }

    if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Nona){
        return false;
    }

    auto FireProtControlSendFlyCmdFun = [this](const unsigned long long int vehicle_id, std::function<void (Vehicle* vehicle)>& vehicleFun)
    {
        if(!MavlinkApp()){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe4\xb8\xad\xe6\xb2\xa1\xe6\x9c\x89\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
            return false;
        }

        auto* vehiclesList = QGCApplication::vehicles();
        if(!vehiclesList){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe4\xb8\xad\xe6\xb2\xa1\xe6\x9c\x89\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
            return false;
        }

        Vehicle * vehicle_oper = nullptr;
        const auto vehiclesCount = vehiclesList->count();
        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe4\xb8\xba\xe7\xa9\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
                return false;
            }

            if(vehicle->weaponId() == vehicle_id)
            {
                vehicle_oper = vehicle;
                break;
            }
        }

        if(vehicle_oper == nullptr){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe4\xb8\xba\xe7\xa9\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
            return false;
        }

        WeaponControl_Before(vehicle_oper);

        vehicleFun(vehicle_oper);

        WeaponControl_After(vehicle_oper);

        logInfo("vehicle_id:%ld", vehicle_id);

        return true;
    };

	m_nValue = systemFireProtControl->nValue;
    if(systemFireProtControl->fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FireRocket)
    {

        if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_Lock)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombLock();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防灭火弹 操作 加锁");
            }
        }
        else if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_UnLock)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombUnLock();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防灭火弹 操作 解锁");
            }
        }
        else if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_Shoot)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombShot();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防灭火弹 操作 发射");
            }
        }
    }
    else if(systemFireProtControl->fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FirePot)
    {

        if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Lock)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombLock();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防罐 操作 加锁");
            }
        }
        else if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_UnLock)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombUnLock();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防罐 操作 解锁");
            }
        }
        else if(systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Throw)
        {

            {

                std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                    Q_UNUSED(this)
                    if(!vehicle){
                        return ;
                    }

                    vehicle->BombShot();
                };
                FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

                logInfo("%s", u8"消防罐 操作 抛投");
            }
        }
    }
	else if (systemFireProtControl->fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Spray)
	{
		if (systemFireProtControl->fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Spray_Count)
		{
				std::function<void(Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
					Q_UNUSED(this)
						if (!vehicle)
						{
							return;
						}

					double dValue = m_nValue / 100.0;
					int nFValue = (MAVLINK_PWM_MAX - MAVLINK_PWM_MIN) *dValue + MAVLINK_PWM_MIN;
					vehicle->setServoPWM(9, nFValue);
				};
				FireProtControlSendFlyCmdFun(systemFireProtControl->vehicle_id, vehicleFun);

				logInfo("%s", u8"喷洒 操作 开始");
		}
	}
#endif

    return false;
}

void UAVFlyManage_MavlinkDialog::WeaponControl_Before(Vehicle *vehicle)
{
    if(!vehicle){
        return ;
    }

    vehicle->setExcludeAllEvents(false);

    vehicle->setArmVehicleAndValidateTimes(1);
    vehicle->setFlightModeAndValidateTryTimes(1);
    vehicle->setFlightModeAndValidateTimes(1);
}

void UAVFlyManage_MavlinkDialog::WeaponControl_After(Vehicle *vehicle)
{
    if(!vehicle){
        return ;
    }

    vehicle->setExcludeAllEvents(false);
    vehicle->setArmVehicleAndValidateTimes(-1);
    vehicle->setFlightModeAndValidateTryTimes(-1);
    vehicle->setFlightModeAndValidateTimes(-1);
}

void UAVFlyManage_MavlinkDialog::BombControl_BombLock()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure \xe5\xbc\xb9\xe8\x8d\xaf\xe5\x8a\xa0\xe9\x94\x81?"), QMessageBox::Yes | QMessageBox::No)) {

        ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot->setEnabled(false);

        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->BombLock();
        };
        SendFlyCmdFun(vehicleFun);
    }

}

void UAVFlyManage_MavlinkDialog::BombControl_BombUnLock()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure \xe5\xbc\xb9\xe8\x8d\xaf\xe8\xa7\xa3\xe9\x94\x81?"), QMessageBox::Yes | QMessageBox::No)) {

        ui->pushButton_UAVFlyControl_UAVFlyManage_BombControl_BombShot->setEnabled(true);

        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->BombUnLock();
        };
        SendFlyCmdFun(vehicleFun);
    }
}

void UAVFlyManage_MavlinkDialog::BombControl_BombShot()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure \xe5\xbc\xb9\xe8\x8d\xaf\xe5\x8f\x91\xe5\xb0\x84?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(!vehicle->BombLockState()){
                vehicle->BombShot();
            }
            else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle \xe5\xbc\xb9\xe8\x8d\xaf\xe6\x9c\xaa\xe8\xa7\xa3\xe9\x94\x81!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

}

void UAVFlyManage_MavlinkDialog::InteAgri_PesticideSpraying_Start()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\xbc\x80\xe5\xa7\x8b\xe5\x96\xb7\xe6\xb4\x92\xe5\x86\x9c\xe8\x8d\xaf?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

			double dValue = ui->spinBox->value() / 100.0;
			int nFValue = (MAVLINK_PWM_MAX - MAVLINK_PWM_MIN) *dValue + MAVLINK_PWM_MIN;
			vehicle->setServoPWM(9, nFValue);
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"智慧农业 农药喷洒 开始");
}

void UAVFlyManage_MavlinkDialog::InteAgri_PesticideSpraying_Stop()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x81\x9c\xe6\xad\xa2\xe5\x96\xb7\xe6\xb4\x92\xe5\x86\x9c\xe8\x8d\xaf?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }
			vehicle->setServoPWM(9, MAVLINK_PWM_MIN);
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"智慧农业 农药喷洒 停止");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Manual()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Manual Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains(vehicle->takeControlFlightMode())){
                vehicle->setFlightMode(vehicle->takeControlFlightMode());
            }
            else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Manual Mode!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Manual");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Stabilized()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe5\xa2\x9e\xe7\xa8\xb3\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->apmFirmware()){
                if(vehicle->flightModes().contains("Stabilize")){
                    vehicle->setFlightMode("Stabilize");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xa2\x9e\xe7\xa8\xb3\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else if(vehicle->px4Firmware()){
                if(vehicle->flightModes().contains("Stabilized")){
                    vehicle->setFlightMode("Stabilized");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xa2\x9e\xe7\xa8\xb3\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else{
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xa2\x9e\xe7\xa8\xb3\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
            }

        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Stabilized");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Acro()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Acro Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains("Acro")){
                vehicle->setFlightMode("Acro");
            }else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Acro Mode!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Acro");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Rattitude()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Rattitude Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains("Rattitude")){
                vehicle->setFlightMode("Rattitude");
            }else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Rattitude Mode!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Rattitude");

}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Altitude()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe5\xae\x9a\xe9\xab\x98\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->apmFirmware()){
                if(vehicle->flightModes().contains("Altitude Hold")){
                    vehicle->setFlightMode("Altitude Hold");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe9\xab\x98\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else if(vehicle->px4Firmware()){
                if(vehicle->flightModes().contains("Altitude")){
                    vehicle->setFlightMode("Altitude");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe9\xab\x98\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else{
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe9\xab\x98\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Altitude");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Offboard()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Offboard Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains("Offboard")){
                vehicle->setFlightMode("Offboard");
            }else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Offboard Mode!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Offboard");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Position()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe5\xae\x9a\xe7\x82\xb9\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->apmFirmware()){
                if(vehicle->flightModes().contains("Position Hold")){
                    vehicle->setFlightMode("Position Hold");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe7\x82\xb9\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else if(vehicle->px4Firmware()){
                if(vehicle->flightModes().contains("Position")){
                    vehicle->setFlightMode("Position");
                }else{
                    LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe7\x82\xb9\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else{
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe5\xae\x9a\xe7\x82\xb9\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
            }

        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Position");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Hold()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Hold Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->apmFirmware()){
                if(vehicle->flightModes().contains("Position Hold")){
                    vehicle->setFlightMode("Position Hold");
                }else{
                    LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Position Hold Mode!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else if(vehicle->px4Firmware()){
                if(vehicle->flightModes().contains("Hold")){
                    vehicle->setFlightMode("Hold");
                }else{
                    LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Hold Mode!"), QMessageBox::Yes | QMessageBox::No);
                }
            }else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Hold Mode!"), QMessageBox::Yes | QMessageBox::No);
            }

        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Hold");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Mission()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe8\x87\xaa\xe4\xb8\xbb\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains(vehicle->missionFlightMode())){
                vehicle->setFlightMode(vehicle->missionFlightMode());
            }else{
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe8\x87\xaa\xe4\xb8\xbb\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Mission");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Return()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("Are you sure want to switch to Return RTL Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->guidedModeRTL(false);
        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Return RTL");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_FollowMe()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe8\xb7\x9f\xe9\x9a\x8f\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains(vehicle->followFlightMode())){
                vehicle->setFlightMode(vehicle->followFlightMode());
            }else{
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("\xe4\xb8\x8d\xe6\x94\xaf\xe6\x8c\x81\xe8\xb7\x9f\xe9\x9a\x8f\xe6\xa8\xa1\xe5\xbc\x8f!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Follow Me");
}

void UAVFlyManage_MavlinkDialog::Mavlink_FlyMode_Guided()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x88\x87\xe6\x8d\xa2\xe5\x88\xb0\xe5\xbc\x95\xe5\xaf\xbc\xe6\xa8\xa1\xe5\xbc\x8f?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->setGuidedMode(true);
        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Guided");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Arm()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe8\xa7\xa3\xe9\x94\x81?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->setArmed(true, true);

			QDateTime curdt = QDateTime::currentDateTime();
			QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
			auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("解锁"), QString::number(vehicle->weaponId()), FireProEvent_Enum_SYSTEM_Unlock);
			LY_UAVFlyControl_MavlinkInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900300, lParam);

        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Arm");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Disarm()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe5\x8a\xa0\xe9\x94\x81?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->setArmed(false, true);

			QDateTime curdt = QDateTime::currentDateTime();
			QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
			auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("加锁"), QString::number(vehicle->weaponId()), FireProEvent_Enum_SYSTEM_Lock);
			LY_UAVFlyControl_MavlinkInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900300, lParam);

        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Disarm");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_TakeOff()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Take Off Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            auto takeoffAltitude = this->ui->doubleSpinBox_UAVFlyControl_UAVFlyManage_Mavlink_CommOper_TakeOff->value();
            if(takeoffAltitude < vehicle->minimumTakeoffAltitude())
            {
                takeoffAltitude = (vehicle->minimumTakeoffAltitude());
            }
            vehicle->altitudeAMSL()->setRawValue(QVariant(takeoffAltitude));
            vehicle->guidedModeTakeoff(takeoffAltitude);

        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Take Off");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Land()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe9\x99\x8d\xe8\x90\xbd?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->guidedModeLand();

			QDateTime curdt = QDateTime::currentDateTime();
			QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
			auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("降落"), QString::number(vehicle->weaponId()), FireProEvent_Enum_SYSTEM_Land);
			LY_UAVFlyControl_MavlinkInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900300, lParam);

        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Land");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_StartMisson()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Start Misson Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->startMission();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Start Misson");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_Pause()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Pause Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->pauseVehicle();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行模式 Flight Mode Pause");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_RTL()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe7\xa1\xae\xe5\xae\x9a\xe8\xbf\x94\xe8\x88\xaa?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            vehicle->guidedModeRTL(false);

			QDateTime curdt = QDateTime::currentDateTime();
			QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
			auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("返航"), QString::number(vehicle->weaponId()), FireProEvent_Enum_SYSTEM_RTL);
			LY_UAVFlyControl_MavlinkInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900300, lParam);

        };

        SendFlyCmdFun(vehicleFun);

		std::function<void(Vehicle* vehicle)> vehicleFunStop = [this](Vehicle* vehicle) {
			Q_UNUSED(this)
				if (!vehicle) {
					return;
				}
			vehicle->setServoPWM(9, MAVLINK_PWM_MIN);
		};
		SendFlyCmdFun(vehicleFunStop);
    }

    logInfo("%s", u8"飞行模式 Flight Mode RTL");
}

void UAVFlyManage_MavlinkDialog::Mavlink_CommOper_TakeControl()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Take Control Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
            Q_UNUSED(this)
            if(!vehicle){
                return ;
            }

            if(vehicle->flightModes().contains(vehicle->takeControlFlightMode())){
                vehicle->setFlightMode(vehicle->takeControlFlightMode());
            }else{
                LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Take Control Mode!"), QMessageBox::Yes | QMessageBox::No);
            }
        };
        SendFlyCmdFun(vehicleFun);
    }
    logInfo("%s", u8"飞行模式 Flight Mode Take Control");
}

void UAVFlyManage_MavlinkDialog::Mavlink_AdvanceOper_Reboot()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Reboot?"), QMessageBox::Yes | QMessageBox::No)) {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Reboot?"), QMessageBox::Yes | QMessageBox::No)) {
            std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                Q_UNUSED(this)
                if(!vehicle){
                    return ;
                }

                if(vehicle->flightModes().contains(vehicle->takeControlFlightMode())){
                    vehicle->rebootVehicle();
                }else{
                    LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Reboot!"), QMessageBox::Yes | QMessageBox::No);
                }
            };
            SendFlyCmdFun(vehicleFun);
        }
    }
    logInfo("%s", u8"高级操作 Advance Oper Reboot");
}

void UAVFlyManage_MavlinkDialog::Mavlink_AdvanceOper_EmergencyStop()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Emergency Stop?"), QMessageBox::Yes | QMessageBox::No)) {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Emergency Stop?"), QMessageBox::Yes | QMessageBox::No)) {
            std::function<void (Vehicle* vehicle)> vehicleFun = [this](Vehicle* vehicle) {
                Q_UNUSED(this)
                if(!vehicle){
                    return ;
                }

                vehicle->emergencyStop();
            };
            SendFlyCmdFun(vehicleFun);
        }
    }
    logInfo("%s", u8"高级操作 Advance Oper Emergency Stop");
}

void UAVFlyManage_MavlinkDialog::Mavlink_horizontalSlider_valueChanged(int value)
{
	ui->spinBox->setValue(value);
}

void UAVFlyManage_MavlinkDialog::Mavlink_NongYe_SetValue()
{
	int nValue = ui->spinBox->value();
	ui->horizontalSlider->setValue(nValue);
	InteAgri_PesticideSpraying_Start();
}

void UAVFlyManage_MavlinkDialog::SendFlyCmdFun(std::function<void(Vehicle* vehicle)>& vehicleFun)
{
    if(!vehicleFun){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    const UAVSelectDialog* pUAVSelectDialog = LY_UAVFlyControl_Mavlink::Get().getPUAVSelectDialog();
    if(!pUAVSelectDialog){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    const UAVPushButton* pSelectCurrentWeaponComponent = pUAVSelectDialog->getSelectCurrentWeaponComponent();
    if(!pSelectCurrentWeaponComponent){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    const UAVPushButton* pSelectAllWeaponComponent = pUAVSelectDialog->getSelectAllWeaponComponent();
    if(!pSelectAllWeaponComponent){

        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe4\xb8\xad\xe6\xb2\xa1\xe6\x9c\x89\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    const auto& listWeaponComponent = pUAVSelectDialog->getButtonListWeaponComponent();
    if(listWeaponComponent.size() <= 0){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机选择列表为空, 无人机选择列表错误, 指令未发送?"), QMessageBox::Yes);
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        const Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机为空, 无人机错误, 指令未发送?"), QMessageBox::Yes);
            return ;
        }

        const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
            = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                           [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
                           {
                               if(!node.first){
                                   return false;
                               }

                               if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                                   return true;
                               }

                               return false;
                           }
                           );

        if(lineListFind == listWeaponComponent.constKeyValueEnd()) {
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("无人机信息和配置按钮不匹配, 无人机ID或配置ID错误, 指令未发送?"), QMessageBox::Yes);
            return ;
        }
    }

    if (pSelectAllWeaponComponent == pSelectCurrentWeaponComponent && "ALL" == pSelectAllWeaponComponent->getStrComponentID())
    {
        if(!pSelectAllWeaponComponent->isChecked()){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机“选择所有无人机按钮”未选中, 指令未发送?"), QMessageBox::Yes);
            return ;
        }

        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机为空, 无人机错误, 指令未发送?"), QMessageBox::Yes);
                return ;
            }

            WeaponControl_Before(vehicle);

            vehicleFun(vehicle);

            WeaponControl_After(vehicle);
        }
    }
    else
    {

        QList<Vehicle*> vehicleListSelected;
        vehicleListSelected.clear();
        const auto vehiclesCount = vehiclesList->count();
        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机为空, 无人机错误, 指令未发送?"), QMessageBox::Yes);
                return ;
            }

            const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
                = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                               [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
                               {
                                   if(!node.first){
                                       return false;
                                   }

                                   if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                                       return true;
                                   }

                                   return false;
                               }
                               );

            if(lineListFind == listWeaponComponent.constKeyValueEnd()) {
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), vehicle->weaponTitle() + tr("无人机信息和配置按钮不匹配, 无人机ID或配置ID错误, 指令未发送?"), QMessageBox::Yes);
                return ;
            }

            if(lineListFind.base().key()->isChecked()){
                vehicleListSelected.append(vehicle);
            }
        }

        for( auto* vehicle : vehicleListSelected){

            WeaponControl_Before(vehicle);

            vehicleFun(vehicle);

            WeaponControl_After(vehicle);

        }
    }
}

