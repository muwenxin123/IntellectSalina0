#include "IUAVFlyManage_DJI_SendTask_Dialog.h"
#include "ui_IUAVFlyManage_DJI_SendTask_Dialog.h"
#include "QCString.h"
#include "LY_UAVFlyControl_DJI_Dock.h"
#include "IUAVFlyManage_DJI_WayLine_UploadDialog.h"
#include "LyMessageBox.h"
#include <QDateTime>
#include <QDateTimeEdit>
#include <dji/sdk/cloudapi/wayline/dji_flight_task_prepare.h>
#include <QComboBox>
#include <QRandomGenerator>
IUAVFlyManage_DJI_SendTask_Dialog::IUAVFlyManage_DJI_SendTask_Dialog(QWidget *parent) :
    LyDialog(tr2("下发任务"), parent),
    ui(new Ui::IUAVFlyManage_DJI_SendTask_Dialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	resize(650, 780);

	Init();

	connect(ui->pushButton_routeline_file_load, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_routeline_file_load);
	connect(ui->pushButton_DJITask_general, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_general);
	connect(ui->pushButton_DJITask_leapfrog, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_leapfrog);
	connect(ui->pushButton_DJITask_RunCondition, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_RunCondition);

	connect(ui->pushButton__wayline_precision_GPS, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton__wayline_precision_GPS);
	connect(ui->pushButton__wayline_precision__RTK, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton__wayline_precision__RTK);
	connect(ui->pushButton_simulation_open, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_simulation_open);
	
	connect(ui->pushButton_safecheck_open, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_safecheck_open);

	connect(ui->pushButton_DJI_flighttask_confirm, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJI_flighttask_confirm);
	connect(ui->pushButton_DJI_flighttask_cancel, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJI_flighttask_cancel);
	connect(ui->comboBox_lineEdit_DJI_flighttask_files, &QComboBox::currentTextChanged, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_comboBox_lineEdit_DJI_flighttask_files);

	connect(ui->pushButton_mapSelectPoint, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_mapSelectPoint);

	connect(ui->pushButton_updateName, &QPushButton::clicked, this, &IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_updateName);
	// 	ui->lineEdit_DJI_flighttask_files_URL->setEnabled(false);
 	ui->lineEdit_DJI_flighttask_files_fingerprint->setEnabled(false);
}

IUAVFlyManage_DJI_SendTask_Dialog::~IUAVFlyManage_DJI_SendTask_Dialog()
{
    delete ui;
}

