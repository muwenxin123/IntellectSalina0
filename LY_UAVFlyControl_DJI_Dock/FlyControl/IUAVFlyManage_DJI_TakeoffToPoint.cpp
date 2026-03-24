#include "IUAVFlyManage_DJI_TakeoffToPoint.h"
#include "ui_IUAVFlyManage_DJI_TakeoffToPoint.h"
#include "QCString.h"
#include "LyMessageBox.h"
#include "LY_UAVFlyControl_DJI_Dock.h"
#include "IUAVFlyManage_DJI_TakeoffToPoint.h"
#include "UAVFlyManage_DJI_DockDialog.h"
#include <QRandomGenerator>
IUAVFlyManage_DJI_TakeoffToPoint::IUAVFlyManage_DJI_TakeoffToPoint(QWidget *parent) :
	LyDialog(tr2("导入数据"),parent),
    ui(new Ui::IUAVFlyManage_DJI_TakeoffToPoint)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	connect(ui->pushButton_confirm, &QPushButton::clicked, this, &IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_confirm);

	connect(ui->checkBox_isopen_simulator, &QCheckBox::stateChanged, this, &IUAVFlyManage_DJI_TakeoffToPoint::on_checkBox_isopen_simulator);

	connect(ui->pushButton_map_selecting_point, &QPushButton::clicked, this, &IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_map_selecting_point);

	connect(ui->pushButton_simulator_map_selectedpoint, &QPushButton::clicked, this, &IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_simulator_map_selectedpoint);

	connect(ui->pushButton_updateID, &QPushButton::clicked, this, &IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_updateID);

	ui->groupBox_2->setEnabled(false);

	ui->checkBox_isexecution_in_simulator->setChecked(true);
// 	ui->lineEdit_targetpoint_lontitude->setText(QString::number(115.9079211, 'f', 6));
// 	ui->lineEdit_targetpoint_latitude->setText(QString::number(40.3766835, 'f', 6));
// 	ui->lineEdit_taskId->setText("507b97f1-efa3-4cee-9121-84e2c47ff0f7");

	if (ui->comboBox_return_wayline_mode->count() >= 2) {
		ui->comboBox_return_wayline_mode->setCurrentIndex(1);
	}
	
}

IUAVFlyManage_DJI_TakeoffToPoint::~IUAVFlyManage_DJI_TakeoffToPoint()
{
    delete ui;
}

void IUAVFlyManage_DJI_TakeoffToPoint::Init(int nmode)
{
	n_flyto_mode = nmode;
	UpdateControlStatus(nmode);

	QString str = generateRandomAlphaNumericString();
	ui->lineEdit_taskId->setText(str);
}

void IUAVFlyManage_DJI_TakeoffToPoint::setTargetPointLocation(const double & dlontitude, const double & dlatitude)
{
	ui->lineEdit_targetpoint_lontitude->setText(QString::number(dlontitude, 'f', 6));
	ui->lineEdit_targetpoint_latitude->setText(QString::number(dlatitude, 'f', 6));
	ui->doubleSpinBox_targetpoint_height->setValue(2);

	d_flyto_lon = dlontitude;
	d_flyto_lat = dlatitude;
}

void IUAVFlyManage_DJI_TakeoffToPoint::setSimulationPointLocation(const double & dlontitude, const double & dlatitude)
{
	ui->lineEdit_simulator_lontitude->setText(QString::number(dlontitude, 'f', 6));
	ui->lineEdit_simulator_latitude->setText(QString::number(dlatitude, 'f', 6));

	d_flyto_lon = dlontitude;
	d_flyto_lat = dlatitude;
}

void IUAVFlyManage_DJI_TakeoffToPoint::UpdateControlStatus(int bstatus)
{
	ui->doubleSpinBox_safeflight_altitude->setEnabled(!bstatus);
	ui->comboBox_return_wayline_mode->setEnabled(!bstatus);
	ui->doubleSpinBox_return_wayline_altitude->setEnabled(!bstatus);
	ui->comboBox_outof_remotecontrol_action->setEnabled(!bstatus);
	ui->comboBox_selected_flight_outof_control_action->setEnabled(!bstatus);
	ui->comboBox_selected_flight_mode->setEnabled(!bstatus);
	ui->doubleSpinBox_selected_flight_altitude->setEnabled(!bstatus);
	ui->checkBox_isopen_simulator->setEnabled(!bstatus);
	ui->groupBox_2->setEnabled(!bstatus);
	ui->checkBox_flight_safe_checked->setEnabled(!bstatus);
}

