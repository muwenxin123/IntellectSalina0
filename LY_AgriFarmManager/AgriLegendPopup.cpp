#include "AgriLegendPopup.h"
#include "ui_agrilegendpopup.h"

AgriLegendPopup::AgriLegendPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriLegendPopup)
{
    ui->setupUi(this);

	QPixmap bkgnd(":/images/AgriFarm/divice_bg.png");
	bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QPalette palette;
	palette.setBrush(QPalette::Background, bkgnd);
	setPalette(palette);
	setAutoFillBackground(true); // 关键点：启用自动填充背景

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_ShowWithoutActivating);


	ui->label->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->weather_L->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->soil_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_6->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->camera_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_8->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->mointer_pest_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_10->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->voice_pest_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_14->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->Irrigation_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_16->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->DJ_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_18->setStyleSheet("font-size: 20px; color: #ffffff; ");
	ui->device_enter_L->setStyleSheet("font-size: 20px; color: #00eaff; ");
	ui->label_12->setStyleSheet("font-size: 20px; color: #ffffff; ");
	

}
void AgriLegendPopup::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	//QPixmap background("/images/AgriFarm/divice_bg.png");
	//painter.drawPixmap(rect(), background);

	//QWidget::paintEvent(event);
}
void AgriLegendPopup::updateFieldInfo(AgriFieldInfo &info)
{

	std::vector<SensorListModel> soilSensorVec;
	std::vector<SensorListModel> weatherSensorVec;
	std::vector<SensorListModel> videoSensorVec;
	std::vector<SensorListModel> pestSensorVec;
	std::vector<SensorListModel> killPestSensorVec;
	std::vector<SensorListModel> valveSensorVec;
	std::vector<SensorListModel> agriMachineSensorVec;
	std::vector<SensorListModel> flySensorVec;
	SensorListModelVec sensorList = IAgriFarmManagerActivator::Get().getIAgricultureHomePageService()->GetSensorListWithFieldID(info.id);
	for (const auto sensor : sensorList)
	{
		int typeID = sensor.typeID;

		if (typeID == 1)
		{
			videoSensorVec.push_back(sensor);
		}
		if (typeID == 2)
		{
			soilSensorVec.push_back(sensor);
		}
		if (typeID == 3)
		{
			weatherSensorVec.push_back(sensor);
		}
		else if (typeID == 4)
		{
			pestSensorVec.push_back(sensor);
		}
		else if (typeID == 5)
		{
			killPestSensorVec.push_back(sensor);
		}
		else if (typeID == 6)
		{
			valveSensorVec.push_back(sensor);
		}
		else if (typeID == 7)
		{
			agriMachineSensorVec.push_back(sensor);
		}
		else if (typeID == 8)
		{
			flySensorVec.push_back(sensor);
		}
	}
	QString solilNumber = QString::number(soilSensorVec.size()) /*+ tr2("个")*/;
	QString weatherNumber = QString::number(weatherSensorVec.size()) /*+ tr2("个")*/;
	QString sensorNumber = QString::number(videoSensorVec.size()) /*+ tr2("个")*/;
	QString pestNumber = QString::number(pestSensorVec.size()) /*+ tr2("台")*/;
	QString killPestNumber = QString::number(killPestSensorVec.size()) /*+ tr2("台")*/;
	QString valueNumber = QString::number(valveSensorVec.size()) /*+ tr2("台")*/;
	QString agriMachineNumber = QString::number(agriMachineSensorVec.size()) /*+ tr2("台")*/;
	QString flyNumber = QString::number(flySensorVec.size()) /*+ tr2("个")*/;

	ui->weather_L->setText(weatherNumber);
	ui->label_6->setText(solilNumber);
	ui->label_8->setText(sensorNumber);
	ui->label_10->setText(pestNumber);
	ui->label_14->setText(killPestNumber);
	ui->label_16->setText(valueNumber);
	ui->label_18->setText(flyNumber);
	ui->label_12->setText(agriMachineNumber);
}
AgriLegendPopup::~AgriLegendPopup()
{
    delete ui;
}
