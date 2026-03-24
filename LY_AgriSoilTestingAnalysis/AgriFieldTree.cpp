#include <QDebug>
#include <QSettings>
#include <QPainter>

#include "AgriFieldTree.h"
#include "ui_AgriFieldTree.h"
#include "LyMessageBox.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "IAgriSoilAnalysisPredictActivator.h"

AgriFieldTree::AgriFieldTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriFieldTree)
{
    ui->setupUi(this);

	InitActiveFarm();

	Init();

	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &AgriFieldTree::onItemClicked);
}

AgriFieldTree::~AgriFieldTree()
{
    delete ui;
}

void AgriFieldTree::Init()
{
	InitTree();

	CreateTree();

	// 默认选中第一块田地
	if (m_vecFieldList.size())
	{
		QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(0);
		if (topItem)
		{
			// 设置选中节点
			ui->treeWidget->setCurrentItem(topItem);

			// 手动触发 itemClicked 信号
			emit ui->treeWidget->itemClicked(topItem, 0);
		}
	}
}

void AgriFieldTree::InitTree()
{
	ui->treeWidget->clear();
	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabels(QStringList() << tr2("盐田列表"));
}

void AgriFieldTree::CreateTree()
{
	BuildTree(ui->treeWidget);

	ui->treeWidget->expandAll();
}

void AgriFieldTree::InitActiveFarm()
{
	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	m_sCurActiveFarm = ini.value("FarmID", "").toString();
}

void AgriFieldTree::SwitchActiveFarm()
{
	InitActiveFarm();

	Init();
}

void AgriFieldTree::ClearZoneHighLight()
{
	m_zoneDrawMap.clear();
}

qnzkna::ZoneManage::CZoneBase AgriFieldTree::GetActiveFieldZone()
{
	qnzkna::ZoneManage::CZoneBase tmpZoneBase;

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = IAgriSoilAnalysisPredictActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			tmpZoneBase = itrZone;
			break;
		}
	}

	return tmpZoneBase;
}

void AgriFieldTree::setActiveFieldInfo(AgriFieldInfo fieldInfo)
{
	m_fieldInfo = fieldInfo;
}

QString AgriFieldTree::GetCurrentFarm()
{
	return m_sCurActiveFarm;
}

void AgriFieldTree::BuildTree(QTreeWidget *treeWidget)
{
	m_vecFieldList.clear();

	if (nullptr != AgriFarmManagerService())
	{
		AgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);
	}

	for(auto& fieldInfo : m_vecFieldList)
	{
		// 匹配激活农场
		//if (m_sCurActiveFarm == fieldInfo.farm_id)
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
			item->setText(0, fieldInfo.name);
			item->setData(0, Qt::UserRole + 1, QVariant::fromValue(fieldInfo)); // 保存结构体数据

			treeWidget->addTopLevelItem(item);
		}
	}
}

void AgriFieldTree::onItemClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr)
	{
		return;
	}

	emit LeafNodeClicked(item, column);

	// 查询所有区域
	m_fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();

	// 清理所有高亮区域
	m_zoneDrawMap.clear();

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
			for (auto& point: tmpCurZone.stZonePointVec)
			{
				zonePoints.emplace_back(QPointF(point.dLon, point.dLat));
			}
		}
	}

	if (AgriSoilAnalysisPredictUtil::CalcPolygonCenterPoint(zonePoints, centerPoint))
	{
		double dLon = centerPoint.x();
		double dLat = centerPoint.y();
		//SetScreenCenter(dLon, dLat);
	}
}

void AgriFieldTree::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// 
}

void AgriFieldTree::SetScreenCenter(double &dLon, double &dLat)
{
	QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
	LY_AgriSoilAnalysisPredictInterface::Get().Script(str.toLocal8Bit());
}

void AgriFieldTree::ZoneHighLight(QPainter &painter, IProj *pProj)
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
				QPoint point = pProj->Jwd2Screen(zonePoint.dLon,zonePoint.dLat);
				points.push_back(point);
			}
			QPolygon poly(points);
			painter.drawPolygon(poly);
		}
	});
}