void IUAVFlyManage_DJI_SendTask_Dialog::Init()
{
	//按钮选中
	ui->pushButton_DJITask_general->setCheckable(true);

	ui->pushButton_DJITask_general->setChecked(true);
	ui->groupBox->setVisible(false);

	ui->pushButton_DJITask_leapfrog->setCheckable(true);
	ui->pushButton_DJITask_RunCondition->setCheckable(true);
	ui->pushButton__wayline_precision_GPS->setCheckable(true);
	ui->pushButton__wayline_precision__RTK->setCheckable(true);
	ui->pushButton__wayline_precision__RTK->setChecked(true);

	ui->pushButton_simulation_open->setCheckable(true);
	ui->pushButton_simulation_open->setChecked(false);
	ui->pushButton_safecheck_open->setCheckable(true);



	ui->pushButton_DJITask_general->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton_DJITask_leapfrog->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton_DJITask_RunCondition->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton__wayline_precision_GPS->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton__wayline_precision__RTK->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton_simulation_open->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
	ui->pushButton_safecheck_open->setStyleSheet(QString("QPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));

	ui->comboBox_lineEdit_DJI_flighttask_files->clear();
	QStringList fileList = LY_UAVFlyControl_DJI_Dock::Get().listObjectsInBucket(s_bucket);
	if (fileList.size() > 0)
	{
		ui->comboBox_lineEdit_DJI_flighttask_files->addItem("");
		ui->comboBox_lineEdit_DJI_flighttask_files->addItems(fileList);
	}

	QString strID = generateRandomAlphaNumericString();
	ui->lineEdit_DJI_flighttask_task_Id->setText(strID);
}

void IUAVFlyManage_DJI_SendTask_Dialog::getListObjectsInBucket()
{
	ui->comboBox_lineEdit_DJI_flighttask_files->clear();
	QStringList fileList = LY_UAVFlyControl_DJI_Dock::Get().listObjectsInBucket(s_bucket);
	if (fileList.size() > 0)
	{
		ui->comboBox_lineEdit_DJI_flighttask_files->addItem("");
		ui->comboBox_lineEdit_DJI_flighttask_files->addItems(fileList);
	}
}

double IUAVFlyManage_DJI_SendTask_Dialog::getReturnRouteHeight()
{
	double value = ui->doubleSpinBox_DJI_flighttask_rthaltitude->value();
	return value;
}

QString IUAVFlyManage_DJI_SendTask_Dialog::generateRandomAlphaNumericString(int length)
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

int IUAVFlyManage_DJI_SendTask_Dialog::getCurrentTaskTypeButton()
{
	if (ui->pushButton_DJITask_general->isChecked())
	{
		return 0;
	}
	if (ui->pushButton_DJITask_leapfrog->isChecked())
	{
		return 1;
	}
	if (ui->pushButton_DJITask_RunCondition->isChecked())
	{
		return 2;
	}
	return -1;
}

int IUAVFlyManage_DJI_SendTask_Dialog::getCurrentWaylinePrecision()
{
	if (ui->pushButton__wayline_precision_GPS->isChecked())
	{
		return 0;
	}
	if (ui->pushButton__wayline_precision__RTK->isChecked())
	{
		return 1;
	}
	return -1;
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJI_flighttask_confirm()
{
	QString taskID = ui->lineEdit_DJI_flighttask_task_Id->text();
	if (taskID.isEmpty())
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("计划名不能为空！"), QMessageBox::Yes);
		return;
	}

 	QDateTime dateTime_starttime = ui->dateTimeEdit_lineEdit_DJI_flighttask_starttime->dateTime();
 
 	// 转换为毫秒时间戳 (自1970-01-01 00:00:00 UTC以来的毫秒数)
	qint64 timestamp_starttime = QDateTime::currentMSecsSinceEpoch() + 6000; // dateTime_starttime.toMSecsSinceEpoch() + 6000;
 
 	int taskType = getCurrentTaskTypeButton();
 	if (taskType == -1 || taskType > 2)
 	{
 		LyMessageBox::information(nullptr, tr2("提示"), tr2("任务类型为[%1],检查任务类型！").arg(taskType), QMessageBox::Yes);
 		return;
 	}
 
 	QString fileURL = ui->lineEdit_DJI_flighttask_files_URL->text();
 	if(fileURL.isEmpty())
 	{
 		LyMessageBox::information(nullptr, tr2("提示"), tr2("文件URL为空！"), QMessageBox::Yes);
		return;
 	}
 
 	QString fileFingerprint = ui->lineEdit_DJI_flighttask_files_fingerprint->text();
 	if(fileFingerprint.isEmpty())
 	{
 		LyMessageBox::information(nullptr, tr2("提示"), tr2("文件签名为空！"), QMessageBox::Yes);
		return;
 	}
 
 	//QString taskReadyconditions = ui->textEdit_DJI_flighttask_readyconditions->toPlainText();
	QString  taskReadyconditions = ui->lineEdit_DJI_flighttask_storage_capacity->text();
	if (taskReadyconditions.isEmpty())
	{

	}
 
 	int batterycapacity = ui->lineEdit_DJI_flighttask_batterycapacity->text().toInt();
 	
 	QDateTime dateTime_can_startTime = ui->dateTimeEdit_DJI_flighttask_begin_time->dateTime();
 
 	qint64 timestamp_can_startTime = dateTime_can_startTime.toMSecsSinceEpoch();
 	if (timestamp_can_startTime < timestamp_starttime)
 	{
 		//LyMessageBox::information(nullptr, tr2("提示"), tr2("任务可执行时段截止时间毫秒时间戳，任务开始执行的时间必须小于！").arg(taskType), QMessageBox::Yes);
 		//return;
 	}
 
 	QDateTime dateTime_can_endtime = ui->dateTimeEdit_DJI_flighttask_begin_time->dateTime();
 
 	qint64 timestamp_can_endtime = dateTime_can_endtime.toMSecsSinceEpoch();
 	if (timestamp_can_endtime > timestamp_starttime)
 	{
 		//LyMessageBox::information(nullptr, tr2("提示"), tr2("任务可执行时段起始时间毫秒时间戳，任务开始执行的时间必须大于！").arg(taskType), QMessageBox::Yes);
 		//return;
 	}
 
 	//QString executionConditions = ui->textEdit_DJI_flighttask_executable_conditions->toPlainText();
 
 	int storageCapacity = ui->lineEdit_DJI_flighttask_storage_capacity->text().toInt();
 
 	QString waylineID = ui->comboBox_lineEdit_DJI_flighttask_files->currentText();
 	if (waylineID.isEmpty())
 	{
 		//LyMessageBox::information(nullptr, tr2("提示"), tr2("航线ID不能为空！").arg(taskType), QMessageBox::Yes);
 		//return;
 	}
 	int returnRoute_height = ui->doubleSpinBox_DJI_flighttask_rthaltitude->value();
	if (returnRoute_height > 1500 || returnRoute_height < 2)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("飞行器返航高度不在[2-1500]区间内！").arg(taskType), QMessageBox::Yes);
		return;
	}
 	int out_of_action = ui->comboBox_DJI_flighttask_outcontrol_action->currentIndex();
 
 	int exit_wayline_rc_lost = ui->comboBox_DJI_flighttask_exit_wayline_rclost->currentIndex();
 
 	int wayline_precision_type = getCurrentWaylinePrecision();
	if (-1 == wayline_precision_type)
	{
		LyMessageBox::information(nullptr, tr2("提示"), tr2("航线精度类型为空！").arg(taskType), QMessageBox::Yes);
		return;
	}

	qnzkna::DjiSdkApi::DjiCloudApi::DjiSimulateSwitchEnum djiSimulateSwitch = qnzkna::DjiSdkApi::DjiCloudApi::DjiSimulateSwitchEnum::DjiSimulateSwitchEnum_DISABLE;
	if (ui->pushButton_simulation_open->isChecked())
	{
		djiSimulateSwitch = qnzkna::DjiSdkApi::DjiCloudApi::DjiSimulateSwitchEnum::DjiSimulateSwitchEnum_ENABLE;
	}
 
	int safety_advance_check = ui->pushButton_safecheck_open->isChecked();
 
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskPrepare dji;
 	dji.setFlightId(taskID);
 	dji.setExecuteTime(timestamp_starttime);
 	dji.setTaskType((qnzkna::DjiSdkApi::DjiCloudApi::DjiTaskTypeEnum)taskType);
 	dji.setWaylineType((qnzkna::DjiSdkApi::DjiCloudApi::DjiWaylineTypeEnum)0);
 
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiFlightTaskFile flightTaskFile;
 	flightTaskFile.setFingerprint(fileFingerprint);
 	flightTaskFile.setUrl(fileURL);
 	dji.setFile(flightTaskFile);
 
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiReadyConditions djiConditions;
 	djiConditions.setBatteryCapacity(batterycapacity);
 	djiConditions.setBeginTime(timestamp_can_startTime);
 	djiConditions.setEndTime(timestamp_can_endtime);
 	dji.setReadyConditions(djiConditions);
 
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiExecutableConditions djiExecution;
 	djiExecution.setStorageCapacity(storageCapacity);
 	dji.setExecutableConditions(djiExecution);
 
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskBreakPoint djiBreakPoint;
 	djiBreakPoint.setIndex(0);
 	djiBreakPoint.setState(1);
 	djiBreakPoint.setProgress(0);
 	djiBreakPoint.setWaylineId(waylineID.toInt());
 	dji.setBreakPoint(djiBreakPoint);
 
 	dji.setRthAltitude(returnRoute_height);
 	dji.setOutOfControlAction((qnzkna::DjiSdkApi::DjiCloudApi::DjiOutOfControlActionEnum)out_of_action);
 	dji.setExitWaylineWhenRcLost((qnzkna::DjiSdkApi::DjiCloudApi::DjiExitWaylineWhenRcLostEnum)exit_wayline_rc_lost);
 	dji.setRthMode((qnzkna::DjiSdkApi::DjiCloudApi::DjiRthModeEnum)1);
 	qnzkna::DjiSdkApi::DjiCloudApi::DjiSimulateMission djiSimulate;
 	djiSimulate.setIsEnable(djiSimulateSwitch);
	djiSimulate.setLongitude(ui->doubleSpinBox_simulation_Longitude->value());
	djiSimulate.setLatitude(ui->doubleSpinBox_simulation_latitude->value());
 	dji.setSimulateMission(djiSimulate);

	
 
 	dji.setWaylinePrecisionType((qnzkna::DjiSdkApi::DjiCloudApi::DjiWaylinePrecisionTypeEnum)wayline_precision_type);
 	dji.setFlightSafetyAdvanceCheck((qnzkna::DjiSdkApi::DjiCloudApi::DjiFlightSafetyAdvanceCheckEnum)safety_advance_check);
 	
 	LY_UAVFlyControl_DJI_Dock::Get().getPUAVFlyManage_DJI_DockDialog()->Load_Dji_FlighttaskPrepare(dji, safety_advance_check, false, false, (djiSimulateSwitch == qnzkna::DjiSdkApi::DjiCloudApi::DjiSimulateSwitchEnum::DjiSimulateSwitchEnum_ENABLE));
