#include "UAVFlyManage_DJI_DockDialog.h"
#include "ui_UAVFlyManage_DJI_DockDialog.h"

#include "FlyControl/IUAVFlyManage_DJI_SendTask_Dialog.h"
#include "FlyControl/IUAVFlyManage_DJI_WayLine_UploadDialog.h"

#include "lyUI/LyWidget.h"
#include "lyUI/LyMessageBox.h"

#include "LY_UAVFlyControl_DJI_Dock.h"
#include "UAVSelectDialog.h"

#include <dji/sdk/cloudapi/device/dji_dock.h>
#include <dji/sdk/dji_coludapi.h>
#include <dji/sdk/dji_coludapimanager.h>
#include <djicloude/DJICloudeAPI.h>
#include <dji/sdk/cloudapi/log/dji_file_upload_progress.h>

#include "FlyControl/IUAVFlyManage_DJI_TakeoffToPoint.h"


#define __DEBUG__LOGINFO__
#if defined(__DEBUG__LOGINFO__)
#include <QDebug>

#define logInfo(format, ...) qDebug("[File:" __FILE__ " ,Line:%d,Func:%s] " format, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define logInfo(format, ...)
#endif




UAVFlyManage_DJI_DockDialog::UAVFlyManage_DJI_DockDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_DJI_DockDialog)
    , m_pUAVSelectDialog(nullptr)
	, m_djiDjiTakeoffToPoint(nullptr)
{
    ui->setupUi(this);


//     ui->groupBox_DJI_Dock__Drone_Command_Control->setVisible(false);
//     ui->groupBox_DJI_Dock__Drone_Command_Control->hide();

//     ui->groupBox_DJI_Dock__Drone_WayLine_Control->setVisible(false);
//     ui->groupBox_DJI_Dock__Drone_WayLine_Control->hide();

    ui->groupBox_DJI_Dock_Payload_Control->setVisible(false);
    ui->groupBox_DJI_Dock_Payload_Control->hide();

    ui->pushButton_DJI_Dock_Video_Control_Dock_Ire->setVisible(false);
    ui->pushButton_DJI_Dock_Video_Control_Dock_Ire->hide();

    ui->pushButton_DJI_Dock_Video_Control_Drone_Ire->setVisible(false);
    ui->pushButton_DJI_Dock_Video_Control_Drone_Ire->hide();

    m_pUAVSelectDialog = new UAVSelectDialog(nullptr);
    if(ui->groupBox_DJI_Dock_Dock_Drone_Selected->layout() != nullptr){
        ui->groupBox_DJI_Dock_Dock_Drone_Selected->layout()->addWidget(m_pUAVSelectDialog);
    }

	ui->horizontalSlider_DJICameraControl_FocalLength->setMinimum(2);
	ui->horizontalSlider_DJICameraControl_FocalLength->setMaximum(200);

    //  调试模式开启
    connect(ui->pushButton_DJI_Dock_debug_mode_open, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_debug_mode_open);

    // 调试模式关闭
    connect(ui->pushButton_DJI_Dock_debug_mode_close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_debug_mode_close);

    // 打开舱盖
    connect(ui->pushButton_DJI_Dock_cover_open, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_cover_open);

    // 关闭舱盖
    connect(ui->pushButton_DJI_Dock_cover_close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_cover_close);

    // 飞行器开机
    connect(ui->pushButton_DJI_Dock_drone_open, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_drone_open);

    // 飞行器关机
    connect(ui->pushButton_DJI_Dock_drone_close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_drone_close);

    // 打开补光灯
    connect(ui->pushButton_DJI_Dock_supplement_light_open, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_supplement_light_open);

    // 关闭补光灯
    connect(ui->pushButton_DJI_Dock_supplement_light_close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_supplement_light_close);

    // 打开充电
    connect(ui->pushButton_DJI_Dock_charge_open, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_charge_open);

    // 关闭充电
    connect(ui->pushButton_DJI_Dock_charge_close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_charge_close);


    // 视频控制 机场-红外
    connect(ui->pushButton_DJI_Dock_Video_Control_Dock_Ire, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Ire);

    // 视频控制 机场-可见光
     connect(ui->pushButton_DJI_Dock_Video_Control_Dock_Light, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Light);

    // 视频控制 机场-关闭
     connect(ui->pushButton_DJI_Dock_Video_Control_Dock_Close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Close);

    // 视频控制 无人机-红外
     connect(ui->pushButton_DJI_Dock_Video_Control_Drone_Ire, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Ire);

    // 视频控制 机场-可见光
     connect(ui->pushButton_DJI_Dock_Video_Control_Drone_Light, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Light);

    // 视频控制 无人机-关闭
     connect(ui->pushButton_DJI_Dock_Video_Control_Drone_Close, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Close);

	//吊舱控制 开始录像
	 connect(ui->pushButton_DJICameraControl_Video, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Video);

	 // 吊舱控制 上
	 connect(ui->pushButton_DJICameraControl_Up, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Up);

	 //吊舱控制 拍照
	 connect(ui->pushButton_DJICameraControl_Photograph, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Photograph);

	 //吊舱控制 左
	 connect(ui->pushButton_DJICameraControl_Left, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Left);

	 //吊舱控制 归中
	 connect(ui->pushButton_DJICameraControl_Center, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Center);

	 //吊舱控制 右
	 connect(ui->pushButton_DJICameraControl_Right, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Right);

	 //吊舱控制 可见光
	 connect(ui->pushButton_DJICameraControl_visibleLight, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Light);

	 //吊舱控制 下
	 connect(ui->pushButton_DJICameraControl_Down, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Down);

	 //吊舱控制 垂直下视
	 connect(ui->pushButton_DJICameraControl_VerticalUp, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_VerticalUp);

	//负载控制权
	 connect(ui->pushButton_command_payload_anthority_grab, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_command_payload_anthority_grab);

	 //切换拍照模式
	 connect(ui->comboBox_camera_mode_switch, &QComboBox::currentTextChanged, this, &UAVFlyManage_DJI_DockDialog::on_camera_contral_mode_switch);

	 //飞行控制器抢夺
	 connect(ui->pushButton_command_flight_control_compete, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_command_flight_control_compete);

	 //停止录像
	 connect(ui->pushButton_DJICameraControl_Video_stop, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Video_stop);

	 //焦距
	 connect(ui->horizontalSlider_DJICameraControl_FocalLength, &QSlider::valueChanged, this, &UAVFlyManage_DJI_DockDialog::slider_horizontalSlider_DJICameraControl_FocalLength);

	 //下发任务
	 connect(ui->pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Prepare, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Prepare);

	 //执行任务
	 connect(ui->pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Execute, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Execute);
	 
	 //取消任务
	 connect(ui->pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Undo, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Undo);

	//导入任务
	 connect(ui->pushButton_takeoff_load_task, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_takeoff_load_task);
	 
	 //一键起飞
	 connect(ui->pushButton_takeoff_to_point, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_takeoff_to_point);

	 //一键返航
	 connect(ui->pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home);

	 //flyto 飞向目标点
	 connect(ui->pushButton_command_flyto_targetpoint, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_targetpoint);

	 //flyto 结束飞向目标点
	 connect(ui->pushButton_command_flyto_endtask, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_endtask);

	 //更新飞向目标点
	 connect(ui->pushButton_command_flyto_updatapoint, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_updatepoint);

	 //DRC急停
	 connect(ui->pushButton_DJI_Dock_Command_DRCemergency_Stop, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCemergency_Stop);

	 //进入指令飞行控制模式
	 connect(ui->pushButton_DJI_Dock_Command_DRCMode_enter, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCMode_enter);

	 //退出指令飞行控制模式
	 connect(ui->pushButton_DJI_Dock_Command_DRCMode_exit, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCMode_exit);

	//指定home点返航
	 connect(ui->pushButton_DJI_Dock_WayLine_SelectedHome_ReturnRoute, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_SelectedHome_ReturnRoute);

	 ////暂停空中航线
	 connect(ui->pushButton_DJI_Dock_WayLine_AirRoute_Stop, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_AirRoute_Stop);

	 //恢复空中航线
	 connect(ui->pushButton_DJI_Dock_WayLine_AirRoute_Recover, &QPushButton::clicked, this, &UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_AirRoute_Recover);
}

