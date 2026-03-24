#include <QDebug>
#include <QSqlError>
#include <QMenu>
#include <QSettings>
#include <QLabel>

#include "AgriSatelliteDataTree.h"
#include "ui_AgriSatelliteDataTree.h"
#include "LyMessageBox.h"
#include "AgriMultispectralSensorDBHandler.h"
#include "LY_AgriMultispectralSensor.h"
#include "AgriMultispectralSensorUtil.h"


AgriSatelliteDataTree::AgriSatelliteDataTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriSatelliteDataTree)
{
    ui->setupUi(this);

	// 设置提示信息
	ui->search_lineEdit->setPlaceholderText(tr2("请输入搜索信息..."));
	connect(ui->search_lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
		this->FilterTreeNodes(ui->treeWidget, text);
	});

	Init();
}

AgriSatelliteDataTree::~AgriSatelliteDataTree()
{
    delete ui;
}

void AgriSatelliteDataTree::Init()
{
	InitTree();

	CreateTree();

	m_pScrollArea = new QScrollArea();
	m_pContainerWidget = new QWidget();
	m_pGridLayout = new QGridLayout(m_pContainerWidget);
	m_pScrollArea->setWidget(m_pContainerWidget);
	m_pScrollArea->setWidgetResizable(true);
}

void AgriSatelliteDataTree::InitTree()
{
	ui->treeWidget->clear();
	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabels(QStringList() << tr2("卫星数据"));
}

void AgriSatelliteDataTree::CreateTree()
{
	BuildTree(ui->treeWidget);

	InsertLeafNode(ui->treeWidget);

	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &AgriSatelliteDataTree::onItemClicked);

	connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &AgriSatelliteDataTree::onItemChanged);

	ui->treeWidget->expandAll();
}


void AgriSatelliteDataTree::BuildTree(QTreeWidget *treeWidget)
{
	auto sentinelDataList = LY_AgriMultispectralSensor::Get().GetpDbHandler()->GetSentinelMetadataList();

	for(auto& sentinelData : sentinelDataList)
	{
		QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidget);

		QString formatData = AgriMultispectralSensorUtil::FormatToCompactISO(sentinelData->ingestion_date);
		if (!formatData.isEmpty()) {
			rootItem->setText(0, formatData);
			rootItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(*(sentinelData.get())));
			rootItem->setFlags(rootItem->flags() | Qt::ItemIsUserCheckable);  // 添加 check box 功能
			rootItem->setCheckState(0, Qt::Unchecked);	// 初始为未选中

			treeWidget->addTopLevelItem(rootItem);
		}else{
			qDebug() << "BuildTree Error: sentinelData FormatToCompactISO failed." << sentinelData->ingestion_date;
		}
	}
}

void AgriSatelliteDataTree::InsertLeafNode(QTreeWidget *treeWidget) 
{
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) 
	{
		QTreeWidgetItem *rootItem = treeWidget->topLevelItem(i);
		AgriSentinelMetadata sentinelData = rootItem->data(0, Qt::UserRole + 1).value<AgriSentinelMetadata>();

		for (auto &vgeIndexPair : m_mVegIndexMap)
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(rootItem);
			childItem->setText(0, vgeIndexPair.second);
			childItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(static_cast<int>(vgeIndexPair.first)));
			childItem->setFlags(childItem->flags() | Qt::ItemIsUserCheckable);
			childItem->setCheckState(0, Qt::Unchecked);

			rootItem->addChild(childItem);
		}
	}
}

void AgriSatelliteDataTree::FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter) 
{
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(i);
		FilterItemNodes(topItem, filter);
	}
}

bool AgriSatelliteDataTree::FilterItemNodes(QTreeWidgetItem *item, const QString &filter) 
{
	bool hasMatchingChild = false;

	for (int i = 0; i < item->childCount(); ++i) 
	{
		if (FilterItemNodes(item->child(i), filter)) 
		{
			hasMatchingChild = true;
		}
	}

	bool matches = item->text(0).contains(filter, Qt::CaseInsensitive);

	if (!matches && !hasMatchingChild) 
	{
		item->setHidden(true);
	}
	else 
	{
		item->setHidden(false);
		if (hasMatchingChild || matches) 
		{
			item->setExpanded(true);
		}
	}

	return matches || hasMatchingChild;
}

