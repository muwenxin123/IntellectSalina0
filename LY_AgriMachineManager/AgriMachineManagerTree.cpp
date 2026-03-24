#include <QDebug>
#include <QSqlError>

#include "AgriMachineManagerTree.h"
#include "ui_AgriMachineManagerTree.h"
#include "LY_AgriMachineManager.h"

AgriMachineManagerTree::AgriMachineManagerTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriMachineManagerTree)
{
    ui->setupUi(this);

	// 设置提示信息
	ui->search_lineEdit->setPlaceholderText(tr2("请输入搜索信息..."));
	connect(ui->search_lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
		this->FilterTreeNodes(ui->treeWidget, text);
	});

	// 仅看有值
	connect(ui->filter_checkBox, &QCheckBox::toggled, this, [this](bool checked) {
		this->onCheckBoxToggled(checked);
	});

    CreateTree();
}

AgriMachineManagerTree::~AgriMachineManagerTree()
{
    delete ui;
}

void AgriMachineManagerTree::CreateTree()
{
	ui->treeWidget->clear();
	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabels(QStringList() << tr2("农业机具管理"));

	strTreeTableName = GENERAL_TREE_TABLE_NAME;
	BuildTree(ui->treeWidget, AGRI_DIR_MACHINE_GENERAL_ROOT, nullptr);
	strTreeTableName = PLANTATION_TREE_TABLE_NAME;
	BuildTree(ui->treeWidget, AGRI_DIR_MACHINE_PLANTATION_ROOT, nullptr);

	InsertLeafNodeData(ui->treeWidget);

	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &AgriMachineManagerTree::onItemClicked);

    //ui->treeWidget->setHeaderHidden(false);

	ui->treeWidget->expandAll();

	// 统计叶子节点
	updateTreeLeafCount(ui->treeWidget);
}

int AgriMachineManagerTree::BuildTree(QTreeWidget *treeWidget, int parentCode, QTreeWidgetItem *parentItem /*= nullptr*/)
{
	auto spSqlQuery = LY_AgriMachineManager::Get().GetpDbHandler()->getQueryOperator();
	QSqlQuery query = *spSqlQuery.data();

	query.prepare("SELECT code, parent_code, name FROM " + strTreeTableName + " WHERE parent_code = :parentCode");
	query.bindValue(":parentCode", parentCode);

	if (!query.exec()) {
		return 0;
	}

	int childCount = 0;

	while (query.next()) 
	{
		int id = query.value(0).toInt();
		QString name = query.value(2).toString();

		AgriDirMachineInfo dirInfo;
		dirInfo.nCode = query.value(0).toInt();
		dirInfo.nParentCode = query.value(1).toInt();
		dirInfo.sName = query.value(2).toString();

		QTreeWidgetItem *item = parentItem ? new QTreeWidgetItem(parentItem) : new QTreeWidgetItem(treeWidget);
		item->setText(0, name);
		item->setData(0, Qt::UserRole + 1, QVariant::fromValue(dirInfo)); // 保存结构体数据

		if (parentItem) {
			parentItem->addChild(item);
		}
		else {
			treeWidget->addTopLevelItem(item);
		}

		int subTreeChildCount = BuildTree(treeWidget, id, item);
		childCount += 1;// +subTreeChildCount;
	}

	// 计数
	//if (parentItem)
	//{
	//	parentItem->setText(0, parentItem->text(0) + " (" + QString::number(childCount) + ")");
	//}

	return childCount;
}

void AgriMachineManagerTree::InsertLeafNodeData(QTreeWidget *treeWidget) 
{
	auto spSqlQuery = LY_AgriMachineManager::Get().GetpDbHandler()->getQueryOperator();
	QSqlQuery query = *spSqlQuery.data();

	// 创建视图
	QString createViewSQL = QString(R"(
        CREATE OR REPLACE VIEW combined_view AS
        SELECT id, guid, domain, category, type, item, name, product_model, length, span, height, manufacturer, count, description, link FROM )")
		+ AGRI_MACHINE_TRACTORS_TABLE_NAME +
		QString(R"( UNION ALL
        SELECT id, guid, domain, category, type, item, name, product_model, length, span, height, manufacturer, count, description, link  FROM )")
		+ AGRI_MACHINE_SUBSOILER_TABLE_NAME + 
		QString(R"( UNION ALL
        SELECT id, guid, domain, category, type, item, name, product_model, length, span, height, manufacturer, count, description, link  FROM )")
		+ AGRI_MACHINE_GRADER_TABLE_NAME +
		QString(R"( UNION ALL
        SELECT id, guid, domain, category, type, item, name, product_model, length, span, height, manufacturer, count, description, link  FROM )")
		+ AGRI_MACHINE_SEEDER_TABLE_NAME +
		QString(R"( UNION ALL
        SELECT id, guid, domain, category, type, item, name, product_model, length, span, height, manufacturer, count, description, link  FROM )")
		+ AGRI_MACHINE_HARVESTER_TABLE_NAME;

	if (!query.exec(createViewSQL)) {
		//qDebug() << "Query failed: " << query.lastError();
		return;
	}

	// 查询
	QString querySQL = "SELECT * FROM combined_view";
	if (!query.exec(querySQL)) {
		qDebug() << "Failed to query view:" << query.lastError().text();
		return;
	}
	
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) 
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(i);
		InsertLeafDataRecursively(topItem, query);
	}
}

