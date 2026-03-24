#include "ly_harmmanagerwidget.h"
#include "ui_ly_harmmanagerwidget.h"

#include <QImage>
#include <QFile>

#include <QDebug>
#include <QDateTime>
#include <qstring.h>
#include "QCString.h"
#include <QBuffer>
#include "LyMessageBox.h"
#include "LY_AgricultureHomepageInterface.h"

#define OPENCV
#include "darknet/yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <direct.h>
#include <io.h>
#include "PutTextZH.h"
#include <iosfwd>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QThread>
#include "LY_AgricultureHomepage.h"
#include "ly_mointorviewmodel.h"
#include "LY_AgricultureHomepageActivator.h"
#include <QSettings>
#include <QHBoxLayout>
#include <QDebug>


std::map<int, std::string> name_map2{
	{ 0, "maize rough dwarf disease" },
	{ 1, "puccinia polysra" },
	{ 2, "leaf blight of corn" },
	{ 3, "leaf spot of corn" },
	{ 4, "Maize sheath blight" },
	{ 5, "Maize Head Smut" },
	{ 6, "common smut" },
	{ 7, "Maize stalk rot" },
	{ 8, "Bacterial stem rot of Maize" },
	{ 9,"gray leaf spot" },
	{ 10,"corn brown spot" },
	{ 11,"Alternaria alternata leaf blight of Maize" },
	{ 12,"Curvularia leaf spot of Maize" },
	{ 13,"Corn anthracnose" },
	{ 14,"Corn eye spot" },
	{ 15,"Corn take all" },
	{ 16,"Maize Downy mildew" },
	{ 17,"Corn dry rot" },
	{ 18,"Maize scab" },
	{ 19,"Maize dwarf mosaic virus disease" },
	{ 20,"Maize stripe dwarf disease" },
	{ 21,"Empty corn stalk" },
	{ 22,"Corn top rot" },
	{ 23,"aphid" },
	{ 24,"bollworm" },
	{ 25,"Beet armyworm" },
	{ 26,"Spodoptera litura" },
	{ 27,"corn borer" },
	{ 28,"armyworm" },
	{ 29,"Spodoptera frugiperda" },
	{ 30,"Athetis lepigone" },
	{ 31,"thrips" },
	{ 32,"cutworm" },
	{ 33,"Corn sunflower mealybug" },
	{ 34,"Scarab Beetle" },
	{ 35,"Oriental Migratory Locust" },
	{ 36,"Atractomorpha sinensis" },
	{ 37,"Miliary stink bug" }
};

