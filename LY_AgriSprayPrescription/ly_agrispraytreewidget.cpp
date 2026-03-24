#include "ly_agrispraytreewidget.h"
#include "ui_ly_agrispraytreewidget.h"
#include <QSettings>
#include <QPainter>
#include "LY_AgriSprayPrescriptionInterface.h"

LY_AgriSprayTreeWidget::LY_AgriSprayTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriSprayTreeWidget)
{
    ui->setupUi(this);
    selectedActiveFarm();
    init();

}
void LY_AgriSprayTreeWidget::init()
{

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabels(QStringList() << tr2("盐田列表"));

    addTree(ui->treeWidget);
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &LY_AgriSprayTreeWidget::onItemClicked);

    ui->treeWidget->expandAll();

    if (m_vecFieldList.size())
        {
            QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(0);

            // 设置选中节点
            ui->treeWidget->setCurrentItem(topItem);

            // 手动触发 itemClicked 信号
            emit ui->treeWidget->itemClicked(topItem, 0);
			
        }

}

void LY_AgriSprayTreeWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr)
	{
		return;
	}

    emit leafNodeClicked(item, column);

        // 查询所有区域
        AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
        qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = LY_AgriSprayPrescriptionActivator::Get().getPIZoneManageService()->GetAllZoneBase();

        // 清理所有高亮区域
        m_zoneDrawMap.clear();

        bool isCenter = false;
        for (const auto &itrZone : zoneBaseVec)
        {
            if (0 == fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
            {

				emit  setupField(fieldInfo.name, fieldInfo.area_id);
                double curLon = itrZone.stRoundParameter.dLon;
                double curLat = itrZone.stRoundParameter.dLat;

                if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
                    itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
                    if (itrZone.stZonePointVec.size() > 0) {
                        curLon = itrZone.stZonePointVec[0].dLon;
                        curLat = itrZone.stZonePointVec[0].dLat;
                    }
                }

                if (!isCenter)
                {
                    //setScreenCenter(curLon, curLat);
                    //isCenter = true;
                }

                // 添加高亮区域
                m_zoneDrawMap.insert(fieldInfo.area_id.toStdString(), itrZone);
            }
        }
}
void LY_AgriSprayTreeWidget::selectedActiveFarm()
{
    // 查询当前激活农场
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Agri");
    m_currentFarm = ini.value("FarmID", "").toString();
}
void LY_AgriSprayTreeWidget::addTree(QTreeWidget *treeWidget)
{
    m_vecFieldList.clear();

    LY_AgriSprayPrescriptionActivator::Get().getPIAgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);

    for(auto& fieldInfo : m_vecFieldList)
    {
        // 匹配激活农场
        if (m_currentFarm == fieldInfo.farm_id)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
            item->setText(0, fieldInfo.name);
            item->setData(0, Qt::UserRole + 1, QVariant::fromValue(fieldInfo)); // 保存结构体数据

            treeWidget->addTopLevelItem(item);
        }
    }
}
void LY_AgriSprayTreeWidget::switchActiveFarm()
{
    selectedActiveFarm();
    init();
}
void LY_AgriSprayTreeWidget::setScreenCenter(double &dLon, double &dLat)
{
    QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
    LY_AgriSprayPrescriptionInterface::Get().Script(str.toLocal8Bit());

}
 void LY_AgriSprayTreeWidget::clearZoneHighLight()
{
     m_zoneDrawMap.clear();

 }
void LY_AgriSprayTreeWidget::zoneHighLight(QPainter &painter, IProj *pProj)
{
    m_zoneDrawMap.iterate_query([&](const std::string& id, const qnzkna::ZoneManage::CZoneBase& zone) {

        if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
            zone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {

            QPen pen(Qt::white);
            pen.setWidth(5);
            painter.setPen(pen);
			painter.setBrush(Qt::NoBrush);
            QVector<QPoint> points;
            points.clear();
            for (auto & zonePoint : zone.stZonePointVec)
            {
                QPoint point1 = pProj->Jwd2Screen(zonePoint.dLon,
                    zonePoint.dLat);
                points.push_back(point1);
            }
            QPolygon poly(points);
            painter.drawPolygon(poly);
        }
    });
}
LY_AgriSprayTreeWidget::~LY_AgriSprayTreeWidget()
{
    delete ui;
}
