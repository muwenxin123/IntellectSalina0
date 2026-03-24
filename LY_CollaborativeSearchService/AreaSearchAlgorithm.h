#ifndef AREASEARCHALGORITHM_H
#define AREASEARCHALGORITHM_H

#include "aStar.h"
#include "kmeans.h"
#include <map>
#include <vector>
#include <QPainter>
#include <QGeoCoordinate>
#include "Zone/ZoneManagerInfo.h"

typedef std::vector<QPointF> LLQPoints;

class AreaSearchAlgorithm
{
public:
    AreaSearchAlgorithm();
	~AreaSearchAlgorithm();

	casic_vector3d CalcPosByDis_Angle(double &lon, double &lat, double distance, double& angle);

	casic_vector3d getStagingPoint(casic_vector2d leftup, casic_vector2d rightdown,casic_vector2d takeoffPoint, double cruiseHeight);

	casic_vector3d getTakeOffPoint(casic_vector2d leftup, casic_vector2d rightdown, casic_vector2d takeoffPoint, double cruiseHeight);

	std::vector<casic_vector2d> calcTerrianObstacle(casic_vector2d start, casic_vector2d goal,double cruiseHeight);

	void stagingPathPlanning(int uid, casic_vector3d start, casic_vector3d goal,double safeDis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap);

	void firstPointPathPlanning(int uid, casic_vector3d start, casic_vector3d goal,double safedis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap);

	bool is_point_in_zone(const QPointF point,const std::vector<QPointF>ZonePoints);

	bool is_point_in_zones(const QPointF point, const std::vector<LLQPoints> Zones);

	double calculateCross(casic_vector3d p1, casic_vector3d p2, casic_vector3d p3);

	bool is_lineA_intersect_lineB(casic_vector3d Apoint1, casic_vector3d Apoint2, casic_vector3d Bpoint1, casic_vector3d Bpoint2);

	bool is_lineIntersectZone(casic_vector3d pointA,  casic_vector3d pointB, std::vector<QPointF> points_Zone);

	int num_lineIntersectZone(casic_vector3d pointA, casic_vector3d pointB, std::vector<QPointF> points_Zone, std::vector<QPointF> &points_List);

	std::pair<double, double> getlineIntersectPoint(casic_vector3d pointA, casic_vector3d pointB, casic_vector3d pointC, casic_vector3d pointD);

	bool is_lineToAllaZones(casic_vector3d pointA, casic_vector3d pointB,std::vector<LLQPoints>zonelist);

	double calDisP2Line(casic_vector3d linePoint1, casic_vector3d linePoint2, casic_vector3d point);

	double calDisP2Line1(casic_vector3d linePoint1, casic_vector3d linePoint2, casic_vector3d point);

	bool isInterObstacle(casic_vector3d point1, casic_vector3d point2, vector<casic_vector2d> obstacleArea);

	bool isInterObstacle(casic_vector3d point1, casic_vector3d point2, vector<casic_vector2d> obstacleArea,double gridwidth);

	class PlanNode {
	public:
		PlanNode();
		PlanNode(double _x, double _y, double _xindex, double yindex, double _gCost = 0, double _hCost = 0);
		~PlanNode();

		void setObstacle();

		bool isObstacle();

		bool operator ==(const PlanNode& rhs);
		PlanNode& operator +=(const PlanNode& rhs);
		PlanNode& operator =(const PlanNode& rhs);

		double coordinateX() const;

		double coordinateY() const;

		void setBackpoint(std::shared_ptr<PlanNode> rhs);

		std::shared_ptr<PlanNode> getBackpoint();

		void setHCost(double _hCost);

		double getHCost() const;

		void setG(double _gCost);

		double getGCost() const;

		void setTotalCost();

		double getTotalCost() const;

		bool isVisited();

		void setVisited();

		double indexX() const;

		double indexY() const;

		void setindexX(double index);

		void setindexY(double index);

		void setAreaAttribute(int areaType);

		int getAreaAttribute();

	private:
		double	m_x, m_y;
		double  m_totalCost;
		double	m_gCost;
		double	m_hCost;
		bool	m_obstacle;
		bool	m_isVisited;
		double  m_indexX, m_indexY;
		std::shared_ptr<PlanNode>	m_backpoint;

		int m_areaAttribute = 0;
	};

