#include <QGraphicsEffect>

#include "AgriMachineManagerDialog.h"
#include "ui_AgriMachineManagerDialog.h"
#include "AgriMachineManagerTree.h"
#include "LY_AgriMachineManager.h"

AgriMachineManagerDialog::AgriMachineManagerDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriMachineManagerDialog),
    m_show(true),
	m_isUpdatingComboBox(false)
{
    ui->setupUi(this);
	InitData();

	// 树叶子节点点击
	connect(ui->tree_widget, &AgriMachineManagerTree::LeafNodeClicked, [this](AgriMachineBasicInfo& machineInfo) {
		
		// 更新基础信息
		this->updateDisplayInfo(machineInfo);

		// 更新详细信息
		ui->machine_details->updateDisplayInfo(machineInfo);

		// 插入图片
		Insertimage(machineInfo.guid);
	});

	// 下拉菜单点击
	connect(ui->domain_category, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_second_currentTextChanged(int)));
	connect(ui->machine_class, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_third_currentTextChanged(int)));
	connect(ui->machine_subclass, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_fourth_currentTextChanged(int)));
}

AgriMachineManagerDialog::~AgriMachineManagerDialog()
{
    delete ui;
}

void AgriMachineManagerDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriMachineManagerDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriMachineManagerDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}


void AgriMachineManagerDialog::InitData()
{
	auto machineDirInfo = LY_AgriMachineManager::Get().GetpDbHandler()->GetDirMachineInfoList();
	for (auto &dirInfo : machineDirInfo)
	{
		int pCode = dirInfo->nParentCode;
		auto pItor = m_vAgriDirMachineInfoMap.find(pCode);
		if (pItor != m_vAgriDirMachineInfoMap.end())
		{
			pItor->second.push_back(dirInfo);
		}
		else
		{
			AgriDirMachineInfoPtrVec tmpMachineInfoPtrVec;
			tmpMachineInfoPtrVec.clear();
			tmpMachineInfoPtrVec.push_back(dirInfo);
			m_vAgriDirMachineInfoMap[pCode] = tmpMachineInfoPtrVec;
		}
	}

	// 更新机器列表
	auto machineList = LY_AgriMachineManager::Get().GetMachineManager()->LoadMachineList();
	ui->machine_info_table->UpdateTableData(machineList);
}


void AgriMachineManagerDialog::Insertimage(const std::string& strGuid)
{
	auto machineDirInfo = LY_AgriMachineManager::Get().GetpDbHandler()->GetAgriMachineImage(strGuid);
	QPixmap pixmap;
	if (pixmap.loadFromData(machineDirInfo)) {
		//pixmap.scaled(QSize(24, 24));
		ui->machine_image->setPixmap(pixmap);
		ui->machine_image->setScaledContents(true);
		//ui->machine_image->resize(24, 24);
		ui->machine_image->setAlignment(Qt::AlignCenter);

		ui->machine_image->setStyleSheet(
			"QLabel {"
			"border: 1px solid qlineargradient("
			"    spread:pad, x1:0, y1:0, x2:1, y2:1,"
			"    stop:0 #ff6b6b, stop:1 #f7d794"
			");"
			"border-radius: 5px;"
			"padding: 1px;"
			"background-color: white;"
			"}"
		);

		auto shadow = new QGraphicsDropShadowEffect(ui->machine_image);
		shadow->setBlurRadius(15);
		shadow->setColor(QColor(0, 0, 0, 80));
		shadow->setOffset(3, 3);
		ui->machine_image->setGraphicsEffect(shadow);

	}
}

void AgriMachineManagerDialog::updateDisplayInfo(const AgriMachineBasicInfo& machineInfo)
{
	ui->machine_id->setText(QString::fromStdString(machineInfo.guid));
	ui->machine_name->setText(QString::fromStdString(machineInfo.name));
	ui->machine_model->setText(QString::fromStdString(machineInfo.product_model));

	updateDisplayCombox(machineInfo);
}

