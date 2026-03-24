#ifndef UAVFLYMANAGE_DJI_DOCKDIALOG_H
#define UAVFLYMANAGE_DJI_DOCKDIALOG_H

#include <QWidget>
#include <functional>
#include "dji/sdk/cloudapi/wayline/dji_takeoff_to_point.h"
#include "dji/sdk/cloudapi/wayline/dji_flight_task_prepare.h"
namespace Ui {
class UAVFlyManage_DJI_DockDialog;
}


namespace qnzkna {
namespace DjiSdkApi {
namespace DjiCloudApi {
// dji device
class DjiDock;

class DjiFlighttaskPrepare;
class DjiTakeoffToPoint;
}
}
}


class UAVSelectDialog;

class UAVFlyManage_DJI_DockDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_DJI_DockDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_DJI_DockDialog();


private slots:

    //  调试模式开启
    void DJI_Dock_debug_mode_open();

    // 调试模式关闭
    void DJI_Dock_debug_mode_close();

    // 打开舱盖
    void DJI_Dock_cover_open();

    // 关闭舱盖
    void DJI_Dock_cover_close();

    // 飞行器开机
    void DJI_Dock_drone_open();

    // 飞行器关机
    void DJI_Dock_drone_close();

    // 打开补光灯
    void DJI_Dock_supplement_light_open();

     // 关闭补光灯
    void DJI_Dock_supplement_light_close();

    // 打开充电
    void DJI_Dock_charge_open();

    // 关闭充电
    void DJI_Dock_charge_close();


    // 视频控制 机场-红外
    void DJI_Dock_Video_Control_Dock_Ire();

    // 视频控制 机场-可见光
    void DJI_Dock_Video_Control_Dock_Light();

    // 视频控制 机场-关闭
    void DJI_Dock_Video_Control_Dock_Close();

    // 视频控制 无人机-红外
    void DJI_Dock_Video_Control_Drone_Ire();

    // 视频控制 机场-可见光
    void DJI_Dock_Video_Control_Drone_Light();

    // 视频控制 无人机-关闭
    void DJI_Dock_Video_Control_Drone_Close();

	//录像
	void on_pushButton_DJICameraControl_Video();

	//吊舱——上
	void on_pushButton_DJICameraControl_Up();

	//吊舱——左
	void on_pushButton_DJICameraControl_Left();

	//吊舱——下
	void on_pushButton_DJICameraControl_Down();

	//吊舱——右
	void on_pushButton_DJICameraControl_Right();

	//拍照
	void on_pushButton_DJICameraControl_Photograph();

	//垂直下视
	void on_pushButton_DJICameraControl_VerticalUp();

	//归中
	void on_pushButton_DJICameraControl_Center();

	//焦距
	void slider_horizontalSlider_DJICameraControl_FocalLength(int value);

	//下发任务
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Prepare();

	//执行任务
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Execute();

	//取消任务
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Undo();

	//航线暂停
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Pause();

	//航线恢复
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_FlightTask_Recovery();

	//一键返航
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home();

	//取消返航
	void on_pushButton_DJI_Dock__Drone_WayLine_Control_Return_Home_Cancel();

	//空中下发航线
	void on_pushButton_DJI_Dock_WayLine_Control_Flysendroute();

	//飞行控制权抢夺
	void on_pushButton_command_flight_control_compete();

	//负载控制权抢夺 
	void on_pushButton_command_payload_anthority_grab();

	//切换拍照模式
	void on_camera_contral_mode_switch();

	//停止录像
	void on_pushButton_DJICameraControl_Video_stop();

	//导入任务
	void on_pushButton_takeoff_load_task();

	//一键起飞
	void on_pushButton_takeoff_to_point();

	//flyto目标点
	void on_pushButton_command_flyto_targetpoint();

	//结束飞向目标点
	void on_pushButton_command_flyto_endtask();

	//更新飞向目标点
	void on_pushButton_command_flyto_updatepoint();

	//DRC急停
	void on_pushButton_DJI_Dock_Command_DRCemergency_Stop();

	//进入指令飞行控制模式
	void on_pushButton_DJI_Dock_Command_DRCMode_enter();

	//退出指令飞行控制模式
	void on_pushButton_DJI_Dock_Command_DRCMode_exit();

	//指定home点的返航
	void on_pushButton_DJI_Dock_WayLine_SelectedHome_ReturnRoute();

	//暂停空中航线
	void on_pushButton_DJI_Dock_WayLine_AirRoute_Stop();

	//恢复空中航线
	void on_pushButton_DJI_Dock_WayLine_AirRoute_Recover();
public:
	qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskPrepare m_djiFlighttaskPrepare;

private:
	qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint* m_djiDjiTakeoffToPoint;

public:
	void Load_Dji_FlighttaskPrepare(qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskPrepare& djiFlighttaskPrepare, bool flight_safety_advance_check = false, bool executableConditions = false, bool break_point = false, bool simulate_mission = false);

	void Load_Dji_SelectedPoint_flight(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint& djiDjiTakeoffToPoint);

	void Flyto_Dji_takeoff_to_point(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint& djiDjiTakeoffToPoint);

	void Flyto_Dji_takeoff_to_point_Update(qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint& djiDjiTakeoffToPoint);
private:
    void WeaponControl_Before(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle);
    void WeaponControl_After(qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle);
    void SendFlyCmdFun(std::function<void (qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*)>& vehicleFun);


protected:
    virtual void changeEvent(QEvent *event);


private:
    Ui::UAVFlyManage_DJI_DockDialog *ui;

    UAVSelectDialog*                 m_pUAVSelectDialog;

	bool							b_isStartVideo = false;
};

#endif // UAVFLYMANAGE_DJI_DOCKDIALOG_H
