#ifndef AGRIDATAANALYSISPREDICTALGORITHM_H_
#define AGRIDATAANALYSISPREDICTALGORITHM_H_

#include <iostream>
#include <vector>
#include <string>
#include <QPoint>
#include <unordered_map>
#include <vector3.h>
#include "ly_agrifertilizepescviewmodel.h"
#include <set>
#include <algorithm>

#include "AgriDataAnalysisPredictAlgorithm.h"

//using namespace AgriDataAnalysisPredictAlgorithm;

namespace AgriDataAnalysisPredictAlgorithm
{
	//struct Poi {
	//	Poi() {};
	//	Poi(double dLon, double dLat, double dAlt = 0.0);
	//	double lon;
	//	double lat;
	//	double alt; //等级
	//	double distanceTo(const Poi &other) const;
	//};
	//typedef enum POINT_STATUS
	//{
	//	POINT_STATUS_NONE,
	//	POINT_STATUS_START, //开始点
	//	POINT_STATUS_MIDDLE,//中间点
	//	POINT_STATUS_END,//结束点
	//};
	//struct AirInstructionPoi {
	//	AirInstructionPoi() {
	//		 startLon = 0;
	//		 middleLon = 0;
	//		 endLon = 0;
	//		 centerLat = 0;
	//		 isOpen = false;
	//		 openValue = 0;
	//		 point_status = POINT_STATUS_NONE;
	//	};

	//	POINT_STATUS point_status;
	//	double startLon; //打开的点
	//	double middleLon;  //调整开关的点
	//	double endLon; //关闭的点
	//	bool     isOpen ;//喷嘴是否打开
	//	double  openValue;//打开的值
	//	double centerLat;
	//	
	//};

	void zoneMinPath(const std::vector<Poi> &points, std::vector<Poi> &path);

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
		const std::vector<double>&origins,
		double spacing,
		std::vector<QPoint>& points,
		std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare>& pointMap);


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