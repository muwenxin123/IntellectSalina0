#include "ly_agrifertilizerpreswidget.h"
#include "ui_ly_agrifertilizerpreswidget.h"
#include "AgrisprayComboBox.h"
#include <QDebug>
#include <LyMessageBox.h>
LY_AgriFertilizerPresWidget::LY_AgriFertilizerPresWidget(QWidget *parent) :
    LyDialog("info",parent),
    ui(new Ui::LY_AgriFertilizerPresWidget)
{
    ui->setupUi(centralWidget());
	this->setResizable(true);
	setupUi();

}
void LY_AgriFertilizerPresWidget::setupUi()
{
	m_viewModel = new LY_AgriFertilizePescViewModel();

	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("当前盐田") << tr2("处方图名称") << tr2("数据集名称") << tr2("分布图") << tr2("养分元素");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setRowCount(0);

	QStringList grid_headerlabels;
	grid_headerlabels << tr2("序号") << tr2("栅格颜色") << tr2("颜色代表值") << tr2("栅格个数") << tr2("栅格大小")  << tr2("施肥用量");
	ui->grid_tableWidget->setColumnCount(grid_headerlabels.size());
	ui->grid_tableWidget->setHorizontalHeaderLabels(grid_headerlabels);
	ui->grid_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->grid_tableWidget->verticalHeader()->setHidden(true);
	ui->grid_tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->grid_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->grid_tableWidget->setAlternatingRowColors(true);
	ui->grid_tableWidget->setRowCount(0);

	ui->pescription_width->addItem("");
	ui->pescription_width->addItem(tr2("4"));
	ui->pescription_width->addItem(tr2("6"));

	ui->calculation_type_box->clear();
	for (auto it = m_calculation_map.begin(); it != m_calculation_map.end(); ++it)
	{
		ui->calculation_type_box->addItem(it->first);
	}
	ui->calculation_type_box->setCurrentIndex(0);
	//ui->on_sub_soilMap_box->addItem(tr2("氮"));
	connect(ui->tableWidget, &QTableWidget::cellClicked, this, &LY_AgriFertilizerPresWidget::onCellClicked);
	connect(ui->on_field_box, &AgrisprayComboBox::setupField, this, &LY_AgriFertilizerPresWidget::addField);
	connect(ui->on_prescription_add, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_add_click);
	connect(ui->on_prescription_edit, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_edit_click);
	connect(ui->on_prescription_delete, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_delete_click);
	connect(ui->on_prescription_export, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_export_click);
	connect(ui->on_prescription_generate, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_generate_click);
	connect(ui->on_prescription_control, &QPushButton::clicked, this, &LY_AgriFertilizerPresWidget::on_prescription_control_click);
	connect(ui->on_pescription_data_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_pescription_data_boxChanged(const int &)));
	connect(ui->pescription_width, SIGNAL(currentIndexChanged(const int &)), this, SLOT(pescription_width_boxChanged(const int &)));
	connect(ui->calculation_type_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(calculation_type_boxChanged(const int &)));
	connect(ui->calculation_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(calculation_boxChanged(const int &)));
	connect(ui->on_soilMap_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_soilMap_boxChanged(const int &)));
}
void LY_AgriFertilizerPresWidget::setupData(QString &field_id)
{
	m_listVec.clear();
	m_listVec = m_viewModel->GetFertilizePrescription(field_id);
	setMapTableWidgetData(m_listVec);

}
void LY_AgriFertilizerPresWidget::addField(QString &text, QString & field_id, QString &id)
{
	ui->on_field_box->setCurrentText(text);
	m_viewModel->setFieldId(field_id, id);
	m_currentFieldID = field_id;
	m_currentFieldName = text;
	setupData(field_id);
	QStringList fieldList(field_id);
	AgriSoilTestingDatasetPtrVec soilDatasetVec = LY_AgriFertilizerPrescriptionActivator::Get().getIAgriSoilTestingAnalysisService()->GetSoilTestingDatasetList(fieldList);
	ui->on_pescription_data_box->clear();
	for (AgriSoilTestingDatasetPtr &ptr : soilDatasetVec) {
		ui->on_pescription_data_box->addItem(ptr->name, QVariant::fromValue(ptr->guid));
	}
	ui->on_pescription_data_box->setCurrentIndex(0);
}
void LY_AgriFertilizerPresWidget::onCellClicked(int row, int column)
{
	m_controcol_type = FertilizerPresConcolType_NONE;
	int currentRow = ui->tableWidget->currentRow();
	m_selectedCheckBoxRow = currentRow;
	if (currentRow < 0 || currentRow >= m_listVec.size()) {
		qDebug() << "未选中有效行";
		return;
	}
	setComboxStatus(false);
	FertilizerPrescription selectedData = m_listVec.at(currentRow);
	ui->pescription_name->setText(selectedData.pescription_name);
	ui->on_pescription_data_box->setCurrentText(selectedData.data_name);
	ui->on_soilMap_box->setCurrentText(selectedData.dis_map_name);
	ui->on_sub_soilMap_box->setCurrentText(selectedData.nutrient_ele);
	ui->pescription_width->setCurrentText(QString::number(selectedData.pyfk));
	mapInfoId = selectedData.mapInfo_id;
	FertilizerPrescriptionGridInfoVec infoVec;
	infoVec.clear();
	infoVec  = m_viewModel->GetFertilizePrescriptionInfo(selectedData.mapInfo_id);

	setMapInfoTableWidgetData(infoVec);
}
void LY_AgriFertilizerPresWidget::setMapTableWidgetData(FertilizerPrescriptionVec vec)
{
	ui->tableWidget->setRowCount(vec.size());
	int row = 1;
	for (int i = 0; i < vec.size(); i++) {
		FertilizerPrescription info = vec.at(i);
		QTableWidgetItem *item = new QTableWidgetItem(QString::number(row));
		ui->tableWidget->setItem(i, 0, item);
		QTableWidgetItem *item1 = new QTableWidgetItem(m_currentFieldName);
		ui->tableWidget->setItem(i, 1, item1);
		QTableWidgetItem *item2 = new QTableWidgetItem(info.pescription_name);
		ui->tableWidget->setItem(i, 2, item2);
		QTableWidgetItem *item3 = new QTableWidgetItem(info.data_name);
		ui->tableWidget->setItem(i, 3, item3);
		QTableWidgetItem *item4 = new QTableWidgetItem(info.dis_map_name);
		ui->tableWidget->setItem(i, 4, item4);
		QTableWidgetItem *item5 = new QTableWidgetItem(info.nutrient_ele);
		ui->tableWidget->setItem(i, 5, item5);

		item->setTextAlignment(Qt::AlignCenter);
		item1->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);
		item4->setTextAlignment(Qt::AlignCenter);
		item5->setTextAlignment(Qt::AlignCenter);

		row++;
	}
}
void LY_AgriFertilizerPresWidget::setMapInfoTableWidgetData(FertilizerPrescriptionGridInfoVec vec)
{
	ui->grid_tableWidget->clearContents();
	ui->grid_tableWidget->setRowCount(vec.size());
	int row = 1;
	for (int i = 0; i < vec.size(); i++) {
		FertilizerPrescriptionGridInfo info = vec.at(i);
		info.guid = mapInfoId;
		info.fert_name = ui->pescription_name->text();
		QTableWidgetItem *item = new QTableWidgetItem(QString::number(row));
		ui->grid_tableWidget->setItem(i, 0, item);

		QWidget *tmpWidget = new QWidget();
		QFrame *colorBox = new QFrame(tmpWidget);
		colorBox->setFixedSize(30, 30);
		colorBox->setFrameShape(QFrame::StyledPanel);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(info.color_name));

		QVBoxLayout* vLayout = new QVBoxLayout(colorBox);
		colorBox->setLayout(vLayout);
		vLayout->setContentsMargins(0, 0, 0, 0);

		// 创建一个容器 widget 用于居中布局
		QWidget* container = new QWidget();
		QHBoxLayout* hLayout = new QHBoxLayout(container);
		hLayout->addStretch();
		hLayout->addWidget(colorBox);
		hLayout->addStretch();
		hLayout->setContentsMargins(0, 0, 0, 0);
		container->setLayout(hLayout);

		ui->grid_tableWidget->setCellWidget(i, 1, container);

		QTableWidgetItem *item2 = new QTableWidgetItem(info.color_dis);
		ui->grid_tableWidget->setItem(i, 2, item2);
		QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(info.grid_num));
		ui->grid_tableWidget->setItem(i, 3, item3);
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(info.grid_size));
		ui->grid_tableWidget->setItem(i, 4, item4);
		QTableWidgetItem *item5 = new QTableWidgetItem(info.fert_dosage);
		ui->grid_tableWidget->setItem(i, 5, item5);

		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);
		item4->setTextAlignment(Qt::AlignCenter);
		item5->setTextAlignment(Qt::AlignCenter);

		if (m_controcol_type == FertilizerPresConcolType_ADD) {
			m_viewModel->AddFertilizePrescriptionInfo(info);
		}else if (m_controcol_type == FertilizerPresConcolType_EDIT) {
			m_viewModel->AddFertilizePrescriptionInfo(info);
		}
		row++;
	}
}