	void AreaSerachPlan(casic_vector3d leftup,casic_vector3d rightdown,casic_vector3d currPos,double Height, double gridWidth,
		double speed, double angularSpeed, double flyTime, double coverRate,double safeDis, std::map<int, std::vector<casic_vector3d>> &PointsVec);

	void KmeansAreaSerachPlan(casic_vector3d leftup, casic_vector3d rightdown, casic_vector3d currPos, double Height, double gridWidth,
		double speed, double angularSpeed, double upSpeed, double flyTime, double coverRate, double safeDis, int areaType,int areaPointsNum, std::map<int, std::vector<casic_vector3d>> &PointsVec,
		int planWay, std::map<int, std::vector<casic_vector3d>> &startReturnPointMap);

	void SpySideAreaSerachPlan(casic_vector3d leftup, casic_vector3d rightdown, casic_vector3d currPos, double Height, double gridWidth,
		double speed, double angularSpeed, double flyTime, double coverRate, double safeDis, int minAngle, int maxAngle, int uavNum,
		int podTurnSpeed, std::map<int, std::vector<casic_vector3d>> &PointsVec);

	void sparsePath(std::map<int, std::vector<casic_vector3d>>& AreaSearchPointMap, std::map<int, std::vector<casic_vector3d>> &searchStartReturnPointMap = std::map<int, std::vector<casic_vector3d>>());

	double calAngle(casic_vector2d point1, casic_vector2d centerPoint, casic_vector2d point3);

	std::vector<casic_vector3d> getEndPoints(casic_vector3d pos,double gridWidth);

	std::vector<QPoint> getNextPointFromImpasse(QPoint nowPoint, std::vector<std::vector<shared_ptr<PlanNode>>> GridMap);

	void createMGraph(const std::vector<std::vector<shared_ptr<PlanNode>>> m_roadMap, int areaLable,
		std::map<shared_ptr<PlanNode>, int>& V2PlanNode, std::vector<std::vector<int>>& mGraph);

	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> createGridsNode(int uid, double height, double cruiseSpeed, const std::vector<double>& gridMapParams, const QGeoCoordinate& gridMapLeftup);

	void addNoflyToObstacleArea(int uid, double height, const std::vector<LLQPoints>& noFlyZonePoints);

	void returnPathPlanning(int uid, casic_vector3d start, casic_vector3d goal, double safeDis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap);

	qnzkna::ZoneManage::CZoneBase searchArea;
	qnzkna::ZoneManage::CZoneBase takeOff_land_Area;
	qnzkna::ZoneManage::CZoneBaseVec noFlyAreaVec;
	std::vector<std::vector<shared_ptr<PlanNode>>> m_PolygonGridMap;

	void PolygonAreaSerachPlan(casic_vector3d currPos, double Height, double gridWidth, double speed, double angularSpeed,
		double upSpeed, double flyTime, double coverRate, double safeDis, int areaType,int areaPointsNum,
		std::map<int, std::vector<casic_vector3d>> &PointsVec,int planWay,
		std::map<int, std::vector<casic_vector3d>> &startReturnPointMap);

	void PolygonAreaSerachPlan1(casic_vector3d currPos, double Height, double gridWidth, double speed, double angularSpeed,
		double upSpeed, double flyTime, double coverRate, double safeDis, int areaType, int areaPointsNum,
		std::map<int, std::vector<casic_vector3d>> &PointsVec, int planWay,
		std::map<int, std::vector<casic_vector3d>> &startReturnPointMap);

public:
	casic_vector3d         stagingPoint;
	casic_vector3d         currTakeOffPoint;

	std::vector<casic_vector3d> allStagingPoint;

	double _safeDistance = 5.0;

	std::vector<std::vector<double>>kmeans_sample;
	std::vector<int> labels;

	std::vector<shared_ptr<PlanNode>> m_countvec;
	std::shared_ptr<PlanNode>	m_point;

	std::vector<casic_vector3d>ObstacleVec1;
	std::vector<casic_vector3d>ObstacleVecf1;
	std::vector<casic_vector3d>ObstacleVec2;
	std::vector<std::vector<casic_vector3d>> allObstacleArea;

	std::map<int, std::vector<casic_vector3d>>stagingPointList;

	std::vector<std::vector<shared_ptr<PlanNode>>> Gridsmap;

	std::vector<std::vector<casic_vector3d>>_coverAreaVec;
	std::vector<std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>>> allGridsMaps;

};

#endif