LY_HarmManagerWidget::LY_HarmManagerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LY_HarmManagerWidget)
{
	ui->setupUi(this);
	LY_AgricultureHomepage::Get().setupWidgetLocal(this, "LY_HarmManagerWidget", "Home");
	init();

}
 void LY_HarmManagerWidget::showEvent(QShowEvent *event)
{  
	 // 查询当前激活农场
	 QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	 ini.beginGroup("Agri");
	 QString m_currentFarm = ini.value("FarmID", "").toString();


	 // m_vecFieldList.clear();
	 AgriFieldInfoModelVec m_vecFieldList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetFieldInfoList(m_currentFarm);

	 ui->nt_comboBox->clear();
	 ui->nt_comboBox->addItem("");
	 for (auto& fieldInfo : m_vecFieldList)
	 {
		 // 匹配激活农场
		 if (m_currentFarm == fieldInfo.farm_id)
		 {
			 QVariant var;
			 var.setValue(fieldInfo);
			 ui->nt_comboBox->addItem(fieldInfo.name, var);
		 }
	 }

}
void LY_HarmManagerWidget::init()
{

	QHBoxLayout *h_layout = new QHBoxLayout(ui->cameraWidget);
	cameraLayoutWIdget = new LY_CameraLayoutWidget(ui->cameraWidget);
	cameraLayoutWIdget->setFixedSize(938, 680);
	h_layout->addWidget(cameraLayoutWIdget);
	ui->cameraWidget->setLayout(h_layout);

	QHBoxLayout *photo_layout = new QHBoxLayout(ui->photoWidget);
	photoLayoutWidget = new LY_PhotoLayoutWidget(ui->photoWidget);
	photoLayoutWidget->setFixedSize(938, 650);
	photo_layout->addWidget(photoLayoutWidget);
	ui->photoWidget->setLayout(photo_layout);

	ui->qb_comboBox->addItem(tr2("虫情"));

	ui->qbly_comboBox->addItem(tr2("苗青监测仪"));
	//ui->qbly_comboBox->addItem(tr2("图片"));

	cameraLayoutWIdget->on_pushButton_2_clicked();
	photoLayoutWidget->on_pushButton_2_clicked();
	//cameraLayoutWIdget->cutOneWidget(0);

	std::vector<QString> timeVec = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetPestHarmTime();


	std::set<QString> uniqueSet(timeVec.begin(), timeVec.end());
	timeVec = std::vector<QString>(uniqueSet.begin(), uniqueSet.end());

	ui->comboBox->clear();
	ui->comboBox->addItem("");
	for (auto m : timeVec)
	{
		ui->comboBox->addItem(m);
	}

	QStringList headerlabels;
	headerlabels << tr2("编号") << tr2(" 采样时间 ") << tr2(" 高度 ") << tr2("经度") << tr2("纬度") << tr2("虫害类型") << tr2("虫害阶段") << tr2("虫害数量") << tr2("虫害范围") << tr2("虫害等级")  << tr2("识别后的图");

	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	connect(ui->pthtoBtn, &QPushButton::clicked, this, &LY_HarmManagerWidget::clickPtotoBtn);
	
	connect(ui->qb_comboBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_qbComboBoxChanged(const int &)));
	connect(ui->qbly_comboBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_qblyComboBoxChanged(const int &)));
	connect(ui->comboBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_histyComboBoxChanged(const int &)));
	connect(ui->nt_comboBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_nt_comboBox_Changed(const int &)));
	connect(this, &LY_HarmManagerWidget::sendOrignalImage, this, &LY_HarmManagerWidget::receiveOrignalImage);
	connect(this, &LY_HarmManagerWidget::sendRecognitionlImage, this, &LY_HarmManagerWidget::receiveRecognitionlImage);
	//connect(ui->sbxx_comboBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_sbxx_comboBox_Changed(const int &)));



	connect(ui->sbxx_comboBox, &LY_CustomComboBox::selectionChange, this, [this](SensorListModelVec sensorListVec) {

		SensorListModelVec vec = filterDuplicateMointIDs(sensorListVec);

		cameraLayoutWIdget->setUrlVec(vec);

		
		if (vec.size() > 1 && vec.size() <= 4)
		{
			cameraLayoutWIdget->on_pushButton_2_clicked();
			photoLayoutWidget->on_pushButton_2_clicked();
		}
		else if (vec.size() == 1)
		{
			cameraLayoutWIdget->cutOneWidget(0);
			photoLayoutWidget->cutOneWidget(0);
		}
		else if (vec.size() > 4 && vec.size() <= 8)
		{
			cameraLayoutWIdget->on_pushButton_3_clicked();
			photoLayoutWidget->on_pushButton_3_clicked();
		}
		else
		{
			cameraLayoutWIdget->on_pushButton_4_clicked();
			photoLayoutWidget->on_pushButton_4_clicked();
		}

		
	});

}

