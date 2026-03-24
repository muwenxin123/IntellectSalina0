#include <QDebug>
#include <QSqlError>
#include <QMenu>
#include <QSettings>

#include "AgriFarmManagerTree.h"
#include "ui_AgriFarmManagerTree.h"
#include "LY_AgriFarmManager.h"
#include "AgriAddFarmDialog.h"
#include "AgriManageFarmDialog.h"
#include "AgriAddFieldDialog.h"
#include "AgriAssociateSensorDialog.h"
#include "LyMessageBox.h"
#include "IAgriFarmManagerActivator.h"
#include "LY_AgriFarmManagerInterface.h"


AgriFarmManagerTree::AgriFarmManagerTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriFarmManagerTree)
{
    ui->setupUi(this);

	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	m_sCurActiveFarm = ini.value("FarmID", "").toString();

	// 设置提示信息
	ui->search_lineEdit->setPlaceholderText(tr2("请输入搜索信息..."));
	connect(ui->search_lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
		this->FilterTreeNodes(ui->treeWidget, text);
	});

	Init();
}

AgriFarmManagerTree::~AgriFarmManagerTree()
{
    delete ui;
}

void AgriFarmManagerTree::Init()
{
	InitTree();

	CreateTree();

	// 设置菜单文本的策略为自定义
	ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	// 为QTreeWidget右键事件
	connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &AgriFarmManagerTree::onRightClicked);
}

void AgriFarmManagerTree::InitTree()
{
	ui->treeWidget->clear();
	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabels(QStringList() << tr2("当前农场"));
}

void AgriFarmManagerTree::DefaultTreeSelect()
{
	// 默认选中第一块田地
	if (m_ActiveFieldList.size())
	{
		QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(0);
		if (topItem && topItem->childCount() > 0)
		{
			QTreeWidgetItem *childItem = topItem->child(0);

			// 设置选中节点
			ui->treeWidget->setCurrentItem(childItem);

			// 手动触发 itemClicked 信号
			emit ui->treeWidget->itemClicked(childItem, 0);
		}
	}
}

void AgriFarmManagerTree::CreateTree()
{
	BuildTree(ui->treeWidget);

	InsertLeafNodeData(ui->treeWidget);

	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &AgriFarmManagerTree::onItemClicked);

	ui->treeWidget->expandAll();
}

AgriAddFarmDialog* AgriFarmManagerTree::getAddFarmDialog()
{
	return m_AddFarmDialog;
}

bool AgriFarmManagerTree::GetActiveFarm(QString &FarmId)
{
	FarmId = m_sCurActiveFarm;

	return !m_sCurActiveFarm.isEmpty();
}

AgriFarmInfoPtr AgriFarmManagerTree::GetActiveFarmInfo()
{
	return m_ActiveFarmInfo;
}

AgriFieldInfoPtrVec AgriFarmManagerTree::GetActiveFieldList()
{
	return m_ActiveFieldList;
}

void AgriFarmManagerTree::ZoneHighLight(QPainter & painter, IProj * pProj)
{
	m_zoneDrawMap.iterate_query([&](const std::string& id, const qnzkna::ZoneManage::CZoneBase& zone) {

		if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
			zone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {

			QBrush brush;
			brush.setColor(Qt::transparent);
			brush.setStyle(Qt::BrushStyle::SolidPattern);
			painter.setBrush(brush);

			QPen pen(Qt::white);
			pen.setWidth(3);
			painter.setPen(pen);

			QVector<QPoint> points;
			points.clear();
			for (auto & zonePoint : zone.stZonePointVec)
			{
				QPoint point = pProj->Jwd2Screen(zonePoint.dLon, zonePoint.dLat);
				points.push_back(point);
			}
			QPolygon poly(points);
			painter.drawPolygon(poly);
		}
	});
}

void AgriFarmManagerTree::BuildTree(QTreeWidget *treeWidget)
{
	auto farmList = LY_AgriFarmManager::Get().GetpDbHandler()->GetFarmInfoList();

	for(auto& farmInfo : farmList)
	{
		// 匹配激活农场
		if (m_sCurActiveFarm == farmInfo.id)
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
			item->setText(0, farmInfo.name);
			item->setData(0, Qt::UserRole + 1, QVariant::fromValue(farmInfo));

			treeWidget->addTopLevelItem(item);

			// 保存激活农场信息
			m_ActiveFarmInfo = std::make_shared<AgriFarmInfo>(farmInfo);

			break;
		}
	}
}

