#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QMovie>
#include <QBuffer>

#include "QCString.h"
#include "LyMessageBox.h"
#include "AgriPestAndDiseaseDataCollectDialog.h"
#include "ui_AgriPestAndDiseaseDataCollectDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "AgriDataAnalysisPredictUtil.h"
#include "LY_AgriDataAnalysisPredict.h"
#include "IAgriDataAnalysisPredictActivator.h"


AgriPestAndDiseaseDataCollectDialog::AgriPestAndDiseaseDataCollectDialog(DataCollectType collectType, AgriPestDataset dataset, QWidget *parent /*= nullptr*/)
  :LyDialog([&]()->QString { 
	QString titleName = tr2("未知");
	switch (collectType)
	{
	case DataCollectType::DATA_COLLECT_ARTIFICIAL:
		titleName = tr2("人工现场调查-添加");
		break;
	case DataCollectType::DATA_COLLECT_SENSOR:
		titleName = tr2("传感器图像识别-添加");
		break;
	case DataCollectType::DATA_COLLECT_IMAGE:
		titleName = tr2("图片文件识别-添加");
		break;
	case DataCollectType::DATA_COLLECT_IMPORT:
		titleName = tr2("数据文件导入-添加");
		break;
	default:
		break;
	}
	return titleName;
  }(), parent)
	, ui(new Ui::AgriPestAndDiseaseDataCollectDialog)
{
	m_eCerateType = CREATE_TYPE_ADD;

	m_eCollectType = collectType;

	m_PestDataset = dataset;

	// 生成数据集uid
	m_DataCollectUid = AgriDataAnalysisPredictUtil::GenerateUuid();

	Init();
}

AgriPestAndDiseaseDataCollectDialog::AgriPestAndDiseaseDataCollectDialog(DataCollectType collectType, AgriPestDataset dataset, AgriPestCollection collect, QWidget *parent /*= nullptr*/)
	  :LyDialog([&]()->QString {
	  QString titleName = tr2("未知");
	  switch (collectType)
	  {
	  case DataCollectType::DATA_COLLECT_ARTIFICIAL:
		  titleName = tr2("人工现场调查-修改");
		  break;
	  case DataCollectType::DATA_COLLECT_SENSOR:
		  titleName = tr2("传感器图像识别-修改");
		  break;
	  case DataCollectType::DATA_COLLECT_IMAGE:
		  titleName = tr2("图片文件识别-修改");
		  break;
	  case DataCollectType::DATA_COLLECT_IMPORT:
		  titleName = tr2("数据文件导入-修改");
		  break;
	  default:
		  break;
	  }
	  return titleName;
  }(), parent)
	, ui(new Ui::AgriPestAndDiseaseDataCollectDialog)
{
	m_eCerateType = CREATE_TYPE_UPDATE;

	m_eCollectType = collectType;

	m_PestDataset = dataset;

	m_Pestcollect = collect;

	m_DataCollectUid = m_Pestcollect.guid;

	Init();

	InitDisplayInfo();
}

  AgriPestAndDiseaseDataCollectDialog::AgriPestAndDiseaseDataCollectDialog(AgriPestDataset dataset, AgriPestCollection collect, DataCollectType collectType, QWidget *parent /*= nullptr*/)
	  :LyDialog([&]()->QString {
	  QString titleName = tr2("未知");
	  switch (collectType)
	  {
	  case DataCollectType::DATA_COLLECT_ARTIFICIAL:
		  titleName = tr2("人工现场调查-查看");
		  break;
	  case DataCollectType::DATA_COLLECT_SENSOR:
		  titleName = tr2("传感器图像识别-查看");
		  break;
	  case DataCollectType::DATA_COLLECT_IMAGE:
		  titleName = tr2("图片文件识别-查看");
		  break;
	  case DataCollectType::DATA_COLLECT_IMPORT:
		  titleName = tr2("数据文件导入-查看");
		  break;
	  default:
		  break;
	  }
	  return titleName;
  }(), parent)
	  , ui(new Ui::AgriPestAndDiseaseDataCollectDialog)
  {
	  m_eCerateType = CREATE_TYPE_VIWE;

	  m_eCollectType = collectType;

	  m_PestDataset = dataset;

	  m_Pestcollect = collect;

	  Init();

	  InitDisplayInfo();

	  // 按钮禁用
	  ui->combo_sensor->setDisabled(true);
	  ui->btnPick->setDisabled(true);
	  ui->btnSelect->setDisabled(true);

	  // 按钮隐藏
	  ui->btnOK->hide();
	  ui->btnCancel->hide();
  }

  AgriPestAndDiseaseDataCollectDialog::~AgriPestAndDiseaseDataCollectDialog()
{
    delete ui;
}

