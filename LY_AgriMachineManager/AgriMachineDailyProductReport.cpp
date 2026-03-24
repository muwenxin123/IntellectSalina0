#include "QCString.h"

#include "AgriMachineDailyProductReport.h"
#include "ui_AgriMachineDailyProductReport.h"

AgriMachineDailyProductReport::AgriMachineDailyProductReport(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineDailyProductReport)
{
	ui->setupUi(this);
}

AgriMachineDailyProductReport::~AgriMachineDailyProductReport()
{
	delete ui;
}