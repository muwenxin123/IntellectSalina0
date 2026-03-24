#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>

#include "QCString.h"
#include "AgriPestAndDiseaseDataSetDialog.h"
#include "ui_AgriPestAndDiseaseDataSetDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LyMessageBox.h"
#include "AgriPestAndDiseaseDataCollectDialog.h"
#include "AgriDataAnalysisPredictUtil.h"
#include "LY_AgriDataAnalysisPredict.h"


AgriPestAndDiseaseDataSetDialog::AgriPestAndDiseaseDataSetDialog(DataSetHandleMode handleMode, const AgriFieldInfoPtr& fieldInfo, AgriPestDataset datasetInfo, QWidget *parent /*= nullptr*/)
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
	ui(new Ui::AgriPestAndDiseaseDataSetDialog)
{
	// 生成数据集uid
	m_DataSetUid = AgriDataAnalysisPredictUtil::GenerateUuid();

	m_DataSetHandleMode = handleMode;

	m_pFieldInfo = fieldInfo;

	m_pDatasetInfo = datasetInfo;

	Init();

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriPestAndDiseaseDataSetDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriPestAndDiseaseDataSetDialog::on_btnCancel_clicked);
}


AgriPestAndDiseaseDataSetDialog::~AgriPestAndDiseaseDataSetDialog()
{
    delete ui;
}

AgriPestAndDiseaseDataCollectDialog* AgriPestAndDiseaseDataSetDialog::getPestAndDiseaseDataCollectDialog()
{
	return m_dataCollectDialog;
}

void AgriPestAndDiseaseDataSetDialog::closeEvent(QCloseEvent* event)
{

}

void AgriPestAndDiseaseDataSetDialog::contextMenuEvent(QContextMenuEvent *event)
{
	// 恢复状态（从鼠标选点）
	if (m_dataCollectDialog)
	{
		m_dataCollectDialog->updateTargetPoint(0.0, 0.0, 0.0);
	}

	if (m_DataSetHandleMode == DataSetHandleMode::DATA_SET_HANDLE_VIEW)
	{
		QMenu menu(this);

		QAction* viewAction = new QAction(tr2("查看采集点"), this);
		QAction* addAction = new QAction(tr2("添加采集点"), this);
		QAction* updateAction = new QAction(tr2("修改采集点"), this);
		QAction* deleteAction = new QAction(tr2("删除采集点"), this);

		// 绑定菜单项的动作
		connect(viewAction, &QAction::triggered, this, &AgriPestAndDiseaseDataSetDialog::viewData);
		connect(addAction, &QAction::triggered, this, &AgriPestAndDiseaseDataSetDialog::addData);
		connect(updateAction, &QAction::triggered, this, &AgriPestAndDiseaseDataSetDialog::updateData);
		connect(deleteAction, &QAction::triggered, this, &AgriPestAndDiseaseDataSetDialog::deleteData);

		menu.addAction(viewAction);
		menu.addAction(addAction);
		menu.addAction(updateAction);
		menu.addAction(deleteAction);

		menu.exec(event->globalPos());
	}
}

void AgriPestAndDiseaseDataSetDialog::Init()
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	ui->dataset_name->setPlaceholderText(tr2("例：A01"));

	switch (m_DataSetHandleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
	{
		ui->dataset_name->setText(m_pDatasetInfo.name);

		InitDisplayCombox();
		InitDisplayTable();
		UpdateTableData();

		ui->btnOK->hide();
		ui->btnCancel->hide();

		// 禁用编辑
		ui->dataset_name->setReadOnly(true);
		// ui->comboBox_collect_type->setDisabled(true);
		ui->comboBox_crop_type->setDisabled(true);
		ui->comboBox_pest_disease->setDisabled(true);
		ui->comboBox_pest_type->setDisabled(true);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
	{
		ui->dataset_name->setText("");

		InitDisplayCombox();

		ui->btnOK->show();
		ui->btnCancel->show();
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
	{
		ui->dataset_name->setText(m_pDatasetInfo.name);

		InitDisplayCombox();
		InitDisplayTable();
		UpdateTableData();

		ui->btnOK->show();
		ui->btnCancel->show();
	}
		break;
	default:
		break;
	}
}

void AgriPestAndDiseaseDataSetDialog::InitDisplayTable()
{
	ui->tableWidget->setColumnCount(12);
	ui->tableWidget->setHorizontalHeaderLabels({
		tr2("序号"),
		tr2("采集点"),
		tr2("采集时间"),
		tr2("经度"),
		tr2("纬度"),
		tr2("高度"),
		tr2("虫类型"),
		tr2("虫阶段"),
		tr2("数量"),
		tr2("范围"),
		tr2("等级"),
		tr2("数据来源")
	});
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
	ui->verticalLayout_2->addLayout(btnLayout);

	connect(prevButton, &QPushButton::clicked, this, &AgriPestAndDiseaseDataSetDialog::onPrevPage);
	connect(nextButton, &QPushButton::clicked, this, &AgriPestAndDiseaseDataSetDialog::onNextPage);

	// 连接双击信号，监听用户的双击操作
	connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &AgriPestAndDiseaseDataSetDialog::onCellDoubleClicked);
}

