#include "QCString.h"

#include "AgriMachineManagerCurStatus.h"
#include "ui_AgriMachineManagerCurStatus.h"

AgriMachineManagerCurStatus::AgriMachineManagerCurStatus(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineManagerCurStatus)
{
	ui->setupUi(this);

	ui->current_state->setText(tr2("作业中"));
	ui->machine_state->setText(tr2("正常"));
	ui->current_farm->setText(tr2("十三陵农场"));
	ui->driver->setText(tr2("张三"));
	ui->vehicle_pos->setText("116.1253145,36.5647124"); 
	ui->pos_time->setText("2024 / 12 / 24 15:35 : 25");
	ui->fuel_consume->setText("100 / 356");
	ui->road_haul->setText("10.254 km");
}

AgriMachineManagerCurStatus::~AgriMachineManagerCurStatus()
{
	delete ui;
}