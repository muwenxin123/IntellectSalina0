#include "ly_mointeraddwidget.h"
#include "ui_ly_mointeraddwidget.h"
#include <qstring.h>
#include "QCString.h"
#include "LY_AgricultureHomepage.h"
#include "ly_mointorviewmodel.h"
#include "LyMessageBox.h"
#include  <QComboBox>
#include <QDateTime>

LY_MointerAddWidget::LY_MointerAddWidget(QWidget *parent) :
	LyDialog(tr2("传感器管理"), parent),
    ui(new Ui::LY_MointerAddWidget)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->setMinimumSize(350, 400);

	m_viewModel = LY_AgricultureHomepage::Get().GetViewModel();

	connect(ui->sureBtn, &QPushButton::clicked, this, &LY_MointerAddWidget::sureAction);
	connect(ui->cancleBtn, &QPushButton::clicked, this, &LY_MointerAddWidget::cancleAction);
	connect(ui->fetchPoint, &QPushButton::clicked, this, &LY_MointerAddWidget::getScreenPoint);
	
	connect(ui->isShow, &QCheckBox::clicked, this, &LY_MointerAddWidget::on_isShow_stateChanged);
	connect(ui->isStatus, &QCheckBox::clicked, this, &LY_MointerAddWidget::on_isStatus_stateChanged);
	connect(ui->isFieldOfView, &QCheckBox::clicked, this, &LY_MointerAddWidget::on_isFieldOfView_stateChanged);
	connect(ui->elevation_slider, &QSlider::valueChanged, this, &LY_MointerAddWidget::on_elevation_slider_valueChanged);
	connect(ui->depression_slider, &QSlider::valueChanged, this, &LY_MointerAddWidget::on_depression_slider_valueChanged);

	ui->elevation_slider->setMinimum(0);
	ui->elevation_slider->setMaximum(180);
	ui->depression_slider->setMinimum(-90);
	ui->depression_slider->setMaximum(0);

	ui->fetchPoint->setChecked(true);
	ui->fetchPoint->setText(tr2("开始取点"));
	ui->isShow->setChecked(true);
	ui->isStatus->setChecked(true);
	ui->isFieldOfView->setChecked(true);
	ui->ID->setEnabled(false);
	QDateTime currentDateTime = QDateTime::currentDateTime();
	qint64 timestamp = currentDateTime.toSecsSinceEpoch();
	ui->ID->setText(QString::number(timestamp));
}
void LY_MointerAddWidget::on_elevation_slider_valueChanged(int value)
{
	ui->orientation_edit->setText(QString::number(value));
}
void LY_MointerAddWidget::on_depression_slider_valueChanged(int value)
{
	ui->depression_edit->setText(QString::number(value));
}

