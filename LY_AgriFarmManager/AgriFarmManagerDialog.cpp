#include "AgriFarmManagerDialog.h"
#include "ui_AgriFarmManagerDialog.h"
#include "AgriFarmManagerTree.h"
#include "LY_AgriFarmManager.h"
#include "AgriAddFarmDialog.h"
#include "AgriFarmManagerUtil.h"

AgriFarmManagerDialog::AgriFarmManagerDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriFarmManagerDialog),
    m_show(true),
	m_isUpdatingComboBox(false)
{
    ui->setupUi(this);

	// 切换农场
	connect(ui->tree_widget, &AgriFarmManagerTree::SwitchFarm, [this](AgriFarmInfoPtr farmInfo) {

		// 清理脏数据
		ui->field_basic->cleanUp();
		ui->field_addition->cleanUp();

		// 更新农场展示
		updateFarmBasicInfo();
	});

	// 树叶子节点点击
	connect(ui->tree_widget, &AgriFarmManagerTree::LeafNodeClicked, [this](QTreeWidgetItem *item, int column) {

		if (item->parent() == nullptr)
		{
			AgriFarmInfo farmInfo = item->data(0, Qt::UserRole + 1).value<AgriFarmInfo>();
			// 更新农场信息
			updateFarmBasicInfo();
		}
		else if (item->parent())
		{
			AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
			if (fieldInfo.id.length())
			{
				AgriFieldInfoPtr pFieldInfo = std::make_shared<AgriFieldInfo>(fieldInfo);
				// 更新盐田信息
				ui->field_basic->setCurrentField(fieldInfo);

				// 插入图片 TODO：

				// 更新盐田附属信息
				ui->field_addition->setCurrentField(pFieldInfo);
			}
		}
	});

	// 获取当前激活农场
	QString activeFarm;
	if (ui->tree_widget->GetActiveFarm(activeFarm))
	{
		updateFarmBasicInfo();
	}

	// 初始化选中
	ui->tree_widget->DefaultTreeSelect();

	setFocusPolicy(Qt::StrongFocus);
}

AgriFarmManagerDialog::~AgriFarmManagerDialog()
{
    delete ui;
}

AgriAddFarmDialog* AgriFarmManagerDialog::getAddFarmDialog()
{
	return ui->tree_widget->getAddFarmDialog();
}

void AgriFarmManagerDialog::ZoneHighLight_Tree(QPainter & painter, IProj * pProj)
{
	return ui->tree_widget->ZoneHighLight(painter, pProj);
}

void AgriFarmManagerDialog::updateFarmBasicInfo()
{
	std::set<int> cropVarieties;
	double totalArea = 0.0;

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();

	// 遍历所有激活盐田信息
	auto fieldList = ui->tree_widget->GetActiveFieldList();
	for (const auto& field : fieldList)
	{
		// 计算区域面积
		double area = 0.0;
		for (const auto &itrZone : zoneBaseVec)
		{
			if (0 == field->area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
			{
				std::vector<QPointF> zonePoints;
				for (const auto& stZonePoint : itrZone.stZonePointVec)
				{
					QPointF tmpPointF(stZonePoint.dLon, stZonePoint.dLat);
					zonePoints.emplace_back(tmpPointF);
				}

				AgriFarmManagerUtil::CalcPolygonArea(zonePoints, area);
				totalArea += area;

				cropVarieties.insert(field->crop);

				break;
			}
		}
	}
	
	ui->label_7->setText(tr2("%1（亩）").arg(AgriFarmManagerUtil::sqmToMu(totalArea)));
	ui->label_8->setText(tr2("%1（亩）").arg(AgriFarmManagerUtil::sqmToMu(totalArea)));
	AgriFarmManagerUtil::filed_area = ui->label_7->text();

	ui->label_6->setText(tr2("100%")); // TODO: 设置面积比例

	ui->label_9->setText(tr2("%1种").arg(cropVarieties.size()));
}

void AgriFarmManagerDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriFarmManagerDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriFarmManagerDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}
