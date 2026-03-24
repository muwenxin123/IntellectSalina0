#include <random>
#include <QApplication>
#include <QDateTime>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "QCString.h"
#include "AgriSoilTestingDataSetDialog.h"
#include "ui_AgriSoilTestingDataSetDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LyMessageBox.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "LY_AgriSoilAnalysisPredict.h"


AgriSoilTestingDataSetDialog::AgriSoilTestingDataSetDialog(DataSetHandleMode handleMode, const AgriFieldInfoPtr& fieldInfo, AgriSoilTestingDataset datasetInfo, QWidget *parent /*= nullptr*/)
	:LyDialog([&]()->QString {
	QString titleName = tr2("未知");
	switch (handleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
		titleName = tr2("查看数据集");
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
		titleName = tr2("添加数据集");
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
		titleName = tr2("修改数据集");
		break;
	default:
		break;
	}
	return titleName;
}(), parent),
	ui(new Ui::AgriSoilTestingDataSetDialog)
{
	// 生成数据集uid
	m_DataSetUid = AgriSoilAnalysisPredictUtil::GenerateUuid();

	m_DataSetHandleMode = handleMode; 

	m_pFieldInfo = fieldInfo;

	m_pDatasetInfo = datasetInfo;

	Init();

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_btnCancel_clicked);
}


AgriSoilTestingDataSetDialog::~AgriSoilTestingDataSetDialog()
{
    delete ui;
}

void AgriSoilTestingDataSetDialog::draw2DGrid(QPainter &painter, IProj *pProj)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (pProj && m_heatMap3D)
	{
		if (m_bMap3DShow)
		{
			// TODO: 此操作2D、3D同时网格化，后续可优化
			m_heatMap3D->setGridLineShowOrHide(true);
			m_heatMap3D->create2DGridDrawing(painter, pProj);
		}
	}
}

void AgriSoilTestingDataSetDialog::closeEvent(QCloseEvent* event)
{
	cleanUpMap3D();
}

void AgriSoilTestingDataSetDialog::contextMenuEvent(QContextMenuEvent *event)
{
	if (m_DataSetHandleMode == DataSetHandleMode::DATA_SET_HANDLE_VIEW)
	{
		QMenu menu(this);

		QAction* viewAction = new QAction(tr2("查看采集点"), this);
		QAction* addAction = new QAction(tr2("添加采集点"), this);
		QAction* updateAction = new QAction(tr2("修改采集点"), this);
		QAction* deleteAction = new QAction(tr2("删除采集点"), this);

		// 绑定菜单项的动作
		connect(viewAction, &QAction::triggered, this, &AgriSoilTestingDataSetDialog::viewData);
		connect(addAction, &QAction::triggered, this, &AgriSoilTestingDataSetDialog::addData);
		connect(updateAction, &QAction::triggered, this, &AgriSoilTestingDataSetDialog::updateData);
		connect(deleteAction, &QAction::triggered, this, &AgriSoilTestingDataSetDialog::deleteData);

		menu.addAction(viewAction);
		menu.addAction(addAction);
		menu.addAction(updateAction);
		menu.addAction(deleteAction);

		menu.exec(event->globalPos());
	}
}

void AgriSoilTestingDataSetDialog::Init()
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	ui->dataset_name->setPlaceholderText(tr2("例：A01"));

	InitDisplayComboBox();
	InitDisplaySlider();
	InitCheckbox();

	switch (m_DataSetHandleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
	{
		ui->dataset_name->setText(m_pDatasetInfo.name);

		InitDisplayTable();
		UpdateTableData();

		// 隐藏部分按钮
		for (int i = 0; i < ui->verticalLayout_2->count(); ++i) {
			QLayoutItem *item = ui->verticalLayout_2->itemAt(i);
			QWidget *widget = item->widget();
			if (widget) {
				widget->hide();
			}
		}

		ui->btnOK->hide();
		ui->btnCancel->hide();

		// 禁用编辑
		ui->dataset_name->setReadOnly(true);
		ui->comboBox_collect_type->setDisabled(true);
		ui->comboBox_sample_mode->setDisabled(true);
		ui->comboBox_sample_type->setDisabled(true);
		ui->comboBox_soil_type->setDisabled(true);
		ui->comboBox_soil_color->setDisabled(true);
		ui->comboBox_soil_texture->setDisabled(true);

		//ui->silder_grid_size->setDisabled(true); // TODO：禁用后样式待调整
		//ui->silder_grid_rotation->setDisabled(true);
		//ui->btn_generate_grid->setDisabled(true);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
	{
		ui->dataset_name->setText("");

		ui->btnOK->show();
		ui->btnCancel->show();
		ui->ptn_group->hide();
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
	{
		ui->dataset_name->setText(m_pDatasetInfo.name);

		InitDisplayTable();
		UpdateTableData();

		ui->btnOK->show();
		ui->btnCancel->show();
		ui->ptn_group->hide();
	}
		break;
	default:
		break;
	}

	// 下拉菜单点击
	connect(ui->comboBox_sample_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_sample_type_currentTextChanged(int)));
	connect(ui->comboBox_sample_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_sample_mode_currentTextChanged(int)));
	// 生成网格点击
	connect(ui->btn_generate_grid, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_btn_generate_grid_clicked);
	// 按钮组点击
	connect(ui->ptn_export_points, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_ptn_export_points_clicked);
	connect(ui->ptn_generate_samples, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_ptn_generate_samples_clicked);
	connect(ui->ptn_save_samples, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_ptn_save_samples_clicked);
	connect(ui->ptn_import_samples, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_ptn_import_samples_clicked);
	connect(ui->ptn_show_samples, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::on_ptn_show_samples_clicked);
}