UAVFlyManage_DJI_DockDialog::~UAVFlyManage_DJI_DockDialog()
{
    delete ui;
}

void UAVFlyManage_DJI_DockDialog::changeEvent(QEvent *event)
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

void UAVFlyManage_DJI_DockDialog::DJI_Dock_debug_mode_open()
{
    //  调试模式开启
    /** * @brief debug_mode_open
     *
     *  调试模式开启

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: debug_mode_open

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: debug_mode_open

        Data:
        Column  Name    Type    constraint  Description
        result  返回码 int     非 0 代表错误
        output  输出  struct
        »status 任务状态    enum_string {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
        #
        调试模式关闭
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to debug mode open?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->debug_mode_open();

            // if(djiDock->flightModes().contains(djiDock->takeControlFlightMode())){
            //     djiDock->setFlightMode(djiDock->takeControlFlightMode());
            // }
            // else{
            //     LyMessageBox::warning(nullptr, tr("Tip"), djiDock->weaponTitle() + tr("Vehicle is not support Manual Mode!"), QMessageBox::Yes | QMessageBox::No);
            // }
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"调试模式开启 debug_mode_open");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_debug_mode_close()
{
    // 调试模式关闭
    /** * @brief debug_mode_close
     *
     *  调试模式关闭

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: debug_mode_close

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: debug_mode_close

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
     **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to debug mode close?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->debug_mode_close();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"调试模式关闭 debug_mode_close");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_cover_open()
{

    // 打开舱盖
    /** * @brief cover_open
     *
     *  打开舱盖

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: cover_open

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: cover_open

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to cover open?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->cover_open();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"打开舱盖 cover_open");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_cover_close()
{
    // 关闭舱盖
    /** * @brief cover_close
     *
     *  关闭舱盖

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: cover_close

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: cover_close

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to cover close?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->cover_close();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"关闭舱盖 cover_close");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_drone_open()
{
    // 飞行器开机
    /** * @brief drone_open
     *
     *  飞行器开机

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: drone_open

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: drone_open

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to drone open?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->drone_open();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行器开机 drone_open");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_drone_close()
{
    // 飞行器关机
    /** * @brief drone_close
     *
     *  飞行器关机

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: drone_close

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: drone_close

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to drone close?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->drone_close();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"飞行器关机 drone_close");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_supplement_light_open()
{
    // 打开补光灯
    /** * @brief supplement_light_open
     *
     *  打开补光灯

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: supplement_light_open

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: supplement_light_open

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to supplement light open?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->supplement_light_open();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"打开补光灯 supplement_light_open");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_supplement_light_close()
{
    // 关闭补光灯
    /** * @brief supplement_light_close
     *
     *  关闭补光灯

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: supplement_light_close

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: supplement_light_close

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to supplement supplement light close?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->supplement_light_close();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"关闭补光灯 supplement_light_close");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_charge_open()
{

    // 打开充电
    /** * @brief charge_open
     *
     *  打开充电

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: charge_open

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: charge_open

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to supplement charge open?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->charge_open();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"打开充电 charge_open");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_charge_close()
{
    // 关闭充电
    /** * @brief charge_close
     *
     *  关闭充电

        Topic: thing/product/{gateway_sn}/services

        Direction: down

        Method: charge_close

        Data: null

        Topic: thing/product/{gateway_sn}/services_reply

        Direction: up

        Method: charge_close

        Data:
        Column    Name    Type    constraint    Description
        result    返回码    int        非 0 代表错误
        output    输出    struct
        »status    任务状态    enum_string    {"canceled":"取消或终止","failed":"失败","in_progress":"执行中","ok":"执行成功","paused":"暂停","rejected":"拒绝","sent":"已下发","timeout":"超时"}
    **/

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to supplement charge close?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->charge_close();
        };
        SendFlyCmdFun(vehicleFun);
    }

    logInfo("%s", u8"关闭充电 charge_close");
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Ire()
{

}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Light()
{
    int video_quality_index = 0;
    if(ui && ui->comboBox_DJI_Dock_Video_Control_Dock_Video_Quality)
    {
        const int currentIndex = ui->comboBox_DJI_Dock_Video_Control_Dock_Video_Quality->currentIndex();
        if(currentIndex >= 0 && currentIndex <= 4){
            video_quality_index = currentIndex;
        }
    }
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to live start push?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this, video_quality_index](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->live_start_push();
        };
        SendFlyCmdFun(vehicleFun);
    }
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Dock_Close()
{
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to live stop push?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->live_stop_push();
        };
        SendFlyCmdFun(vehicleFun);
    }
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Ire()
{

}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Light()
{
    int video_quality_index = 0;
    if(ui && ui->comboBox_DJI_Dock_Video_Control_Drone_Video_Quality)
    {
        const int currentIndex = ui->comboBox_DJI_Dock_Video_Control_Drone_Video_Quality->currentIndex();
        if(currentIndex >= 0 && currentIndex <= 4){
            video_quality_index = currentIndex;
        }
    }

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to live start push drone?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this, video_quality_index](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->live_start_push_drone(video_quality_index);
        };
        SendFlyCmdFun(vehicleFun);
    }
}

