#ifndef AGRIDATAANALYSISPREDICTALGORITHM_H_
#define AGRIDATAANALYSISPREDICTALGORITHM_H_

#include <iostream>
#include <vector>
#include <string>
#include <QPoint>
#include <unordered_map>
#include "gridbasesweepplanner.h"
#include <vector3.h>
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"

using namespace AgriSprayPrescriptionDecl;
namespace AgriDataAnalysisPredictAlgorithm
{
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

	/**
	边界x
	边界y
	障碍物
	起点
	每个格子的宽
	方向
	返回的x
	返回的y
	输入的方向
	*/
	void planning(std::vector<double>& ox, std::vector<double>& oy,
		std::vector<std::vector<QPointF>>foxy,
		std::vector<Poi> stZonePointVec,
		QPointF areaFarPos,
		double resolution,
		std::vector<double>& rx,
		std::vector<double>& ry,
		int planWay);


}

#endif  // AgriDataAnalysisPredictAlgorithm_H