void AgriSoilTestingDataSetDialog::InitDisplayTable()
{
	// 根据valid_identify构建表
	QStringList selectedHeaders;
	QVector<int> selectedIndices;

	selectedHeaders << tr2("序号") << tr2("样本点") << tr2("采集时间") << tr2("经度") << tr2("纬度") << tr2("高度");
	for (size_t i = 0; i < selectedHeaders.size(); i++){
		selectedIndices << i;
	}

	for (int i = 0; i < checkboxes.size(); ++i) {
		if ((m_pDatasetInfo.valid_identify >> i) & 1) {
			selectedHeaders.append(checkboxes[i]->text());
			selectedIndices.append(i);
		}
	}

	ui->tableWidget->clear();  // 清空旧表头与内容
	ui->tableWidget->setColumnCount(selectedHeaders.size());
	ui->tableWidget->setHorizontalHeaderLabels(selectedHeaders);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	// 添加分页查询按钮
	prevButton = new QPushButton(tr2("上一页"));
	nextButton = new QPushButton(tr2("下一页"));
	pageLabel = new QLabel(tr2("第 1 / 1 页"));

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addStretch();
	btnLayout->addWidget(prevButton);
	btnLayout->addWidget(pageLabel);
	btnLayout->addWidget(nextButton);
	btnLayout->addStretch();
	ui->verticalLayout_3->addLayout(btnLayout);

	connect(prevButton, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::onPrevPage);
	connect(nextButton, &QPushButton::clicked, this, &AgriSoilTestingDataSetDialog::onNextPage);

	// 连接双击信号，监听用户的双击操作
	connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &AgriSoilTestingDataSetDialog::onCellDoubleClicked);
}

void AgriSoilTestingDataSetDialog::InitDisplayComboBox()
{
	int curIndex = 0, typeIndex = 0;

	ui->comboBox_sample_type->clear();
	ui->comboBox_sample_mode->clear();

	for (int i = 1; i < 4; ++i)
	{
		SampleType sample_type = static_cast<SampleType>(i);
		ui->comboBox_sample_type->addItem(AgriSoilAnalysisPredictUtil::SampleTypeToString(sample_type), QVariant::fromValue(sample_type));

		if (!m_pDatasetInfo.guid.isEmpty() && m_pDatasetInfo.sample_type == static_cast<int>(sample_type))
		{
			typeIndex = curIndex;
		}
		curIndex++;
	}
	ui->comboBox_sample_type->setCurrentIndex(typeIndex);

	// 级联操作
	on_comboBox_sample_type_currentTextChanged(typeIndex);

	/* ------------------------  采集类型 ------------------------ */

	for (int i = 1; i <= 4; ++i)
	{
		DataCollectType collect_type = static_cast<DataCollectType>(i);
		ui->comboBox_collect_type->addItem(AgriSoilAnalysisPredictUtil::DataCollectTypeToString(collect_type), QVariant::fromValue(collect_type));
	}
	ui->comboBox_collect_type->setCurrentIndex(m_pDatasetInfo.collect_type ? m_pDatasetInfo.collect_type - 1 : 0);

	/* ------------------------  N、P、K ------------------------ */

	for (int i = 1; i <= 8; ++i)
	{
		NitrogenType n_type = static_cast<NitrogenType>(i);
		ui->comboBox_N_type->addItem(AgriSoilAnalysisPredictUtil::NitrogenTypeToString(n_type), QVariant::fromValue(n_type));
	}
	ui->comboBox_N_type->setCurrentIndex(m_pDatasetInfo.n_type ? m_pDatasetInfo.n_type - 1 : 0);

	for (int i = 1; i <= 4; ++i)
	{
		PhosphorusType p_type = static_cast<PhosphorusType>(i);
		ui->comboBox_P_type->addItem(AgriSoilAnalysisPredictUtil::PhosphorusTypeToString(p_type), QVariant::fromValue(p_type));
	}
	ui->comboBox_P_type->setCurrentIndex(m_pDatasetInfo.p_type ? m_pDatasetInfo.p_type - 1 : 0);

	for (int i = 1; i <= 4; ++i)
	{
		PotassiumType k_type = static_cast<PotassiumType>(i);
		ui->comboBox_K_type->addItem(AgriSoilAnalysisPredictUtil::PotassiumTypeToString(k_type), QVariant::fromValue(k_type));
	}
	ui->comboBox_K_type->setCurrentIndex(m_pDatasetInfo.k_type ? m_pDatasetInfo.k_type - 1 : 0);

	/* ------------------------  土壤信息 ------------------------ */

	for (int i = 1; i <= 8; ++i)
	{
		SoilType soil_type = static_cast<SoilType>(i);
		ui->comboBox_soil_type->addItem(AgriSoilAnalysisPredictUtil::SoilTypeToString(soil_type), QVariant::fromValue(soil_type));
	}
	ui->comboBox_soil_type->setCurrentIndex(m_pDatasetInfo.soil_type ? m_pDatasetInfo.soil_type - 1 : 0);

	for (int i = 1; i <= 15; ++i)
	{
		SoilColor soil_color = static_cast<SoilColor>(i);
		ui->comboBox_soil_color->addItem(AgriSoilAnalysisPredictUtil::SoilColorToString(soil_color), QVariant::fromValue(soil_color));
	}
	ui->comboBox_soil_color->setCurrentIndex(m_pDatasetInfo.soil_color ? m_pDatasetInfo.soil_color - 1 : 0);

	for (int i = 1; i <= 13; ++i)
	{
		SoilTexture soil_texture = static_cast<SoilTexture>(i);
		ui->comboBox_soil_texture->addItem(AgriSoilAnalysisPredictUtil::SoilTextureToString(soil_texture), QVariant::fromValue(soil_texture));
	}
	ui->comboBox_soil_texture->setCurrentIndex(m_pDatasetInfo.soil_texture ? m_pDatasetInfo.soil_texture - 1 : 0);
}


