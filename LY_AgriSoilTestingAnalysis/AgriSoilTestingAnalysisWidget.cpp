#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QSettings>
#include <QCheckBox>

#include "CreateShape.h"
#include "ShapeClip.h"
#include "DrawShape.h"
#include "LyMessageBox.h"
#include "AgriSoilTestingAnalysisWidget.h"
#include "ui_AgriSoilTestingAnalysisWidget.h"
#include "LY_AgriSoilAnalysisPredict.h"
#include "AgriSoilTestingDataSetDialog.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "AgriSoilAnalysisPredictAlgorithm.h"
#include "LY_AgriSoilAnalysisPredict.h"


AgriSoilTestingAnalysisWidget::AgriSoilTestingAnalysisWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriSoilTestingAnalysisWidget),
    m_show(false)
{
    ui->setupUi(this);

	//土地图标
	m_sIconPath = (":/images/mointor/");

	Init();

	// 树叶子节点点击
	connect(ui->field_tree_widget, &AgriFieldTree::LeafNodeClicked, [this](QTreeWidgetItem *item, int column) {

		if (item && item->parent() == nullptr)
		{
			AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
			if (fieldInfo.id.length())
			{
				m_pFieldInfo = std::make_shared<AgriFieldInfo>(fieldInfo);

				// 更新表格展示数据
				UpdateTableData();

				// 清理数据集脏数据
				ui->distribute_widget->cleanUp();
			}
		}
	});
}

AgriSoilTestingAnalysisWidget::~AgriSoilTestingAnalysisWidget()
{
    delete ui;
}

AgriSoilTestingDistributeWidget* AgriSoilTestingAnalysisWidget::getPestAndDiseaseDataCollectWidget()
{
	return ui->distribute_widget;
}

AgriFieldTree* AgriSoilTestingAnalysisWidget::getAgriFieldTree()
{
	return ui->field_tree_widget;
}

qnzkna::ZoneManage::CZoneBase AgriSoilTestingAnalysisWidget::GetActiveFieldZone()
{
	if (ui->field_tree_widget)
	{
		return ui->field_tree_widget->GetActiveFieldZone();
	}
	return qnzkna::ZoneManage::CZoneBase();
}

void AgriSoilTestingAnalysisWidget::UpdataCurActiveFarm()
{
	if (ui->field_tree_widget)
	{
		ui->field_tree_widget->SwitchActiveFarm();
	}
}

void AgriSoilTestingAnalysisWidget::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (!m_show)
	{
		return;
	}

	m_soiltestingDrawMap.iterate_query([&](const std::string& id, const AgriSoilTestingCollectionPtrVec& collectionVec) {

		for (auto & collect : collectionVec)
		{
			QPoint point = pProj->Jwd2Screen(collect->lon, collect->lat);

			QPixmap pixmap;
			if (pixmap.load(m_sIconPath + "pest_warning.png"))
			{
				painter.drawPixmap(point.x() - 15, point.y() - 15, 30, 30, pixmap);
			}
		}
	});

	// 更新区域高亮
	ui->field_tree_widget->ZoneHighLight(painter, pProj);

	// 更新分布图(2D)
	ui->distribute_widget->draw2DGrid(painter, pProj);

	if (m_pestAndDiseaseDataSetDialog)
	{
		m_pestAndDiseaseDataSetDialog->draw2DGrid(painter, pProj);
	}
}

void AgriSoilTestingAnalysisWidget::On2d3dChanged(int wParam)
{

}

void AgriSoilTestingAnalysisWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriSoilTestingAnalysisWidget::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriSoilTestingAnalysisWidget::hideEvent(QHideEvent *event)
{
    m_show = false;

	// 清除绘制
	m_soiltestingDrawMap.clear();

	ui->field_tree_widget->ClearZoneHighLight();
}

void AgriSoilTestingAnalysisWidget::Init()
{
	ui->dateTime_begin->setDateTime(QDateTime::currentDateTime());
	ui->dateTime_end->setDateTime(QDateTime::currentDateTime());

	InitDisplayTable();

	InitDisplayCombox();
}

void AgriSoilTestingAnalysisWidget::InitDisplayTable()
{
	ui->tableWidget->setColumnCount(10);
	ui->tableWidget->setHorizontalHeaderLabels({
		tr2("序号"),
		tr2("显示"),
		tr2("盐田"),
		tr2("数据集名称"),
		tr2("采样类型"),
		tr2("采样方式"),
		tr2("创建时间"),
		tr2("修改时间"),
		tr2("网格大小"),
		tr2("网格朝向")
	});
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	// 连接单击信号，监听用户的单击操作
	connect(ui->tableWidget, &QTableWidget::cellClicked, this, &AgriSoilTestingAnalysisWidget::onCellClicked);

	// 连接双击信号，监听用户的双击操作
	connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &AgriSoilTestingAnalysisWidget::onCellDoubleClicked);

}