void AgriFarmManagerTree::InsertLeafNodeData(QTreeWidget *treeWidget) 
{
	m_ActiveFieldList.clear();

	auto fieldList = LY_AgriFarmManager::Get().GetpDbHandler()->GetFieldInfoList();

	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) 
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(i);
		AgriFarmInfo farmInfo = topItem->data(0, Qt::UserRole + 1).value<AgriFarmInfo>();

		for (const auto & fieldInfo : fieldList)
		{
			if (farmInfo.id == fieldInfo.farm_id)
			{
				QTreeWidgetItem *dataItem = new QTreeWidgetItem(topItem);
				dataItem->setText(0, fieldInfo.name);
				dataItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(fieldInfo)); // 保存结构体数据

				topItem->addChild(dataItem);

				// 保存农场下地块列表
				m_ActiveFieldList.emplace_back(std::make_shared<AgriFieldInfo>(fieldInfo));
			}
		}
	}
}

void AgriFarmManagerTree::FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter) 
{
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *topItem = treeWidget->topLevelItem(i);
		FilterItemNodes(topItem, filter);
	}
}

bool AgriFarmManagerTree::FilterItemNodes(QTreeWidgetItem *item, const QString &filter) 
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

void AgriFarmManagerTree::DeleteTreeNode(QTreeWidgetItem *item)
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

void AgriFarmManagerTree::AddFarm()
{
	if (m_AddFarmDialog)
	{
		delete m_AddFarmDialog;
	}

	m_AddFarmDialog = new AgriAddFarmDialog(this);
	m_AddFarmDialog->show();

	connect(m_AddFarmDialog, &AgriAddFarmDialog::addFinish, [&]() {

		/*
		auto farmInfo = m_AddFarmDialog->GetDialogInfo();

		QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
		item->setText(0, farmInfo->name);
		item->setData(0, Qt::UserRole + 1, QVariant::fromValue(*farmInfo)); // 保存结构体数据

		ui->treeWidget->addTopLevelItem(item);
		*/

		m_AddFarmDialog->close();
	});
}

void AgriFarmManagerTree::ManageFarm()
{
	if (m_ManageFarmDialog)
	{
		delete m_ManageFarmDialog;
	}

	auto farmList = LY_AgriFarmManager::Get().GetpDbHandler()->GetFarmInfoList();

	m_ManageFarmDialog = new AgriManageFarmDialog(farmList, this);
	m_ManageFarmDialog->show();

	connect(m_ManageFarmDialog, &AgriManageFarmDialog::addFarm, [&]() {

		AddFarm();
	});

	connect(m_ManageFarmDialog, &AgriManageFarmDialog::switchFarm, [&](QString farmId) {

		m_sCurActiveFarm = farmId;

		// 刷新展示
		Init();

		// 清理脏数据
		emit SwitchFarm(m_ActiveFarmInfo);

		// 关闭dialog
		m_ManageFarmDialog->close();
	});
}

void AgriFarmManagerTree::AddField(QTreeWidgetItem *parentItem, AgriFarmInfo farmInfo)
{
	if (m_AddFieldDialog)
	{
		delete m_AddFieldDialog;
	}

	m_AddFieldDialog = new AgriAddFieldDialog(farmInfo.id,this);
	m_AddFieldDialog->show();

	connect(m_AddFieldDialog, &AgriAddFieldDialog::addFinish, [&, parentItem]() {
		auto fieldInfoVec = m_AddFieldDialog->GetDialogFieldInfoVec();
		for (const auto& fieldInfo : *fieldInfoVec)
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(0, fieldInfo.name);
			childItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(fieldInfo)); // 保存结构体数据

			parentItem->addChild(childItem);
		}

		ui->treeWidget->expandAll();

		m_AddFieldDialog->close();
	});
}

void AgriFarmManagerTree::DeleteFarm(QTreeWidgetItem *item, AgriFarmInfo farmInfo)
{
	if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除农场[%1]？").arg(farmInfo.name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		if (LY_AgriFarmManager::Get().GetpDbHandler()->DeleteFarmInfo(farmInfo.id))
		{
			DeleteTreeNode(item);
		}
	}
}

void AgriFarmManagerTree::DeleteField(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo)
{
	if (pFieldInfo && LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除盐田[%1]？").arg(pFieldInfo->name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		if (LY_AgriFarmManager::Get().GetpDbHandler()->DeleteFieldInfo(pFieldInfo->id))
		{
			DeleteTreeNode(item);
		}
	}
}

void AgriFarmManagerTree::UpdateField(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo)
{
	if (m_UpdateFieldDialog)
	{
		delete m_UpdateFieldDialog;
	}

	m_UpdateFieldDialog = new AgriAddFieldDialog(pFieldInfo, this);
	m_UpdateFieldDialog->show();

	connect(m_UpdateFieldDialog, &AgriAddFieldDialog::updateFinish, [&, item]() {

		auto fieldInfo = m_UpdateFieldDialog->GetDialogFieldInfo();
		if (fieldInfo)
		{
			// 更新树显示
			item->setText(0, fieldInfo->name);
			item->setData(0, Qt::UserRole + 1, QVariant::fromValue(*(fieldInfo.get()))); // 保存结构体数据

			// 刷新界面
			ui->treeWidget->update();

			// 更新田地信息显示
			onItemClicked(item, 0);
		}

		m_UpdateFieldDialog->close();
	});
}