void AgriSoilTestingDataSetDialog::InitDisplaySlider()
{
	ui->silder_grid_size->setRange(5, 30);
	ui->silder_grid_size->setSingleStep(1);
	ui->silder_grid_size->setValue(20);

	if (!m_pDatasetInfo.guid.isEmpty())
	{
		ui->grid_size->setText(QString("%1").arg(m_pDatasetInfo.grid_size));
		ui->grid_rotation->setText(QString("%1").arg(m_pDatasetInfo.grid_rotation));
		ui->silder_grid_size->setValue(m_pDatasetInfo.grid_size);
		m_spacing = m_pDatasetInfo.grid_size;
	}

	connect(ui->silder_grid_size, &QSlider::valueChanged, this, [&](int value) {
		m_spacing = value;
		ui->grid_size->setText(QString("%1").arg(value));
	});
}

void AgriSoilTestingDataSetDialog::InitCheckbox()
{
	QWidget *groupBox = new QWidget(this);
	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->setContentsMargins(0, 0, 0, 0);

	std::vector<std::string> names = {
		"钙", "镁", "铜", "锌", "铁",
		"硫", "锰", "钼", "硼", "氯", "硅", "铅", "铬",
		"镉", "汞", "有机质", "温度", "湿度", "PH", "EC", "水势" 
	};

	checkboxes.clear();

	// 加入N、P、K
	checkboxes.append(ui->checkBox_N);
	checkboxes.append(ui->checkBox_P);
	checkboxes.append(ui->checkBox_K);

	for (int i = 0; i < names.size(); ++i) {
		QCheckBox *cb = new QCheckBox(tr2(names[i].c_str()), this);
		checkboxes.append(cb);
		// 11列2行布局
		gridLayout->addWidget(cb, i / 11, (i % 11) * 2);
	}

	groupBox->setLayout(gridLayout);
	ui->verticalLayout_2->addWidget(groupBox);

	// 初始化数据
	if (!m_pDatasetInfo.guid.isEmpty())
	{
		decodeCheckboxes(m_pDatasetInfo.valid_identify);
	}
}

void AgriSoilTestingDataSetDialog::AddDataSet(AgriSoilTestingDatasetPtr dataset)
{
	bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->AddSoilTestingDataset(dataset);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("新增测土数据集失败！"));
	}
	else
	{
		emit addFinish(dataset);
	}
}


void AgriSoilTestingDataSetDialog::UpdateDataSet(AgriSoilTestingDatasetPtr dataset)
{
	bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->UpdateSoilTestingDataset(dataset);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("更新测土数据集失败！"));
	}
	else
	{
		emit updateFinish(dataset);
	}
}

void AgriSoilTestingDataSetDialog::HandleDataCollectionAddFinish(AgriSoilTestingCollectionPtr soiltestingCollection)
{
	if (soiltestingCollection)
	{
		AddOneRowToTable(soiltestingCollection);

		// 翻页检测
		int rowCount = ui->tableWidget->rowCount();
		if (rowCount == m_pageSize + 1)
		{
			m_totalPages += 1;
			onNextPage();
		}
	}

	// 更新采集点绘制（添加）
	emit updateCollection(DataCollectHandle::DATA_COLLECT_ADD, soiltestingCollection);
}


