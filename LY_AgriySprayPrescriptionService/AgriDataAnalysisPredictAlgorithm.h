#ifndef AGRIDATAANALYSISPREDICTALGORITHM_H_
#define AGRIDATAANALYSISPREDICTALGORITHM_H_

#include <iostream>
#include <vector>
#include <string>
#include <QPoint>
#include <unordered_map>
#include <vector3.h>
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"
using namespace AgriSprayPrescriptionDecl;

using namespace  AgriSoilTestingFertilizer;


namespace AgriDataAnalysisPredictAlgorithm
{
	
	//typedef enum POINT_STATUS
	//{
	//	POINT_STATUS_NONE,
	//	POINT_STATUS_START,
	//	POINT_STATUS_MIDDLE,
	//	POINT_STATUS_END,
	//};

	//void zoneMinPath(const std::vector<Poi> &points, std::vector<Poi> &path);

	//水平
	void generatePath(std::vector<QPoint> points,
		std::vector<QPoint> &path, 
		std::vector<QPoint> &uploadPath);
	//垂直
	void generateVerticalPath(std::vector<QPoint> points, std::vector<QPoint> &path);

	struct QPointFCompare {
		bool operator()(const QPointF& p1, const QPointF& p2) const {
			return p1.x() < p2.x() || (p1.x() == p2.x() && p1.y() < p2.y());
		}
	};
	//水平
	bool createGridGeometryMap(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons,
		const std::vector<double>& lats,
		const std::vector<double>& levels,
		double spacing,
		std::vector<QPoint>& points,
		std::map<QPoint, Poi, QPointFCompare>& pointMap);
	//肥力航线
	bool createFertilityGridGeometryMap(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons,
		const std::vector<double>& lats,
		const std::vector<double>& levels,
		double spacing,
		std::vector<QPoint>& points,
		std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare>& pointMap);

	//垂直
	bool createVerticalGridGeometryMap(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons,
		const std::vector<double>& lats,
		double spacing,
		std::vector<QPoint>& points,
		std::map<QPoint, Poi, QPointFCompare>& pointMap);

	// 射线法判断点是否在多边形内
	bool isPointInPolygon(const QPointF &point, const std::vector<QPointF> &polygon);

	//std::vector<AirInstructionPoi> AirInstructionPoints;
	bool createGridPointDiffuse(const std::vector<QPointF>& boundPoings,
		const std::vector<double>& lons,
		const std::vector<double>& lats,
		const std::vector<double>& values,
		double spacing,
		int difCount,
		std::vector<Poi>& difPoints);

	


}

#endif  // AgriDataAnalysisPredictAlgorithm_H