void AgriSatelliteDataTree::DeleteTreeNode(QTreeWidgetItem *item)
{
	if (item) {
		QTreeWidgetItem *parentItem = item->parent();
		if (parentItem) {
			parentItem->removeChild(item);  // 删除子节点
		}
		else {
			int index = ui->treeWidget->indexOfTopLevelItem(item);
			if (index != -1) {
				ui->treeWidget->takeTopLevelItem(index);  // 删除顶层节点
			}
		}
		delete item;  // 删除节点的内存
	}
}


void AgriSatelliteDataTree::onItemClicked(QTreeWidgetItem *item, int column)
{
	//emit LeafNodeClicked(item, column);
}

void AgriSatelliteDataTree::onItemChanged(QTreeWidgetItem *item, int column)
{
	if (column != 0) return;

	Qt::CheckState state = item->checkState(0);

	QTreeWidgetItem *parent = item->parent();
	if (parent) {
		QString key = parent->text(0) + "_" + item->text(0);
		switch (state)
		{
		case Qt::Unchecked:
			m_mImageInfoMap.remove(key);
			break;
		case Qt::Checked:
		{
			ImageInfo tInfo;
			VegetationIndex vegIndex = item->data(0, Qt::UserRole + 1).value<VegetationIndex>();

			tInfo.data = parent->text(0);
			tInfo.category = item->text(0);

			auto& itrMap = m_mVegIndexMap.find(vegIndex);
			if (itrMap != m_mVegIndexMap.end()){
				QString geneName = m_mVegIndexMap[vegIndex];
				tInfo.imagePath = QString("sentinel_image/output/%1/%2.png").arg(parent->text(0)).arg(geneName);
			}

			auto& itrDescMap = m_mVegIndexDescMap.find(vegIndex);
			if (itrDescMap != m_mVegIndexDescMap.end()){
				tInfo.title = m_mVegIndexDescMap[vegIndex];
				//tInfo.description = tr2("高NDRE值表明作物叶片叶绿素含量较高，间接说明氮素供应充足，光合作用能力强，生长健康。");
			}

			m_mImageInfoMap.insert(key, tInfo);
			break;
		}
		default:
			break;
		}
	}
	else{
		QString preKey = item->text(0) + "_";
		int count = item->childCount();
		switch (state)
		{
		case Qt::Unchecked:
			for (int i = 0; i < count; ++i) {
				QTreeWidgetItem *child = item->child(i);
				QString key = preKey + child->text(0);
				m_mImageInfoMap.remove(key);
			}
			break;
		case Qt::Checked:
			for (int i = 0; i < count; ++i) {
				QTreeWidgetItem *child = item->child(i);
				ImageInfo tInfo;
				VegetationIndex vegIndex = child->data(0, Qt::UserRole + 1).value<VegetationIndex>();

				tInfo.data = item->text(0);
				tInfo.category = child->text(0);

				auto& itrMap = m_mVegIndexMap.find(vegIndex);
				if (itrMap != m_mVegIndexMap.end()) {
					QString geneName = m_mVegIndexMap[vegIndex];
					tInfo.imagePath = QString("sentinel_image/output/%1/%2.png").arg(item->text(0)).arg(geneName);
				}

				auto& itrDescMap = m_mVegIndexDescMap.find(vegIndex);
				if (itrDescMap != m_mVegIndexDescMap.end()) {
					tInfo.title = m_mVegIndexDescMap[vegIndex];
					//tInfo.description = tr2("高NDRE值表明作物叶片叶绿素含量较高，间接说明氮素供应充足，光合作用能力强，生长健康。");
				}

				QString key = preKey + child->text(0);
				m_mImageInfoMap.insert(key, tInfo);
			}
			break;
		default:
			break;
		}

	}

	// 防止递归调用时多余触发
	disconnect(ui->treeWidget, &QTreeWidget::itemChanged, this, &AgriSatelliteDataTree::onItemChanged);

	// 处理子节点联动
	updateChildState(item, state);

	// 处理父节点联动
	updateParentState(item);

	connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &AgriSatelliteDataTree::onItemChanged);

	// 处理树节点更新
	AgriSatelliteDataTree::onTreeItemChanged(item, column);
}

