#include "ly_addmointtypewidget.h"
#include "ui_ly_addmointtypewidget.h"
#include "LyMessageBox.h"
#include <qstring.h>
#include "QCString.h"

Ly_AddMointTypeWidget::Ly_AddMointTypeWidget(QWidget *parent) :
	LyDialog(tr2("传感器类型添加"), parent),
	ui(new Ui::Ly_AddMointTypeWidget)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->setMinimumSize(350, 200);

	connect(ui->sensor_type_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_sensor_type_box(const int &)));

	ui->sensor_type_box->addItem("");
	ui->sensor_type_box->addItem(tr2("实时监控"), QVariant::fromValue(SensorType::SensorTypeType_Video));
	ui->sensor_type_box->addItem(tr2("土壤传感器"), QVariant::fromValue(SensorType::SensorTypeType_Soil));
	ui->sensor_type_box->addItem(tr2("气象传感器"), QVariant::fromValue(SensorType::SensorTypeType_Weather));
	ui->sensor_type_box->addItem(tr2("虫情检测仪"), QVariant::fromValue(SensorType::SensorTypeType_PestMonitor));
	ui->sensor_type_box->addItem(tr2("声光驱虫仪"), QVariant::fromValue(SensorType::SensorTypeType_KillInsects));
	ui->sensor_type_box->addItem(tr2("智能灌溉阀"), QVariant::fromValue(SensorType::SensorTypeType_Valve));
	ui->sensor_type_box->addItem(tr2("农机进入点"), QVariant::fromValue(SensorType::SensorTypeType_FarmCar));
	ui->sensor_type_box->addItem(tr2("大疆机场"), QVariant::fromValue(SensorType::SensorTypeType_FlyStop));

	connect(ui->sureAction, &QPushButton::clicked, this, [=]() {

	
		emit getMointName(m_sensorType);
		this->close();
		
		
	});

	connect(ui->cancleAction, &QPushButton::clicked, this, [=]() {

		this->close();
	});
}
void Ly_AddMointTypeWidget::on_sensor_type_box(const int &index)
{
	//sensor_name = GetSensorStringWithType(static_cast<SensorType>(index));
	m_sensorType = ui->sensor_type_box->currentData().value<SensorType>();
}
Ly_AddMointTypeWidget::~Ly_AddMointTypeWidget()
{
    delete ui;
}