// 	close();
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJI_flighttask_cancel()
{
	close();
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_routeline_file_load()
{
	if (LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_WayLine_UploadDialog()
		&& LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_WayLine_UploadDialog()->isHidden())
	{
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_WayLine_UploadDialog()->Init();
		LY_UAVFlyControl_DJI_Dock::Get().getPIUAVFlyManage_DJI_WayLine_UploadDialog()->show();
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_general()
{
	if (ui->pushButton_DJITask_general->isChecked())
	{
		ui->pushButton_DJITask_leapfrog->setChecked(false);
		ui->pushButton_DJITask_RunCondition->setChecked(false);
		ui->groupBox->setVisible(false);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_leapfrog()
{
	if (ui->pushButton_DJITask_leapfrog->isChecked())
	{
		ui->pushButton_DJITask_general->setChecked(false);
		ui->pushButton_DJITask_RunCondition->setChecked(false);
		ui->groupBox->setVisible(false);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_DJITask_RunCondition()
{
	if (ui->pushButton_DJITask_RunCondition->isChecked())
	{
		ui->pushButton_DJITask_leapfrog->setChecked(false);
		ui->pushButton_DJITask_general->setChecked(false);
		ui->groupBox->setVisible(true);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton__wayline_precision_GPS()
{
	if (ui->pushButton__wayline_precision_GPS->isChecked())
	{
		ui->pushButton__wayline_precision__RTK->setChecked(false);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton__wayline_precision__RTK()
{
	if (ui->pushButton__wayline_precision__RTK->isChecked())
	{
		ui->pushButton__wayline_precision_GPS->setChecked(false);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_simulation_open()
{
	if (ui->pushButton_simulation_open->isChecked())
	{
		ui->groupBox_4->setVisible(true);
	}
	else {
		ui->groupBox_4->setVisible(false);
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_safecheck_open()
{
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_comboBox_lineEdit_DJI_flighttask_files()
{
	QString str = ui->comboBox_lineEdit_DJI_flighttask_files->currentText();
	if (str.isEmpty()) {
		return;
	}
	std::string  url, md5;
	bool bsucc = LY_UAVFlyControl_DJI_Dock::Get().getRouteLineURLorMD5Fingerpoint(str.toStdString(), url,md5);
	if (bsucc)
	{
		ui->lineEdit_DJI_flighttask_files_URL->setText(tr2(url.c_str()));
		ui->lineEdit_DJI_flighttask_files_fingerprint->setText(md5.c_str());
	}
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_mapSelectPoint()
{
	b_isMapPoint = true;
}

void IUAVFlyManage_DJI_SendTask_Dialog::on_pushButton_updateName()
{
	QString str = generateRandomAlphaNumericString();
	ui->lineEdit_DJI_flighttask_task_Id->setText(str);
}

void IUAVFlyManage_DJI_SendTask_Dialog::setWaylineURLorFingerpoint(const QString& url, const QString& md5)
{
	ui->lineEdit_DJI_flighttask_files_URL->setText(url);
	ui->lineEdit_DJI_flighttask_files_fingerprint->setText(md5);
}

void IUAVFlyManage_DJI_SendTask_Dialog::setMapSelectedPointEdit(const double & dlon, const double & dlat)
{
	ui->doubleSpinBox_simulation_Longitude->setValue(dlon);
	ui->doubleSpinBox_simulation_latitude->setValue(dlat);
}