void AgriPestAndDiseaseDataCollectDialog::InitActiveFieldZone(const qnzkna::ZoneManage::CZoneBase& zone)
{
	if (m_curZone.stZonePointVec.empty())
	{
		m_curZone = zone;
		m_bInitZone = true;
	}
}

void AgriPestAndDiseaseDataCollectDialog::updateTargetPoint(double dLon, double dLat, float fAlt)
{
	if (!m_bPick) return;

	// 获取当前区域，判断是否在区域内
	std::vector<QPointF> zonePoints;
	for (const auto& stZonePoint : m_curZone.stZonePointVec)
	{
		QPointF tmpPointF(stZonePoint.dLon, stZonePoint.dLat);
		zonePoints.emplace_back(tmpPointF);
	}

	if (AgriDataAnalysisPredictUtil::JudgePointInZone_OddEven(QPointF(dLon, dLat), zonePoints)) {

		ui->target_pos_lon->setText(QString::number(dLon, 'f', 7));
		ui->target_pos_lat->setText(QString::number(dLat, 'f', 7));
		ui->target_pos_alt->setText(QString::number(fAlt, 'f', 3));
	}
	else
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("采集点未在当前盐田内！"));
	}

	this->show();

	// 恢复默认光标样式
	QApplication::restoreOverrideCursor();

	m_bPick = false;
}

void AgriPestAndDiseaseDataCollectDialog::closeEvent(QCloseEvent* event)
{

}

void AgriPestAndDiseaseDataCollectDialog::Init()
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	ui->collect_time->setDateTime(QDateTime::currentDateTime());
	ui->pest_count->setText("0");
	ui->pest_range->setText("1");
	ui->collect_point_name->setPlaceholderText(tr2("例：CJ_01"));

	InitDisplayCombox();

	switch (m_eCollectType)
	{
	case DataCollectType::DATA_COLLECT_ARTIFICIAL:
	{
		ui->sensor_recognize_widget->hide();
		ui->image_recognize_widget->hide();
	}
		break;
	case DataCollectType::DATA_COLLECT_SENSOR:
	{
		ui->image_recognize_widget->hide();
		// 查询盐田关联传感器（监控）采集图像列表
		m_vecAssociteIndef = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestPhotoIndeficationList(m_PestDataset.field_id);
		for (const auto& idef : m_vecAssociteIndef)
		{
			ui->combo_sensor->addItem(QString("%1_%2").arg(QString::number(idef->mointId)).arg(QString::number(idef->id)) , QVariant::fromValue(*(idef.get())));
		}

		// 下拉菜单点击
		connect(ui->combo_sensor, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_sensor_currentTextChanged(int)));

		if (ui->combo_sensor->count())
		{
			ui->combo_sensor->setCurrentIndex(0);
			emit ui->combo_sensor->currentIndexChanged(ui->combo_sensor->currentIndex());
		}
	}
		break;
	case DataCollectType::DATA_COLLECT_IMAGE:
	{
		ui->sensor_recognize_widget->hide();

		ui->image_path->setText(tr2("未选择图片"));

		QPixmap pixmap;
		pixmap.loadFromData(m_Pestcollect.recognize_image, "PNG");
		ui->image_recognize_image->setPixmap(pixmap);
		ui->image_recognize_image->setFixedSize(QSize(450, 300));
		ui->image_recognize_image->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);//将图片在QLabel中居中
		ui->image_recognize_image->setScaledContents(true);

		if (!m_recognizeInter)
		{
			m_recognizeInter = new recognize::AgriYoloRecognize;
		}

		connect(ui->btnSelect, &QPushButton::clicked, this, &AgriPestAndDiseaseDataCollectDialog::on_btnSelect_clicked);
	}
		break;
	case DataCollectType::DATA_COLLECT_IMPORT:
		break;
	default:
		break;
	}

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriPestAndDiseaseDataCollectDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriPestAndDiseaseDataCollectDialog::on_btnCancel_clicked);
	connect(ui->btnPick, &QPushButton::clicked, this, &AgriPestAndDiseaseDataCollectDialog::on_btnPick_clicked);
}


