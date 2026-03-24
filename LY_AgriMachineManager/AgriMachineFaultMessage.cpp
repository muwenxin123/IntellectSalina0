#include "QCString.h"

#include "AgriMachineFaultMessage.h"
#include "ui_AgriMachineFaultMessage.h"

AgriMachineFaultMessage::AgriMachineFaultMessage(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineFaultMessage)
{
	ui->setupUi(this);
}

AgriMachineFaultMessage::~AgriMachineFaultMessage()
{
	delete ui;
}