void UAVFlyManage_DJI_DockDialog::DJI_Dock_Video_Control_Drone_Close()
{
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to live stop push drone?"), QMessageBox::Yes | QMessageBox::No)) {
        std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
            Q_UNUSED(this)
            if(!djiDock){
                return ;
            }

            djiDock->live_stop_push_drone();
        };
        SendFlyCmdFun(vehicleFun);
    }
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Video()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to start video?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_video_start();
		};
		SendFlyCmdFun(vehicleFun);
		b_isStartVideo = true;

		ui->pushButton_DJICameraControl_Video->setText(u8"停止录像");
		ui->pushButton_DJICameraControl_Video->setStyleSheet("background-color: red;");
		logInfo("%s", u8"开始录像 camera_recording_start");
	}
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Up()
{
	
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_control_up();
		};
		SendFlyCmdFun(vehicleFun);
	
	logInfo("%s", u8"摄像头上视 camera_aim");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Left()
{
	
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_control_left();
		};
		SendFlyCmdFun(vehicleFun);
	
	logInfo("%s", u8"摄像头左视 camera_aim");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Down()
{

	std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
		Q_UNUSED(this)
			if (!djiDock) {
				return;
			}

		djiDock->camera_control_down();
	};
	SendFlyCmdFun(vehicleFun);

	logInfo("%s", u8"摄像头下视 camera_aim");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Right()
{

	std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
		Q_UNUSED(this)
			if (!djiDock) {
				return;
			}

		djiDock->camera_control_right();
	};
	SendFlyCmdFun(vehicleFun);

	logInfo("%s", u8"摄像头右视 camera_aim");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Photograph()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to photograph?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_photo_take();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"开始拍照 camera_photo_take");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_VerticalUp()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to camera vertical up?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_verticalup();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"摄像头垂直下视 camera_focus_value_set");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Center()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to camera center?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_center();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"摄像头归中 gimbal_reset");
}

