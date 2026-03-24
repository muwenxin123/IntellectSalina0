#include <QTimer>

#include "QCString.h"
#include "AgriAssociateSensorDialog.h"
#include "ui_AgriAssociateSensorDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LY_AgriFarmManager.h"
#include "LyMessageBox.h"
#include "AgriFarmManagerUtil.h"
#include "AgriFieldWidget.h"

AgriAssociateSensorDialog::AgriAssociateSensorDialog(const AgriFieldInfoPtr pFieldInfo, QWidget *parent) :
	LyDialog(tr2("关联传感器(盐田内部)"), parent),
	m_pFieldInfo(pFieldInfo),
    ui(new Ui::AgriAssociateSensorDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	Init();
}

AgriAssociateSensorDialog::~AgriAssociateSensorDialog()
{
    delete ui;
}

void AgriAssociateSensorDialog::Init()
{
	CreateSensorTree();

	AddSensorTree();

	UpdateTreeCheckState();

	ui->treeWidget->expandAll();

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
	connect(ui->btnOK, &QPushButton::clicked, this, &AgriAssociateSensorDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriAssociateSensorDialog::on_btnCancel_clicked);
}

void AgriAssociateSensorDialog::CreateSensorTree()
{
	ui->treeWidget->clear();

	m_TreeHeader.clear();

	m_TreeHeader << tr2("分组") << tr2("传感器Id") << tr2("传感器名称") << tr2("传感器类型") << tr2("传感器型号");
	ui->treeWidget->setHeaderLabels(m_TreeHeader);
	ui->treeWidget->setColumnCount(m_TreeHeader.size());

	for (int i = 0; i < m_TreeHeader.size(); i++)
	{
		ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
}

void AgriAssociateSensorDialog::AddSensorTree()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem(ui->treeWidget);
	pRootItem->setText(0, tr2("传感器"));
	pRootItem->setTextAlignment(0, Qt::AlignCenter);
	pRootItem->setCheckState(0, Qt::Unchecked);

	// 查询关联传感器列表
	auto monitorVec = LY_AgriFarmManager::Get().GetpDbHandler()->GetMonitorInfoList();
	// 查询区域地块列表
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();

	for (const auto &monitor : monitorVec)
	{
		if (monitor && m_pFieldInfo)
		{
			// 校验传感器是否在田地内部
			QPointF point(monitor->longitude, monitor->latitude);
			for (const auto &itrZone : zoneBaseVec)
			{
				if (0 == m_pFieldInfo->area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
				{
					std::vector<QPointF> zonePoints;
					for (const auto& stZonePoint : itrZone.stZonePointVec)
					{
						QPointF tmpPointF(stZonePoint.dLon, stZonePoint.dLat);
						zonePoints.emplace_back(tmpPointF);
					}

					if (AgriFarmManagerUtil::JudgePointInZone_OddEven(point, zonePoints))
					{
						QTreeWidgetItem* CarChildItem = new QTreeWidgetItem(pRootItem);
						CarChildItem->setCheckState(0, Qt::Unchecked);

						CarChildItem->setText(1, QString::number(monitor->mointID));
						CarChildItem->setTextAlignment(1, Qt::AlignCenter);
						CarChildItem->setData(1, Qt::UserRole, QString::number(monitor->mointID));

						CarChildItem->setText(2, monitor->mointName);
						CarChildItem->setTextAlignment(2, Qt::AlignCenter);

						CarChildItem->setText(3, monitor->mointType);
						CarChildItem->setTextAlignment(3, Qt::AlignCenter);

						CarChildItem->setText(4, monitor->mointModenlName);
						CarChildItem->setTextAlignment(4, Qt::AlignCenter);
					}
					break;
				}
			}
		}
	}
}

void AgriAssociateSensorDialog::UpdateParentItem(QTreeWidgetItem* item)
{
	QTreeWidgetItem* parent = item->parent();
	if (parent == nullptr)
	{
		return;
	}

	int selectCount = 0;
	int nPartiallCount = 0;
	int childCount = parent->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem* childItem = parent->child(i);
		if (childItem->checkState(0) == Qt::Checked)
		{
			selectCount++;
		}
		else if (childItem->checkState(0) == Qt::PartiallyChecked)
		{
			nPartiallCount++;
		}
	}

	if (selectCount == childCount)
	{
		parent->setCheckState(0, Qt::Checked);
	}
	else  if (selectCount > 0 && selectCount < childCount || selectCount == 0 && nPartiallCount > 0)
	{
		parent->setCheckState(0, Qt::PartiallyChecked);
	}
	else if (selectCount == 0 && nPartiallCount == 0)
	{
		parent->setCheckState(0, Qt::Unchecked);
	}
}