void AgriSoilTestingAnalysisWidget::InitDisplayCombox()
{
	for (int i = 0; i <= 3; ++i)
	{
		CropType crop_type = static_cast<CropType>(i);
		ui->comboBox_crop_type->addItem(AgriSoilAnalysisPredictUtil::CropTypeToString(crop_type), QVariant::fromValue(crop_type));
	}

	for (int i = 0; i <= 4; ++i)
	{
		DataCollectType acq_type = static_cast<DataCollectType>(i);
		ui->comboBox_acq_mode->addItem(AgriSoilAnalysisPredictUtil::DataCollectTypeToString(acq_type), QVariant::fromValue(acq_type));
	}

	for (int i = 1; i <= 3; ++i)
	{
		SampleType sampleType = static_cast<SampleType>(i);
		ui->comboBox_sample_type->addItem(AgriSoilAnalysisPredictUtil::SampleTypeToString(sampleType), QVariant::fromValue(sampleType));
	}

	// 下拉菜单点击
	connect(ui->comboBox_sample_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_sample_type_currentTextChanged(int)));

	on_comboBox_sample_type_currentTextChanged(0);
}


void AgriSoilTestingAnalysisWidget::CreateDataSetDialog(DataSetHandleMode handleMode)
{
	if (!m_pFieldInfo)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中到地块信息!"));
		return;
	}

	if (m_pestAndDiseaseDataSetDialog)
	{
		delete m_pestAndDiseaseDataSetDialog;
		m_pestAndDiseaseDataSetDialog = nullptr;
	}

	AgriSoilTestingDataset dataset;
	int row = ui->tableWidget->currentRow();
	if (row >= 0) 
	{
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
		{
			dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();
		}
	}

	switch (handleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
	{
		m_pestAndDiseaseDataSetDialog = new AgriSoilTestingDataSetDialog(handleMode, m_pFieldInfo, dataset, this);
	}
	break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
	{
		m_pestAndDiseaseDataSetDialog = new AgriSoilTestingDataSetDialog(handleMode, m_pFieldInfo, AgriSoilTestingDataset(), this);
		connect(m_pestAndDiseaseDataSetDialog, &AgriSoilTestingDataSetDialog::addFinish, this, &AgriSoilTestingAnalysisWidget::HandleDatasetAddFinish);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
	{
		m_pestAndDiseaseDataSetDialog = new AgriSoilTestingDataSetDialog(handleMode, m_pFieldInfo, dataset, this);
		connect(m_pestAndDiseaseDataSetDialog, &AgriSoilTestingDataSetDialog::updateFinish, this, &AgriSoilTestingAnalysisWidget::HandleDatasetUpdateFinish);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_DELETE:
		break;
	default:
		break;
	}

	if (m_pestAndDiseaseDataSetDialog)m_pestAndDiseaseDataSetDialog->show();

	// 采集点更新
	connect(m_pestAndDiseaseDataSetDialog, &AgriSoilTestingDataSetDialog::updateCollection, [this](DataCollectHandle handle, AgriSoilTestingCollectionPtr soiltestingCollection) {

		switch (handle)
		{
		case DataCollectHandle::DATA_COLLECT_ADD:
		{
			m_soiltestingDrawMap.iterate_update([&](const std::string& id, AgriSoilTestingCollectionPtrVec& collectionVec) {

				if (id == soiltestingCollection->dataset_id)
				{
					// 添加
					collectionVec.emplace_back(soiltestingCollection);
				}
			});
		}
			break;
		case DataCollectHandle::DATA_COLLECT_UPDATE:
		{
			m_soiltestingDrawMap.iterate_update([&](const std::string& id, AgriSoilTestingCollectionPtrVec& collectionVec) {

				if (id == soiltestingCollection->dataset_id)
				{
					// 更新(先删除再添加)
					collectionVec.erase(std::remove_if(collectionVec.begin(), collectionVec.end(),
						[soiltestingCollection](const auto& collect) {
						return collect->guid == soiltestingCollection->guid;
					}),
						collectionVec.end());
					collectionVec.emplace_back(soiltestingCollection);
				}
			});
		}
			break;
		case DataCollectHandle::DATA_COLLECT_DELETE:
		{
			m_soiltestingDrawMap.iterate_update([&](const std::string& id, AgriSoilTestingCollectionPtrVec& collectionVec) {

				if (id == soiltestingCollection->dataset_id)
				{
					// 使用 remove_if 删除
					collectionVec.erase(std::remove_if(collectionVec.begin(), collectionVec.end(),
						[soiltestingCollection](const auto& collect) {
						return collect->guid == soiltestingCollection->guid;
					}),
						collectionVec.end());
				}
			});
		}
			break;
		default:
			break;
		}
	});
}

void AgriSoilTestingAnalysisWidget::HandleDatasetAddFinish(AgriSoilTestingDatasetPtr soilTestingDataset)
{
	m_pestAndDiseaseDataSetDialog->close();

	if (soilTestingDataset)
	{
		AddOneRowTotable(soilTestingDataset);
	}

	LyMessageBox::information(this, tr2("成功"), tr2("添加数据集成功！"));
}

void AgriSoilTestingAnalysisWidget::HandleDatasetUpdateFinish(AgriSoilTestingDatasetPtr soilTestingDataset)
{
	m_pestAndDiseaseDataSetDialog->close();

	if (soilTestingDataset)
	{
		UpdateOneRowTotable(soilTestingDataset);
	}

	LyMessageBox::information(this, tr2("成功"), tr2("更新数据集成功！"));
}

void AgriSoilTestingAnalysisWidget::UpdateTableData()
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	if (m_pFieldInfo)
	{
		// 查询数据集列表
		auto soilTestingDatasetList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingDatasetList(QStringList({ m_pFieldInfo->id }));

		for (int row = 0; row < soilTestingDatasetList.size(); ++row)
		{
			AddOneRowTotable(soilTestingDatasetList[row]);
		}
	}
}