void AgriMachineManagerDialog::updateDisplayCombox(const AgriMachineBasicInfo& machineInfo)
{
	m_isUpdatingComboBox = true;

	ui->domain_category->clear();
	ui->machine_class->clear();
	ui->machine_subclass->clear();
	ui->machine_item->clear();

	// nDomain
	auto pItor = m_vAgriDirMachineInfoMap.find(AGRI_DIR_MACHINE_ROOT);
	if (pItor != m_vAgriDirMachineInfoMap.end())
	{
		int curIndex = 0, domainIndex = 0, classIndex = 0, subClassIndex = 0, itemIndex = 0;

		for (auto &machineDirInfo : pItor->second)
		{
			ui->domain_category->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
			if (machineInfo.domain == machineDirInfo->nCode)
			{
				domainIndex = curIndex;
			}
			++curIndex;
		}
		 ui->domain_category->setCurrentIndex(domainIndex);

		// nCategory
		pItor = m_vAgriDirMachineInfoMap.find(machineInfo.domain);
		if (pItor != m_vAgriDirMachineInfoMap.end())
		{
			curIndex = 0;
			for (auto &machineDirInfo : pItor->second)
			{
				ui->machine_class->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
				if (machineInfo.category == machineDirInfo->nCode)
				{
					classIndex = curIndex;
				}
				++curIndex;
			}
			ui->machine_class->setCurrentIndex(classIndex);

			// nType
			pItor = m_vAgriDirMachineInfoMap.find(machineInfo.category);
			if (pItor != m_vAgriDirMachineInfoMap.end())
			{
				curIndex = 0;
				for (auto &machineDirInfo : pItor->second)
				{
					ui->machine_subclass->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
					if (machineInfo.type == machineDirInfo->nCode)
					{
						subClassIndex = curIndex;
					}
					++curIndex;
				}
				ui->machine_subclass->setCurrentIndex(subClassIndex);

				// nItem
				pItor = m_vAgriDirMachineInfoMap.find(machineInfo.type);
				if (pItor != m_vAgriDirMachineInfoMap.end())
				{
					curIndex = 0;
					for (auto &machineDirInfo : pItor->second)
					{
						ui->machine_item->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
						if (machineInfo.item == machineDirInfo->nCode)
						{
							itemIndex = curIndex;
						}
						++curIndex;
					}
					ui->machine_item->setCurrentIndex(itemIndex);
				}
			}
		}
	}

	m_isUpdatingComboBox = false;
}

void AgriMachineManagerDialog::on_comboBox_second_currentTextChanged(int index)
{
	if (m_isUpdatingComboBox) {
		return; 
	}

	auto itemData = ui->domain_category->itemData(index).value<AgriDirMachineInfo>();

	ui->machine_class->clear();
	ui->machine_subclass->clear();
	ui->machine_item->clear();

	auto pItor = m_vAgriDirMachineInfoMap.find(itemData.nCode);
	if (pItor != m_vAgriDirMachineInfoMap.end())
	{
		for (auto &machineDirInfo : pItor->second)
		{
			ui->machine_class->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
		}
	}
}

void AgriMachineManagerDialog::on_comboBox_third_currentTextChanged(int index) 
{
	if (m_isUpdatingComboBox) {
		return;
	}

	auto itemData = ui->machine_class->itemData(index).value<AgriDirMachineInfo>();

	ui->machine_subclass->clear();
	ui->machine_item->clear();

	auto pItor = m_vAgriDirMachineInfoMap.find(itemData.nCode);
	if (pItor != m_vAgriDirMachineInfoMap.end())
	{
		for (auto &machineDirInfo : pItor->second)
		{
			ui->machine_subclass->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
		}
	}
}

void AgriMachineManagerDialog::on_comboBox_fourth_currentTextChanged(int index)
{
	if (m_isUpdatingComboBox) {
		return;
	}

	auto itemData = ui->machine_subclass->itemData(index).value<AgriDirMachineInfo>();

	ui->machine_item->clear();

	auto pItor = m_vAgriDirMachineInfoMap.find(itemData.nCode);
	if (pItor != m_vAgriDirMachineInfoMap.end())
	{
		for (auto &machineDirInfo : pItor->second)
		{
			ui->machine_item->addItem(machineDirInfo->sName, QVariant::fromValue(*machineDirInfo));
		}
	}
}
