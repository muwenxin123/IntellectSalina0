#include "AgrisprayComboBox.h"
#include <string>
#include "QPen"
#include "QVector"
#include "QPoint"
#include "QPolygon"
#include "QVariant"
#include "QSettings"
#include "AgriFarm\AgriFarmManagerDecl.h"
#include "IAgriDataAnalysisPredictActivator.h"
#include "LY_AgriDataAnalysisPredictInterface.h"

AgrisprayComboBox::AgrisprayComboBox(QWidget *parent /*= nullptr*/)
{
	selectedActiveFarm();
	initData();
	connect(this, &AgrisprayComboBox::currentTextChanged, this, &AgrisprayComboBox::slotCurrentTextChange);
}

AgrisprayComboBox::~AgrisprayComboBox()
{

}

void AgrisprayComboBox::zoneHighLight(QPainter &painter, IProj *pProj)
{
	m_zoneDrawMap.iterate_query([&](const std::string& id, const qnzkna::ZoneManage::CZoneBase& zone) {

		if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
			zone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {

			QBrush brush;
			brush.setColor(Qt::transparent);
			brush.setStyle(Qt::BrushStyle::SolidPattern);
			painter.setBrush(brush);

			QPen pen(Qt::white);
			pen.setWidth(5);
			painter.setPen(pen);

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

void AgrisprayComboBox::clearZoneHighLight()
{
	m_zoneDrawMap.clear();
}

void AgrisprayComboBox::initData()
{
	m_vecFieldList.clear();

	IAgriDataAnalysisPredictActivator::Get().getPIAgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);

	this->clear();
	this->addItem("");

	for (auto& fieldInfo : m_vecFieldList) {
		// 匹配激活农场
		if (m_currentFarm == fieldInfo.farm_id) {
			this->addItem(fieldInfo.name, QVariant::fromValue(fieldInfo));// 保存结构体数据
		}
	}
}

void AgrisprayComboBox::selectedActiveFarm()
{
	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	m_currentFarm = ini.value("FarmID", "").toString();
}

void AgrisprayComboBox::switchActiveFarm()
{
	selectedActiveFarm();
	initData();
}

qnzkna::ZoneManage::CZoneBase AgrisprayComboBox::GetActiveFieldZone()
{
	qnzkna::ZoneManage::CZoneBase tmpZoneBase;

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = IAgriDataAnalysisPredictActivator::Get().getPIZoneManageService()->GetAllZoneBase();
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

void AgrisprayComboBox::setActiveFieldInfo(AgriFieldInfo fieldInfo)
{
	m_fieldInfo = fieldInfo;
}

void AgrisprayComboBox::slotCurrentTextChange(const QString& strText)
{
	emit sigCurrentTextChange(strText);

	// 查询所有区域
	m_fieldInfo = this->currentData().value<AgriFieldInfo>();
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = IAgriDataAnalysisPredictActivator::Get().getPIZoneManageService()->GetAllZoneBase();

	// 清理所有高亮区域
	m_zoneDrawMap.clear();

	bool isCenter = false;
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{

			emit  setupField(m_fieldInfo.name, m_fieldInfo.area_id);
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
			m_zoneDrawMap.insert(m_fieldInfo.area_id.toStdString(), itrZone);

			break;
		}
	}
}

void AgrisprayComboBox::setScreenCenter(double &dLon, double &dLat)
{
	QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
	LY_AgriDataAnalysisPredictInterface::Get().Script(str.toLocal8Bit());
}