void UAVFlyManage_DJI_DockDialog::slider_horizontalSlider_DJICameraControl_FocalLength(int value)
{
	//判断摄像头类型 
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [=](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_focallength(value);
		};
		SendFlyCmdFun(vehicleFun);
	
	logInfo("%s", u8"摄像头焦距 camera_focus_value_set");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Prepare()
{
	if (nullptr != LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()
		&& LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->isHidden())
	{
        LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->getListObjectsInBucket();
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_SendTask_Dialog()->show();
	}

}

QString s_flight_id = "";
void UAVFlyManage_DJI_DockDialog::Load_Dji_FlighttaskPrepare(qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskPrepare& djiFlighttaskPrepare, bool flight_safety_advance_check, bool executableConditions, bool break_point, bool simulate_mission)
{
	s_flight_id = djiFlighttaskPrepare.getFlightId();
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to flight task prepare?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [=](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->flighttask_prepare(djiFlighttaskPrepare, flight_safety_advance_check, executableConditions, break_point, simulate_mission);
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"下发任务 flighttask_prepare");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Execute()
{
	const QString& flight_id = s_flight_id;
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to execute flight task?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this, &flight_id](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->flighttask_execute(flight_id);
		};
		SendFlyCmdFun(vehicleFun);

		// s_flight_id = "";
	}
	logInfo("%s", u8"执行任务 flighttask_execute");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Undo()
{
	const QStringList& flight_id_list = QStringList(s_flight_id);
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to execute flight task?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this, &flight_id_list](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->taskroute_flighttask_undo(flight_id_list);
		};
		SendFlyCmdFun(vehicleFun);

		// s_flight_id = "";
	}
	logInfo("%s", u8"取消任务 taskroute_flighttask_undo");
}

void UAVFlyManage_DJI_DockDialog::Load_Dji_SelectedPoint_flight(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint& djiDjiTakeoffToPoint)
{
	if (m_djiDjiTakeoffToPoint == nullptr)
	{
		m_djiDjiTakeoffToPoint = new qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint;
	}
	*m_djiDjiTakeoffToPoint = djiDjiTakeoffToPoint;
	ui->checkBox_current_task->setText(m_djiDjiTakeoffToPoint->getFlightID());
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_takeoff_to_point()
{
	//一键起飞
	//command_takeoff_to_point
	//
	if (!ui->checkBox_current_task->isChecked())
	{
		return;
	}
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to takeoff to point?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
				if (!djiDock) {
					return;
				}

			djiDock->command_takeoff_to_point(*m_djiDjiTakeoffToPoint);
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"一键起飞 flighttask_prepare");
}