void AgriSoilTestingDataSetDialog::HandleDataCollectionUpdateFinish(AgriSoilTestingCollectionPtr soiltestingCollection)
{
	if (soiltestingCollection)
	{
		UpdateRowToTable(soiltestingCollection);
	}

	// 更新采集点绘制（更新）
	emit updateCollection(DataCollectHandle::DATA_COLLECT_UPDATE, soiltestingCollection);
}

void AgriSoilTestingDataSetDialog::UpdateTableData()
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	if (m_pFieldInfo)
	{
		// 分页查询
		LoadTablePagedData();
	}
}


void AgriSoilTestingDataSetDialog::AddOneRowToTable(AgriSoilTestingCollectionPtr soiltestingCollection)
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);

	// 计算表格行序号
	int rowNum = rowCount + 1 + m_pageSize * (m_currentPage - 1);

	auto userDefItem = new QTableWidgetItem(QString::number(rowNum));
	userDefItem->setTextAlignment(Qt::AlignCenter);
	userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(soiltestingCollection.get())));
	ui->tableWidget->setItem(rowCount, 0, userDefItem);

	FillTableRowContents(soiltestingCollection, rowCount);
}

void AgriSoilTestingDataSetDialog::UpdateRowToTable(AgriSoilTestingCollectionPtr soiltestingCollection)
{
	if (!soiltestingCollection) return;

	// 匹配当前行
	for (int row = 0; row < ui->tableWidget->rowCount(); ++row) 
	{
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
		{
			// 采集点信息
			AgriSoilTestingCollection dataCollection = item->data(Qt::UserRole).value<AgriSoilTestingCollection>();
			if (dataCollection.guid == soiltestingCollection->guid)
			{
				item->setData(Qt::UserRole, QVariant::fromValue(*(soiltestingCollection.get())));
				ui->tableWidget->setItem(row, 0, item);

				FillTableRowContents(soiltestingCollection, row);
			}
		}
	}
}


void AgriSoilTestingDataSetDialog::LoadTablePagedData()
{
	AgriSoilTestingCollectionPtrVec	soilTestingCollectionPagedDataVec;
	if (m_soilTestingCollectionVec.empty())
	{
		soilTestingCollectionPagedDataVec = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionPagedData(m_pDatasetInfo.guid, m_currentPage, m_pageSize);// 数据库获取
	}
	else
	{
		soilTestingCollectionPagedDataVec = SimulateGetSoilTestingCollectionPagedData(m_currentPage, m_pageSize); // 缓存获取
	}
	
	int totalRecords = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionCount(m_pDatasetInfo.guid);
	m_totalPages = (totalRecords + m_pageSize - 1) / m_pageSize; // 计算总页数

	ui->tableWidget->setRowCount((int)soilTestingCollectionPagedDataVec.size());
	for (size_t i = 0; i < soilTestingCollectionPagedDataVec.size(); ++i)
	{
		int rowCount = static_cast<int>(i);
		AgriSoilTestingCollectionPtr soilTestingCollection = soilTestingCollectionPagedDataVec[i];

		// 计算表格行序号
		int rowNum = rowCount + 1 + m_pageSize * (m_currentPage - 1);
		auto userDefItem = new QTableWidgetItem(QString::number(rowNum));
		userDefItem->setTextAlignment(Qt::AlignCenter);
		userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(soilTestingCollection.get())));
		ui->tableWidget->setItem(rowCount, 0, userDefItem);

		FillTableRowContents(soilTestingCollection, rowCount);
	}

	pageLabel->setText(tr2("第 %1 / %2 页").arg(m_totalPages ? m_currentPage : 0 ).arg(m_totalPages));
}


void AgriSoilTestingDataSetDialog::FillTableRowContents(AgriSoilTestingCollectionPtr soiltestingCollection, int nRow)
{
	ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(QString::fromStdString(soiltestingCollection->name)));
	ui->tableWidget->setItem(nRow, 2, new QTableWidgetItem(QString::fromStdString(soiltestingCollection->sample_time)));
	ui->tableWidget->setItem(nRow, 3, new QTableWidgetItem(QString::number(soiltestingCollection->lon, 'f', 7)));
	ui->tableWidget->setItem(nRow, 4, new QTableWidgetItem(QString::number(soiltestingCollection->lat, 'f', 7)));
	ui->tableWidget->setItem(nRow, 5, new QTableWidgetItem(QString::number(soiltestingCollection->alt)));
	ui->tableWidget->setItem(nRow, 6, new QTableWidgetItem(QString::number(soiltestingCollection->ele_nutrient_N)));
	ui->tableWidget->setItem(nRow, 7, new QTableWidgetItem(QString::number(soiltestingCollection->ele_nutrient_P)));
	ui->tableWidget->setItem(nRow, 8, new QTableWidgetItem(QString::number(soiltestingCollection->ele_nutrient_K)));
}