void AgriMachineManagerTree::InsertLeafDataRecursively(QTreeWidgetItem *item, QSqlQuery &query) 
{
	query.first();
	do {
		int leafNodeId = query.value("item").toInt();
		QString machineName = query.value("name").toString();

		AgriDirMachineInfo& dirInfo = item->data(0, Qt::UserRole + 1).value<AgriDirMachineInfo>();
		if (QString::number(leafNodeId).length() != QString::number(dirInfo.nCode).length())
		{
			break;
		}

		if (dirInfo.nCode == leafNodeId) 
		{
			QTreeWidgetItem *dataItem = new QTreeWidgetItem(item);
			dataItem->setText(0, machineName);

			AgriMachineBasicInfo machineInfo = LY_AgriMachineManager::Get().GetpDbHandler()->MapRowToStruct<AgriMachineBasicInfo>(query.record());
			dataItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(machineInfo)); // 保存机器设备数据

			item->addChild(dataItem);
		}
	} while (query.next());

	for (int i = 0; i < item->childCount(); ++i) 
	{
		InsertLeafDataRecursively(item->child(i), query);
	}
}

void AgriMachineManagerTree::FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter) 
{
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(i);
		FilterItemNodes(topItem, filter);
	}
}

bool AgriMachineManagerTree::FilterItemNodes(QTreeWidgetItem *item, const QString &filter) 
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

bool AgriMachineManagerTree::FilterExistItemNodes(QTreeWidgetItem *item, bool filter)
{
	bool hasExistChild = false;

	for (int i = 0; i < item->childCount(); ++i)
	{
		if (FilterExistItemNodes(item->child(i), filter))
		{
			hasExistChild = true;
		}
	}

	bool matches = false;
	if (filter && !item->childCount())
	{
		AgriMachineBasicInfo machineInfo = item->data(0, Qt::UserRole + 1).value<AgriMachineBasicInfo>();
		if (machineInfo.guid.length())
		{
			matches = true;
		}
	}

	if (!matches && !hasExistChild && filter)
	{
		item->setHidden(true);
	}
	else
	{
		item->setHidden(false);
		if (hasExistChild || matches)
		{
			item->setExpanded(true);
		}
	}

	return matches || hasExistChild;
}

void AgriMachineManagerTree::onItemClicked(QTreeWidgetItem *item, int column)
{
	AgriMachineBasicInfo machineInfo = item->data(0, Qt::UserRole + 1).value<AgriMachineBasicInfo>();
	if (machineInfo.guid.length()) // 叶子节点点击
	{
		emit LeafNodeClicked(machineInfo);
	}
}

void AgriMachineManagerTree::onCheckBoxToggled(bool checked)
{
	for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(i);
		FilterExistItemNodes(topItem, checked);
	}
}

// 递归统计叶子节点数量
int AgriMachineManagerTree::countLeafNodes(QTreeWidgetItem *item) {
	if (!item) return 0;

	if (item->childCount() == 0) {
		// 判断是否为农机
		AgriMachineBasicInfo machineInfo = item->data(0, Qt::UserRole + 1).value<AgriMachineBasicInfo>();
		if (machineInfo.guid.length())
		{
			return 1;
		}
		return 0;
	}

	int count = 0;
	for (int i = 0; i < item->childCount(); ++i) {
		count += countLeafNodes(item->child(i));
	}
	return count;
}

// 遍历并更新父节点 text
void AgriMachineManagerTree::updateParentTextWithLeafCount(QTreeWidgetItem *item) {
	if (!item) return;

	int leafCount = countLeafNodes(item);

	// 判断是否为农机
	AgriMachineBasicInfo machineInfo = item->data(0, Qt::UserRole + 1).value<AgriMachineBasicInfo>();
	if (machineInfo.guid.length()){
	}else{
		// 原本 text(0) + 叶子数量
		QString baseText = item->text(0);
		// 防止重复追加数字，这里去掉旧的 "(xxx)"
		baseText.remove(QRegularExpression("\\s*\\(\\d+\\)$"));
		item->setText(0, QString("%1 (%2)").arg(baseText).arg(leafCount));

		// 递归子节点
		for (int i = 0; i < item->childCount(); ++i) {
			updateParentTextWithLeafCount(item->child(i));
		}
	}
}

void AgriMachineManagerTree::updateTreeLeafCount(QTreeWidget *tree) {
	for (int i = 0; i < tree->topLevelItemCount(); ++i) {
		updateParentTextWithLeafCount(tree->topLevelItem(i));
	}
}