void UAVFlyManage_DJI_DockDialog::Flyto_Dji_takeoff_to_point(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint & djiDjiTakeoffToPoint)
{
	//flyto 飞向目标点
	//command_takeoff_to_point
	//
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to fly to point?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this,&djiDjiTakeoffToPoint](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			if (!djiDock) {
				return;
			}

			djiDock->command_takeoff_to_point_flyto(djiDjiTakeoffToPoint);
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"flyto 飞向目标点 fly_to_point");
}

void UAVFlyManage_DJI_DockDialog::Flyto_Dji_takeoff_to_point_Update(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint & djiDjiTakeoffToPoint)
{
	//flyto 更新目标点
	//
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to fly to point update?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this, &djiDjiTakeoffToPoint](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			if (!djiDock) {
				return;
			}

			djiDock->command_takeoff_to_point_flyto_update(djiDjiTakeoffToPoint);
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"flyto 更新目标点 fly_to_point_update");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_targetpoint()
{
	//flyto 飞向目标点
	//command_takeoff_to_point
	//
	if (LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()
		&& LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->isHidden())
	{
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->Init(true);
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->show();
	}

}
void UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_endtask()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to takeoff to stop flyto target point?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			if (!djiDock) {
				return;
			}

			djiDock->command_takeoff_to_point_flyto_stop();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"flyto_stop 结束飞向目标点 fly_to_point_stop");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_command_flyto_updatepoint()
{
	//flyto 更新目标点
	//command_takeoff_to_point
	//
	if (LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()
		&& LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->isHidden())
	{
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->Init(true);
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->show();
	}
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCemergency_Stop()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to DRC emergency stop?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_drone_emergency_stop();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"DRC急停 drone_emergency_stop");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCMode_enter()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to DRC mode enter?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_drc_mode_enter();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"进入指令飞行控制模式 drc_mode_enter");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_Command_DRCMode_exit()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to DRC mode enter?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_drc_mode_exit();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"退出指令飞行控制模式 drc_mode_exit");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_SelectedHome_ReturnRoute()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to return route?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_wayline_specific_home();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"指定home点返航 return_specific_home");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_AirRoute_Stop()
{
// 	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to return route?"), QMessageBox::Yes | QMessageBox::No)) {
// 		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
// 			Q_UNUSED(this)
// 				if (!djiDock) {
// 					return;
// 				}
// 
// 			djiDock->command_wayline_specific_home();
// 		};
// 		SendFlyCmdFun(vehicleFun);
// 	}
// 	logInfo("%s", u8"指定home点返航 return_specific_home");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_AirRoute_Recover()
{
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Pause()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to flight task pause?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->taskroute_flighttask_pause();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"航线暂停 taskroute_flighttask_pause");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Recovery()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to flight task recovery?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->taskroute_flighttask_recovery();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"一键返航 flighttask_recovery");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to return home?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->taskroute_route_home();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"一键返航 return_home");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home_Cancel()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to return home cancel?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->taskroute_return_home_cancel();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"返航取消 return_home_cancel");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJI_Dock_WayLine_Control_Flysendroute()
{
	if (LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()
		&& LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->isHidden())
	{
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->Init();
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->show();
	}
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_command_flight_control_compete()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to flight control compete?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_flight_control_compete();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"飞行控制权抢夺 flight_authority_grab");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_command_payload_anthority_grab()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to payload anthority grab?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->command_payload_anthority_grab();
		};
		SendFlyCmdFun(vehicleFun);
	}
	logInfo("%s", u8"负载控制权抢夺 payload_authority_grab");
}