void LY_AgriFertilizerPresWidget::calculation_type_boxChanged(const int &)
{
	ui->calculation_box->clear();
	//ui->calculation_box->addItem("");

	auto it = m_calculation_map.find(ui->calculation_type_box->currentText()); 
	ui->calculation_box->clear();
	if (it != m_calculation_map.end()) { 
		for (size_t i = 0; i < it->second.size(); i++)
		{
			ui->calculation_box->addItem(it->second.at(i));
		}
	}
	ui->calculation_box->setCurrentIndex(0);
}
void LY_AgriFertilizerPresWidget::calculation_boxChanged(const int &)
{
	
}
void LY_AgriFertilizerPresWidget::on_pescription_data_boxChanged(const int & index)
{
	QString guid = ui->on_pescription_data_box->currentData().value<QString>();
	dataset_id = guid;
	m_soilDistributeMapVec.clear();

	m_soilDistributeMapVec =LY_AgriFertilizerPrescriptionActivator::Get().getIAgriSoilTestingAnalysisService()->GetSoilTestingDistributeMap(guid);
	ui->on_soilMap_box->clear();
	ui->on_sub_soilMap_box->clear();
	for (AgriSoilFertilityDistributeMapPtr &ptr : m_soilDistributeMapVec) {
		ui->on_soilMap_box->addItem(ptr->name, QVariant::fromValue(ptr->guid));
		int valid_identify = ptr->valid_identify;
		QString str = m_viewModel->getFertilizeName(valid_identify);
		ui->on_sub_soilMap_box->addItem(str);
	}
	ui->on_soilMap_box->setCurrentIndex(0);
	ui->on_sub_soilMap_box->setCurrentIndex(0);

}
void LY_AgriFertilizerPresWidget::on_soilMap_boxChanged(const int &)
{
	QString guid = ui->on_soilMap_box->currentData().value<QString>();
	for (const AgriSoilFertilityDistributeMapPtr &ptr : m_soilDistributeMapVec) {
		if (guid == ptr->guid)
		{
			m_currentDistributeMap = ptr;
			break;
		}
	}
}
void LY_AgriFertilizerPresWidget::pescription_width_boxChanged(const int & index)
{

}
void LY_AgriFertilizerPresWidget::on_prescription_add_click()
{
	setComboxStatus(true);
	m_controcol_type = FertilizerPresConcolType_ADD;
}
void LY_AgriFertilizerPresWidget::on_prescription_edit_click()
{
	setComboxStatus(true);
	m_controcol_type = FertilizerPresConcolType_EDIT;
}
void LY_AgriFertilizerPresWidget::on_prescription_delete_click()
{
	m_controcol_type = FertilizerPresConcolType_DELETE;

	if (m_selectedCheckBoxRow < 0)
	{
		LyMessageBox::information(this, tr2("警告"), tr2("请选择一行"));
		return;
	}
	if (m_selectedCheckBoxRow >= m_listVec.size())
	{
		return;
	}
	FertilizerPrescription model = m_listVec.at(m_selectedCheckBoxRow);

	if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除处方图[%1]？").arg(model.pescription_name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		bool isSuccess = m_viewModel->DeleteFertilizePrescription(model.guid);
		if (!isSuccess)
		{
			LyMessageBox::warning(this, "错误", "数据库删除失败！");
		}
		else
		{
			m_listVec.erase(m_listVec.begin() + m_selectedCheckBoxRow);
			setMapTableWidgetData(m_listVec);
			m_viewModel->DeleteFertilizePrescriptionInfo(model.mapInfo_id);
			ui->grid_tableWidget->clearContents();
			ui->grid_tableWidget->setRowCount(0);

		}
	}

}
void LY_AgriFertilizerPresWidget::on_prescription_export_click()
{
	m_controcol_type = FertilizerPresConcolType_NONE;
}
void LY_AgriFertilizerPresWidget::on_prescription_generate_click()
{
	AgriSoilTestingFertilityPointPtrVec   outSoilFertilityPointVec;
	if (m_currentDistributeMap == nullptr)
	{
		return;
	}
	QPair<bool, QString> pair = LY_AgriFertilizerPrescriptionActivator::Get().getIAgriSoilTestingAnalysisService()->GetSoilFertilityDistribute(m_currentDistributeMap, outSoilFertilityPointVec);
	if (!pair.first)
	{
		return;
	}
	m_isGenerate = true;
	m_viewModel->draw3DGridMap(outSoilFertilityPointVec, ui->pescription_width->currentText().toDouble());

	switch (m_controcol_type)
	{
	case FertilizerPresConcolType_ADD:
	{
		FertilizerPrescription model;
		model.guid = GenerateUuid();
		model.field_id = m_currentFieldID;
		model.pescription_name = ui->pescription_name->text();
		model.data_name = ui->on_pescription_data_box->currentText();
		model.dis_map_name = ui->on_soilMap_box->currentText();
		model.nutrient_ele = ui->on_sub_soilMap_box->currentText();
		mapInfoId = GenerateUuid();
		model.mapInfo_id = mapInfoId;
		model.dataset_id = dataset_id;
		model.datamap_id = m_currentDistributeMap->guid;
		model.pyfk = ui->pescription_width->currentText().toInt();
		bool isSuc = m_viewModel->AddFertilizePrescription(model);
		setMapInfoTableWidgetData(m_viewModel->getMapInfoData());

		if (isSuc)
		{
			LyMessageBox::information(this, tr2("警告"), tr2("添加成功"));
			m_listVec.push_back(model);
			setMapTableWidgetData(m_listVec);
		}
	}
	break;
	case FertilizerPresConcolType_EDIT:
	{
		if (m_selectedCheckBoxRow < 0)
		{
			LyMessageBox::information(this, tr2("警告"), tr2("请选择一行"));
			return;
		}
		FertilizerPrescription model = m_listVec.at(m_selectedCheckBoxRow);
		model.pescription_name = ui->pescription_name->text();
		model.data_name = ui->on_pescription_data_box->currentText();
		model.dis_map_name = ui->on_soilMap_box->currentText();
		model.nutrient_ele = ui->on_sub_soilMap_box->currentText();
		model.dataset_id = dataset_id;
		model.datamap_id = m_currentDistributeMap->guid;
		model.pyfk = ui->pescription_width->currentText().toInt();
		bool isSuc = m_viewModel->UpdateFertilizePrescription(model);
		
		if (isSuc)
		{
			m_viewModel->DeleteFertilizePrescriptionInfo(model.mapInfo_id);
			LyMessageBox::information(this, tr2("警告"), tr2("修改成功"));
			m_listVec[m_selectedCheckBoxRow] = model;
			setMapTableWidgetData(m_listVec);
			setMapInfoTableWidgetData(m_viewModel->getMapInfoData());
		}
	}
	break;
	case FertilizerPresConcolType_DELETE:
	{

	}
	break;
	case FertilizerPresConcolType_NONE:
		break;
	default:
		break;
	}
	m_controcol_type = FertilizerPresConcolType_NONE;
}

