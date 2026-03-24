#include "QCString.h"

#include "AgriMachineMonthlyProductReport.h"
#include "ui_AgriMachineMonthlyProductReport.h"

AgriMachineMonthlyProductReport::AgriMachineMonthlyProductReport(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineMonthlyProductReport)
{
	ui->setupUi(this);
}

AgriMachineMonthlyProductReport::~AgriMachineMonthlyProductReport()
{
	delete ui;
}