void UAVFlyManage_DJI_DockDialog::on_camera_contral_mode_switch()
{
	int index = ui->comboBox_camera_mode_switch->currentIndex();
	if (index == 0 || index == -1)
	{
		return;
	}

	std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [=](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
		Q_UNUSED(this)
			if (!djiDock) {
				return;
			}

		djiDock->camera_contral_mode_switch(index - 1);
	};
	SendFlyCmdFun(vehicleFun);

	logInfo("%s", u8"切换相机模式 payload_authority_grab");
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_DJICameraControl_Video_stop()
{
	if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to stop video?"), QMessageBox::Yes | QMessageBox::No)) {
		std::function<void(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> vehicleFun = [this](qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* djiDock) {
			Q_UNUSED(this)
				if (!djiDock) {
					return;
				}

			djiDock->camera_contral_video_stop();
		};
		SendFlyCmdFun(vehicleFun);

		logInfo("%s", u8"停止录像 camera_recording_stop");

		b_isStartVideo = false;

		ui->pushButton_DJICameraControl_Video->setText(u8"开始录像");
		ui->pushButton_DJICameraControl_Video->setStyleSheet("");
	}
}

void UAVFlyManage_DJI_DockDialog::on_pushButton_takeoff_load_task()
{
	if (LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint())
	{
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->Init();
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_TakeoffToPoint()->show();
	}
}

void UAVFlyManage_DJI_DockDialog::WeaponControl_Before(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock *vehicle)
{

}

void UAVFlyManage_DJI_DockDialog::WeaponControl_After(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock *vehicle)
{

}

void UAVFlyManage_DJI_DockDialog::SendFlyCmdFun(std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)> &vehicleFun)
{
    if(!vehicleFun){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    // const UAVFlyManage_DJI_DockDialog* pUAVFlyManage_DJI_DockDialog = LY_UAVFlyControl_DJI_Dock::Get().pUAVFlyManage_DJI_DockDialog();
    // if(!pUAVFlyManage_DJI_DockDialog){
    //     LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x9c\xaa\xe9\x80\x89\xe6\x8b\xa9\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xef\xbc\x8c\xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
    //     return ;
    // }

    const UAVSelectDialog* pUAVSelectDialog = m_pUAVSelectDialog;
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

    DJICloudeAPI* pDJICloudeAPITemp = LY_UAVFlyControl_DJI_Dock::Get().getPDJICloudeAPI();
    if(!pDJICloudeAPITemp){
        return ;
    }

    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApi* pDjiColudApi = pDJICloudeAPITemp->pDjiCcoludApi();
    if (pDjiColudApi == nullptr) {
        return;
    }

    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager* pDjiColudApiManager = pDjiColudApi->pDjiColudApiManager();
    if (pDjiColudApiManager == nullptr) {
        return;
    }

    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*>& djiGatewayDevice_MAP = pDjiColudApiManager->getDjiGatewayDevice_MAP();

    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDevice*>& djiSubDevice_MAP = pDjiColudApiManager->getDjiSubDevice_MAP();

    if (djiGatewayDevice_MAP.size() <= 0) {
        return;
    }

    const auto& vehiclesList = djiGatewayDevice_MAP.values();
    if(vehiclesList.size() <= 0){
        return ;
    }

    if(vehiclesList.size() <= 0){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe4\xb8\xad\xe6\xb2\xa1\xe6\x9c\x89\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba, \xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\x88\x97\xe8\xa1\xa8\xe9\x94\x99\xe8\xaf\xaf, \xe6\x8c\x87\xe4\xbb\xa4\xe6\x9c\xaa\xe5\x8f\x91\xe9\x80\x81?"), QMessageBox::Yes);
        return ;
    }

    const auto& listWeaponComponent = pUAVSelectDialog->getButtonListWeaponComponent();
    if(listWeaponComponent.size() <= 0){
        LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机选择列表为空, 无人机选择列表错误, 指令未发送?"), QMessageBox::Yes);
        return ;
    }

    const auto vehiclesCount = vehiclesList.count();
    for (int i = 0; i < vehiclesCount; i++) {
        const qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle = vehiclesList.at(i);
        if(!vehicle){
            LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机为空, 无人机错误, 指令未发送?"), QMessageBox::Yes);
            return ;
        }

        const QMap<UAVPushButton *, QString>::const_key_value_iterator lineListFind
            = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                           [&vehicle](const std::pair<UAVPushButton *, QString>& node)
                           {
                               if(!node.first){
                                   return false;
                               }

                               if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->getSn()){
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
            qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle = vehiclesList.at(i);
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

        QList<qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*> vehicleListSelected;
        vehicleListSelected.clear();
        const auto vehiclesCount = vehiclesList.count();
        for (int i = 0; i < vehiclesCount; i++) {
            qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle = vehiclesList.at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("\xe6\x8f\x90\xe7\xa4\xba"), tr("无人机为空, 无人机错误, 指令未发送?"), QMessageBox::Yes);
                return ;
            }

            const QMap<UAVPushButton *, QString>::const_key_value_iterator lineListFind
                = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                               [&vehicle](const std::pair<UAVPushButton *, QString>& node)
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