void LY_AgriFertilizerPresWidget::on_prescription_control_click()
{
	m_isGenerate = false;
	m_viewModel->showAndHideMap(m_isGenerate);
	if (m_viewModel == nullptr || m_viewModel->getHeatMap3D() == nullptr)
	{
		//m_viewModel->getHeatMap3D()->cleanMap();
	}
}
void LY_AgriFertilizerPresWidget::hideEvent(QHideEvent *event)
{
	QMutexLocker locker(&sMutex);
	m_show = false;
	m_isGenerate = false;
	ui->on_field_box->clearZoneHighLight();
	if (m_viewModel == nullptr || m_viewModel->getHeatMap3D() == nullptr)
	{
		return;
	}
	m_viewModel->getHeatMap3D()->cleanMap();
	m_controcol_type = FertilizerPresConcolType_NONE;
}
void LY_AgriFertilizerPresWidget::showEvent(QShowEvent *event)
{
	m_show = true;
}
void LY_AgriFertilizerPresWidget::draw3DGridMap(QPainter &painter, IProj *pProj)
{
	QMutexLocker locker(&sMutex);
	if (pProj == nullptr)
	{
		return;
	}

	if (m_show)
	{
		ui->on_field_box->zoneHighLight(painter, pProj);
	}
}
void LY_AgriFertilizerPresWidget::draw2DGridMap(QPainter &painter, IProj *pProj)
{
	QMutexLocker locker(&sMutex);
	if (pProj == nullptr || !m_isGenerate)
	{
		return;
	}
	m_viewModel->draw2DGridMap(painter, pProj);
}
void LY_AgriFertilizerPresWidget::updataCurActiveFarm()
{
	ui->on_field_box->initData();
}
void LY_AgriFertilizerPresWidget::setComboxStatus(bool isEnable)
{
	ui->pescription_name->setEnabled(isEnable);
	ui->on_pescription_data_box->setEnabled(isEnable);
	ui->on_soilMap_box->setEnabled(isEnable);
	ui->pescription_width->setEnabled(isEnable);
	ui->on_sub_soilMap_box->setEnabled(isEnable);
	ui->calculation_type_box->setEnabled(isEnable);
	ui->calculation_box->setEnabled(isEnable);
}
LY_AgriFertilizerPresWidget::~LY_AgriFertilizerPresWidget()
{
    delete ui;
}