void AgriSoilTestingDataSetDialog::cleanUpMap3D()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// 清理地图数据
	if (m_heatMap3D)
	{
		m_heatMap3D->cleanUp();
		delete m_heatMap3D;
		m_heatMap3D = nullptr;
	}
}

quint32 AgriSoilTestingDataSetDialog::encodeCheckboxes()
{
	quint32 value = 0;
	for (int i = 0; i < checkboxes.size(); ++i) {
		if (checkboxes[i]->isChecked()) {
			value |= (1u << i);
		}
	}
	return value;
}

void AgriSoilTestingDataSetDialog::decodeCheckboxes(quint32 value)
{
	for (int i = 0; i < checkboxes.size(); ++i) {
		checkboxes[i]->setChecked((value >> i) & 1);
	}
}

void AgriSoilTestingDataSetDialog::addData()
{
	DataCollectType collectType = static_cast<DataCollectType>(ui->comboBox_sample_type->itemData(ui->comboBox_sample_type->currentIndex()).value<int>());

	switch (collectType)
	{
	case DataCollectType::DATA_COLLECT_ARTIFICIAL:
	case DataCollectType::DATA_COLLECT_SENSOR:
	case DataCollectType::DATA_COLLECT_NDVI:
		break;
	case DataCollectType::DATA_COLLECT_MIX:
		break;
	default:
		break;
	}
}

void AgriSoilTestingDataSetDialog::updateData()
{
	// 获取当前选中的行
	QModelIndex currentIndex = ui->tableWidget->selectionModel()->currentIndex();
	if (currentIndex.isValid())
	{
		int row = ui->tableWidget->currentRow();
		if (row >= 0)
		{
			QTableWidgetItem *item = ui->tableWidget->item(row, 0);
			if (item)
			{
				// 采集点信息
				AgriSoilTestingCollection dataCollection = item->data(Qt::UserRole).value<AgriSoilTestingCollection>();

				DataCollectType tmpCollectType = DataCollectType::DATA_COLLECT_UNKNOW; //// = static_cast<DataCollectType>(dataCollection.collect_type);

				switch (tmpCollectType)
				{
				case DataCollectType::DATA_COLLECT_ARTIFICIAL:
				case DataCollectType::DATA_COLLECT_SENSOR:
				case DataCollectType::DATA_COLLECT_NDVI:
				break;
				case DataCollectType::DATA_COLLECT_MIX:
					break;
				default:
					break;
				}
			}
		}
	}
}

void AgriSoilTestingDataSetDialog::deleteData()
{
	// 获取当前选中的行
	QModelIndex currentIndex = ui->tableWidget->selectionModel()->currentIndex();
	if (currentIndex.isValid()) 
	{
		int row = ui->tableWidget->currentRow();
		if (row >= 0)
		{
			QTableWidgetItem *item = ui->tableWidget->item(row, 0);
			if (item)
			{
				AgriSoilTestingCollection dataCollection = item->data(Qt::UserRole).value<AgriSoilTestingCollection>();

				if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除样本点[%1]？").arg(QString::fromStdString(dataCollection.name)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				{
					bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->DeleteSoilTestingCollection(QString::fromStdString(dataCollection.guid));
					if (!isSuccess)
					{
						LyMessageBox::warning(this, tr2("错误"), tr2("数据库删除失败！"));
					}
					else
					{
						ui->tableWidget->removeRow(row); // 删除该行
					}
				}

				// 更新采集点绘制（删除）
				emit updateCollection(DataCollectHandle::DATA_COLLECT_DELETE, std::make_shared<AgriSoilTestingCollection>(dataCollection));
			}
		}
		else
		{
			LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
		}
	}
}

void AgriSoilTestingDataSetDialog::viewData()
{
	QModelIndex currentIndex = ui->tableWidget->selectionModel()->currentIndex();
	if (currentIndex.isValid())
	{
		int row = ui->tableWidget->currentRow();
		if (row >= 0)
		{
			if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
			{
				viewTableData(item);
			}
		}
	}
}

void AgriSoilTestingDataSetDialog::viewTableData(QTableWidgetItem *item)
{
	if (item)
	{
		// 采集点信息
		AgriSoilTestingCollection dataCollection = item->data(Qt::UserRole).value<AgriSoilTestingCollection>();

		//DataCollectType tmpCollectType = static_cast<DataCollectType>(dataCollection.collect_type);
	}
}