void AgriPestAndDiseaseDataSetDialog::InitDisplayCombox()
{
	for (int i = 1; i < 4; ++i)
	{
		CropType crop_type = static_cast<CropType>(i);
		ui->comboBox_crop_type->addItem(AgriDataAnalysisPredictUtil::CropTypeToString(crop_type), QVariant::fromValue(crop_type));
	}

	for (int i = 1; i < 5; ++i)
	{
		DataCollectType acq_type = static_cast<DataCollectType>(i);
		ui->comboBox_collect_type->addItem(AgriDataAnalysisPredictUtil::DataCollectTypeToString(acq_type), QVariant::fromValue(acq_type));
	}

	for (int i = 1; i < 4; ++i)
	{
		PestDisease pestDisease = static_cast<PestDisease>(i);
		ui->comboBox_pest_disease->addItem(AgriDataAnalysisPredictUtil::PestDiseaseToString(pestDisease), QVariant::fromValue(pestDisease));
	}

	for (int i = 1; i < 5; ++i)
	{
		PestCategory category = static_cast<PestCategory>(i);
		ui->comboBox_pest_type->addItem(AgriDataAnalysisPredictUtil::PestCategoryToString(category), QVariant::fromValue(category));
	}
}


void AgriPestAndDiseaseDataSetDialog::AddDataSet(AgriPestDatasetPtr pestDataset)
{
	bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->AddPestDataset(pestDataset);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("新增数据集失败！"));
	}
	else
	{
		emit addFinish(pestDataset);
	}
}


void AgriPestAndDiseaseDataSetDialog::UpdateDataSet(AgriPestDatasetPtr pestDataset)
{
	bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->UpdatePestDataset(pestDataset);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("更新数据集失败！"));
	}
	else
	{
		emit updateFinish(pestDataset);
	}
}

void AgriPestAndDiseaseDataSetDialog::HandleDataCollectionAddFinish(AgriPestCollectionPtr pestCollection)
{
	m_dataCollectDialog->close();

	if (pestCollection)
	{
		AddOneRowToTable(pestCollection);

		// 翻页检测
		int rowCount = ui->tableWidget->rowCount();
		if (rowCount == m_pageSize + 1)
		{
			m_totalPages += 1;
			onNextPage();
		}
	}

	// 更新采集点绘制（添加）
	emit updateCollection(DataCollectHandle::DATA_COLLECT_ADD, pestCollection);
}


void AgriPestAndDiseaseDataSetDialog::HandleDataCollectionUpdateFinish(AgriPestCollectionPtr pestCollection)
{
	m_dataCollectDialog->close();

	if (pestCollection)
	{
		UpdateRowToTable(pestCollection);
	}

	// 更新采集点绘制（更新）
	emit updateCollection(DataCollectHandle::DATA_COLLECT_UPDATE, pestCollection);
}

void AgriPestAndDiseaseDataSetDialog::UpdateTableData()
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	if (m_pFieldInfo)
	{
		/*
		// 查询采集点列表
		auto pestCollectionList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionList(m_pDatasetInfo.guid);

		for (int row = 0; row < pestCollectionList.size(); ++row)
		{
			AddOneRowToTable(pestCollectionList[row]);
		}
		*/

		// 分页查询
		LoadTablePagedData();
	}
}


void AgriPestAndDiseaseDataSetDialog::AddOneRowToTable(AgriPestCollectionPtr pestCollection)
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);

	// 计算表格行序号
	int rowNum = rowCount + 1 + m_pageSize * (m_currentPage - 1);

	auto userDefItem = new QTableWidgetItem(QString::number(rowNum));
	userDefItem->setTextAlignment(Qt::AlignCenter);
	userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(pestCollection.get())));
	ui->tableWidget->setItem(rowCount, 0, userDefItem);

	FillTableRowContents(pestCollection, rowCount);
}

