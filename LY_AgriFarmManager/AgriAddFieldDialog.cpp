#include <QTimer>

#include "QCString.h"
#include "AgriAddFieldDialog.h"
#include "ui_AgriAddFieldDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LY_AgriFarmManager.h"
#include "LyMessageBox.h"
#include "AgriFarmManagerUtil.h"
#include "IAgriFarmManagerActivator.h"

AgriAddFieldDialog::AgriAddFieldDialog(const QString& farmId, QWidget *parent) :
	LyDialog(tr2("添加盐田"), parent),
    ui(new Ui::AgriAddFieldDialog)
{
	m_sFarmId = farmId;

	m_eCerateType = CREATE_TYPE_ADD;

	Init();
}


AgriAddFieldDialog::AgriAddFieldDialog(const AgriFieldInfoPtr pFieldInfo, QWidget *parent /*= nullptr*/):
	LyDialog(tr2("修改盐田"), parent),
	ui(new Ui::AgriAddFieldDialog)
{
	m_pFieldInfo = pFieldInfo;

	m_eCerateType = CREATE_TYPE_UPDATE;

	Init();

	InitFieldBasicInfo();

	UpdateZoneTree();
}

AgriAddFieldDialog::~AgriAddFieldDialog()
{
	if (m_vFieldInfo)
	{
		delete m_vFieldInfo;
		m_vFieldInfo = nullptr;
	}

    delete ui;
}

AgriFieldInfoVec* AgriAddFieldDialog::GetDialogFieldInfoVec()
{
	return m_vFieldInfo;
}


AgriFieldInfoPtr AgriAddFieldDialog::GetDialogFieldInfo()
{
	return m_pFieldInfo;
}

void AgriAddFieldDialog::closeEvent(QCloseEvent* event)
{

}

void AgriAddFieldDialog::Init()
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	InitDisplayCombox();

	CreateZoneTree();
	AddZoneTree();

	ui->treeWidget->expandAll(); 

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriAddFieldDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriAddFieldDialog::on_btnCancel_clicked);

	// 下拉菜单点击
	connect(ui->comboBox_crop, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_crop_currentTextChanged(int)));
	connect(ui->comboBox_crop_variety, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_crop_variety_currentTextChanged(int)));
}

void AgriAddFieldDialog::InitFieldBasicInfo()
{
	if (m_pFieldInfo)
	{
		ui->field_name->setText(m_pFieldInfo->name);
		ui->field_category->setText(m_pFieldInfo->category);
		ui->field_desc->setText(m_pFieldInfo->description);
		//ui->comboBox_crop_variety->setText(m_pFieldInfo->crop_variety);TODO:
	}
}

void AgriAddFieldDialog::InitDisplayCombox()
{
	if (m_vCropInfo.empty())
	{
		m_vCropInfo = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropInfoList();
	}

	if (m_vCropVarietyInfo.empty())
	{
		m_vCropVarietyInfo = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropVarietyInfoList();
	}

	if (m_vEnumPlantStandard.empty())
	{
		m_vEnumPlantStandard = LY_AgriFarmManager::Get().GetpDbHandler()->FetchCropPlantStandardEnumInfoList();
	}
	
	if (m_vEnumSowModel.empty())
	{
		m_vEnumSowModel = LY_AgriFarmManager::Get().GetpDbHandler()->FetchCropSowModelEnumInfoList();
	}

	ui->comboBox_crop->clear();
	ui->comboBox_crop_variety->clear();

	int curIndex = 0, cropIndex = 0, crop_variety_Index = 0, plantStandardIndex = 0, sowModelIndex = 0;

	for (auto &cropInfo : m_vCropInfo)
	{
		ui->comboBox_crop->addItem(cropInfo->name, QVariant::fromValue(*(cropInfo.get())));
		if (m_pFieldInfo && m_pFieldInfo->crop == cropInfo->id)
		{
			cropIndex = curIndex;
		}
		curIndex++;
	}

	curIndex = 0;
	for (auto &cropVarietyInfo : m_vCropVarietyInfo)
	{
		if (m_pFieldInfo && m_pFieldInfo->crop == cropVarietyInfo->crop_id)
		{
			ui->comboBox_crop_variety->addItem(cropVarietyInfo->name, QVariant::fromValue(*(cropVarietyInfo.get())));
			if (m_pFieldInfo && m_pFieldInfo->crop_variety == cropVarietyInfo->id)
			{
				crop_variety_Index = curIndex;
			}
			curIndex++;
		}
		else if (!m_vCropInfo.empty() && m_vCropInfo[cropIndex]->id  == cropVarietyInfo->crop_id)
		{
			ui->comboBox_crop_variety->addItem(cropVarietyInfo->name, QVariant::fromValue(*(cropVarietyInfo.get())));
		}
	}
}

