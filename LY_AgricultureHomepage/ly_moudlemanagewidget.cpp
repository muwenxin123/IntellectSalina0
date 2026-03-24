#include "ly_moudlemanagewidget.h"
#include "ui_ly_moudlemanagewidget.h"
#include "LY_AgricultureHomepage.h"
#include "ly_monitortreewidget.h"
#include <QDebug>
#include <iostream>
#include <string>
#include  <QPainter>
#include  <QStyleOption>
#include "camerawidget.h"
#include <QVBoxLayout>
#include "LyMessageBox.h"
#include <QTreeWidgetItem>
#include "LY_AgricultureHomepageInterface.h"
#include "LY_AgricultureHomepageActivator.h"

class LY_MointorViewModel;

LY_MoudleManageWidget::LY_MoudleManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_MoudleManageWidget)
{
    ui->setupUi(this);

	//this->move(100, 160);
	//this->setMinimumSize(QSize(1820,920));
	this->setFixedSize(QSize(1530, 1060));
	//m_cameraLayoutWidget = new LY_CameraLayoutWidget();
	m_cameraWidget = new CameraWidget();
	QVBoxLayout *v_layout = new QVBoxLayout();
	v_layout->addWidget(m_cameraWidget);
	ui->camgeWidget->setLayout(v_layout);

	LY_AgricultureHomepage::Get().setupWidgetLocal(this, "LY_MoudleManageWidget", "Home");

	ui->stackedWidget->setCurrentIndex(0);

	ui->deviceicon->setPixmap(QPixmap(":/images/mointor/soilMax.png"));

	ui->on_biaopai_box->setChecked(true);
	ui->on_shichang_box->setChecked(true);
	ui->on_sensor_box->setChecked(true);


	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setRowCount(0);



	connect(ui->on_biaopai_box, &QCheckBox::stateChanged, this, [this](int state) {

		LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->showBrand(state == 0 ? false : true);
	});
	connect(ui->on_shichang_box, &QCheckBox::stateChanged, this, [this](int state) {

		LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->showView(state == 0 ? false : true);
	});
	connect(ui->on_sensor_box, &QCheckBox::stateChanged, this, [this](int state) {

		LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->showSensor(state == 0 ? false : true);
	});
	connect(ui->treeWidget, &LY_MonitorTreeWidget::clickTreeItem, this, &LY_MoudleManageWidget::showWidget);
	connect(ui->treeWidget, &LY_MonitorTreeWidget::doubleClickTreeItem, this, &LY_MoudleManageWidget::handleDoubleClickTreeItem);
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
		return;
	}

	qRegisterMetaType<Soil>("Soil");
	connect(viewModel, &LY_MointorViewModel::receiveSoilData, this, &LY_MoudleManageWidget::setSoilData, Qt::QueuedConnection);

	connect(viewModel, &LY_MointorViewModel::receiveWeather, this, &LY_MoudleManageWidget::showWeather);
	ui->weathicon->setPixmap(QPixmap(":/images/mointor/weatherMax.png"));
	ui->wea_tem->setText(tr2("温度") + tr2("℃"));
	ui->wea_hum->setText(tr2("湿度") + "%RH");
}

void LY_MoudleManageWidget::updateData()
{
	if (ui && ui->treeWidget)
	{
		ui->treeWidget->updateData();
	}
	
}