void AgriSoilTestingDataSetDialog::on_btnOK_clicked()
{
	if (ui->dataset_name->text().isEmpty())
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("数据集名称不能为空！"));
		return;
	}

	auto generateDataset = [&](bool isAdd = true)->AgriSoilTestingDatasetPtr {

		AgriSoilTestingDatasetPtr pDataset = std::make_shared<AgriSoilTestingDataset>();
		pDataset->guid = isAdd ? m_DataSetUid : m_pDatasetInfo.guid;
		pDataset->name = ui->dataset_name->text();
		pDataset->field_id = m_pFieldInfo->id;
		pDataset->sample_type = ui->comboBox_sample_type->itemData(ui->comboBox_sample_type->currentIndex()).value<int>();
		pDataset->sample_mode = ui->comboBox_sample_mode->itemData(ui->comboBox_sample_mode->currentIndex()).value<int>();

		QDateTime currentTime = QDateTime::currentDateTime();
		QString formattedTime = currentTime.toString("yyyy-MM-dd HH:mm:ss");
		pDataset->create_time = isAdd ? formattedTime : m_pDatasetInfo.create_time;
		pDataset->update_time = formattedTime;

		pDataset->grid_size = ui->grid_size->text().toDouble();
		pDataset->grid_rotation = ui->grid_rotation->text().toInt();

		pDataset->valid_identify = encodeCheckboxes();

		pDataset->collect_type = ui->comboBox_collect_type->itemData(ui->comboBox_collect_type->currentIndex()).value<int>();
		pDataset->soil_type = ui->comboBox_soil_type->itemData(ui->comboBox_soil_type->currentIndex()).value<int>();
		pDataset->soil_color = ui->comboBox_soil_color->itemData(ui->comboBox_soil_color->currentIndex()).value<int>();
		pDataset->soil_texture = ui->comboBox_soil_texture->itemData(ui->comboBox_soil_texture->currentIndex()).value<int>();
		pDataset->n_type = ui->comboBox_N_type->itemData(ui->comboBox_N_type->currentIndex()).value<int>();
		pDataset->p_type = ui->comboBox_P_type->itemData(ui->comboBox_P_type->currentIndex()).value<int>();
		pDataset->k_type = ui->comboBox_K_type->itemData(ui->comboBox_K_type->currentIndex()).value<int>();

		return pDataset;
	};

	switch (m_DataSetHandleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
		AddDataSet(generateDataset(true));
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
		UpdateDataSet(generateDataset(false));
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_DELETE:
		break;
	default:
		break;
	}
}

void AgriSoilTestingDataSetDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}

void AgriSoilTestingDataSetDialog::onPrevPage()
{
	if (m_currentPage > 1)
	{
		m_currentPage--;

		LoadTablePagedData();
	}
}

void AgriSoilTestingDataSetDialog::onNextPage()
{
	if (m_currentPage < m_totalPages)
	{
		m_currentPage++;

		LoadTablePagedData();
	}
}

void AgriSoilTestingDataSetDialog::onCellDoubleClicked(int row, int column)
{
	if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
	{
		viewTableData(item);
	}
}


void AgriSoilTestingDataSetDialog::on_comboBox_sample_type_currentTextChanged(int index)
{
	ui->comboBox_sample_mode->clear();

	SampleType sample_type = static_cast<SampleType>(ui->comboBox_sample_type->itemData(index).value<int>());
	switch (sample_type)
	{
	case SampleType::CELL_SAMPLING:
	{
		for (int i = 1; i < 5; ++i)
		{
			CellSample cell_sample = static_cast<CellSample>(i);
			ui->comboBox_sample_mode->addItem(AgriSoilAnalysisPredictUtil::SampleModeToString(sample_type, i), QVariant::fromValue(cell_sample));
		}
		break;
	}
	case SampleType::POINT_SAMPLING:
	{
		for (int i = 1; i < 5; ++i)
		{
			PointSample point_sample = static_cast<PointSample>(i);
			ui->comboBox_sample_mode->addItem(AgriSoilAnalysisPredictUtil::SampleModeToString(sample_type, i), QVariant::fromValue(point_sample));
		}
		break;
	}
	case SampleType::AREA_SAMPLING:
	{
		for (int i = 1; i < 2; ++i)
		{
			AreaSample area_sample = static_cast<AreaSample>(i);
			ui->comboBox_sample_mode->addItem(AgriSoilAnalysisPredictUtil::SampleModeToString(sample_type, i), QVariant::fromValue(area_sample));
		}
		break;
	}
	default:
		break;
	}

	ui->comboBox_sample_mode->setCurrentIndex(m_pDatasetInfo.sample_mode ? m_pDatasetInfo.sample_mode - 1 : 0);
}

void AgriSoilTestingDataSetDialog::on_comboBox_sample_mode_currentTextChanged(int index)
{
}