void AgriSoilTestingAnalysisWidget::AddOneRowTotable(AgriSoilTestingDatasetPtr soilTestingDataset)
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);

	QCheckBox* checkBox = new QCheckBox();
	QWidget* radioWidget = new QWidget();
	QHBoxLayout* checkBoxLayout = new QHBoxLayout(radioWidget);
	checkBoxLayout->addWidget(checkBox);
	checkBoxLayout->setAlignment(Qt::AlignCenter);
	checkBoxLayout->setContentsMargins(0, 0, 0, 0);

	// 连接复选框的选中信号
	connect(checkBox, &QCheckBox::toggled, this, [this, rowCount, checkBox](bool checked) {
		onCheckBoxToggled(rowCount, checked);
	});

	auto userDefItem = new QTableWidgetItem(QString::number(rowCount + 1));
	userDefItem->setTextAlignment(Qt::AlignCenter);
	userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(soilTestingDataset.get())));
	ui->tableWidget->setItem(rowCount, 0, userDefItem);

	ui->tableWidget->setCellWidget(rowCount, 1, radioWidget);
	ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(m_pFieldInfo->name));
	ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(soilTestingDataset->name));
	ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleTypeToString(static_cast<SampleType>(soilTestingDataset->sample_type))));
	ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleModeToString(static_cast<SampleType>(soilTestingDataset->sample_type), soilTestingDataset->sample_mode)));
	ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(soilTestingDataset->create_time));
	ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(soilTestingDataset->update_time));
	ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(QString::number(soilTestingDataset->grid_size)));
	ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(QString::number(soilTestingDataset->grid_rotation)));
}


void AgriSoilTestingAnalysisWidget::UpdateOneRowTotable(AgriSoilTestingDatasetPtr soilTestingDataset)
{
	int rowCount = ui->tableWidget->rowCount();
	for (int row = 0; row < rowCount; ++row) {
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0)) {
			AgriSoilTestingDataset dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();
			if (dataset.guid == soilTestingDataset->guid) {

				auto userDefItem = new QTableWidgetItem(QString::number(row + 1));
				userDefItem->setTextAlignment(Qt::AlignCenter);
				userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(soilTestingDataset.get())));
				ui->tableWidget->setItem(row, 0, userDefItem);
				ui->tableWidget->setItem(row, 2, new QTableWidgetItem(m_pFieldInfo->name));
				ui->tableWidget->setItem(row, 3, new QTableWidgetItem(soilTestingDataset->name));
				ui->tableWidget->setItem(row, 4, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleTypeToString(static_cast<SampleType>(soilTestingDataset->sample_type))));
				ui->tableWidget->setItem(row, 5, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleModeToString(static_cast<SampleType>(soilTestingDataset->sample_type), soilTestingDataset->sample_mode)));
				ui->tableWidget->setItem(row, 6, new QTableWidgetItem(soilTestingDataset->create_time));
				ui->tableWidget->setItem(row, 7, new QTableWidgetItem(soilTestingDataset->update_time));
				ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(soilTestingDataset->grid_size)));
				ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(soilTestingDataset->grid_rotation)));
			}
		}
	}
}