void  LY_MoudleManageWidget::setSoilData(Soil s)
{
	
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
		return;
	}
	if (s.nodeName == u8"土壤温湿度1")
	{
		ui->hum_tem_label->setText(QString::fromStdString(s.nodeName));
		ui->tem->setText(QString::fromStdString(s.temName + s.temUnit));
		ui->hum->setText(QString::fromStdString(s.humName + s.humUnit));
		ui->tem_num->setText(QString::fromStdString(s.temValueStr));
		ui->hum_num->setText(QString::fromStdString(s.humValueStr));
		//th = ui->tem->text() + ui->tem_num->text() + ui->hum->text() + ui->hum_num->text();
		th = QString::fromStdString(s.temName) + ": "
			+ ui->tem_num->text() + QString::fromStdString(s.temUnit) + "  "
			+ QString::fromStdString(s.humName) + ": "
			+ ui->hum_num->text() + QString::fromStdString(s.humUnit);
		
	}
	else if (s.nodeName == u8"土壤PH1、EC1")
	{
		ui->soil_ph_ec->setText(QString::fromStdString(s.nodeName));
		ui->ph->setText(QString::fromStdString(s.temName + s.temUnit));
		ui->ec->setText(QString::fromStdString(s.humName + s.humUnit));
		ui->ph_num->setText(QString::fromStdString(s.temValueStr));
		ui->ec_num->setText(QString::fromStdString(s.humValueStr));
		//pe = ui->ph->text() + ui->ph_num->text() + ui->ec->text() + ui->ec_num->text();

		pe = QString::fromStdString(s.temName) + " :"
			+ ui->ph_num->text() + QString::fromStdString(s.temUnit) + "  "
			+ QString::fromStdString(s.humName) + ": "
			+ ui->ec_num->text() + QString::fromStdString(s.humUnit);

	}
	else if (s.nodeName == "K")
	{
		ui->soil_k->setText(QString::fromStdString(s.nodeName));
		ui->K->setText(QString::fromStdString(s.humName + s.humUnit));
		ui->k_num->setText(QString::fromStdString(s.humValueStr));
		k = ui->K->text() + ui->k_num->text();

	}
	else if (s.nodeName == u8"N、P")
	{
		ui->soil_n_p->setText(QString::fromStdString(s.nodeName));
		ui->N->setText(QString::fromStdString(s.temName + s.temUnit));
		ui->P->setText(QString::fromStdString(s.humName + s.humUnit));
		ui->N_num->setText(QString::fromStdString(s.temValueStr));
		ui->P_num->setText(QString::fromStdString(s.humValueStr));
		//np =  ui->N->text() + ui->N_num->text() + ui->P->text() + ui->P_num->text();

		np = QString::fromStdString(s.temName) + ": "
			+ ui->N_num->text() + QString::fromStdString(s.temUnit) + "  "
			+ QString::fromStdString(s.humName) + ": "
			+ ui->P_num->text() + QString::fromStdString(s.humUnit);
	}
	soidData = th.toStdString() + "\n" + pe.toStdString() + "\n" + k.toStdString() + "\n" + np.toStdString();
	//qDebug() << "112===:" << QString::fromStdString(soidData);
}

void LY_MoudleManageWidget::showParentWidget(QString str, int value)
{
	if (str == tr2("实时监控"))
	{

	}
}
void LY_MoudleManageWidget::draw3dText()
{
	/*
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
	return;
	}
	if (m_isClickSoil)
	{
	viewModel->clearn3dText();
	viewModel->drawMointer3dText(soidData, m_sensorListModel, 20);
	}
	*/
}

bool LY_MoudleManageWidget::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (ui->treeWidget)
	{
		ui->treeWidget->DrawDynamic(painter, pProj);
	}

	return true;
}

void LY_MoudleManageWidget::showWidget(SensorListModel &model, int column)
{
	m_isClickSoil = false;
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel)
	{
		viewModel->clearn3dText();
	}
	LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->drawMointer3dText(model);
	if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
	{
		ui->stackedWidget->setCurrentIndex(3);
		//m_cameraLayoutWidget->cutOneWidget(column + 1);
		m_cameraWidget->stopPlay();
		m_cameraWidget->startPlay(model.requsetUrl);
	}
	else if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Weather)
	{
		ui->stackedWidget->setCurrentIndex(1);

		ui->w_name->setText(model.mointName);
		ui->w_alt->setText(QString::number(model.localHeight, 'f', 2));
		ui->w_lon->setText(QString::number(model.longitude, 'f', 7));
		ui->w_lat->setText(QString::number(model.latitude, 'f', 7));
		ui->w_serson_id->setText(model.mointID);
		//ui->moint_model_L->setText(model.mointModenlName);
	}
	else if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_PestMonitor)
	{
		ui->stackedWidget->setCurrentIndex(2);

		ui->w_name->setText(model.mointName);
		ui->w_alt->setText(QString::number(model.localHeight, 'f', 2));
		ui->w_lon->setText(QString::number(model.longitude, 'f', 7));
		ui->w_lat->setText(QString::number(model.latitude, 'f', 7));
		ui->w_serson_id->setText(model.mointID);
		//ui->moint_model_L->setText(model.mointModenlName);
	}
	else if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Soil)
	{
		m_isClickSoil = true;
		LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
		if (viewModel == nullptr)
		{
			return;
		}
		
		m_sensorListModel = model;
		viewModel->GetSoilTargetEnabledNodeList();

		connect(viewModel, &LY_MointorViewModel::getSoilSensorDataType, this, &LY_MoudleManageWidget::getSoilSensorDataType, Qt::QueuedConnection);
		connect(viewModel, &LY_MointorViewModel::getSoilHistoryList, this, &LY_MoudleManageWidget::getSoilHistoryList, Qt::QueuedConnection);
		connect(ui->soil_type, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_soil_typeChanged(const int &)));

		ui->soil_type->setCurrentIndex(0);

		ui->stackedWidget->setCurrentIndex(0);

		ui->moint_type_L->setText(model.mointType);
		ui->moint_name_L->setText(model.mointName);
		ui->moint_ID_L->setText(model.mointID);
		ui->moint_model_L->setText(model.mointModenlName);
		ui->moint_lotHeight_L->setText(QString::number(model.localHeight, 'f', 2));
		ui->moint_lon_L->setText(QString::number(model.longitude, 'f', 7));
		ui->moint_lat_L->setText(QString::number(model.latitude, 'f', 7));
		ui->moint_url_L->setText(model.requsetUrl);
	}
}