void AgriSoilTestingDataSetDialog::on_btn_generate_grid_clicked()
{
	m_bMap3DShow = true;
	m_bPointIdShow = true;

	// 清理Map3D
	cleanUpMap3D();

	m_heatMap3D = new HeatMap3D;

	SampleType sample_type = static_cast<SampleType>(ui->comboBox_sample_type->itemData(ui->comboBox_sample_type->currentIndex()).value<int>());
	int sample_mode = ui->comboBox_sample_mode->itemData(ui->comboBox_sample_mode->currentIndex()).value<int>();
	m_heatMap3D->setGridSampleInfo(sample_type, sample_mode);

	m_heatMap3D->setPixelSpacing(m_spacing);
	m_heatMap3D->setEqualCount(m_equalCount);
	m_heatMap3D->setDisCount(m_disCount);

	std::vector<double> _lons, _lats, _values; 

	// 获取当前激活区域
	auto activeZone = LY_AgriSoilAnalysisPredict::Get().GetActiveFieldZone();

	std::vector<QPointF> boundPoings;

	if (activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		if (activeZone.stZonePointVec.size() > 0) {

			for (auto zone : activeZone.stZonePointVec)
			{
				QPointF pointF;
				pointF.setX(zone.dLon);
				pointF.setY(zone.dLat);
				boundPoings.push_back(pointF);
			}
		}
	}

	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	m_heatMap3D->setMapNode(mapNode);
	m_heatMap3D->setBoundPoints(boundPoings);
	m_heatMap3D->setLonLatValue(_lons, _lats, _values);
	double gridAngle = osg::DegreesToRadians(10.0);
	m_heatMap3D->startCreateHeatMap(false, true, gridAngle);
}

void AgriSoilTestingDataSetDialog::on_ptn_export_points_clicked()
{
	const string& outputFile = "soilTesting.csv";
	ofstream out(outputFile);
	out << "name,lon,lat,\n";

	AgriSoilTestingCollectionPtrVec collectVec = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(m_pDatasetInfo.guid);
	for (const auto& point : collectVec) {
		out << point->name << ","
			<< QString::number(point->lon, 'f', 7).toStdString() << ","
			<< QString::number(point->lat, 'f', 7).toStdString() << "\n";
	}

	out.close();
	LyMessageBox::information(this, tr2("成功"), tr2("文件导出到soilTesting.csv！"));
}

void AgriSoilTestingDataSetDialog::on_ptn_generate_samples_clicked()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_pDatasetInfo.guid.isEmpty())
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("获取数据集id失败！"));
		return;
	}

	// 生成采样数据
	if (m_heatMap3D)
	{
		m_soilTestingCollectionVec.clear();

		m_heatMap3D->generateSamplesData(m_soilTestingCollectionVec);

		// 随机数生成器
		std::random_device rd;
		std::mt19937 gen(rd());

		/**  Beta 分布参数：a < b → 右偏（峰值偏左）
		 **		目标效果		|		参数设置
		 **  峰值更靠左（更偏） |	  a = 1, b = 6
		 **  稍微偏左			|	  a = 2, b = 4
		 **  中性（近似正态）	|	  a = 3, b = 3
		 **  峰值靠右（左偏）	|	  a = 6, b = 2
		**/
		double a = 2.0;
		double b = 4.0;

		std::gamma_distribution<double> distA(a, 1.0);
		std::gamma_distribution<double> distB(b, 1.0);

		auto generateRandomVal = [&](double zoomIn)->double {
			double x = distA(gen);
			double y = distB(gen);
			double beta = x / (x + y); // 在 0 ~ 1 区间
			return beta * zoomIn; // 调整缩放区间 (0, zoomIn)
		};

		// 按高斯（正态）分布生成浮点数
		//std::normal_distribution<qreal> dist(175.0, 50.0);

		//std::uniform_real_distribution<qreal> distN(0, 350);
		//std::uniform_real_distribution<qreal> distP(0, 50);
		//std::uniform_real_distribution<qreal> distK(0, 250);

		// 附加field_id
		std::for_each(m_soilTestingCollectionVec.begin(), m_soilTestingCollectionVec.end(),
			[&](AgriSoilTestingCollectionPtr& collect) { 
				collect->dataset_id = m_pDatasetInfo.guid.toStdString();

				// 保留两位小数
				collect->ele_nutrient_N = QString::number(generateRandomVal(350.0), 'f', 2).toDouble();
				collect->ele_nutrient_P = QString::number(generateRandomVal(50), 'f', 2).toDouble();
				collect->ele_nutrient_K = QString::number(generateRandomVal(250), 'f', 2).toDouble();

				QDateTime currentTime = QDateTime::currentDateTime();
				QString formattedTime = currentTime.toString("yyyy-MM-dd HH:mm:ss");
				collect->sample_time = formattedTime.toStdString();
		});

		// 更新表格数据
		updateDataToTable(m_soilTestingCollectionVec);
	}
}