void LY_MointerAddWidget::getScreenPoint()
{
	if (ui->fetchPoint->isCheckable())
	{
		ui->fetchPoint->setCheckable(false);
		ui->fetchPoint->setText(tr2("开始取点"));
	}
	else
	{
		ui->fetchPoint->setCheckable(true);
		ui->fetchPoint->setText(tr2("结束取点"));
	}
	ui->longitude->setText(QString::number(m_viewModel->lon, 'f', 7));
	ui->latitude->setText(QString::number(m_viewModel->lat, 'f', 7));
	ui->localHigh->setText(QString::number(m_viewModel->altitude, 'f', 2));
}
void LY_MointerAddWidget::setData(SensorListModel& listModel)
{
	currentModel = listModel;
	ui->mointName->setText(listModel.mointName);
	ui->ID->setText(listModel.mointID);
	ui->modelName->setText(listModel.mointModenlName);
	ui->localHigh->setText(QString::number(listModel.localHeight, 'f', 2));
	ui->longitude->setText(QString::number(listModel.longitude, 'f', 7));
	ui->latitude->setText(QString::number(listModel.latitude, 'f', 7));
	ui->textEdit->setText(listModel.requsetUrl);

	if (static_cast<SensorType>(listModel.typeID) == SensorType::SensorTypeType_Video)
	{
		ui->depression_edit->setText(QString::number(listModel.yj));
		ui->orientation_edit->setText(QString::number(listModel.fw));
		ui->perspective_h->setText(QString::number(listModel.spsj));
		ui->perspective_v->setText(QString::number(listModel.czsj));
		ui->elevation_slider->setValue(listModel.yj);
		ui->depression_slider->setValue(listModel.fw);
	}

	/*SensorListModel *m = m_viewModel->readStructLocalFile(listModel);
	if (m != nullptr)
	{
		ui->isShow->setChecked(m->isShow);
	}*/

}
void LY_MointerAddWidget::setSensorType(SensorType type)
{
	m_sensorType = type;

	if (type == SensorType::SensorTypeType_Video)
	{
		isShow = false;
		ui->isStatus->hide();
		isMap = true;
		ui->isFieldOfView->show();
		ui->cameraWidget->show();
		this->setMinimumSize(350, 520);
	}
	else
	{
		isShow = true;
		ui->isStatus->show();
		isMap = false;
		ui->isFieldOfView->hide();
		ui->cameraWidget->hide();
		this->setMinimumSize(350, 400);

	}
}
void LY_MointerAddWidget::setType(int type)
{
	ui->fetchPoint->setText(tr2("开始取点"));
	m_type = type;
	if (m_type == 2)
	{
		//ui->ID->setEnabled(false);
		ui->mointName->setEnabled(false);
	}
	else
	{
		//ui->ID->setEnabled(true);
		ui->mointName->setEnabled(true);
	}

}
void LY_MointerAddWidget::sureAction()
{
	int typeID = 0;
	SensorListModel listModel;
	//listModel.id = 1;
	listModel.mointName = ui->mointName->text();
	listModel.mointID = ui->ID->text();
	listModel.mointType = GlobalUtils::GetSensorStringWithType(m_sensorType);
	listModel.mointModenlName = ui->modelName->text();
	listModel.longitude = ui->longitude->text().toDouble();
	listModel.latitude = ui->latitude->text().toDouble();
	listModel.requsetUrl = ui->textEdit->toPlainText();
	listModel.screenX = m_viewModel->screenX;
	listModel.screenY = m_viewModel->screenY;
	listModel.localHeight = ui->localHigh->text().toDouble();
	listModel.isShow = isShow;
	listModel.isStatus = isStatus;
	listModel.isMap = isMap;
	listModel.typeID = static_cast<int>(m_sensorType);
	if (m_sensorType == SensorType::SensorTypeType_Video)
	{
		listModel.yj = ui->depression_edit->text().toInt();
		listModel.fw = ui->orientation_edit->text().toInt();
		listModel.spsj = ui->perspective_h->text().toInt();
		listModel.czsj = ui->perspective_v->text().toInt();
	}
	else
	{
		listModel.yj = 0;
		listModel.fw = 0;
		listModel.spsj = 0;
		listModel.czsj = 0;
	}

	if (m_type == 1)//添加
	{
		this->close();
		
		emit addSensor(listModel);
		//	LY_AgricultureHomepage::Get().GetViewModel()->addStructLocalFile(listModel);
		//	LY_AgricultureHomepage::Get().GetViewModel()->addSqlData(listModel);

	}
	else if (m_type == 2) { //修改

		this->close();
		listModel.sensor_type_id = currentModel.sensor_type_id;
		emit updateSensor(listModel);
	/*	LY_AgricultureHomepage::Get().GetViewModel()->updateSqlData(listModel);
		LY_AgricultureHomepage::Get().GetViewModel()->editStructLocalFile(listModel);*/
	}
}
void LY_MointerAddWidget::cancleAction()
{
	this->close();
}
LY_MointerAddWidget::~LY_MointerAddWidget()
{
    delete ui;
}

void LY_MointerAddWidget::on_isShow_stateChanged(int arg1)
{
	currentModel.isShow = arg1;
	LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->editStructLocalFile(currentModel);

}

void LY_MointerAddWidget::on_isFieldOfView_stateChanged(int arg1)
{
	currentModel.isMap = arg1;
	LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->editStructLocalFile(currentModel);
}

void LY_MointerAddWidget::on_isStatus_stateChanged(int arg1)
{
	currentModel.isStatus = arg1;
	LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->editStructLocalFile(currentModel);
}