void AgriPestAndDiseaseDataSetDialog::UpdateRowToTable(AgriPestCollectionPtr pestCollection)
{
	if (!pestCollection) return;

	// 匹配当前行
	for (int row = 0; row < ui->tableWidget->rowCount(); ++row) 
	{
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
		{
			// 采集点信息
			AgriPestCollection dataCollection = item->data(Qt::UserRole).value<AgriPestCollection>();
			if (dataCollection.guid == pestCollection->guid)
			{
				item->setData(Qt::UserRole, QVariant::fromValue(*(pestCollection.get())));
				ui->tableWidget->setItem(row, 0, item);

				FillTableRowContents(pestCollection, row);
			}
		}
	}
}


void AgriPestAndDiseaseDataSetDialog::LoadTablePagedData()
{
	auto pestCollectionList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionPagedData(m_pDatasetInfo.guid, m_currentPage, m_pageSize);
	int totalRecords = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionCount(m_pDatasetInfo.guid);
	m_totalPages = (totalRecords + m_pageSize - 1) / m_pageSize; // 计算总页数

	ui->tableWidget->setRowCount((int)pestCollectionList.size());
	for (size_t i = 0; i < pestCollectionList.size(); ++i)
	{
		int rowCount = static_cast<int>(i);
		AgriPestCollectionPtr pestCollection = pestCollectionList[i];

		// 计算表格行序号
		int rowNum = rowCount + 1 + m_pageSize * (m_currentPage - 1);
		auto userDefItem = new QTableWidgetItem(QString::number(rowNum));
		userDefItem->setTextAlignment(Qt::AlignCenter);
		userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(pestCollection.get())));
		ui->tableWidget->setItem(rowCount, 0, userDefItem);

		FillTableRowContents(pestCollection, rowCount);
	}

	pageLabel->setText(tr2("第 %1 / %2 页").arg(m_currentPage).arg(m_totalPages));
}


void AgriPestAndDiseaseDataSetDialog::FillTableRowContents(AgriPestCollectionPtr pestCollection, int nRow)
{
	ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(pestCollection->name));
	ui->tableWidget->setItem(nRow, 2, new QTableWidgetItem(pestCollection->collect_time));
	ui->tableWidget->setItem(nRow, 3, new QTableWidgetItem(QString::number(pestCollection->lon, 'f', 7)));
	ui->tableWidget->setItem(nRow, 4, new QTableWidgetItem(QString::number(pestCollection->lat, 'f', 7)));
	ui->tableWidget->setItem(nRow, 5, new QTableWidgetItem(QString::number(pestCollection->alt)));
	ui->tableWidget->setItem(nRow, 6, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestCategoryToString(static_cast<PestCategory>(pestCollection->category))));
	ui->tableWidget->setItem(nRow, 7, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestStageToString(static_cast<PestStage>(pestCollection->stage))));
	ui->tableWidget->setItem(nRow, 8, new QTableWidgetItem(QString::number(pestCollection->count)));
	ui->tableWidget->setItem(nRow, 9, new QTableWidgetItem(QString::number(pestCollection->range)));
	ui->tableWidget->setItem(nRow, 10, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestLevelToString(static_cast<PestLevel>(pestCollection->level))));
	ui->tableWidget->setItem(nRow, 11, new QTableWidgetItem(AgriDataAnalysisPredictUtil::DataCollectTypeToString(static_cast<DataCollectType>(pestCollection->collect_type))));
}

void AgriPestAndDiseaseDataSetDialog::addData()
{
	if (m_dataCollectDialog)
	{
		delete m_dataCollectDialog;
	}

	DataCollectType collectType = static_cast<DataCollectType>(ui->comboBox_collect_type->itemData(ui->comboBox_collect_type->currentIndex()).value<int>());
	m_dataCollectDialog = new AgriPestAndDiseaseDataCollectDialog(collectType, m_pDatasetInfo, this);
	m_dataCollectDialog->show();

	switch (collectType)
	{
	case DataCollectType::DATA_COLLECT_ARTIFICIAL:
	case DataCollectType::DATA_COLLECT_SENSOR:
	case DataCollectType::DATA_COLLECT_IMAGE:
	{
		connect(m_dataCollectDialog, &AgriPestAndDiseaseDataCollectDialog::addDataCollectionFinish, this, &AgriPestAndDiseaseDataSetDialog::HandleDataCollectionAddFinish);
	}
		break;
	case DataCollectType::DATA_COLLECT_IMPORT:
		break;
	default:
		break;
	}
}

