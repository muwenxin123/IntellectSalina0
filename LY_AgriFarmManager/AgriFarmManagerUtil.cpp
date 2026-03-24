#include <iostream>
#include <QUuid>
#include <QPointF>
#include <QPainterPath>
#include <osgEarth/GeoData>
#include <qmath.h>

#include "AgriFarmManagerUtil.h"

namespace AgriFarmManagerUtil
{

	QString filed_area = "";

}


QString AgriFarmManagerUtil::FetchValueByEnumId(const AgriEnumInfoPtrVec& vecEnums, const int& nEnumId)
{
	for (const auto& emusInfo : vecEnums)
	{
		if (emusInfo->id == nEnumId)
		{
			return QString::fromStdString(emusInfo->description);
		}
	}
	return "";
}

QString AgriFarmManagerUtil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}

// 近似转换为米的比例（适用于小区域）
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）

bool AgriFarmManagerUtil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
{
	// 至少需要3个点形成多边形
	int n = zonePoints.size();
	if (n < 3) return false;

	QPointF origin = zonePoints[0];
	std::vector<osg::Vec3d> utmPoints;
	for (const auto& point : zonePoints)
	{
		osg::Vec3d tmpPoint;

		double dLon = (point.x() - origin.x()) * M_PI / 180.0;
		double dLat = (point.y() - origin.y()) * M_PI / 180.0;

		tmpPoint.x() = EARTH_RADIUS * dLat; // Y 方向
		tmpPoint.y() = EARTH_RADIUS * dLon * cos(origin.y() * M_PI / 180.0); // X 方向

		utmPoints.emplace_back(tmpPoint);
	}

	// 使用 Shoelace 公式计算面积
	double sum = 0.0;
	for (size_t i = 0; i < utmPoints.size(); ++i)
	{
		size_t next = (i + 1) % utmPoints.size();
		sum += (utmPoints[i].x() * utmPoints[next].y()) - (utmPoints[next].x() * utmPoints[i].y());
	}

	area = std::abs(sum) * 0.5;
	return true;
}

bool AgriFarmManagerUtil::CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point)
{
	int n = zonePoints.size();
	if (n < 3) {
		//std::cerr << "错误：多边形至少需要 3 个点！" << std::endl;
		return false;
	}

	double area = 0.0;
	double Cx = 0.0, Cy = 0.0;

	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;  // 下一个点，首尾相连
		double cross = zonePoints[i].x() * zonePoints[j].y() - zonePoints[j].x() * zonePoints[i].y();
		area += cross;
		Cx += (zonePoints[i].x() + zonePoints[j].x()) * cross;
		Cy += (zonePoints[i].y() + zonePoints[j].y()) * cross;
	}

	area *= 0.5;
	if (area == 0) {
		//std::cerr << "错误：多边形面积为 0，可能是退化情况！" << std::endl;
		return false;
	}

	Cx /= (6.0 * area);
	Cy /= (6.0 * area);

	point.setX(Cx);
	point.setY(Cy);

	return true;
}

bool AgriFarmManagerUtil::JudgePointInZone_OddEven(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
	QPolygonF qPolygon = QPolygonF(QVector<QPointF>::fromStdVector(zonePoints));
	return qPolygon.containsPoint(point, Qt::OddEvenFill);  // 使用奇偶规则
}
