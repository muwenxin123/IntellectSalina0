#include "AgriFieldWidget.h"
#include "ui_AgriFieldWidget.h"
#include "LY_AgriFarmManager.h"


AgriFieldWidget::AgriFieldWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriFieldWidget)
{
	ui->setupUi(this);
	InitData();
}

AgriFieldWidget::~AgriFieldWidget()
{
	delete ui;
}

void AgriFieldWidget::setCurrentField(const AgriFieldInfo& fieldInfo)
{
	m_fieldInfo = fieldInfo;

	ui->field_name->setText(fieldInfo.name);
	ui->field_category->setText(fieldInfo.category);

	if (LY_AgriFarmManager::Get().GetLegendPopup() != nullptr)
	{
		LY_AgriFarmManager::Get().GetLegendPopup()->updateFieldInfo(m_fieldInfo);
	}
	

	auto cropInfoList = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropInfoList();
	auto cropVarietyInfoList = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropVarietyInfoList();
	auto plantStandardEnumList = LY_AgriFarmManager::Get().GetpDbHandler()->FetchCropPlantStandardEnumInfoList();
	auto sowModelEnumList = LY_AgriFarmManager::Get().GetpDbHandler()->FetchCropSowModelEnumInfoList();

	ui->crop->setText([&]()->QString {
		AgriCropInfoPtrVec::iterator it = std::find_if(cropInfoList.begin(), cropInfoList.end(), [&](const AgriCropInfoPtr& pCrop) { return pCrop->id == fieldInfo.crop; });
		if (it != cropInfoList.end()) {
			return (*it)->name;
		}
		return "";
	}());

	ui->crop_variety->setText([&]()->QString {
		AgriCropVarietyInfoPtrVec::iterator it = std::find_if(cropVarietyInfoList.begin(), cropVarietyInfoList.end(), [&](const AgriCropVarietyInfoPtr& pCropVariety) { return pCropVariety->id == fieldInfo.crop_variety; });
		if (it != cropVarietyInfoList.end()) {
			return (*it)->name;
		}
		return "";
	}());

	ui->field_desc->setText(fieldInfo.description);

	// 区域坐标展示 
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			m_zoneBase = itrZone;
			UpdatePointListData(itrZone);
			break;
		}
	}
}

void AgriFieldWidget::getActiveZoneData(qnzkna::ZoneManage::CZoneBase& zoneBase)
{
	zoneBase = m_zoneBase;
}

void AgriFieldWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
}

void AgriFieldWidget::showEvent(QShowEvent *event)
{
	m_show = true;
}

void AgriFieldWidget::hideEvent(QHideEvent *event)
{
	m_show = false;
}

void AgriFieldWidget::InitData()
{
	CreatePointListHeader();
}

void AgriFieldWidget::CreatePointListHeader()
{
	ui->tableWidget->clear();
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("经度") << tr2("纬度");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setRowCount(0);

	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 0)
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
	}

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AgriFieldWidget::UpdatePointListData(const qnzkna::ZoneManage::CZoneBase& zoneBase)
{
	ui->tableWidget->clearContents();

	ui->tableWidget->setRowCount(zoneBase.stZonePointVec.size());

	QString strNo, strLon, strLat;
	int nNo = 0;
	for (auto itr : zoneBase.stZonePointVec)
	{
		strNo = QString::number(nNo).sprintf("%02d", nNo + 1);
		strLon = QString::number(itr.dLon, 'f', 7);
		strLat = QString::number(itr.dLat, 'f', 7);

		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(strLon);
		QTableWidgetItem* item2 = new QTableWidgetItem(strLat);

		ui->tableWidget->setItem(nNo, 0, item0);
		ui->tableWidget->setItem(nNo, 1, item1);
		ui->tableWidget->setItem(nNo, 2, item2);

		ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		nNo++;
	}
}

void AgriFieldWidget::cleanUp()
{
	setCurrentField(AgriFieldInfo());

	ui->tableWidget->clear();

	CreatePointListHeader();
}