void AgriPestAndDiseaseDataSetDialog::updateData()
{
	if (m_dataCollectDialog)
	{
		delete m_dataCollectDialog;
		m_dataCollectDialog = nullptr;
	}

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
				AgriPestCollection dataCollection = item->data(Qt::UserRole).value<AgriPestCollection>();

				DataCollectType tmpCollectType = static_cast<DataCollectType>(dataCollection.collect_type);
				m_dataCollectDialog = new AgriPestAndDiseaseDataCollectDialog(tmpCollectType, m_pDatasetInfo, dataCollection, this);
				m_dataCollectDialog->show();

				switch (tmpCollectType)
				{
				case DataCollectType::DATA_COLLECT_ARTIFICIAL:
				case DataCollectType::DATA_COLLECT_SENSOR:
				case DataCollectType::DATA_COLLECT_IMAGE:
				{
					connect(m_dataCollectDialog, &AgriPestAndDiseaseDataCollectDialog::updateDataCollectionFinish, this, &AgriPestAndDiseaseDataSetDialog::HandleDataCollectionUpdateFinish);
				}
				break;
				case DataCollectType::DATA_COLLECT_IMPORT:
					break;
				default:
					break;
				}
			}
		}
	}
}

void AgriPestAndDiseaseDataSetDialog::deleteData()
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
				AgriPestCollection dataCollection = item->data(Qt::UserRole).value<AgriPestCollection>();

				if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除采集点[%1]？").arg(dataCollection.name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				{
					bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->DeletePestCollection(dataCollection.guid);
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
				emit updateCollection(DataCollectHandle::DATA_COLLECT_DELETE, std::make_shared<AgriPestCollection>(dataCollection));
			}
		}
		else
		{
			LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
		}
	}
}

void AgriPestAndDiseaseDataSetDialog::viewData()
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

void AgriPestAndDiseaseDataSetDialog::viewTableData(QTableWidgetItem *item)
{
	if (m_dataCollectDialog)
	{
		delete m_dataCollectDialog;
		m_dataCollectDialog = nullptr;
	}

	if (item)
	{
		// 采集点信息
		AgriPestCollection dataCollection = item->data(Qt::UserRole).value<AgriPestCollection>();

		DataCollectType tmpCollectType = static_cast<DataCollectType>(dataCollection.collect_type);
		m_dataCollectDialog = new AgriPestAndDiseaseDataCollectDialog(m_pDatasetInfo, dataCollection, tmpCollectType, this);
		m_dataCollectDialog->show();
	}
}

void AgriPestAndDiseaseDataSetDialog::on_btnOK_clicked()
{
	if (ui->dataset_name->text().isEmpty())
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("数据集名称不能为空！"));
		return;
	}

	auto generateDataset = [&](bool isAdd = true)->AgriPestDatasetPtr {
	
		AgriPestDatasetPtr pDataset = std::make_shared<AgriPestDataset>();
		pDataset->guid = isAdd ? m_DataSetUid : m_pDatasetInfo.guid;
		pDataset->name = ui->dataset_name->text();
		pDataset->field_id = m_pFieldInfo->id;

		//int curIndex = ui->comboBox_crop_type->currentIndex();
		//QVariant tmpVariant = ui->comboBox_crop_type->itemData(curIndex);
		//int tmpId = tmpVariant.value<int>();

		pDataset->crop_type = ui->comboBox_crop_type->itemData(ui->comboBox_crop_type->currentIndex()).value<int>();
		pDataset->collect_type = ui->comboBox_collect_type->itemData(ui->comboBox_collect_type->currentIndex()).value<int>();
		/*	TODO：暂未添加日期选项框
		pDataset->begin_time = ui->begin_time->dateTime().toString("yyyy-MM-dd HH:mm:ss");
		pDataset->end_time = ui->end_time->dateTime().toString("yyyy-MM-dd HH:mm:ss");
		*/
		pDataset->pest_disease = ui->comboBox_pest_disease->itemData(ui->comboBox_pest_disease->currentIndex()).value<int>();
		pDataset->pest_category = ui->comboBox_pest_type->itemData(ui->comboBox_pest_type->currentIndex()).value<int>();

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

void AgriPestAndDiseaseDataSetDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}

void AgriPestAndDiseaseDataSetDialog::onPrevPage()
{
	if (m_currentPage > 1)
	{
		m_currentPage--;

		LoadTablePagedData();
	}
}

void AgriPestAndDiseaseDataSetDialog::onNextPage()
{
	if (m_currentPage < m_totalPages)
	{
		m_currentPage++;

		LoadTablePagedData();
	}
}

void AgriPestAndDiseaseDataSetDialog::onCellDoubleClicked(int row, int column)
{
	if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
	{
		viewTableData(item);
	}
}

