#include <random>
#include <qmath.h>
#include <QUuid>
#include <QDebug>
#include <QPolygonF>
#include <QPainterPath>
#include <osgEarth/GeoData>

#include "AgriLayerControlUtil.h"
#include "QCString.h"


QString AgriDataAnalysisPredictUtil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}

bool AgriDataAnalysisPredictUtil::CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point)
{
	int n = zonePoints.size();
	if (n < 3) {
		//std::cerr << "错误：多边形至少需要 3 个点！" << std::endl;
		return false;
	}

#if 0

	double area = 0.0;
	double cx = 0.0;
	double cy = 0.0;

	for (int i = 0; i < n; ++i) {
		const QPointF& p0 = zonePoints[i];
		const QPointF& p1 = zonePoints[(i + 1) % n]; // 环形连接

		double cross = p0.x() * p1.y() - p1.x() * p0.y();
		area += cross;
		cx += (p0.x() + p1.x()) * cross;
		cy += (p0.y() + p1.y()) * cross;
	}

	area *= 0.5;
	if (std::abs(area) < 1e-10) return false; // 避免除以0

	cx /= (6.0 * area);
	cy /= (6.0 * area);

	point.setX(cx);
	point.setY(cy);

#else

	// WGS84 constants
	constexpr double deg2rad = M_PI / 180.0;
	constexpr double rad2deg = 180.0 / M_PI;

	double x = 0, y = 0, z = 0;
	for (const auto& pt : zonePoints) {
		double lat = pt.y() * deg2rad;
		double lon = pt.x() * deg2rad;

		x += cos(lat) * cos(lon);
		y += cos(lat) * sin(lon);
		z += sin(lat);
	}

	int total = zonePoints.size();
	x /= total;
	y /= total;
	z /= total;

	double hyp = sqrt(x * x + y * y);
	double lat = atan2(z, hyp);
	double lon = atan2(y, x);

	point.setX(lon * rad2deg);
	point.setY(lat * rad2deg);

#endif

	return true;
}

// 近似转换为米的比例（适用于小区域）
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）

bool AgriDataAnalysisPredictUtil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
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

// **计算交集**
QPolygonF AgriDataAnalysisPredictUtil::calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
{
	QPolygonF intersectionPolygon;

	intersectionPolygon = polygon1.intersected(polygon2);
	if (!intersectionPolygon.isEmpty()) {
		// qDebug() << "交集区域点数：" << intersectionPolygon.size();
	}
	else {
		// qDebug() << "无交集";
	}

	return intersectionPolygon;
}