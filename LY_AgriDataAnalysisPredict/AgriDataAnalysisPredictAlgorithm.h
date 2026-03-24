#ifndef AGRIDATAANALYSISPREDICTALGORITHM_H_
#define AGRIDATAANALYSISPREDICTALGORITHM_H_
 
#include <iostream>
#include <vector>
#include <string>
#include <QPoint>
#include <unordered_map>

namespace AgriDataAnalysisPredictAlgorithm
{
	struct Poi {
		Poi() {};
		Poi(double dLon, double dLat, double dAlt = 0.0);
		double lon;
		double lat;
		double alt;
		double distanceTo(const Poi &other) const;
	};
	void zoneMinPath(const std::vector<Poi> &points, std::vector<Poi> &path);

	void generatePath(std::vector<QPoint> points, std::vector<QPoint> &path);

	struct QPointFCompare {
		bool operator()(const QPointF& p1, const QPointF& p2) const {
			return p1.x() < p2.x() || (p1.x() == p2.x() && p1.y() < p2.y());
		}
	};
	bool createGridGeometryMap(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons, 
		const std::vector<double>& lats, 
		double spacing,
		std::vector<QPoint>& points,  
		std::map<QPoint, Poi, QPointFCompare>& pointMap);

	bool createGridPointDiffuse(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons,
		const std::vector<double>& lats,
		const std::vector<double>& values,
		double spacing,
		int difCount,
		std::vector<Poi>& difPoints);
}

#endif  // AgriDataAnalysisPredictAlgorithm_H