void AgriAddFieldDialog::CreateZoneTree()
{
	ui->treeWidget->clear();

	m_TreeHeader.clear();

	m_TreeHeader << tr2("区域类型") << tr2("区域名称") << tr2("区域形状") << tr2("占地面积");
	ui->treeWidget->setHeaderLabels(m_TreeHeader);
	ui->treeWidget->setColumnCount(m_TreeHeader.size());

	for (int i = 0; i < m_TreeHeader.size(); i++)
	{
		ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
}

void AgriAddFieldDialog::AddZoneTree()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem(ui->treeWidget);
	pRootItem->setText(0, tr2("盐田"));
	pRootItem->setTextAlignment(0, Qt::AlignCenter);
	pRootItem->setCheckState(0, Qt::Unchecked);

	// 查询区域列表 （nProperty == 0 其他）
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();
	for (const auto &itrZone : zoneBaseVec)
	{
		QTreeWidgetItem* CarChildItem = new QTreeWidgetItem(pRootItem);
		CarChildItem->setCheckState(0, Qt::Unchecked);

		CarChildItem->setText(1, QString::fromLocal8Bit(itrZone.strID));
		CarChildItem->setTextAlignment(1, Qt::AlignCenter);
		CarChildItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(itrZone.strID));

		CarChildItem->setText(2, QString::fromLocal8Bit(qnzkna::ZoneManage::ZoneTypeToString(itrZone.nZoneType)));
		CarChildItem->setTextAlignment(2, Qt::AlignCenter);

		CarChildItem->setText(3, tr2("--"));
		CarChildItem->setTextAlignment(3, Qt::AlignCenter);

	}
}


void AgriAddFieldDialog::UpdateParentItem(QTreeWidgetItem* item)
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

void AgriAddFieldDialog::UpdateZoneTree()
{
	if (m_pFieldInfo)
	{
		int nCount = ui->treeWidget->topLevelItemCount();
		for (int i = 0; i < nCount; i++)
		{
			bool isFinded = false;
			QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
			int nItemCount = item->childCount();
			for (int j = 0; j < nItemCount; j++)
			{
				QTreeWidgetItem* itemChild = item->child(j);
				QString pAreaId = itemChild->data(1, Qt::UserRole).toString();
				if (pAreaId == m_pFieldInfo->area_id)
				{
					itemChild->setCheckState(0, Qt::Checked);
					UpdateParentItem(itemChild);
					isFinded = true;
					break;
				}
			}
			if (isFinded) break;
		}
	}
}

void AgriAddFieldDialog::on_btnOK_clicked()
{
	m_vFieldInfo = new AgriFieldInfoVec;

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
				// 选中区域id
				AgriFieldInfo pFieldInfo;
				pFieldInfo.id = (CREATE_TYPE_ADD == m_eCerateType) ? AgriFarmManagerUtil::GenerateUuid() : m_pFieldInfo->id;
				pFieldInfo.farm_id = (CREATE_TYPE_ADD == m_eCerateType) ? m_sFarmId : m_pFieldInfo->farm_id;
				QString pAreaId = itemChild->data(1, Qt::UserRole).toString();
				pFieldInfo.area_id = pAreaId;
				pFieldInfo.name = (CREATE_TYPE_ADD == m_eCerateType) ? pAreaId : ui->field_name->text();
				pFieldInfo.category = ui->field_category->text();
				pFieldInfo.description = ui->field_desc->toPlainText();

				auto cropData = ui->comboBox_crop->itemData(ui->comboBox_crop->currentIndex()).value<AgriCropInfo>();
				pFieldInfo.crop = cropData.id;
				auto cropVarietyData = ui->comboBox_crop_variety->itemData(ui->comboBox_crop_variety->currentIndex()).value<AgriCropVarietyInfo>();
				pFieldInfo.crop_variety = cropVarietyData.id;

				m_vFieldInfo->emplace_back(pFieldInfo);

				nCheckNum++;
			}
		}
	}
	
	switch (m_eCerateType)
	{
	case AgriAddFieldDialog::CREATE_TYPE_UNKNOW:
		break;
	case AgriAddFieldDialog::CREATE_TYPE_ADD:
	{
		bool isSuccess = LY_AgriFarmManager::Get().GetpDbHandler()->BatchInsertField(*m_vFieldInfo);
		if (!isSuccess)
		{
			QMessageBox::warning(this, "错误", "存储数据库失败！");
		}
		else
		{
			emit addFinish();
		}
	}
		break;
	case AgriAddFieldDialog::CREATE_TYPE_UPDATE:
	{
		if (nCheckNum != 1)
		{
			LyMessageBox::warning(nullptr, tr2("错误"), tr2("仅可关联单一区域!"), QMessageBox::Ok);
		}
		else
		{
			// 更新数据
			m_pFieldInfo = std::make_shared<AgriFieldInfo>((*m_vFieldInfo)[0]);
			bool isSuccess = LY_AgriFarmManager::Get().GetpDbHandler()->UpdateFieldInfo((*m_vFieldInfo)[0]);
			if (!isSuccess)
			{
				QMessageBox::warning(this, "错误", "更新数据库失败！");
			}
			else
			{
				emit updateFinish();
			}
		}
	}
		break;
	default:
		break;
	}

}

void AgriAddFieldDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}

void AgriAddFieldDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
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

void AgriAddFieldDialog::on_comboBox_crop_currentTextChanged(int index)
{
	// 修改作物品种列表

	ui->comboBox_crop_variety->clear();

	for (auto &cropVarietyInfo : m_vCropVarietyInfo)
	{
		if (!m_vCropInfo.empty() && m_vCropInfo[index]->id == cropVarietyInfo->crop_id)
		{
			ui->comboBox_crop_variety->addItem(cropVarietyInfo->name, QVariant::fromValue(*(cropVarietyInfo.get())));
		}
	}
}

void AgriAddFieldDialog::on_comboBox_crop_variety_currentTextChanged(int index)
{

}