void LY_HarmManagerWidget::on_sbxx_comboBox_Changed(const int &index) {


}
void LY_HarmManagerWidget::on_nt_comboBox_Changed(const int &index)
{
	ui->sbxx_comboBox->clearnItem();
	AgriFieldInfoModel fieldInfo = ui->nt_comboBox->itemData(index).value<AgriFieldInfoModel>();
	SensorTypeModelVec typeVec = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorTypeList(fieldInfo.id);
	for (auto &typeModel: typeVec) {
		SensorListModelVec listVec = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorList(typeModel.id);
		for (SensorListModel &model: listVec) {
			if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
			{
				QVariant var;
				var.setValue(model);

				ui->sbxx_comboBox->addItem(model, var);
			}
		}
	}
}
void LY_HarmManagerWidget::on_qbComboBoxChanged(const int &)
{

}
void LY_HarmManagerWidget::on_qblyComboBoxChanged(const int &)
{

}
void LY_HarmManagerWidget::on_histyComboBoxChanged(const int &)
{
	//LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetPestHarmTime();

	QString str = ui->comboBox->currentText();
	if (str.length() <= 0 )
	{
		return;
	}
	m_pestList.clear();
	m_pestList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetPestHarmData(str);
	setItem(m_pestList);
}
void LY_HarmManagerWidget::setItem(PestPhotoInficationVec vec)
{
	ui->tableWidget->setRowCount(vec.size());
	int index = 0;
	for (auto model : vec)
	{
		ui->tableWidget->setItem(index, 0, new QTableWidgetItem(QString::number(index)));
		ui->tableWidget->setItem(index, 1, new QTableWidgetItem(model.recode_time));
		ui->tableWidget->setItem(index, 2, new QTableWidgetItem(QString::number(model.localHeight, 'f', 2)));
		ui->tableWidget->setItem(index, 3, new QTableWidgetItem(QString::number(model.longitude, 'f', 2)));
		ui->tableWidget->setItem(index, 4, new QTableWidgetItem(QString::number(model.latitude, 'f', 2)));
		ui->tableWidget->setItem(index, 5, new QTableWidgetItem(model.pest_type));
		ui->tableWidget->setItem(index, 6, new QTableWidgetItem(model.pest_stage));
		ui->tableWidget->setItem(index, 7, new QTableWidgetItem(QString::number(model.pest_number)));
		ui->tableWidget->setItem(index, 8, new QTableWidgetItem(QString::number(model.recode_range)));
		ui->tableWidget->setItem(index, 9, new QTableWidgetItem(model.pest_grade));

		QLabel *label = new QLabel();
		QImage image;
		if (image.loadFromData(model.recongzine_byteArray, "PNG")) {
			QPixmap pixmap = QPixmap::fromImage(image);
			label->setPixmap(pixmap);
			label->setFixedSize(QSize(30, 30));
			//label->setStyleSheet("background-color: rgb(14, 18, 28);");
			label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);//将图片在QLabel中居中
			ui->tableWidget->setCellWidget(index, 10, label);
		}

		index++;
	}
}
void LY_HarmManagerWidget::clickPtotoBtn()
{
	//LY_AgricultureHarmmanager::Get().GetViewModel()->draw3DMointorImg(116.1964204 + 0.0002, 40.2646477 + 0.0002);

	cameraLayoutWIdget->addPhoto();

	photoLayoutWidget->setPhoto(cameraLayoutWIdget->m_photoVec);
	if (photoLayoutWidget->m_pestPhotoVec.size() > 0)
	{
		for (const auto& pest: photoLayoutWidget->m_pestPhotoVec)
		{
			m_pestList.push_back(pest);

		}

		setItem(m_pestList);
	}
	cameraLayoutWIdget->m_photoVec.clear();

	//std::thread([this]() {

	//}).detach();
}

void LY_HarmManagerWidget::receiveOrignalImage(QImage image)
{
	if (image.isNull())
	{
		return;
	}
	QPixmap originaPixmap = QPixmap::fromImage(image);
	//ui->originalPhotoL->setPixmap(originaPixmap);
	QDateTime originaCurrentDateTime = QDateTime::currentDateTime();
	QString originaFormattedDateTime = originaCurrentDateTime.toString("yyyy-MM-dd HH:mm:ss");
	//ui->origninalTine->setText(originaFormattedDateTime);


	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG");  // 使用PNG格式保存图像到buffer
	buffer.close();
	original_byteArray = buffer.buffer();  // 获取完整的字节数组


}
void LY_HarmManagerWidget::receiveRecognitionlImage(QImage image)
{

	
}
void LY_HarmManagerWidget::paintEvent(QPaintEvent *event)
{
	//QPainter painter(this);
	//if (m_image.size().width() <= 0)
	//	return;

	//QImage img = m_image.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	//int x = ui->label->geometry().x();
	//int y = ui->label->geometry().y();
	//painter.drawImage(QPoint(x, y), img);

}


LY_HarmManagerWidget::~LY_HarmManagerWidget()
{
	delete ui;
}