void AgriPestAndDiseaseDataCollectDialog::InitDisplayCombox()
{
	for (int i = 0; i <= 10; ++i)
	{
		PestLevel level = static_cast<PestLevel>(i);
		ui->combo_pest_level->addItem(AgriDataAnalysisPredictUtil::PestLevelToString(level), QVariant::fromValue(level));
	}

	for (int i = 1; i <= 3; ++i)
	{
		PestStage stage = static_cast<PestStage>(i);
		ui->combo_pest_stage->addItem(AgriDataAnalysisPredictUtil::PestStageToString(stage), QVariant::fromValue(stage));
	}

	for (int i = 1; i <= 19; ++i)
	{
		PestCategory category = static_cast<PestCategory>(i);
		ui->combo_pest_category->addItem(AgriDataAnalysisPredictUtil::PestCategoryToString(category), QVariant::fromValue(category));
	}

	// 下拉菜单点击
	connect(ui->combo_pest_category, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_crop_currentTextChanged(int)));
	connect(ui->combo_pest_stage, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_crop_variety_currentTextChanged(int)));
	connect(ui->combo_pest_level, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_plant_standard_currentTextChanged(int)));
}


void AgriPestAndDiseaseDataCollectDialog::InitDisplayInfo()
{
	ui->collect_point_name->setText(m_Pestcollect.name);

	for (int i = 0; i < ui->combo_sensor->count(); ++i) {
		auto itemData = ui->combo_sensor->itemData(i).value<AgriPestPhotoIndefication>();
		if (m_Pestcollect.sensor_id == QString::number(itemData.mointId))
		{
			ui->combo_sensor->setCurrentIndex(i);
			break;
		}
	}

	ui->collect_time->setDateTime(QDateTime::fromString(m_Pestcollect.collect_time, "yyyy-MM-dd HH:mm:ss"));
	ui->target_pos_lon->setText(QString::number(m_Pestcollect.lon, 'f', 7));
	ui->target_pos_lat->setText(QString::number(m_Pestcollect.lat, 'f', 7));
	ui->target_pos_alt->setText(QString::number(m_Pestcollect.alt));

	int nIndex = ui->combo_pest_category->findData(m_Pestcollect.category);
	if (nIndex != -1)
	{
		ui->combo_pest_category->setCurrentIndex(nIndex);
	}

	nIndex = ui->combo_pest_level->findData(m_Pestcollect.level);
	if (nIndex != -1)
	{
		ui->combo_pest_level->setCurrentIndex(nIndex);
	}

	nIndex = ui->combo_pest_stage->findData(m_Pestcollect.stage);
	if (nIndex != -1)
	{
		ui->combo_pest_stage->setCurrentIndex(nIndex);
	}

	ui->pest_count->setText(QString::number(m_Pestcollect.count));
	ui->pest_range->setText(QString::number(m_Pestcollect.range));
}

void AgriPestAndDiseaseDataCollectDialog::AddDataCollection(AgriPestCollectionPtr pestCollection)
{
	bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->AddPestCollection(pestCollection);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("存储数据库失败！"));
	}
	else
	{
		emit addDataCollectionFinish(pestCollection);
	}
}