void AgriSoilTestingDataSetDialog::on_ptn_save_samples_clicked()
{
	if (m_pDatasetInfo.guid.isEmpty())
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("获取数据集id失败！"));
		return;
	}

	if (m_soilTestingCollectionVec.empty()) 
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("未生成采样数据！"));
		return;
	}

	// 先清理后保存
	bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->BatchDeleteSoilTestingCollection(m_pDatasetInfo.guid);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("数据库删除失败！"));
	}
	else
	{
		bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->BatchInsertSoilTestingCollection(m_soilTestingCollectionVec);
		if (!isSuccess)
		{
			LyMessageBox::warning(this, tr2("错误"), tr2("数据库存储失败！"));
		}
		else
		{
			LyMessageBox::information(this, tr2("成功"), tr2("数据库保存成功！"));
		}
	}
}

void AgriSoilTestingDataSetDialog::on_ptn_import_samples_clicked()
{
	// 打开文件选择对话框
	QString	 sFilePath = QFileDialog::getOpenFileName(this, tr2("请选择测土文件"), "../data/", "Text Files (*.csv);;All Files (*.*)", nullptr, QFileDialog::Option::ReadOnly);
	if (!sFilePath.isEmpty()) {
		QFile file(sFilePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

			m_soilTestingCollectionVec.clear();
			m_soilTestingCollectionVec = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(m_pDatasetInfo.guid);

			QTextStream in(&file);
			bool firstLine = true;

			int importNums = 0;

			while (!in.atEnd()) {
				QString line = in.readLine().trimmed();
				if (line.isEmpty()) continue;

				if (firstLine) {
					firstLine = false;  // 跳过表头
					continue;
				}

				QStringList parts = line.split(","); // split(QRegExp("\\s+"));

				/*
				if (parts.size() < 6) {
					qWarning() << "Invalid line format:" << line;
					continue;
				}
				*/

				auto iter = std::find_if(m_soilTestingCollectionVec.begin(), m_soilTestingCollectionVec.end(), [&](const AgriSoilTestingCollectionPtr& collect) {
					return collect->name == parts[0].toStdString();
				});
				if (iter != m_soilTestingCollectionVec.end()) {
					auto tmpCollect = iter->get();
					tmpCollect->lon = parts[1].toDouble();
					tmpCollect->lat = parts[2].toDouble();
					tmpCollect->ele_nutrient_N = parts[3].isEmpty() ? 0.0 : parts[3].toDouble();
					tmpCollect->ele_nutrient_P = parts[4].toDouble();
					tmpCollect->ele_nutrient_K = parts[5].toDouble();

					importNums++;
				}
			}

			file.close();

			// 更新表格数据
			updateDataToTable(m_soilTestingCollectionVec);

			LyMessageBox::information(this, tr2("成功"), tr2("成功导入%1条数据！").arg(importNums));
		}
		else {
			LyMessageBox::warning(this, tr2("错误"), tr2("无法打开文件！"));
		}
	}
}

void AgriSoilTestingDataSetDialog::on_ptn_show_samples_clicked()
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setPointIdShowOrHide(m_bPointIdShow);
	}

	m_bPointIdShow = !m_bPointIdShow;
}

void AgriSoilTestingDataSetDialog::updateDataToTable(const AgriSoilTestingCollectionPtrVec& soilTestingCollectionVec)
{
	// 表格原数据清理
	while (ui->tableWidget->rowCount() > 0) {
		ui->tableWidget->removeRow(0);
	}

	m_currentPage = 1;
	m_totalPages = (m_soilTestingCollectionVec.size() + m_pageSize - 1) / m_pageSize; // 计算总页数;
	pageLabel->setText(tr2("第 %1 / %2 页").arg(m_totalPages ? m_currentPage : 0).arg(m_totalPages));

	// 数据添加到表格
	for (size_t i = 0; i < m_soilTestingCollectionVec.size(); i++)
	{
		if (i >= m_pageSize)
		{
			break; // TODO： 仅存放一页数据，剩余通过其数据库获取，数据未保存需单独处理		
		}
		AddOneRowToTable(m_soilTestingCollectionVec[i]);
	}
}

AgriSoilTestingFertilizer::AgriSoilTestingCollectionPtrVec AgriSoilTestingDataSetDialog::SimulateGetSoilTestingCollectionPagedData(int nPage, int nPageSize) const
{
	AgriSoilTestingCollectionPtrVec pageData;

	if (nPage <= 0 || nPageSize <= 0)
		return pageData; // 返回空结果

	int startIndex = (nPage - 1) * nPageSize;
	size_t endIndex = startIndex + nPageSize;

	// 假设 data 是成员变量或从某处传入
	if (startIndex >= m_soilTestingCollectionVec.size())
		return pageData; // 超出范围，返回空

	endIndex = std::min(endIndex, m_soilTestingCollectionVec.size());

	for (int i = startIndex; i < endIndex; ++i) {
		pageData.emplace_back(m_soilTestingCollectionVec[i]);
	}

	return pageData;
}