void AgriAssociateSensorDialog::UpdateTreeCheckState()
{
	if (m_pFieldInfo)
	{
		// 获取关联表信息
		LY_AgriFarmManager::Get().GetpDbHandler()->GetFieldSensorRelation(m_pFieldInfo->id, m_vecFieldRelateSensorId);

		int nCount = ui->treeWidget->topLevelItemCount();
		for (int i = 0; i < nCount; i++)
		{
			QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
			int nItemCount = item->childCount();
			for (int j = 0; j < nItemCount; j++)
			{
				QTreeWidgetItem* itemChild = item->child(j);
				int mointId = itemChild->data(1, Qt::UserRole).toInt();
				if(std::find(m_vecFieldRelateSensorId.begin(), m_vecFieldRelateSensorId.end(), mointId) != m_vecFieldRelateSensorId.end())
				{
					itemChild->setCheckState(0, Qt::Checked);
					UpdateParentItem(itemChild);
				}
			}
		}
	}
}

void AgriAssociateSensorDialog::closeEvent(QCloseEvent* event)
{
}

void AgriAssociateSensorDialog::on_btnOK_clicked()
{
	std::vector<int> mointIdVec;

	int nCount = ui->treeWidget->topLevelItemCount();
	int nCheckNum = 0;
	for (int i = 0; i < nCount; i++)
	{
		QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
		int nItemCount = item->childCount();
		for (int j = 0; j < nItemCount; j++)
		{
			QTreeWidgetItem* itemChild = item->child(j);
			if (itemChild->checkState(0) == Qt::Checked)
			{
				int mointId  = itemChild->data(1, Qt::UserRole).toInt();
				mointIdVec.emplace_back(mointId);

				nCheckNum++;
			}
		}
	}

	// 筛选删除（原有项未被选中）
	std::vector<int> vecToDelete;
	for (const auto& sensorId : m_vecFieldRelateSensorId)
	{
		if (std::find(mointIdVec.begin(), mointIdVec.end(), sensorId) == mointIdVec.end())
		{
			vecToDelete.emplace_back(sensorId);
		}
	}

	// 筛选添加（去重）
	std::unordered_set<int> tmp(m_vecFieldRelateSensorId.begin(), m_vecFieldRelateSensorId.end());
	std::vector<int> vecToAdd;
	for (int num : mointIdVec) {
		if (tmp.find(num) == tmp.end()) {
			vecToAdd.push_back(num);
		}
	}

	if (!m_pFieldInfo)
	{
		QMessageBox::warning(this, tr2("错误"), tr2("无盐田信息！"));
	}
	else
	{
		bool isSuccess = LY_AgriFarmManager::Get().GetpDbHandler()->UpdateFieldSensorRelation(m_pFieldInfo->id, vecToDelete, vecToAdd);
		if (!isSuccess)
		{
			QMessageBox::warning(this, tr2("错误"), tr2("存储数据库失败！"));
		}
		else
		{
			emit relationFinish();
		}
	}
}

void AgriAssociateSensorDialog::on_btnCancel_clicked()
{
	this->close();
}

void AgriAssociateSensorDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr)
		return;

	int nCount = item->childCount();

	ui->treeWidget->disconnect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));

	bool bCheck = false;
	if (item->checkState(0) == Qt::Checked)
	{
		bCheck = true;
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{
				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Checked);
			}
			UpdateParentItem(item);
		}
		else
		{
			UpdateParentItem(item);
		}
	}
	else if (Qt::Unchecked == item->checkState(0))
	{
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{
				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Unchecked);
			}
			UpdateParentItem(item);
		}
		else
		{
			UpdateParentItem(item);
		}
	}

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}