void AgriSoilTestingAnalysisWidget::Update3DIconDraw(const AgriSoilTestingCollectionPtrVec& soilTestingCollectVec, bool isDraw)
{
	if (isDraw)
	{
		for (auto & collect : soilTestingCollectVec)
		{
			IEarth *pEarth = IEarth::GetEarth();
			if (pEarth == nullptr) return;

			QCString m_strDataDir = (m_sIconPath + "pest_warning.png").toStdString().c_str();
			QString strLable = "";
			double terrainHeight = 0.0;

			osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
			if (mapNode)
			{
				if (const auto& terrain = mapNode->getTerrain())
				{
					terrain->getHeight(mapNode->getMapSRS(), collect->lon, collect->lat, &terrainHeight);
				}
			}

			// 通过terrainHeight设置自动贴地
			if (const auto& graphics = pEarth->GetGraphics())
			{
				int pixel = 30;
				double mapDis = 0.0;
				if (auto proj = LY_AgriSoilAnalysisPredict::Get().GetProj())
				{
					mapDis = proj->Screen2MapDistance(1/*(pixel/2)*/); // TODO：确定实际pixel转距离值
				}
				;
				ILabel *pLabel = graphics->CreateLabel(strLable.toLocal8Bit(), m_strDataDir, collect->lon, collect->lat, terrainHeight + mapDis, pixel, pixel, 0);
				if (pLabel != nullptr) {
					m_3DPestCollectIconVec.emplace_back(pLabel);
				}
			}
		}
	}
	else
	{
		IEarth *pEarth = IEarth::GetEarth();
		if (pEarth == nullptr)
			return;
		auto itr = m_3DPestCollectIconVec.begin();
		for (; itr != m_3DPestCollectIconVec.end(); itr++)
		{
			IEarth *pEarth = IEarth::GetEarth();
			pEarth->GetGraphics()->RemoveObject(*itr);
			*itr = nullptr;
			delete *itr;
		}
		m_3DPestCollectIconVec.clear();
	}
}

void AgriSoilTestingAnalysisWidget::on_ptn_view_dataset_clicked()
{
	if (-1 == m_nSelectedIndex)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
		return;
	}
	
	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_VIEW);
}

void AgriSoilTestingAnalysisWidget::on_ptn_add_dataset_clicked()
{
	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_ADD);
}

void AgriSoilTestingAnalysisWidget::on_ptn_delete_dataset_clicked()
{
	int row = ui->tableWidget->currentRow();
	if (row >= 0) {

		QTableWidgetItem *item = ui->tableWidget->item(row, 0);
		if (item)
		{
			AgriSoilTestingDataset dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();

			if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除数据集[%1]？").arg(dataset.name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->DeleteSoilTestingDataset(dataset.guid);
				if (!isSuccess)
				{
					LyMessageBox::warning(this, "错误", "数据库删除失败！");
				}
				else
				{
					ui->tableWidget->removeRow(row); // 删除该行

					// 移除所有采集点展示
					m_soiltestingDrawMap.remove(dataset.guid.toStdString());
				}
			}
		}
	}
	else {
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
	}
}

void AgriSoilTestingAnalysisWidget::on_ptn_update_dataset_clicked()
{
	if (-1 == m_nSelectedIndex)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
		return;
	}

	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_UPDATE);
}

void AgriSoilTestingAnalysisWidget::on_ptn_export_dataset_clicked()
{
	// 导出
}

void AgriSoilTestingAnalysisWidget::onCellClicked(int row, int column)
{
	QTableWidgetItem *item = ui->tableWidget->item(row, 0);
	if (item) {
		m_nSelectedIndex = row;

		AgriSoilTestingDataset dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();

		// 更新病虫害分布图界面
		ui->distribute_widget->setCurrentDataset(std::make_shared<AgriSoilTestingDataset>(dataset));
	}
}

void AgriSoilTestingAnalysisWidget::onCellDoubleClicked(int row, int column)
{
	// 获取双击单元格的数据
	QTableWidgetItem *item = ui->tableWidget->item(row, column);
	if (item) {

		m_nSelectedIndex = row;

		CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_VIEW);
	}
}

void AgriSoilTestingAnalysisWidget::onCheckBoxToggled(int row, bool checked)
{
	QTableWidgetItem *item = ui->tableWidget->item(row, 0);
	if (item) {

		AgriSoilTestingDataset dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();

		// 查询土测点列表
		auto soiltestingCollectionList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(dataset.guid);

		if (checked)
		{
			m_soiltestingDrawMap.insert(dataset.guid.toStdString(), soiltestingCollectionList);
		}
		else
		{
			m_soiltestingDrawMap.remove(dataset.guid.toStdString());
		}

		// 更新3D绘制
		Update3DIconDraw(soiltestingCollectionList, checked);
	}
}

void AgriSoilTestingAnalysisWidget::on_comboBox_sample_type_currentTextChanged(int index)
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

	ui->comboBox_sample_mode->setCurrentIndex(0);
}