QString IUAVFlyManage_DJI_TakeoffToPoint::generateRandomAlphaNumericString(int length)
{

	const QString charset =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789";

	const int charsetLength = charset.length(); // 62 个字符

	QString result;
	result.reserve(length);

	for (int i = 0; i < length; ++i) {
		const int index = QRandomGenerator::global()->bounded(charsetLength);
		QChar randomChar = charset.at(index);
		result.append(randomChar);
	}

	return result;
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_confirm()
{
	QString taskId = ui->lineEdit_taskId->text();
	if (taskId.isEmpty())
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("任务ID不允许为空！"), QMessageBox::Yes);
		return;
	}

	bool bToOk = false;
	double dlontitude = ui->lineEdit_targetpoint_lontitude->text().toDouble(&bToOk);
	if (!bToOk)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("目标点经度格式错误！"), QMessageBox::Yes);
		return;
	}
	double dlatitude = ui->lineEdit_targetpoint_latitude->text().toDouble(&bToOk);
	if (!bToOk)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("目标点纬度格式错误！"), QMessageBox::Yes);
		return;
	}
	float targetHeight = ui->doubleSpinBox_targetpoint_height->value();
	if (targetHeight > 1500 || targetHeight < 2)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("目标点高度不属于[2-1500]区间！"), QMessageBox::Yes);
		return;
	}
	int max_speed = ui->doubleSpinBox_task_max_speed->value();
	if (max_speed < 1 || max_speed > 15)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("过程中能达到的最大速度未在[1-15]范围内！"), QMessageBox::Yes);
		return;
	}
	qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffToPoint m_takeOffToPoint;
	if (n_flyto_mode == 1)
	{
		m_takeOffToPoint.setTargetLatitude(dlatitude);
		m_takeOffToPoint.setTargetLontitude(dlontitude); 
		m_takeOffToPoint.setTargetFlightHeight(targetHeight);
		m_takeOffToPoint.setFlightID(taskId);
		m_takeOffToPoint.setFlightMaxSpeed(max_speed);

		LY_UAVFlyControl_DJI_Dock::Get().getPUAVFlyManage_DJI_DockDialog()->Flyto_Dji_takeoff_to_point(m_takeOffToPoint);
	}
	else if (n_flyto_mode == 2)
	{
		m_takeOffToPoint.setTargetLatitude(dlatitude);
		m_takeOffToPoint.setTargetLontitude(dlontitude);
		m_takeOffToPoint.setTargetFlightHeight(targetHeight);
		m_takeOffToPoint.setFlightID(taskId);
		m_takeOffToPoint.setFlightMaxSpeed(max_speed);

		LY_UAVFlyControl_DJI_Dock::Get().getPUAVFlyManage_DJI_DockDialog()->Flyto_Dji_takeoff_to_point(m_takeOffToPoint);
	}
	else
	{
		bool bToOk = false;
		
		float safeFlightHeight = ui->doubleSpinBox_safeflight_altitude->value();
		if (safeFlightHeight > 1500 || safeFlightHeight < 20)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("安全起飞高度不属于[20-1500]区间！"), QMessageBox::Yes);
			return;
		}
		int return_wayline_mode = ui->comboBox_return_wayline_mode->currentIndex();
		if (return_wayline_mode < 0 || return_wayline_mode > 2)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("返航模式设置错误！"), QMessageBox::Yes);
			return;
		}
		int return_wayline_height = ui->doubleSpinBox_return_wayline_altitude->value();
		if (return_wayline_height > 1500 || return_wayline_height < 2)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("返航高度不属于[2-1500]区间！"), QMessageBox::Yes);
			return;
		}
		int remote_outof_control = ui->comboBox_outof_remotecontrol_action->currentIndex();
		if (remote_outof_control < 0 || remote_outof_control > 3)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("遥控器失控动作设置错误！"), QMessageBox::Yes);
			return;
		}
		int selected_flight_point = ui->comboBox_selected_flight_outof_control_action->currentIndex();
		if (selected_flight_point < 0 || selected_flight_point > 2)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("指点飞行失控动作设置错误！"), QMessageBox::Yes);
			return;
		}
		int selected_flight_mode = ui->comboBox_selected_flight_mode->currentIndex();

		float selected_flight_height = ui->doubleSpinBox_selected_flight_altitude->value();
		if (selected_flight_height < 2 || selected_flight_height > 3000)
		{
			LyMessageBox::information(nullptr, tr2("提示"), tr2("指点飞行高度未在[2-3000]范围内！"), QMessageBox::Yes);
			return;
		}

		bool safeFlightCheck = ui->checkBox_flight_safe_checked->isChecked();

		m_takeOffToPoint.setTargetLatitude(dlatitude);
		m_takeOffToPoint.setTargetLontitude(dlontitude);
		m_takeOffToPoint.setTargetFlightHeight(targetHeight);
		m_takeOffToPoint.setSecurityTakeoffHeight(safeFlightHeight);
		m_takeOffToPoint.setRTHMode((qnzkna::DjiSdkApi::DjiCloudApi::RTH_Mode)return_wayline_mode);
		m_takeOffToPoint.setRTHAltitude(return_wayline_height);
		m_takeOffToPoint.setRemoteControlLostAction((qnzkna::DjiSdkApi::DjiCloudApi::RemoteControlLostAction)remote_outof_control);
		m_takeOffToPoint.setCommanderModeLostAction((qnzkna::DjiSdkApi::DjiCloudApi::CommanderModeLostActionTask)selected_flight_point);
		m_takeOffToPoint.setCommanderFlightMode((qnzkna::DjiSdkApi::DjiCloudApi::CommanderFlightMode)selected_flight_mode);
		m_takeOffToPoint.settFcommanderFlightHeight(selected_flight_height);
		m_takeOffToPoint.setFlightID(taskId);
		m_takeOffToPoint.setFlightMaxSpeed(max_speed);

		bool isOpenSimulate = ui->checkBox_isopen_simulator->isChecked();

		qnzkna::DjiSdkApi::DjiCloudApi::DjiTakeoffSimulateMission takeoff_simulate;

		if (isOpenSimulate) {
			takeoff_simulate.setSimulateEnable(1);
		}
		else {
			takeoff_simulate.setSimulateEnable(0);
		}

		if (isOpenSimulate)
		{
			double simulate_lon = ui->lineEdit_simulator_lontitude->text().toDouble(&bToOk);
			if (!bToOk)
			{
				LyMessageBox::information(nullptr, tr2("提示"), tr2("模拟器经度格式错误！"), QMessageBox::Yes);
				return;
			}
			double simulate_lat = ui->lineEdit_simulator_latitude->text().toDouble(&bToOk);
			if (!bToOk)
			{
				LyMessageBox::information(nullptr, tr2("提示"), tr2("模拟器纬度格式错误！"), QMessageBox::Yes);
				return;
			}
			if (isOpenSimulate)
			{
				takeoff_simulate.setSimulateLat(simulate_lat);
				takeoff_simulate.setSimulateLon(simulate_lon);
			}
		}
		m_takeOffToPoint.setTakeoffSimulateMission(takeoff_simulate);
		m_takeOffToPoint.setFlightSafetyAdvanceCheck(safeFlightCheck);

		LY_UAVFlyControl_DJI_Dock::Get().getPUAVFlyManage_DJI_DockDialog()->Load_Dji_SelectedPoint_flight(m_takeOffToPoint);
		close();
	}
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_cancal()
{
	close();
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_checkBox_isopen_simulator()
{
	bool isOpenSimulate = ui->checkBox_isopen_simulator->isChecked();
	if (isOpenSimulate)
	{
		ui->groupBox_2->setEnabled(true);
	}
	else
	{
		ui->groupBox_2->setEnabled(false);
	}
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_map_selecting_point()
{
	b_map_selecting_point = true;
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_simulator_map_selectedpoint()
{
	b_simulation_selecting_point = true;
}

void IUAVFlyManage_DJI_TakeoffToPoint::on_pushButton_updateID()
{
	QString str = generateRandomAlphaNumericString();
	ui->lineEdit_taskId->setText(str);
}

void IUAVFlyManage_DJI_TakeoffToPoint::DrawTargetPointData(QPainter & painter, IProj * pProj)
{
	QPoint ptPoint1 = pProj->Jwd2Screen(d_flyto_lon, d_flyto_lat);
	QColor greeb1 = 0x0000ff;
	QPen pen1(greeb1);
	pen1.setWidth(3);
	painter.setPen(pen1);
	painter.setBrush(greeb1);
	painter.drawPoint(ptPoint1);
}