void AgriSatelliteDataTree::updateChildState(QTreeWidgetItem *item, Qt::CheckState state)
{
	int count = item->childCount();

	for (int i = 0; i < count; ++i) 
	{
		QTreeWidgetItem *child = item->child(i);
		child->setCheckState(0, state);
	}
}

void AgriSatelliteDataTree::updateParentState(QTreeWidgetItem *item)
{
	QTreeWidgetItem *parent = item->parent();
	if (!parent) return;

	int checkedCount = 0;
	int uncheckedCount = 0;
	int childCount = parent->childCount();

	for (int i = 0; i < childCount; ++i) {
		Qt::CheckState state = parent->child(i)->checkState(0);
		if (state == Qt::Checked)
			++checkedCount;
		else if (state == Qt::Unchecked)
			++uncheckedCount;
	}

	Qt::CheckState newState;
	if (checkedCount == childCount)
		newState = Qt::Checked;
	else if (uncheckedCount == childCount)
		newState = Qt::Unchecked;
	else
		newState = Qt::PartiallyChecked;

	parent->setCheckState(0, newState);

	// 递归向上更新
	updateParentState(parent);
}

void AgriSatelliteDataTree::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
	if (column != 0) return;

	// 清空旧布局
	QLayoutItem *child;
	while ((child = m_pGridLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// 遍历所有子节点，找出已选中项
	int row = 0, col = 0;
	int totalRow = 0, tmpCol = 0;
	const int columnsPerRow = 3;
	const int maxExpand = 3;

	for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(i);
		for (int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *child = parent->child(j);
			if (child->checkState(0) == Qt::Checked) {
				tmpCol++;
				if (tmpCol >= columnsPerRow) {
					tmpCol = 0;
					totalRow++;
				}
			}
		}
	}

	std::vector<ImageInfo> imageCheckedVec;

	int parentCount = ui->treeWidget->topLevelItemCount();
	for (int i = 0; i < parentCount; ++i) {
		QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(i);
		int childCount = parent->childCount();
		for (int j = 0; j < childCount; ++j) {
			QTreeWidgetItem *child = parent->child(j);
			if (child->checkState(0) == Qt::Checked) {
				QString key = parent->text(0) + "_" + child->text(0); // e.g., "2024-09-23T03:05:19.000_NDRE"
				if (m_mImageInfoMap.contains(key)) {
					const ImageInfo &info = m_mImageInfoMap[key];

					// 创建图像显示控件
					QLabel *imageLabel = new QLabel;
					int expandSize = (maxExpand - totalRow) > 1 ? (maxExpand - totalRow) : 1;
					imageLabel->setPixmap(QPixmap(info.imagePath).scaled(400 * expandSize * 0.75, 300 * expandSize * 0.75, Qt::KeepAspectRatio));
					imageLabel->setAlignment(Qt::AlignCenter);
					imageLabel->setStyleSheet("border-width: 0px;");

					QLabel *titleLabel = new QLabel(/*tr2("图像名称：") +*/ info.title);
					titleLabel->setAlignment(Qt::AlignCenter);
					titleLabel->setStyleSheet("border-width: 0px;");
					//QLabel *descLabel = new QLabel(tr2("图像描述：") + info.description);
					QLabel *descLabel = new QLabel(parent->text(0));
					descLabel->setAlignment(Qt::AlignCenter);
					descLabel->setStyleSheet("border-width: 0px;");

					QVBoxLayout *vbox = new QVBoxLayout;
					vbox->addStretch();
					vbox->addWidget(imageLabel);
					vbox->addWidget(titleLabel);
					vbox->addWidget(descLabel);
					vbox->addStretch();

					QWidget *group = new QWidget;
					group->setStyleSheet("border-width: 1px; border-style: solid; border-color: #804ffd;");
					group->setLayout(vbox);

					m_pGridLayout->addWidget(group, row, col);

					col++;
					if (col >= columnsPerRow) {
						col = 0;
						row++;
					}

					imageCheckedVec.emplace_back(info);
				}
			}
		}
	}

	m_pContainerWidget->adjustSize();  // 刷新显示

	emit UpdateImageShow(m_pScrollArea);

	// 选中节点信号
	emit UpdateCheckedImages(imageCheckedVec);
}