void AgriPestAndDiseaseDataCollectDialog::UpdateDataCollection(AgriPestCollectionPtr pestCollection)
{
	bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->UpdatePestCollection(pestCollection);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("更新数据库失败！"));
	}
	else
	{
		emit updateDataCollectionFinish(pestCollection);
	}
}

void AgriPestAndDiseaseDataCollectDialog::on_btnOK_clicked()
{
	if (ui->collect_point_name->text().isEmpty())
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("采集点名称不能为空！"));
		return;
	}

	double tmpLon = ui->target_pos_lon->text().toDouble();
	double tmpLat = ui->target_pos_lat->text().toDouble();
	double tmpAlt = ui->target_pos_alt->text().toDouble();

	if (!tmpLon || !tmpLat)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("采集点位置不能为空！"));
		return;
	}

	AgriPestCollectionPtr pDataCollection = std::make_shared<AgriPestCollection>();
	pDataCollection->guid = m_DataCollectUid;
	pDataCollection->name = ui->collect_point_name->text();
	pDataCollection->dataset_id = m_PestDataset.guid;

	auto itemData = ui->combo_sensor->itemData(ui->combo_sensor->currentIndex()).value<AgriPestPhotoIndefication>();
	pDataCollection->sensor_id = itemData.mointId;
	
	pDataCollection->collect_type = static_cast<int>(m_eCollectType); // 单独匹配采集类型
	pDataCollection->collect_time = ui->collect_time->dateTime().toString("yyyy-MM-dd HH:mm:ss");
	pDataCollection->lon = tmpLon;
	pDataCollection->lat = tmpLat;
	pDataCollection->alt = tmpAlt;
	pDataCollection->category = ui->combo_pest_category->itemData(ui->combo_pest_category->currentIndex()).value<int>();
	pDataCollection->stage = ui->combo_pest_stage->itemData(ui->combo_pest_stage->currentIndex()).value<int>();
	pDataCollection->count = ui->pest_count->text().toInt();
	pDataCollection->range = ui->pest_range->text().toDouble();
	pDataCollection->level = ui->combo_pest_level->itemData(ui->combo_pest_level->currentIndex()).value<int>();
	
	const QPixmap *retrievedPixmap = ui->image_recognize_image->pixmap();
	if (retrievedPixmap) {
		QPixmap copiedPixmap = *retrievedPixmap;
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		copiedPixmap.toImage().save(&buffer, "PNG");
		pDataCollection->recognize_image = byteArray;
	}

	switch (m_eCollectType)
	{
	case DataCollectType::DATA_COLLECT_ARTIFICIAL:
	case DataCollectType::DATA_COLLECT_SENSOR:
	case DataCollectType::DATA_COLLECT_IMAGE:
	{
		switch (m_eCerateType)
		{
		case AgriPestAndDiseaseDataCollectDialog::CREATE_TYPE_UNKNOW:
		case AgriPestAndDiseaseDataCollectDialog::CREATE_TYPE_VIWE:
			break;
		case AgriPestAndDiseaseDataCollectDialog::CREATE_TYPE_ADD:
			AddDataCollection(pDataCollection);
			break;
		case AgriPestAndDiseaseDataCollectDialog::CREATE_TYPE_UPDATE:
			UpdateDataCollection(pDataCollection);
			break;
		default:
			break;
		}
	}
		break;
	case DataCollectType::DATA_COLLECT_IMPORT:
		break;
	default:
		break;
	}
}

void AgriPestAndDiseaseDataCollectDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}


void AgriPestAndDiseaseDataCollectDialog::on_btnPick_clicked()
{
	// 设置全局的自定义光标样式
	QApplication::setOverrideCursor(Qt::CrossCursor);

	m_bPick = true;

	this->hide();
}