void AgriFarmManagerTree::AssociateSensor(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo)
{
	if (m_AssociateSensorDialog)
	{
		delete m_AssociateSensorDialog;
	}

	m_AssociateSensorDialog = new AgriAssociateSensorDialog(pFieldInfo, this);
	m_AssociateSensorDialog->show();

	connect(m_AssociateSensorDialog, &AgriAssociateSensorDialog::relationFinish, [&, item]() {

		// 更新田地信息显示
		onItemClicked(item, 0);

		m_AssociateSensorDialog->close();
	});
}

void AgriFarmManagerTree::onItemClicked(QTreeWidgetItem *item, int column)
{
	emit LeafNodeClicked(item, column);

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();
	m_zoneDrawMap.clear();
	m_fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();

	qnzkna::ZoneManage::CZoneBase tmpCurZone;
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			tmpCurZone = itrZone;

			// 添加高亮区域
			m_zoneDrawMap.insert(m_fieldInfo.area_id.toStdString(), itrZone);
			break;
		}
	}

	// 区域居中展示
	QPointF centerPoint;
	std::vector<QPointF> zonePoints;

	if (tmpCurZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		tmpCurZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		if (tmpCurZone.stZonePointVec.size() > 0) {
			for (auto& point : tmpCurZone.stZonePointVec)
			{
				zonePoints.emplace_back(QPointF(point.dLon, point.dLat));
			}
		}
	}
}

void AgriFarmManagerTree::onRightClicked(const QPoint& point)
{
	QTreeWidget* curTree = qobject_cast<QTreeWidget*>(sender());

	QMenu* menu = new QMenu(curTree);

	QTreeWidgetItem* item = curTree->itemAt(point);
	if (item == nullptr)
	{
		// 初始菜单
		QAction* action1 = new QAction(tr2("管理盐田"));
		menu->addAction(action1);
		connect(action1, &QAction::triggered, this, &AgriFarmManagerTree::ManageFarm);
	}
	else if(item->parent() == nullptr)
	{
		AgriFarmInfo farmInfo = item->data(0, Qt::UserRole + 1).value<AgriFarmInfo>();
		if (farmInfo.id.length())
		{
			// 农场菜单
			QAction* action = new QAction(tr2("添加盐田"));
			menu->addAction(action);
			connect(action, &QAction::triggered, this, [this, item, farmInfo]() {
				AddField(item, farmInfo);
			});

			//QAction* action1 = new QAction(tr2("删除农场"));
			//menu->addAction(action1);
			//connect(action1, &QAction::triggered, this, [this, item, farmInfo]() {
			//	DeleteFarm(item, farmInfo);
			//});
		}
	}
	else if (item->parent())
	{
		auto &tmpInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
		AgriFieldInfoPtr pFieldInfo = std::make_shared<AgriFieldInfo>(tmpInfo);
		if (pFieldInfo->id.length())
		{
			// 盐田菜单
			QAction* action = new QAction(tr2("修改盐田"));
			menu->addAction(action);
			connect(action, &QAction::triggered, this, [this, item, pFieldInfo]() {
				UpdateField(item, pFieldInfo);
			});
			
			QAction* action1 = new QAction(tr2("删除盐田"));
			menu->addAction(action1);
			connect(action1, &QAction::triggered, this, [this, item, pFieldInfo]() {
				DeleteField(item, pFieldInfo);
			});

			QAction* action2 = new QAction(tr2("关联传感器"));
			menu->addAction(action2);
			connect(action2, &QAction::triggered, this, [this, item, pFieldInfo]() {
				AssociateSensor(item, pFieldInfo);
			});
		}
	}

	menu->exec(QCursor::pos());
}

void AgriFarmManagerTree::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// 地块居中展示
	AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			double curLon = itrZone.stRoundParameter.dLon;
			double curLat = itrZone.stRoundParameter.dLat;

			QPointF centerPoint;
			std::vector<QPointF> zonePoints;
			if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
				itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
				for (auto& point : itrZone.stZonePointVec)
				{
					zonePoints.emplace_back(QPointF(point.dLon, point.dLat));
				}
			}

			if (AgriFarmManagerUtil::CalcPolygonCenterPoint(zonePoints, centerPoint))
			{
				double dLon = centerPoint.x();
				double dLat = centerPoint.y();
				//SetScreenCenter(dLon, dLat);
			}
			break;
		}
	}
}

void AgriFarmManagerTree::SetScreenCenter(double &dLon, double &dLat)
{
	QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
	LY_AgriFarmManagerInterface::Get().Script(str.toLocal8Bit());
}