void LY_MoudleManageWidget::handleDoubleClickTreeItem(SensorListModel &model, int column)
{
	QString str = "CenterAt(" + QString::number(model.longitude, 'f', 8) + "," + QString::number(model.latitude, 'f', 8) + ")";
	LY_AgricultureHomepageInterface::Get().Script(str.toLocal8Bit());
}

void LY_MoudleManageWidget::getSoilSensorDataType(QVector <SoilSensorData> vec)
{
	ui->soil_type->clear();
	for (size_t i = 0; i < vec.size(); i++)
	{
		SoilSensorData data = vec.at(i);
		ui->soil_type->addItem(QString::fromStdString(data.nodeName), QVariant::fromValue(data));
	}
}
void LY_MoudleManageWidget::getSoilHistoryList(SoilHistoryData data)
{

	QStringList recode_header;
	if (m_data.nodeId == 3)
	{
		recode_header << tr2("节点名称") << tr2("土壤温度") + tr2("℃") << tr2("土壤湿度")  + tr2("℃") << tr2("记录时间");
	}
	else if (m_data.nodeId == 4)
	{
		recode_header << tr2("节点名称") << tr2("土壤PH1")  << tr2("土壤EC1(us/cm)")  << tr2("记录时间");
	}
	else if (m_data.nodeId == 28)
	{
		recode_header << tr2("节点名称") << tr2("N(mg/kg)") << tr2("P(mg/kg)") << tr2("记录时间");
	}
	else if (m_data.nodeId == 29)
	{
		recode_header << tr2("节点名称") << tr2("K(mg/kg)") << tr2("记录时间");
	}
	
	ui->tableWidget->setColumnCount(recode_header.size());
	ui->tableWidget->setHorizontalHeaderLabels(recode_header);

	if (data.rows.size() <= 0) return;
	ui->tableWidget->setRowCount(data.rows.size());
	for (size_t i = 0; i < data.rows.size(); i++)
	{
		SoilHistoryItemData model = data.rows.at(i);
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
		item1->setData(Qt::UserRole, QVariant::fromValue(data));
		ui->tableWidget->setItem(i, 0, item1);
		ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		if (m_data.nodeId == 29)
		{
			QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(model.humStr));
			QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(model.recordTime));
			
			ui->tableWidget->setItem(i, 1, item3);
			ui->tableWidget->setItem(i, 2, item4);

			ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			continue;
		}
		QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(model.temStr));
		QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(model.humStr));
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(model.recordTime));

		ui->tableWidget->setItem(i, 1, item2);
		ui->tableWidget->setItem(i, 2, item3);
		ui->tableWidget->setItem(i, 3, item4);

		ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
}
void LY_MoudleManageWidget::on_soil_typeChanged(const int &)
{
	SoilSensorData data = ui->soil_type->currentData().value<SoilSensorData>();
	m_data = data;
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
		return;
	}
	viewModel->GetSoilHistoryDataList(QString::number(data.nodeId));

}
void LY_MoudleManageWidget::showWeather(QString &tem, QString &hum)
{
	ui->wea_tem_hum_label->setText(tr2("温湿度传感器"));

	ui->wea_tem_num->setText(tem);
	ui->wea_hum_num->setText(hum);


}
void  LY_MoudleManageWidget::closeWidget()
{
	LY_AgricultureHomepage::Get().GetMoudleManagerWidget()->hide();

}
void LY_MoudleManageWidget::clickDeviceMonitor()
{
	

}
void LY_MoudleManageWidget::clickMeteorMonitor()
{
	
	ui->stackedWidget->setCurrentIndex(1);

}
void LY_MoudleManageWidget::clickSoilMonitor()
{
	ui->stackedWidget->setCurrentIndex(1);
}
void LY_MoudleManageWidget::clickWormMonitor()
{
	ui->stackedWidget->setCurrentIndex(1);
}
void LY_MoudleManageWidget::clickVideoMonitor()
{
	ui->stackedWidget->setCurrentIndex(1);
}


LY_MoudleManageWidget::~LY_MoudleManageWidget()
{
    delete ui;
}
void LY_MoudleManageWidget::paintEvent(QPaintEvent *event)
{
	//如果不写此代码,会导致画面在地图上透明
	QPainter painter(this);
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

}