void AgriPestAndDiseaseDataCollectDialog::on_btnSelect_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(this, "选择图片", "", "Images (*.png *.jpg *.jpeg *.bmp)");
	if (!filePath.isEmpty())
	{
		m_selectedImagePath = filePath;
		QPixmap pixmap(filePath);

		ui->image_path->setText(filePath);

		ui->image_recognize_image->setPixmap(pixmap);
		ui->image_recognize_image->setFixedSize(QSize(450, 300));
		ui->image_recognize_image->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);//将图片在QLabel中居中
		ui->image_recognize_image->setScaledContents(true);

		// 绝对定位 Loading
		if (!m_loadingMovie){
			m_loadingMovie = new QMovie(":/images/mointor/loading.gif");
			if (!m_loadingMovie->isValid()) {
				qDebug() << tr2("GIF 加载失败！请检查文件路径");
			}
			m_loadingMovie->setScaledSize(QSize(50, 50));
		}

		if (!m_loadingLabel)
		{
			m_loadingLabel = new QLabel(ui->image_recognize_image);
			m_loadingLabel->setAlignment(Qt::AlignCenter);
			m_loadingLabel->setFixedSize(450, 300);
			//m_loadingLabel->move(ui->image_recognize_image->pos().x(), ui->image_recognize_image->pos().y() - 50);
			m_loadingLabel->setStyleSheet("background-color: rgba(0, 0, 0, 100); color: white; font-size: 12px;");
		}
		
		m_loadingLabel->setMovie(m_loadingMovie);
		m_loadingLabel->setText(tr2("正在识别..."));
		m_loadingLabel->show();
		m_loadingLabel->raise(); // 放到最上层

		m_loadingMovie->start();

		std::thread worker([=]() {
			if (m_recognizeInter)
			{
				QImage originImage = pixmap.toImage();
				QImage recoginzedImage;
				QString recoginzedName;
				if (m_recognizeInter->Recognize(originImage, recoginzedImage, recoginzedName))
				{
					ui->image_recognize_image->setPixmap(QPixmap::fromImage(recoginzedImage));
					m_loadingLabel->hide();

					// 更新虫类别
					int nIndex = ui->combo_pest_category->findText(recoginzedName);
					if (nIndex != -1)
					{
						ui->combo_pest_category->setCurrentIndex(nIndex);
					}
					ui->combo_pest_stage->setCurrentIndex(static_cast<int>(PestStage::PEST_STAGE_3) - 1);
					ui->pest_count->setText(QString::number(1));
				}
				else
				{
					m_loadingLabel->setText(tr2("识别失败!"));
				}
			}
		});

		worker.detach();
	}
}

void AgriPestAndDiseaseDataCollectDialog::on_combo_pest_category_currentTextChanged(int index)
{

}

void AgriPestAndDiseaseDataCollectDialog::on_combo_pest_stage_currentTextChanged(int index)
{

}

void AgriPestAndDiseaseDataCollectDialog::on_combo_pest_level_currentTextChanged(int index)
{

}

void AgriPestAndDiseaseDataCollectDialog::on_comboBox_sensor_currentTextChanged(int index)
{
	// 设置图片
	auto itemData = ui->combo_sensor->itemData(index).value<AgriPestPhotoIndefication>();

	QPixmap pixmap;
	if (pixmap.loadFromData(itemData.recognize_image)) 
	{
		ui->sensor_recognize_image->setPixmap(pixmap);
		ui->sensor_recognize_image->setFixedSize(QSize(450, 300));
		ui->sensor_recognize_image->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);//将图片在QLabel中居中
		ui->sensor_recognize_image->setScaledContents(true);
	}
	else 
	{
		qDebug() << "Failed to load image from QByteArray";
	}

	ui->target_pos_lon->setText(QString::number(itemData.longitude, 'f', 7));
	ui->target_pos_lat->setText(QString::number(itemData.latitude, 'f', 7));
	ui->target_pos_alt->setText(QString::number(itemData.localHeight));
	ui->pest_count->setText(QString::number(itemData.pest_number));
	ui->pest_range->setText(QString::number(itemData.recode_range));
	int tmpStage = itemData.pest_stage.toInt();
	if (tmpStage <= 2)
	{
		ui->combo_pest_stage->setCurrentIndex(tmpStage);
	}
	int nIndex = ui->combo_pest_level->findData(itemData.pest_grade);
	if (nIndex != -1)
	{
		ui->combo_pest_level->setCurrentIndex(nIndex);
	}
}

